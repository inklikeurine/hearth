// SPDX-License-Identifier: GPL-3.0-or-later
//
// How the two protocols fit together was learned from Stevia, Phosh's keyboard
// (GPL-3.0-or-later, https://gitlab.gnome.org/World/Phosh/stevia).
#include "waylandlink.h"

#include "qwayland-input-method-unstable-v2.h"
#include "qwayland-virtual-keyboard-unstable-v1.h"

#include <LayerShellQt/Window>
#include <QElapsedTimer>
#include <QGuiApplication>
#include <QtGui/qguiapplication_platform.h>
#include <QtWaylandClient/QWaylandClientExtensionTemplate>

#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client.h>

using namespace Qt::StringLiterals;

namespace {

// Linux's number for Enter.
constexpr uint32_t enterKey = 28;
constexpr uint32_t xkbKeymap = 1;

// What the compositor offers.
struct Globals {
    QStringList names;
    wl_shm *shm = nullptr;
};

Globals compositorGlobals(wl_display *display)
{
    Globals globals;
    static constexpr wl_registry_listener listener {
        [](void *data, wl_registry *registry, uint32_t id, const char *name, uint32_t) {
            auto *found = static_cast<Globals *>(data);
            found->names << QString::fromLatin1(name);
            if (found->names.constLast() == QLatin1StringView(wl_shm_interface.name)) {
                found->shm
                    = static_cast<wl_shm *>(wl_registry_bind(registry, id, &wl_shm_interface, 1));
            }
        },
        [](void *, wl_registry *, uint32_t) { },
    };
    wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &listener, &globals);
    wl_display_roundtrip(display);
    wl_registry_destroy(registry);
    return globals;
}

// One see-through pixel.
wl_buffer *emptyPixel(wl_shm *shm)
{
    constexpr int bytes = 4;
    const int file = memfd_create("hearth-keyboard-pixel", MFD_CLOEXEC);
    if (file < 0) {
        return nullptr;
    }
    wl_buffer *buffer = nullptr;
    if (ftruncate(file, bytes) == 0) {
        wl_shm_pool *pool = wl_shm_create_pool(shm, file, bytes);
        buffer = wl_shm_pool_create_buffer(pool, 0, 1, 1, bytes, WL_SHM_FORMAT_ARGB8888);
        wl_shm_pool_destroy(pool);
    }
    close(file);
    return buffer;
}

class InputMethodManager : public QWaylandClientExtensionTemplate<InputMethodManager>,
                           public QtWayland::zwp_input_method_manager_v2 {
public:
    InputMethodManager()
        : QWaylandClientExtensionTemplate<InputMethodManager>(1)
    {
        initialize();
    }
};

class VirtualKeyboardManager : public QWaylandClientExtensionTemplate<VirtualKeyboardManager>,
                               public QtWayland::zwp_virtual_keyboard_manager_v1 {
public:
    VirtualKeyboardManager()
        : QWaylandClientExtensionTemplate<VirtualKeyboardManager>(1)
    {
        initialize();
    }
};

// Shift, Ctrl and so on, as Wayland sends them.
struct Modifiers {
    uint32_t held = 0;
    uint32_t latched = 0;
    uint32_t locked = 0;
    uint32_t group = 0;

    bool operator==(const Modifiers &) const = default;
};

// A keyboard that isn't there.
class VirtualKeyboard : public QtWayland::zwp_virtual_keyboard_v1 {
public:
    explicit VirtualKeyboard(struct ::zwp_virtual_keyboard_v1 *keyboard)
        : QtWayland::zwp_virtual_keyboard_v1(keyboard)
    {
        m_clock.start();
    }
    ~VirtualKeyboard() override
    {
        destroy();
    }

    void setKeymap(const QByteArray &text)
    {
        if (text != m_keymap) {
            m_keymap = text;
            m_keymapSent = false;
        }
    }
    void setModifiers(const Modifiers &now)
    {
        m_now = now;
        if (m_sent != Modifiers()) {
            sendModifiers();
        }
    }
    void letGoOfModifiers()
    {
        setModifiers({ 0, 0, m_now.locked, m_now.group });
    }
    void press(uint32_t code, bool pressed)
    {
        // It must have a keymap before it presses a key. Sent only now: each one
        // reaches the app too.
        if (!m_keymapSent) {
            m_keymapSent = sendKeymap();
        }
        if (m_keymapSent) {
            sendModifiers();
            key(uint32_t(m_clock.elapsed()), code, pressed ? 1 : 0);
        }
    }

private:
    bool sendKeymap()
    {
        // With the zero at the end.
        const auto size = size_t(m_keymap.size()) + 1;
        const int file = memfd_create("hearth-keyboard-keymap", MFD_CLOEXEC);
        const bool written = file >= 0 && !m_keymap.isEmpty()
            && write(file, m_keymap.constData(), size) == ssize_t(size);
        if (written) {
            keymap(xkbKeymap, file, uint32_t(size));
            m_sent = Modifiers();
        }
        if (file >= 0) {
            close(file);
        }
        return written;
    }
    void sendModifiers()
    {
        if (m_keymapSent && m_now != m_sent) {
            m_sent = m_now;
            modifiers(m_now.held, m_now.latched, m_now.locked, m_now.group);
        }
    }

    QElapsedTimer m_clock;
    QByteArray m_keymap;
    bool m_keymapSent = false;
    Modifiers m_now;
    Modifiers m_sent;
};

} // namespace

class WaylandLink::Private : public QtWayland::zwp_input_method_v2 {
public:
    // The keys, while they come here instead of the app.
    class Grab : public QtWayland::zwp_input_method_keyboard_grab_v2 {
    public:
        Grab(struct ::zwp_input_method_keyboard_grab_v2 *grab, Private *owner)
            : QtWayland::zwp_input_method_keyboard_grab_v2(grab)
            , d(owner)
        {
        }
        ~Grab() override
        {
            release();
        }

    protected:
        void zwp_input_method_keyboard_grab_v2_keymap(
            uint32_t format, int32_t file, uint32_t size) override
        {
            if (format == xkbKeymap && size > 0) {
                void *map = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, file, 0);
                if (map != MAP_FAILED) {
                    const QByteArray text(static_cast<const char *>(map), qsizetype(size) - 1);
                    munmap(map, size);
                    if (d->keymap.load(text)) {
                        // Keys passed on mean the same to the app as they did here.
                        d->passing->setKeymap(text);
                    }
                }
            }
            close(file);
        }
        void zwp_input_method_keyboard_grab_v2_key(
            uint32_t, uint32_t, uint32_t key, uint32_t state) override
        {
            if (state == 0) {
                Q_EMIT d->q->keyReleased(key);
            } else {
                Q_EMIT d->q->keyPressed(d->keymap.key(key));
            }
        }
        void zwp_input_method_keyboard_grab_v2_modifiers(
            uint32_t, uint32_t held, uint32_t latched, uint32_t locked, uint32_t group) override
        {
            d->keymap.setModifiers(held, latched, locked, group);
            d->passing->setModifiers({ held, latched, locked, group });
        }
        void zwp_input_method_keyboard_grab_v2_repeat_info(int32_t rate, int32_t delay) override
        {
            Q_EMIT d->q->repeatChanged(rate, delay);
        }

    private:
        Private *d;
    };

    // Where the text box's cursor is, which the compositor tells only windows it
    // places next to the cursor. This one can't be seen or clicked.
    class Cursor : public QtWayland::zwp_input_popup_surface_v2 {
    public:
        Cursor(wl_compositor *compositor, wl_shm *shm, Private *owner)
            : d(owner)
            , m_surface(wl_compositor_create_surface(compositor))
            , m_pixel(emptyPixel(shm))
        {
            init(d->get_input_popup_surface(m_surface));
            wl_region *nowhere = wl_compositor_create_region(compositor);
            wl_surface_set_input_region(m_surface, nowhere);
            wl_region_destroy(nowhere);
            wl_surface_attach(m_surface, m_pixel, 0, 0);
            wl_surface_commit(m_surface);
        }
        ~Cursor() override
        {
            destroy();
            wl_surface_destroy(m_surface);
            if (m_pixel) {
                wl_buffer_destroy(m_pixel);
            }
        }

    protected:
        // Some cursors have no width, but a cursor always has a height.
        void zwp_input_popup_surface_v2_text_input_rectangle(
            int32_t, int32_t, int32_t, int32_t height) override
        {
            Q_EMIT d->q->cursorChanged(height > 0);
        }

    private:
        Private *d;
        wl_surface *m_surface;
        wl_buffer *m_pixel;
    };

    Private(WaylandLink *link, struct ::zwp_input_method_v2 *inputMethod,
        struct ::zwp_virtual_keyboard_v1 *ownKeyboard,
        struct ::zwp_virtual_keyboard_v1 *passingKeyboard)
        : QtWayland::zwp_input_method_v2(inputMethod)
        , q(link)
        , own(std::make_unique<VirtualKeyboard>(ownKeyboard))
        , passing(std::make_unique<VirtualKeyboard>(passingKeyboard))
    {
        own->setKeymap(Keymap::us());
    }
    ~Private() override
    {
        grab.reset();
        cursor.reset();
        destroy();
    }

    WaylandLink *q;
    Keymap keymap;
    std::unique_ptr<VirtualKeyboard> own;
    // Presses the grabbed keys that are for the app.
    std::unique_ptr<VirtualKeyboard> passing;
    std::unique_ptr<Grab> grab;
    std::unique_ptr<Cursor> cursor;
    // What goes with a commit: how many "done"s came.
    uint32_t serial = 0;
    bool unavailable = false;

protected:
    void zwp_input_method_v2_activate() override
    {
        Q_EMIT q->activated();
    }
    void zwp_input_method_v2_deactivate() override
    {
        Q_EMIT q->deactivated();
    }
    void zwp_input_method_v2_surrounding_text(
        const QString &text, uint32_t cursor, uint32_t anchor) override
    {
        Q_EMIT q->surroundingText(text.toUtf8(), cursor, anchor);
    }
    void zwp_input_method_v2_content_type(uint32_t hints, uint32_t purpose) override
    {
        Q_EMIT q->contentType(hints, purpose);
    }
    void zwp_input_method_v2_done() override
    {
        ++serial;
        Q_EMIT q->done();
    }
    void zwp_input_method_v2_unavailable() override
    {
        unavailable = true;
        Q_EMIT q->lost();
    }
};

WaylandLink::WaylandLink(QObject *parent)
    : CompositorLink(parent)
{
}

WaylandLink::~WaylandLink() = default;

QString WaylandLink::start()
{
    const auto *wayland = qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>();
    if (!wayland || !wayland->display() || !wayland->seat()) {
        return u"needs a Wayland session"_s;
    }
    const Globals offered = compositorGlobals(wayland->display());
    for (const QString &needed : { u"zwp_input_method_manager_v2"_s,
             u"zwp_virtual_keyboard_manager_v1"_s, u"zwlr_layer_shell_v1"_s, u"wl_shm"_s }) {
        if (!offered.names.contains(needed)) {
            return u"the compositor doesn't offer %1"_s.arg(needed);
        }
    }

    InputMethodManager inputMethods;
    VirtualKeyboardManager virtualKeyboards;
    if (!inputMethods.isActive() || !virtualKeyboards.isActive()) {
        return u"the compositor's protocols didn't start"_s;
    }
    d = std::make_unique<Private>(this, inputMethods.get_input_method(wayland->seat()),
        virtualKeyboards.create_virtual_keyboard(wayland->seat()),
        virtualKeyboards.create_virtual_keyboard(wayland->seat()));
    // If another keyboard has the job, the compositor says so now.
    wl_display_roundtrip(wayland->display());
    if (d->unavailable) {
        d.reset();
        return u"another keyboard program is running"_s;
    }
    if (offered.shm) {
        d->cursor = std::make_unique<Private::Cursor>(wayland->compositor(), offered.shm, d.get());
        wl_shm_destroy(offered.shm);
    }
    return { };
}

void WaylandLink::makeStrip(QQuickWindow *window)
{
    using LayerShellQt::Window;
    Window *layer = Window::get(window);
    layer->setLayer(Window::LayerOverlay);
    layer->setAnchors({ Window::AnchorBottom, Window::AnchorLeft, Window::AnchorRight });
    // Along the very bottom, whatever else is there.
    layer->setExclusiveZone(-1);
    // The app keeps the keyboard focus; the keys come through the grab.
    layer->setKeyboardInteractivity(Window::KeyboardInteractivityNone);
    layer->setActivateOnShow(false);
    layer->setScope(u"hearth-keyboard"_s);
}

void WaylandLink::type(const QString &text)
{
    if (d) {
        d->commit_string(text);
        d->commit(d->serial);
    }
}

void WaylandLink::deleteBefore(int bytes)
{
    if (d) {
        d->delete_surrounding_text(uint32_t(bytes), 0);
        d->commit(d->serial);
    }
}

void WaylandLink::pressEnter()
{
    if (d) {
        d->own->press(enterKey, true);
        d->own->press(enterKey, false);
    }
}

void WaylandLink::passOn(uint32_t code, bool pressed)
{
    if (d) {
        d->passing->press(code, pressed);
    }
}

void WaylandLink::setGrab(bool on)
{
    if (!d) {
        return;
    }
    if (!on) {
        d->grab.reset();
        d->passing->letGoOfModifiers();
    } else if (!d->grab) {
        d->grab = std::make_unique<Private::Grab>(d->grab_keyboard(), d.get());
    }
}
