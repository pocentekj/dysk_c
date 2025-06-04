#include "colors.h"
#include "str_utils.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>

/** DATA */

static const char *fname = "/proc/mounts";

struct m_entry {
  char *device;
  char *mount_point;
  char *fs_type;
  char *options;
  int dump;
  int pass;
};

struct vfs_info {
  char *total;
  char *avail;
  char *used;
  int used_pcrt;
};

typedef enum { PARSE_OK = 0, PARSE_ERR_ALLOC, PARSE_ERR_FORMAT, PARSE_ERR_STATVFS } ParseResult;

/** HELPERS */

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

void print_border() {
  size_t cols[] = {32, 13, 12, 17, 13};
  putchar('|');
  for (size_t i = 0; i < sizeof(cols) / sizeof(size_t); i++) {
    for (; cols[i] > 0; cols[i]--)
      putchar('-');
    putchar('|');
  }
  putchar('\n');
}

/** UTILS */

void free_entry(struct m_entry *ptr) {
  free(ptr->device);
  free(ptr->mount_point);
  free(ptr->fs_type);
  free(ptr->options);
  free(ptr);
}

void free_vfs_info(struct vfs_info *ptr) {
  free(ptr->total);
  free(ptr->avail);
  free(ptr->used);
  free(ptr);
}

struct vfs_info *vfs_info_new(int64_t total, int64_t avail, int64_t used) {
  struct vfs_info *ptr = (struct vfs_info *)malloc(sizeof(struct vfs_info));
  if (!ptr)
    return NULL;
  ptr->used_pcrt = (int)(((double)used / (double)total) * 100.0);
  ptr->total = ptr->avail = ptr->used = NULL;

  char buf[BUFSIZ];
  if (human_size(buf, BUFSIZ, total))
    goto fail;
  ptr->total = strdup(buf);
  if (human_size(buf, BUFSIZ, avail))
    goto fail;
  ptr->avail = strdup(buf);
  if (human_size(buf, BUFSIZ, used))
    goto fail;
  ptr->used = strdup(buf);

  return ptr;

fail:
  free_vfs_info(ptr);
  return NULL;
}

/** PARSERS */

struct m_entry *parse_line(char *line) {
  struct m_entry *entry = malloc(sizeof(struct m_entry));
  if (!entry)
    return NULL;
  entry->device = entry->mount_point = entry->fs_type = entry->options = NULL;
  entry->dump = entry->pass = 0;

  char *fields[6] = {0};
  int field = 0;
  char *start = line;
  char *p = line;
  while (*p && field < 5) {
    if (*p == ' ') {
      *p = '\0';
      fields[field++] = start;
      start = p + 1;
    }
    p++;
  }
  fields[field++] = start; // last field

  if (field != 6) {
    free(entry);
    return NULL;
  }

  // Decode escapes in string fields
  for (int i = 0; i < 4; i++)
    decode_escapes(fields[i]);
  entry->device = strdup(fields[0]);
  entry->mount_point = strdup(fields[1]);
  entry->fs_type = strdup(fields[2]);
  entry->options = strdup(fields[3]);
  entry->dump = atoi(fields[4]);
  entry->pass = atoi(fields[5]);
  return entry;
}

struct vfs_info *parse_statvfs(const char *path) {
  struct statvfs vfs;
  if (statvfs(path, &vfs))
    return NULL;

  int64_t total = (int64_t)((int64_t)vfs.f_frsize * (int64_t)vfs.f_blocks);
  int64_t avail = (int64_t)((int64_t)vfs.f_frsize * (int64_t)vfs.f_bavail);
  int64_t used = total - avail;

  if (total <= 0) {
    fprintf(stderr, "Invalid total size reported.\n");
    return NULL;
  }

  struct vfs_info *ptr = vfs_info_new(total, avail, used);
  if (!ptr)
    return NULL;
  return ptr;
}

/** OUTPUT */

void print_entry(const struct m_entry *mp, const struct vfs_info *fs) {
  char *abbr_device = truncate(mp->device, 30);
  printf("| %-30s ", abbr_device);
  printf("| %-11s | ", mp->fs_type);
  set_fg_color(YELLOW);
  printf("%-11s", fs->total);
  reset_colors();
  printf("| %3d%% ", fs->used_pcrt);
  print_progress_bar(fs->used_pcrt);
  printf(" | ");
  set_fg_color(GREEN);
  printf("%-11s", fs->avail);
  reset_colors();
  printf(" |");
  free(abbr_device);
}

ParseResult display_entry_info(const char *input) {
  char *cp = strdup(input);
  if (!cp) {
    return PARSE_ERR_ALLOC;
  }

  struct m_entry *mp = parse_line(cp);
  if (!mp) {
    free(cp);
    return PARSE_ERR_FORMAT;
  }

  struct vfs_info *fs = parse_statvfs(mp->mount_point);
  if (!fs) {
    free(cp);
    free_entry(mp);
    return PARSE_ERR_STATVFS;
  }

  print_entry(mp, fs);

  free(cp);
  free_entry(mp);
  free_vfs_info(fs);

  printf("\n");
  return PARSE_OK;
}

/** INIT */

int main(int argc, char *argv[]) {
  FILE *fp = fopen(fname, "r");
  if (!fp) {
    perror("Failed to open file");
    return EXIT_FAILURE;
  }

  // Table header
  print_border();
  printf("| %-30s | File system | %-10s | %-15s | %-11s |\n", "Device", "Total", "Used", "Available");
  print_border();
  // Table body
  char buf[BUFSIZ];
  while (fgets(buf, BUFSIZ, fp)) {
    if (!startswith(buf, "/dev/"))
      continue;
    if (display_entry_info(buf))
      fprintf(stderr, "parser failure\n"); // TODO: make it more verbose.
  }
  // Table footer
  print_border();

  fclose(fp);
  return EXIT_SUCCESS;
}

