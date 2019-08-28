#ifndef UTIL_H
#define UTIL_H

#include <iostream>

inline char *loadFile(const char *filename) {
  FILE *f;
  fopen_s(&f, filename, "r");

  if (!f)
    return nullptr;

  fseek(f, 0, SEEK_END);
  const size_t len = static_cast<size_t>(ftell(f));
  fseek(f, 0, SEEK_SET);

  char *str = static_cast<char *>(malloc((len + 1) * sizeof(char)));

  if (!str) {
    fclose(f);
    return nullptr;
  }

  size_t read = 0;
  if (len > 0)
    read = fread(str, sizeof(char), len, f);

  str[read] = '\0';

  fclose(f);

  return str;
}

inline float lerp(float a, float b, float f) { return a + f * (b - a); }
#endif // UTIL_H
