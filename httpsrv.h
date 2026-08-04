/* compat: for unix-like and windows */

#ifndef HTTPSRV_H
#define HTTPSRV_H

/* simple HTTP server. Serves argv[1], or "index.html"; port is argv[2] or 80 */
int http_server(const char *file_to_serve_path, int port);

#endif