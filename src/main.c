#include "colors.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>

int human_size(char *buffer, size_t size, int64_t bytes) {
  const char *units[] = {"Bytes", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB"};
  uint8_t unit_index = 0;
  double value = (double)bytes;
  uint8_t max_units = sizeof(units) / sizeof(units[0]);

  while (value >= 1024.0 && unit_index < max_units - 1) {
    value /= 1024.0;
    unit_index++;
  }

  // Strip decimals if unnecessary
  if (floor(value) == value) {
    if (snprintf(buffer, size, "%.0f %s", value, units[unit_index]) >= size)
      return 1;
  } else {
    if (snprintf(buffer, size, "%.1f %s", value, units[unit_index]) >= size)
      return 1;
  }

  return 0;
}

void print_progress_bar(const unsigned percentage) {
  int fc = percentage / 10;

  // Ignore errors
  if (fc > 10)
    fc = 0;

  set_bg_color(RED);
  for (size_t i = 0; i < fc; i++)
    printf(" ");
  set_bg_color(GREEN);
  for (size_t i = 0; i < 10 - fc; i++)
    printf(" ");
  reset_colors();
}

void print_statvfs_entry(const struct statvfs *vfs) {
  int64_t total = (int64_t)((int64_t)vfs->f_frsize * (int64_t)vfs->f_blocks);
  int64_t avail = (int64_t)((int64_t)vfs->f_frsize * (int64_t)vfs->f_bavail);
  int64_t used = total - avail;

  if (total <= 0) {
    fprintf(stderr, "Invalid total size reported.\n");
    return;
  }

  int64_t stats[] = {total, avail, used};
  int used_pcrt = (int)(((double)used / (double)total) * 100.0);

  size_t bufsize = 512;
  char buffer[512];
  for (size_t i = 0; i < 3; i++) {
    if (human_size(buffer, bufsize, stats[i])) {
      fprintf(stderr, "Failed to convert %ld into human-readable format\n", stats[i]);
      exit(EXIT_FAILURE);
    }
    printf("%s %s |", (i == 0) ? "|" : "", buffer);
  }
  printf(" %3d%% ", used_pcrt);
  print_progress_bar(used_pcrt);
  printf(" |\n");
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "USAGE: %s <path>\n", argv[0]);
    return EXIT_FAILURE;
  }

  struct statvfs vfs;
  if (statvfs(argv[1], &vfs)) {
    perror("Failed to get usage info");
    return EXIT_FAILURE;
  }
  print_statvfs_entry(&vfs);

  return EXIT_SUCCESS;
}
