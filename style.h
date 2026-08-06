#ifndef SIMPLE_STYLE_H
#define SIMPLE_STYLE_H

/* ANSI Escape Sequence */
#define ESC "\x1b["

/* Reset */
#define RESET ESC "0m"

/* Text Styles */
#define BOLD          ESC "1m"
#define DIM           ESC "2m"
#define ITALIC        ESC "3m"
#define UNDERLINE     ESC "4m"
#define BLINK         ESC "5m"
#define REVERSE       ESC "7m"
#define HIDDEN        ESC "8m"
#define STRIKETHROUGH ESC "9m"

/* Cursor Control */
#define CURSOR_HOME    ESC "H"
#define CLEAR_SCREEN   ESC "2J"
#define CLEAR_LINE     ESC "2K"
#define SAVE_CURSOR    ESC "s"
#define RESTORE_CURSOR ESC "u"

/* Convenience */
#define COLOR_RESET RESET

#endif /* SIMPLE_STYLE_H */
