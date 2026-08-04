# Common utils

These helpers are small, reusable building blocks intended to feel like a lightweight standard library for C projects. They are meant to be compiled directly into the consuming application.

## Compatibility

Some modules are pure C and should work wherever C can compile. Others are Unix-like only and are marked as such in their headers.

## Public modules

### simple_strings
A very simple strings implementation for C. (not professional do not use for anything load-bearing). The type looks like this:
```c
typedef struct String {
    char *data;
    size_t len;
    size_t cap;
} string;
#define str string
```

Functions include:
- `str str_create(const char *src)`
- `str str_with_cap(size_t cap)`
- `void str_destroy(str *s)`
- `const char *cstr(const str *s)`
- `str *str_append(str *s, const char *suffix)`
- `str *str_prepend(str *s, const char *prefix)`
- `str *str_trim(str *s)`
- `str *str_ltrim(str *s)`
- `str *str_rtrim(str *s)`
- `str *str_chomp(str *s)`
- `str *str_chomp_all(str *s)`
- `str *str_chop(str *s)`
- `str *str_chop_n(str *s, size_t n)`
- `str *to_upper(str *s)`
- `str *to_lower(str *s)`
- `bool str_ends_with(const str *s, const char *suffix)`
- `bool str_starts_with(const str *s, const char *prefix)`
- `const char *str_find(const str *s, const char *substr)`
- `str str_quoted_substring(const char *src)`
- `str str_join(const char *sep, const char **strs, size_t count)`
- `str str_join_strs(const char *sep, const char strings[], size_t count)`
- `str str_repeat(const char *src, int n)`
- `str str_replace(const char *src, const char *old, const char *new_str)`
- `str str_remove_prefix(const char *src, const char *prefix)`
- `str str_remove_suffix(const char *src, const char *suffix)`
- `bool str_eq(const str *a, const str *b)`
- `int str_cmp(const str *a, const str *b)`
- `void str_clear(str *s)`
- `void str_reserve(str *s, size_t cap)`
- `str str_substr(const str *s, size_t start, size_t len)`
- `str *str_insert(str *s, size_t pos, const char *substr)`
- `str *str_erase(str *s, size_t pos, size_t len)`
- `str str_clone(const str *s)`
- `str *str_split(const str *s, const char *delim, size_t *out_count)`

### args
- `bool arg_is_present(const char *arg, int argc, char **argv)`
- `bool arg_is_present_with_value(const char *arg, const char *value, int argc, char **argv)`
- `bool arg_is_present_at_nth_position(const char *arg, int n, int argc, char **argv)`
- `bool arg_is_in_list(const char *arg, const char *list)`
- `str get_arg_value(const char *arg, int argc, char **argv)`
- `bool config_arg_is_val(const char *key, const char *val, int argc, char **argv)`
- `str all_args_to_string(int argc, char **argv, char sep, bool skip_argv_0)`

### elevate
- `const char *determine_elevator(void)`
- `const char *elevate_command(const char *command)`

### file_utils
- `bool file_exists(const char *path)`
- `bool dir_exists(const char *path)`
- `bool is_symlink(const char *path)`
- `size_t file_size(const char *path)`
- `str read_entire_file(const char *path)`
- `void write_entire_file(const char *path, const char *contents)`
- `int file_line_count(const char *path)`
- `bool file_is_lines_long(const char *path, int line_count, const char *least_or_most)`
- `str read_lines(const char *path, int line_count, bool skip_empty, bool trim_newline)`

### have
- `int command_exists(const char *command)`
- `const char *which(const char *command)`

### parsing_utils
- `int parse_int(const char *s)`
- `long parse_long(const char *s)`
- `bool parse_bool(const char *s)`
- `bool is_numeric(const char *s)`

### path_utils
- `str expand_home(const char *path)`
- `str path_join(const char *a, const char *b)`
- `str canonical_path(const char *path)`
- `str file_extension(const char *path)`
- `str tidy_up_path(const char *path)`

### process_utils
- `int run(const char *cmd)`
- `char *capture_output(const char *cmd)`
- `int run_quiet(const char *cmd)`
- `bool is_process_running(pid_t pid)`
- `pid_t pidof(const char *name)`
- `int process_spawn(const char *path, char *const argv[], char *const envp[], const process_spawn_options *options, pid_t *pid_out)`
- `int process_wait(pid_t pid, int *exit_status_out)`
- `int process_exec(const char *path, char *const argv[], char *const envp[], const process_spawn_options *options, bool capture_stdout, bool capture_stderr, char **stdout_data, char **stderr_data, int *exit_status_out)`

### unit_utils
- `double meter_to_kilometer(double meters)`
- `double kilometer_to_meter(double kilometers)`
- `double meter_to_foot(double meters)`
- `double foot_to_meter(double feet)`
- `double kilometer_to_mile(double kilometers)`
- `double mile_to_kilometer(double miles)`
- `double meter_to_mile(double meters)`
- `double mile_to_meter(double miles)`
- `double kilometer_to_foot(double kilometers)`
- `double square_meter_to_square_foot(double m2)`
- `double square_foot_to_square_meter(double ft2)`
- `double acre_to_square_meter(double acres)`
- `double square_meter_to_acre(double m2)`
- `double kilogram_to_gram(double kg)`
- `double gram_to_kilogram(double g)`
- `double kilogram_to_pound(double kg)`
- `double pound_to_kilogram(double lb)`
- `double celsius_to_fahrenheit(double c)`
- `double fahrenheit_to_celsius(double f)`
- `double celsius_to_kelvin(double c)`
- `double kelvin_to_celsius(double k)`
- `double fahrenheit_to_kelvin(double f)`
- `double kelvin_to_fahrenheit(double k)`
- `double liter_to_cubic_meter(double liters)`
- `double cubic_meter_to_liter(double m3)`
- `double liter_to_gallon(double liters)`
- `double gallon_to_liter(double gallons)`
- `double mps_to_kmph(double mps)`
- `double kmph_to_mps(double kmph)`
- `double mps_to_mph(double mps)`
- `double mph_to_mps(double mph)`
- `double kmph_to_mph(double kmph)`
- `double mph_to_kmph(double mph)`
- `double pascal_to_psi(double pa)`
- `double psi_to_pascal(double psi)`
- `double pascal_to_bar(double pa)`
- `double bar_to_pascal(double bar)`
- `double pascal_to_atm(double pa)`
- `double atm_to_pascal(double atm)`
- `double joule_to_calorie(double j)`
- `double calorie_to_joule(double cal)`
- `double joule_to_kwh(double j)`
- `double kwh_to_joule(double kwh)`
- `double watt_to_horsepower(double w)`
- `double horsepower_to_watt(double hp)`
- `double degree_to_radian(double deg)`
- `double radian_to_degree(double rad)`
- `double byte_to_kibibyte(double bytes)`
- `double kibibyte_to_byte(double kib)`
- `double byte_to_kilobyte(double bytes)`
- `double kilobyte_to_byte(double kb)`

### which_os
- `const char *try_to_determine_linux_distro(const bool like)`
- `const char *try_to_determine_windows_version(void)`

### httpsrv
- `int http_server(const char *file_to_serve_path, int port)`