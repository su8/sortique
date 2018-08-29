/*
   08/29/2018

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.
*/
#define _DEFAULT_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static int compare(const void *, const void *);
static int compare(const void *a, const void *b) {
	const char *const *const x = a;
	const char *const *const z = b;
	return strcmp(*x, *z);
}

int main(int argc, char *argv[]) {
  FILE *fp = NULL;
  char *buf = NULL;
  char *tok = NULL;
  char *ptrtofree = buf;
  char **arr = NULL;
  size_t len = 0;
  size_t fz = 0;
  size_t x = 0;
  int fd = 0;
  off_t file_size = 0;
  struct stat st;

  if (1 == argc) {
    puts("usage: sortique FILE");
    return EXIT_FAILURE;
  }

  if (-1 == (fd = open(argv[1], O_RDONLY))) {
    puts("open(argv[1], O_RDONLY) failed");
    return EXIT_FAILURE;
  }
  if (NULL == (fp = fdopen(fd, "r"))) {
    puts("fdopen(fd, \"r\") failed");
    goto err;
  }

  if (0 != (fstat(fd, &st)) || (!S_ISREG(st.st_mode))) {
    puts("The program operates only on filename(s)");
    goto err;
  }
  if (0 != (fseeko(fp, 0, SEEK_END))) {
    puts("fseeko(fp, 0, SEEK_END) failed");
    goto err;
  }
  if (-1 == (file_size = ftello(fp))) {
    puts("ftello(fp) failed");
    goto err;
  }
  rewind(fp);

  fz = (size_t)file_size;
  buf = (char *)malloc(fz);
  if (NULL == buf) {
    puts("malloc(fz) failed");
    goto err;
  }
  if (0 == (fread(buf, 1, fz, fp))) {
    puts("fread(buf, 1, fz, fp) failed");
    goto err;
  }

  arr = (char **)malloc(fz);
  if (NULL == arr) {
    puts("malloc(fz) failed");
    goto err;
  }

  while ((tok = strsep(&buf, "\n"))) {
    arr[len++] = tok;
  }
  qsort(arr, len, sizeof(char *), compare);

  puts(arr[0]);
  for (x = 1; x < len; x++) {
    if (0 == (strcmp(arr[x], ""))) {
      continue;
    }
    if (0 != (strcmp(arr[x], arr[x - 1]))) {
      puts(arr[x]);
    }
  }

err:
  close(fd);
  if (NULL != fp) {
    fclose(fp);
  }
  if (NULL != ptrtofree) {
    free(ptrtofree);
  }
  if (arr) {
    free(arr);
  }
  return EXIT_SUCCESS;
}
