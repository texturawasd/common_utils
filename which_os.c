#ifndef WHICH_OS
#define WHICH_OS

#include "have.c"

/*
 * WHICH_OS -- try my best guess to see which OS i'm running on.
 */

const char *os;

#if defined(_WIN32) || defined(_WIN64)
os = "windows";
#elif defined(__linux__)

const char *try_to_determine_linux_distro() {
    if (command_exists("lsb_release")) {
        // lsb_release is a common tool to get Linux distribution information
        // We can use it to get the distro name
        FILE *fp = popen("lsb_release -si", "r");
        if (fp) {
            char buffer[128];
            if (fgets(buffer, sizeof(buffer), fp)) {
                // Remove trailing newline
                buffer[strcspn(buffer, "\n")] = 0;
                os = strdup(buffer);
            }
            pclose(fp);
        }
    } else if (access("/etc/os-release", R_OK) == 0) {
        // If lsb_release is not available, try reading /etc/os-release
        FILE *fp = fopen("/etc/os-release", "r");
        if (fp) {
            char line[256];
            while (fgets(line, sizeof(line), fp)) {
                if (strncmp(line, "ID=", 3) == 0) {
                    // Extract the ID value
                    char *id = line + 3;
                    // Remove any surrounding quotes and trailing newline
                    id[strcspn(id, "\n")] = 0;
                    if (*id == '"' || *id == '\'') {
                        id++;
                        id[strcspn(id, "\"'")] = 0;
                    }
                    os = strdup(id);
                    break;
                }
            }
            fclose(fp);
        }
    }

    // If we couldn't determine the distro, default to "linux"
    if (!os) {
        os = "linux";
    }
    return os;
}

#elif defined(__APPLE__) && defined(__MACH__)
const char *os = "macos";
#elif defined(__FreeBSD__)
const char *os = "freebsd";
#elif defined(__NetBSD__)
const char *os = "netbsd";
#elif defined(__OpenBSD__)
const char *os = "openbsd";
#elif defined(__DragonFly__)
const char *os = "dragonfly";
#elif defined(__sun) && defined(__SVR4)
const char *os = "solaris";
#elif defined(__hpux)
const char *os = "hpux";
#elif defined(__aix)
const char *os = "aix";
#elif defined(__unix__)
const char *os = "unix";
#else
const char *os = "unknown";
#endif
#endif /* WHICH_OS */
