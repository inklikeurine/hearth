// SPDX-License-Identifier: GPL-3.0-or-later
//
// hearth-send COMMAND...: passes one command to the running shell, for labwc's
// key bindings. "hearth-send state" waits for the answer and prints it.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

namespace {

// Where the shell listens: one socket per compositor, so a test session never
// reaches the real one. CommandServer::socketPath() has the same rule.
std::string socketPath()
{
    const char *folder = std::getenv("XDG_RUNTIME_DIR");
    const char *display = std::getenv("WAYLAND_DISPLAY");
    if (!folder || !display || std::strchr(display, '/')) {
        return { };
    }
    return std::string(folder) + "/hearth-shell-" + display + ".sock";
}

} // namespace

int main(int argc, char **argv)
{
    if (argc < 2) {
        std::fprintf(stderr, "usage: hearth-send COMMAND...\n");
        return 2;
    }
    std::string line;
    for (int i = 1; i < argc; ++i) {
        line += (i > 1 ? " " : "") + std::string(argv[i]);
    }
    line += '\n';

    const std::string path = socketPath();
    sockaddr_un address { };
    address.sun_family = AF_UNIX;
    if (path.empty() || path.size() >= sizeof address.sun_path) {
        std::fprintf(stderr, "hearth-send: not inside a Hearth session\n");
        return 1;
    }
    path.copy(address.sun_path, sizeof address.sun_path - 1);

    const int fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (fd < 0 || connect(fd, reinterpret_cast<const sockaddr *>(&address), sizeof address) < 0) {
        std::fprintf(stderr, "hearth-send: no shell is listening on %s\n", path.c_str());
        return 1;
    }
    if (send(fd, line.data(), line.size(), MSG_NOSIGNAL) != ssize_t(line.size())) {
        return 1;
    }
    if (std::strcmp(argv[1], "state") == 0) {
        char answer[4096];
        const ssize_t length = recv(fd, answer, sizeof answer, 0);
        if (length <= 0) {
            return 1;
        }
        std::fwrite(answer, 1, size_t(length), stdout);
    }
    close(fd);
    return 0;
}
