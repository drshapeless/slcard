#include "Utilities.h"
#include "Config.h"
#include <stdlib.h>
#include <string.h>

void toggle(int *i) {
  if (*i)
    *i = 0;
  else
    *i = 1;
}

char *fgetline(FILE *f) {
  /* This is a custom getline function. */
  int c;
  char line[STRING_SIZE];
  size_t pos = 0;
  while ((c = fgetc(f)) != EOF || pos == STRING_SIZE - 1) {
    if (c != '\n')
      line[pos++] = c;
    else
      break;
  }
  line[pos] = '\0';

  if (c == EOF)
    return NULL;
  else
    return strdup(line);
}
