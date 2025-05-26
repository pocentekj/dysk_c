#ifndef COLORS_H
#define COLORS_H

#include <stddef.h>

// clang-format off
typedef enum {
  BLACK = 30,
  RED,
  GREEN,
  YELLOW,
  BLUE,
  MAGENTA,
  CYAN,
  WHITE
} Color;
// clang-format on

struct color_entry {
  Color code;
  const char *name;
  const char *hex;
};

extern struct color_entry colors[];
extern const size_t colors_count;

void set_bg_color(const Color color);
void set_fg_color(const Color color);
void set_colors(const Color fg, const Color bg);
void reset_colors();

#endif
