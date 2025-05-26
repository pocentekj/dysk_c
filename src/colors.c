#include "colors.h"
#include <stdio.h>

// clang-format off
struct color_entry colors[] = {
  { .code=BLACK,   .name="black",   .hex="000000" },
  { .code=RED,     .name="red",     .hex="FF0000" },
  { .code=GREEN,   .name="green",   .hex="00FF00" },
  { .code=YELLOW,  .name="yellow",  .hex="FFFF00" },
  { .code=BLUE,    .name="blue",    .hex="0000FF" },
  { .code=MAGENTA, .name="magenta", .hex="FF00FF" },
  { .code=CYAN,    .name="cyan",    .hex="00FFFF" },
  { .code=WHITE,   .name="white",   .hex="FFFFFF" }
};
// clang-format on

const size_t colors_count = sizeof(colors) / sizeof(struct color_entry);

void set_bg_color(const Color color) { printf("\033[%dm", color + 10); }

void set_fg_color(const Color color) { printf("\033[%dm", color); }

void set_colors(const Color fg, const Color bg) { printf("\033[%d;%dm", fg, bg + 10); }

void reset_colors() { printf("\033[0m"); }
