/* compat: for unix-like systems only (note: for now only tested on linux) */

#ifndef TEXTURAWASD_PROCESS_UTILS
#define TEXTURAWASD_PROCESS_UTILS

#include <stdbool.h>
#include <sys/types.h>

/* old function that just runs a thing, not saving the output */
int run(const char *cmd);

/* note: free it, will you? */
char *capture_output(const char *cmd);

/* runs a thing and explicitly throws out the output*/
int run_quiet(const char *cmd);

/* shorthand to check if its running */
bool is_process_running(pid_t pid);

/* pidof without shelling out */
pid_t pidof(const char *name);

/* robust fork/exec helpers */
typedef struct {
    const char *working_dir;
    char *const *envp;
    bool inherit_stdin;
    bool inherit_stdout;
    bool inherit_stderr;
} process_spawn_options;

int process_spawn(const char *path,
                  char *const argv[],
                  char *const envp[],
                  const process_spawn_options *options,
                  pid_t *pid_out);

int process_wait(pid_t pid, int *exit_status_out);

int process_exec(const char *path,
                 char *const argv[],
                 char *const envp[],
                 const process_spawn_options *options,
                 bool capture_stdout,
                 bool capture_stderr,
                 char **stdout_data,
                 char **stderr_data,
                 int *exit_status_out);

#endif /* TEXTURAWASD_PROCESS_UTILS */