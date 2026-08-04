/* compat: for unix-like systems only (note: for now only tested on linux) */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../process_utils.h"

static int append_chunk(char **buffer, size_t *length, const char *data, size_t data_len) {
    char *new_buffer = realloc(*buffer, *length + data_len + 1);
    if (!new_buffer) {
        return -1;
    }
    memcpy(new_buffer + *length, data, data_len);
    *length += data_len;
    new_buffer[*length] = '\0';
    *buffer = new_buffer;
    return 0;
}

static int read_from_pipe(int fd, char **data) {
    if (!data) {
        return 0;
    }
    char *output = NULL;
    size_t length = 0;
    char buffer[4096];
    while (1) {
        ssize_t n = read(fd, buffer, sizeof(buffer));
        if (n > 0) {
            if (append_chunk(&output, &length, buffer, (size_t)n) != 0) {
                free(output);
                return -1;
            }
        } else if (n == 0) {
            break;
        } else if (errno != EINTR) {
            free(output);
            return -1;
        }
    }
    *data = output ? output : calloc(1, 1);
    return *data ? 0 : -1;
}

static int read_from_pipes(int stdout_fd, int stderr_fd, char **stdout_data, char **stderr_data) {
    int result = 0;
    char *captured_stdout = NULL;
    char *captured_stderr = NULL;

    if (stdout_fd >= 0 && read_from_pipe(stdout_fd, &captured_stdout) != 0) {
        result = -1;
    }
    if (result == 0 && stderr_fd >= 0 && read_from_pipe(stderr_fd, &captured_stderr) != 0) {
        result = -1;
    }

    if (stdout_data) {
        *stdout_data = captured_stdout;
    } else {
        free(captured_stdout);
    }
    if (stderr_data) {
        *stderr_data = captured_stderr;
    } else {
        free(captured_stderr);
    }

    return result;
}

/* old function that just runs a thing, not saving the output */
int run(const char *cmd) {
    char *const argv[] = {"/bin/sh", "-c", (char *)cmd, NULL};
    int status = -1;
    if (process_exec("/bin/sh", argv, NULL, NULL, false, false, NULL, NULL, &status) != 0) {
        return -1;
    }
    return status;
}

/* note: free it, will you? */
char *capture_output(const char *cmd) {
    char *output = NULL;
    char *const argv[] = {"/bin/sh", "-c", (char *)cmd, NULL};
    int status = -1;
    if (process_exec("/bin/sh", argv, NULL, NULL, true, false, &output, NULL, &status) != 0) {
        return NULL;
    }
    return output;
}

/* runs a thing and explicitly throws out the output*/
int run_quiet(const char *cmd) {
    process_spawn_options options;
    memset(&options, 0, sizeof(options));
    options.inherit_stdin = true;
    options.inherit_stdout = false;
    options.inherit_stderr = false;

    char *const argv[] = {"/bin/sh", "-c", (char *)cmd, NULL};
    int status = -1;
    if (process_exec("/bin/sh", argv, NULL, &options, false, false, NULL, NULL, &status) != 0) {
        return -1;
    }
    return status;
}

/* shorthand to check if its running */
bool is_process_running(pid_t pid) {
    if (pid <= 0) {
        return false;
    }
    return kill(pid, 0) == 0;
}

/* pidof without shelling out */
pid_t pidof(const char *name) {
    DIR *dir = opendir("/proc");
    if (!dir) {
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '\0' || !isdigit((unsigned char)entry->d_name[0])) {
            continue;
        }

        char path[128];
        snprintf(path, sizeof(path), "/proc/%s/comm", entry->d_name);
        FILE *fp = fopen(path, "r");
        if (!fp) {
            continue;
        }

        char buffer[256];
        if (fgets(buffer, sizeof(buffer), fp)) {
            buffer[strcspn(buffer, "\n")] = '\0';
            if (strcmp(buffer, name) == 0) {
                pid_t pid = (pid_t)atoi(entry->d_name);
                fclose(fp);
                closedir(dir);
                return pid;
            }
        }
        fclose(fp);
    }

    closedir(dir);
    return -1;
}

int process_spawn(const char *path,
                  char *const argv[],
                  char *const envp[],
                  const process_spawn_options *options,
                  pid_t *pid_out) {
    process_spawn_options local_options;
    memset(&local_options, 0, sizeof(local_options));
    local_options.inherit_stdin = true;
    local_options.inherit_stdout = true;
    local_options.inherit_stderr = true;
    if (options) {
        local_options = *options;
    }

    if (!argv) {
        char *const default_argv[] = {(char *)path, NULL};
        argv = default_argv;
    }

    pid_t pid = fork();
    if (pid < 0) {
        return -1;
    }
    if (pid == 0) {
        if (!local_options.inherit_stdin) {
            int devnull = open("/dev/null", O_RDONLY);
            if (devnull >= 0) {
                dup2(devnull, STDIN_FILENO);
                close(devnull);
            }
        }
        if (!local_options.inherit_stdout) {
            int devnull = open("/dev/null", O_WRONLY);
            if (devnull >= 0) {
                dup2(devnull, STDOUT_FILENO);
                close(devnull);
            }
        }
        if (!local_options.inherit_stderr) {
            int devnull = open("/dev/null", O_WRONLY);
            if (devnull >= 0) {
                dup2(devnull, STDERR_FILENO);
                close(devnull);
            }
        }
        if (local_options.working_dir && chdir(local_options.working_dir) != 0) {
            _exit(127);
        }
        if (envp) {
            execve(path, argv, envp);
        } else {
            execvp(path, argv);
        }
        _exit(127);
    }

    if (pid_out) {
        *pid_out = pid;
    }
    return 0;
}

int process_wait(pid_t pid, int *exit_status_out) {
    int status = 0;
    if (waitpid(pid, &status, 0) < 0) {
        return -1;
    }
    if (exit_status_out) {
        if (WIFEXITED(status)) {
            *exit_status_out = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            *exit_status_out = 128 + WTERMSIG(status);
        } else {
            *exit_status_out = -1;
        }
    }
    return 0;
}

int process_exec(const char *path,
                 char *const argv[],
                 char *const envp[],
                 const process_spawn_options *options,
                 bool capture_stdout,
                 bool capture_stderr,
                 char **stdout_data,
                 char **stderr_data,
                 int *exit_status_out) {
    process_spawn_options local_options;
    memset(&local_options, 0, sizeof(local_options));
    local_options.inherit_stdin = true;
    local_options.inherit_stdout = true;
    local_options.inherit_stderr = true;
    if (options) {
        local_options = *options;
    }

    if (!argv) {
        char *const default_argv[] = {(char *)path, NULL};
        argv = default_argv;
    }

    int stdout_pipe[2] = {-1, -1};
    int stderr_pipe[2] = {-1, -1};

    if (capture_stdout && pipe(stdout_pipe) != 0) {
        return -1;
    }
    if (capture_stderr && pipe(stderr_pipe) != 0) {
        if (stdout_pipe[0] >= 0) {
            close(stdout_pipe[0]);
        }
        if (stdout_pipe[1] >= 0) {
            close(stdout_pipe[1]);
        }
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        if (stdout_pipe[0] >= 0) {
            close(stdout_pipe[0]);
        }
        if (stdout_pipe[1] >= 0) {
            close(stdout_pipe[1]);
        }
        if (stderr_pipe[0] >= 0) {
            close(stderr_pipe[0]);
        }
        if (stderr_pipe[1] >= 0) {
            close(stderr_pipe[1]);
        }
        return -1;
    }

    if (pid == 0) {
        if (capture_stdout) {
            dup2(stdout_pipe[1], STDOUT_FILENO);
            close(stdout_pipe[0]);
            close(stdout_pipe[1]);
        } else if (!local_options.inherit_stdout) {
            int devnull = open("/dev/null", O_WRONLY);
            if (devnull >= 0) {
                dup2(devnull, STDOUT_FILENO);
                close(devnull);
            }
        }

        if (capture_stderr) {
            dup2(stderr_pipe[1], STDERR_FILENO);
            close(stderr_pipe[0]);
            close(stderr_pipe[1]);
        } else if (!local_options.inherit_stderr) {
            int devnull = open("/dev/null", O_WRONLY);
            if (devnull >= 0) {
                dup2(devnull, STDERR_FILENO);
                close(devnull);
            }
        }

        if (!local_options.inherit_stdin) {
            int devnull = open("/dev/null", O_RDONLY);
            if (devnull >= 0) {
                dup2(devnull, STDIN_FILENO);
                close(devnull);
            }
        }

        if (local_options.working_dir && chdir(local_options.working_dir) != 0) {
            _exit(127);
        }

        if (envp) {
            execve(path, argv, envp);
        } else {
            execvp(path, argv);
        }
        _exit(127);
    }

    if (capture_stdout) {
        close(stdout_pipe[1]);
    }
    if (capture_stderr) {
        close(stderr_pipe[1]);
    }

    if (read_from_pipes(capture_stdout ? stdout_pipe[0] : -1,
                        capture_stderr ? stderr_pipe[0] : -1,
                        stdout_data,
                        stderr_data) != 0) {
        if (capture_stdout && stdout_pipe[0] >= 0) {
            close(stdout_pipe[0]);
        }
        if (capture_stderr && stderr_pipe[0] >= 0) {
            close(stderr_pipe[0]);
        }
        waitpid(pid, NULL, 0);
        return -1;
    }

    if (capture_stdout && stdout_pipe[0] >= 0) {
        close(stdout_pipe[0]);
    }
    if (capture_stderr && stderr_pipe[0] >= 0) {
        close(stderr_pipe[0]);
    }

    int status = 0;
    if (process_wait(pid, &status) != 0) {
        return -1;
    }

    if (exit_status_out) {
        *exit_status_out = status;
    }
    return 0;
}

void print_command_output(const char *stdout_data, const char *stderr_data) {
    if (stdout_data && *stdout_data) {
        fputs(stdout_data, stdout);
    }
    if (stderr_data && *stderr_data) {
        fputs(stderr_data, stderr);
    }
}

int run_command_via_exec(const char *command, char **stdout_data, char **stderr_data, int *exit_status_out) {
    char *const argv[] = {"/bin/sh", "-c", (char *)command, NULL};
    return process_exec("/bin/sh", argv, NULL, NULL, true, true, stdout_data, stderr_data, exit_status_out);
}