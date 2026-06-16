# Common utils

these are simple functions that are likely to be reused across different programs or stuff - stuff i'd expect to find in the standard library of a modern higher-level
language. Small, meant to just be compiled in

## compatibility:
some of these are "pure C" meaning they will (or at least, should) run anywhere C can, within reason. Others are for unix-like systems only. This is specified with the comment in the first line of all them.

includes:
### elevate
- `const char* determine_elevator()` - determines elevator (`sudo` or `doas`)
- `const char *elevate_command(const char *command)` - elevates the command

### have
- `int command_exists(const char *command)` - `0` if it doesn't, `1` if it does
- `const char *which(const char *command)` - you should know what which is

### simple_strings
A very simple strings implementation for C. (not professional do not use for anything load-bearing). The struct struct looks like this:
```c
typedef struct String {
    char *data;
    size_t len;
    size_t cap;
} string;
#define str string
```
Functions:
- `str str_create(const char *src)` - create a new string from a C string
- `str str_with_cap(size_t cap)` - same but with initial capacity
- `void str_destroy(str *s)` - free the string's allocated memory
- `const char *cstr(const str *s)` - simply get .data (turn back into a normal C string):
```c
#define to_cstr(s) cstr(s) /* alias for cstr() */
```
- `str *str_append(str *s, const char *suffix)` - append a C string to the string. returns pointer for chaining
- `str *str_prepend(str *s, const char *prefix)` - prepend a C string to the sstring. Returns pointer for chaining
- `str *str_trim(str *s)` - trims off leading and trailing whitespace
- `str *str_ltrim(str *s)` - trims off the left (or leading) whitespace (just a pointer to the desired location)
- `str *str_rtrim(str *s)` - trims off the right (or trailing) whitespace (returns the string)
- `str *to_upper(str *s)` - uppercases the string, returns pointer
- `str *to_lower(str *s)` - same thing, lowercasing
- `bool str_ends_with(const str *s, const char *suffix)`
- `bool str_starts_with(const str *s, const char *prefix)`
- `const char *str_find(const str *s, const char *substr)`
- `str str_quoted_substring(const char *src)` - this is a weird one: it finds a quoted substring. for instance: input is `eurgn"josdSC"IUVIRV`, then output is `josdSC`
- `str str_join(const char *sep, const char **strs, size_t count)` - join array of C strings with separator
- `str str_repeat(const char *src, int n)` - repeat a number `n` of times
- `str str_replace(const char *src, const char *old, const char *new_str)` - like `tr`
- `str str_remove_prefix(const char *src, const char *prefix)` - a sort of --exec for `str_starts_with`, you know what I mean?
- `str str_remove_suffix(const char *src, const char *suffix)` - same as above but suffix

### which_os
- `const char *try_to_determine_linux_distro(const bool like)` - tries to determine the linux distro, base on `lsb_release` and `/etc/os-release`. If bool `like` is provided, also get ID_LIKE from /etc/os-release. Returns const char *, `"linux"` if it failed to determine the distro.

### process_utils
- `int run(const char *cmd)` - old function, just runs a thing
- `char *capture_output(const char *cmd)` - runs a thing and gets the output
- `int run_quiet(const char *cmd)` - runs a thing and explicitly discards the output
- `bool is_process_running(pid_t pid)` - shorthand to check if the is running
- `pid_t pidof(const char *name)` - pidof without shelling out

### path_utils
- `str expand_home(const char *path)` - expands a path string with `~` in it
- `str path_join(const char *a, const char *b)` - joins two paths
- `str canonical_path(const char *path)` - get the actual, real path
- `str file_extension(const char *path)` - get the file extension of the given file. Not for hidden files or files wit no extension.
- `str tidy_up_path(const char *path)` - clean up the path (remove duplicated `/`, only leave the final `/` if the last item is actually a directory)

### file_utils
- `bool file_exists(const char *path)`
- `bool dir_exists(const char *path)`
- `bool is_symlink(const char *path)`
- `size_t file_size(const char *path)` in bytes
- `str read_entire_file(const char *path)`
- `void write_entire_file(const char *path, const char *contents)`
- `int file_line_count(const char *path)`
- `bool file_is_lines_long(const char *path, int line_count, const char *least_or_most)` - the third argument can be `"least"` to check if the file is at ~ _`line_count`_ lines long, `"most"` ~~, and `NULL` to check if the file is *exactly* _`line_count`_ lines long
- `str read_lines(const char *path, int line_count, bool skip_empty, bool trim_newline)`

### httpsrv
- `int http_server(const char *file_to_serve_path)` - serves a file. defaults to "index.html" if no file is specified. (if defined _STANDALONE_HTTPSRV, function is int main(int argc, char **argv))