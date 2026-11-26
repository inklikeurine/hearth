// SPDX-License-Identifier: GPL-3.0-or-later
#include "buses.h"
#include "mediaplayers.h"
#include "networkstate.h"
#include "notifications.h"
#include "power.h"
#include "volume.h"

#include <QAbstractItemModelTester>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusServer>
#include <QDBusVirtualObject>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

using namespace Qt::StringLiterals;

// Plays the system's services: answers whatever is called on it.
class PretendServices : public QDBusVirtualObject {
    Q_OBJECT

public:
    QStringList calls;
    QList<QVariantList> arguments;
    QVariantMap network;
    // In the order they're listed in and asked.
    QStringList players;
    QStringList statuses;
    bool refuse = false;

    [[nodiscard]] QString introspect(const QString &) const override
    {
        return { };
    }
    bool handleMessage(const QDBusMessage &message, const QDBusConnection &bus) override
    {
        calls << message.member();
        arguments << message.arguments();
        if (refuse) {
            bus.send(message.createErrorReply(QDBusError::AccessDenied, u"no"_s));
        } else if (message.member() == "ListNames"_L1) {
            bus.send(message.createReply(QStringList { u"org.example.Other"_s } + players));
        } else if (message.member() == "GetAll"_L1) {
            bus.send(message.createReply(QVariant(message.path().contains("mpris"_L1)
                    ? QVariantMap { { u"PlaybackStatus"_s, statuses.takeFirst() } }
                    : network)));
        } else {
            bus.send(message.createReply());
        }
        return true;
    }
};

// A bus with the test on one end and what's tested on the other. It's a direct
// connection, so nothing reaches any real service.
class PretendBus {
public:
    PretendBus()
        : m_server(u"unix:tmpdir=/tmp"_s)
    {
        QObject::connect(
            &m_server, &QDBusServer::newConnection, &m_server, [this](const QDBusConnection &bus) {
                theirs = std::make_unique<QDBusConnection>(bus);
                theirs->registerVirtualObject(u"/"_s, &services, QDBusConnection::SubPath);
            });
        static int number = 0;
        m_name = u"pretend%1"_s.arg(++number);
        ours = std::make_unique<QDBusConnection>(
            QDBusConnection::connectToPeer(m_server.address(), m_name));
        // The other end is there once this end is.
        (void)QTest::qWaitFor([this] { return theirs != nullptr; });
    }
    ~PretendBus()
    {
        ours.reset();
        QDBusConnection::disconnectFromPeer(m_name);
    }

    PretendServices services;
    std::unique_ptr<QDBusConnection> theirs;
    std::unique_ptr<QDBusConnection> ours;

private:
    QDBusServer m_server;
    QString m_name;
};

class TestSystem : public QObject {
    Q_OBJECT

private:
    // Puts a pretend wpctl first on the PATH.
    static void pretendWpctl(const QTemporaryDir &dir, const QByteArray &script)
    {
        QFile file(dir.filePath(u"wpctl"_s));
        if (!file.open(QIODevice::WriteOnly) || file.write("#!/bin/sh\n" + script) < 0) {
            qFatal("can't write the pretend wpctl");
        }
        file.setPermissions(QFileDevice::ReadOwner | QFileDevice::ExeOwner);
        qputenv("PATH", QFile::encodeName(dir.path()) + ":/usr/bin:/bin");
    }

    // As an app would, over the bus.
    static uint notify(QDBusConnection &bus, const QString &summary, const QString &body = { },
        uint replaces = 0, const QVariantMap &hints = { })
    {
        QDBusMessage call = QDBusMessage::createMethodCall(u""_s,
            u"/org/freedesktop/Notifications"_s, u"org.freedesktop.Notifications"_s, u"Notify"_s);
        call << u"An app"_s << replaces << u"icon"_s << summary << body << QStringList() << hints
             << -1;
        const QDBusReply<uint> reply = bus.call(call, QDBus::BlockWithGui);
        return reply.isValid() ? reply.value() : 0;
    }

private Q_SLOTS:
    // These tests themselves must never reach the developer's desktop.
    void theDevBoxNeverUsesTheHostsBuses()
    {
        qunsetenv("HEARTH_SESSION_DEV");
        QVERIFY(Buses::isTestSession());
        QVERIFY(!Buses::session().isConnected());
        QVERIFY(!Buses::system().isConnected());
        Power power;
        QSignalSpy failed(&power, &Power::failed);
        power.turnOff();
        QCOMPARE(failed.count(), 1);
    }

    void onlyARealSessionUsesTheBuses()
    {
        QVERIFY(Buses::isTestSession({ }, false));
        QVERIFY(Buses::isTestSession(u"1"_s, false));
        QVERIFY(Buses::isTestSession(u"0"_s, true));
        QVERIFY(!Buses::isTestSession(u"0"_s, false));
    }

    void powerAsksLogind()
    {
        PretendBus bus;
        Power power(*bus.ours, nullptr);
        QSignalSpy failed(&power, &Power::failed);
        power.sleep();
        power.restart();
        power.turnOff();
        QTRY_COMPARE(
            bus.services.calls, (QStringList { u"Suspend"_s, u"Reboot"_s, u"PowerOff"_s }));
        // "interactive": false. There's nothing to show a password prompt.
        for (const QVariantList &arguments : std::as_const(bus.services.arguments)) {
            QCOMPARE(arguments, QVariantList { false });
        }
        QTest::qWait(50);
        QCOMPARE(failed.count(), 0);
    }

    void powerSaysWhenItsRefused()
    {
        PretendBus bus;
        bus.services.refuse = true;
        Power power(*bus.ours, nullptr);
        QSignalSpy failed(&power, &Power::failed);
        power.sleep();
        QTRY_COMPARE(failed.count(), 1);
    }

    void networkKinds_data()
    {
        QTest::addColumn<uint>("state");
        QTest::addColumn<QString>("type");
        QTest::addColumn<int>("kind");
        QTest::newRow("cable") << 70U << u"802-3-ethernet"_s << int(NetworkState::Wired);
        QTest::newRow("wifi") << 70U << u"802-11-wireless"_s << int(NetworkState::Wifi);
        QTest::newRow("no internet beyond it")
            << 50U << u"802-3-ethernet"_s << int(NetworkState::Wired);
        QTest::newRow("connecting") << 40U << u"802-11-wireless"_s << int(NetworkState::None);
        QTest::newRow("nothing") << 20U << u""_s << int(NetworkState::None);
    }

    void networkKinds()
    {
        QFETCH(uint, state);
        QFETCH(QString, type);
        QFETCH(int, kind);
        PretendBus bus;
        bus.services.network = { { u"State"_s, state }, { u"PrimaryConnectionType"_s, type } };
        NetworkState network(*bus.ours, nullptr);
        QCOMPARE(network.kind(), NetworkState::Unknown);
        QTRY_COMPARE(int(network.kind()), kind);
    }

    void networkFollowsChanges()
    {
        PretendBus bus;
        bus.services.network
            = { { u"State"_s, 70U }, { u"PrimaryConnectionType"_s, u"802-3-ethernet"_s } };
        NetworkState network(*bus.ours, nullptr);
        QTRY_COMPARE(network.kind(), NetworkState::Wired);
        QDBusMessage change = QDBusMessage::createSignal(u"/org/freedesktop/NetworkManager"_s,
            u"org.freedesktop.DBus.Properties"_s, u"PropertiesChanged"_s);
        change << u"org.freedesktop.NetworkManager"_s << QVariantMap { { u"State"_s, 20U } }
               << QStringList();
        bus.theirs->send(change);
        QTRY_COMPARE(network.kind(), NetworkState::None);
    }

    void volumeSetsAndReads()
    {
        QTemporaryDir dir;
        pretendWpctl(dir,
            "echo \"$@\" >> \"$(dirname \"$0\")/calls\"\n"
            "[ \"$1\" = get-volume ] && echo 'Volume: 0.45 [MUTED]'; exit 0\n");
        Volume volume;
        QSignalSpy shown(&volume, &Volume::shown);
        volume.up();
        QTRY_COMPARE(shown.count(), 1);
        QCOMPARE(volume.percent(), 45);
        QVERIFY(volume.muted());
        QFile calls(dir.filePath(u"calls"_s));
        QVERIFY(calls.open(QIODevice::ReadOnly));
        QCOMPARE(calls.readAll(),
            "set-volume --limit 1.0 @DEFAULT_AUDIO_SINK@ 5%+\nget-volume @DEFAULT_AUDIO_SINK@\n");
    }

    // A held button sends 25 presses a second, whatever the box can do.
    void volumeNeverFallsBehindAHeldButton()
    {
        QTemporaryDir dir;
        pretendWpctl(dir,
            "echo x >> \"$(dirname \"$0\")/calls\"; sleep 0.05\n"
            "[ \"$1\" = get-volume ] && echo 'Volume: 0.10'; exit 0\n");
        Volume volume;
        QSignalSpy shown(&volume, &Volume::shown);
        for (int i = 0; i < 50; ++i) {
            volume.down();
        }
        QTRY_VERIFY(shown.count() >= 1);
        QTest::qWait(400);
        QFile calls(dir.filePath(u"calls"_s));
        QVERIFY(calls.open(QIODevice::ReadOnly));
        QVERIFY(calls.readAll().count('x') <= 8);
    }

    void volumeSaysWhenThereIsNoOutput()
    {
        QTemporaryDir dir;
        pretendWpctl(dir, "exit 1\n");
        Volume volume;
        QSignalSpy failed(&volume, &Volume::failed);
        volume.toggleMute();
        QTRY_COMPARE(failed.count(), 1);
    }

    void notificationsArriveNewestFirst()
    {
        PretendBus bus;
        Notifications notifications;
        const QAbstractItemModelTester tester(&notifications);
        QVERIFY(notifications.attach(*bus.ours));
        QSignalSpy arrived(&notifications, &Notifications::arrived);
        QCOMPARE(notify(*bus.theirs, u"First"_s, u"Body"_s), 1U);
        QCOMPARE(notify(*bus.theirs, u"Second"_s), 2U);
        QCOMPARE(notifications.count(), 2);
        QCOMPARE(notifications.unseen(), 2);
        QCOMPARE(
            notifications.data(notifications.index(0), Notifications::SummaryRole), u"Second"_s);
        QCOMPARE(arrived.at(0), (QVariantList { u"An app"_s, u"First"_s, u"Body"_s }));
        notifications.markSeen();
        QCOMPARE(notifications.unseen(), 0);
    }

    void notificationsAreReplaced()
    {
        PretendBus bus;
        Notifications notifications;
        QVERIFY(notifications.attach(*bus.ours));
        const uint id = notify(*bus.theirs, u"50%"_s);
        QCOMPARE(notify(*bus.theirs, u"100%"_s, { }, id), id);
        QCOMPARE(notifications.count(), 1);
        QCOMPARE(notifications.data(notifications.index(0), Notifications::SummaryRole), u"100%"_s);
        // One that isn't there is a new one.
        QVERIFY(notify(*bus.theirs, u"New"_s, { }, 999) != 999);
    }

    void transientOnesOnlyPopUp()
    {
        PretendBus bus;
        Notifications notifications;
        QVERIFY(notifications.attach(*bus.ours));
        QSignalSpy arrived(&notifications, &Notifications::arrived);
        notify(*bus.theirs, u"Now playing"_s, { }, 0, { { u"transient"_s, true } });
        QCOMPARE(arrived.count(), 1);
        QCOMPARE(notifications.count(), 0);
    }

    void markupAndPicturesAreLeftOut()
    {
        PretendBus bus;
        Notifications notifications;
        QVERIFY(notifications.attach(*bus.ours));
        QSignalSpy arrived(&notifications, &Notifications::arrived);
        notify(*bus.theirs, u"A &amp; B"_s,
            u"<b>Bold</b> <a href='https://example.org'>link</a> <img src='file:///etc/passwd'>"_s);
        QCOMPARE(arrived.at(0).at(1).toString(), u"A & B"_s);
        QCOMPARE(arrived.at(0).at(2).toString(), u"Bold link"_s);
    }

    void longTextIsCut()
    {
        PretendBus bus;
        Notifications notifications;
        QVERIFY(notifications.attach(*bus.ours));
        QSignalSpy arrived(&notifications, &Notifications::arrived);
        notify(*bus.theirs, QString(100'000, u's'), QString(100'000, u'b'));
        QCOMPARE(arrived.at(0).at(1).toString().size(), 120);
        QCOMPARE(arrived.at(0).at(2).toString().size(), 400);
    }

    void aFloodIsRefused()
    {
        PretendBus bus;
        Notifications notifications;
        QVERIFY(notifications.attach(*bus.ours));
        for (int i = 0; i < 10; ++i) {
            QVERIFY(notify(*bus.theirs, u"Again"_s) != 0);
        }
        QCOMPARE(notify(*bus.theirs, u"Again"_s), 0U);
        QCOMPARE(notifications.count(), 10);
    }

    void theListIsBounded()
    {
        Notifications notifications;
        for (int i = 0; i < 200; ++i) {
            // Each from another app, so it isn't a flood.
            notifications.add(u":1.%1"_s.arg(i), u"a"_s, 0, u"s"_s, { }, false);
        }
        QCOMPARE(notifications.count(), 50);
    }

    void anAppCantTouchAnothersNotifications()
    {
        Notifications notifications;
        const uint id = notifications.add(u":1.1"_s, u"a"_s, 0, u"Mine"_s, { }, false);
        QVERIFY(notifications.add(u":1.2"_s, u"b"_s, id, u"Yours"_s, { }, false) != id);
        notifications.close(u":1.2"_s, id);
        QCOMPARE(notifications.count(), 2);
        notifications.close(u":1.1"_s, id);
        QCOMPARE(notifications.count(), 1);
    }

    void appsHearWhenTheirsCloses()
    {
        PretendBus bus;
        Notifications notifications;
        QVERIFY(notifications.attach(*bus.ours));
        QSignalSpy closed(&notifications, &Notifications::closed);
        const uint id = notify(*bus.theirs, u"One"_s);
        notify(*bus.theirs, u"Two"_s);
        notifications.dismiss(1);
        QCOMPARE(closed.count(), 1);
        QCOMPARE(closed.at(0), (QVariantList { id, 2U }));
        notifications.clear();
        QCOMPARE(notifications.count(), 0);
        QCOMPARE(notifications.unseen(), 0);
    }

    void saysWhatItCanDo()
    {
        PretendBus bus;
        Notifications notifications;
        QVERIFY(notifications.attach(*bus.ours));
        const QDBusMessage call
            = QDBusMessage::createMethodCall(u""_s, u"/org/freedesktop/Notifications"_s,
                u"org.freedesktop.Notifications"_s, u"GetCapabilities"_s);
        const QDBusReply<QStringList> reply = bus.theirs->call(call, QDBus::BlockWithGui);
        QCOMPARE(reply.value(), (QStringList { u"body"_s, u"persistence"_s }));
        const QDBusMessage about = bus.theirs->call(
            QDBusMessage::createMethodCall(u""_s, u"/org/freedesktop/Notifications"_s,
                u"org.freedesktop.Notifications"_s, u"GetServerInformation"_s),
            QDBus::BlockWithGui);
        QCOMPARE(about.arguments().size(), 4);
        QCOMPARE(about.arguments().at(3).toString(), u"1.2"_s);
    }

    void findsThePlayerThatsPlaying()
    {
        PretendBus bus;
        bus.services.players = { u"org.mpris.MediaPlayer2.a"_s, u"org.mpris.MediaPlayer2.b"_s,
            u"org.mpris.MediaPlayer2.c"_s };
        bus.services.statuses = { u"Paused"_s, u"Playing"_s, u"Stopped"_s };
        MediaPlayers players(*bus.ours, nullptr);
        players.look();
        QTRY_VERIFY(players.playing());
        players.playPause();
        QVERIFY(!players.playing());
        QTRY_COMPARE(bus.services.calls.last(), u"PlayPause"_s);
    }

    void noPlayerMeansNoButton()
    {
        PretendBus bus;
        bus.services.players = { u"org.mpris.MediaPlayer2.c"_s };
        bus.services.statuses = { u"Stopped"_s };
        MediaPlayers players(*bus.ours, nullptr);
        players.look();
        QTRY_VERIFY(bus.services.calls.contains(u"GetAll"_s));
        QTest::qWait(50);
        QVERIFY(!players.available());
        players.playPause();
        QVERIFY(!bus.services.calls.contains(u"PlayPause"_s));
    }
};

QTEST_GUILESS_MAIN(TestSystem)
#include "tst_system.moc"
