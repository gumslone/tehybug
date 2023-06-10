#pragma once

// find in string
uint8_t strContains(const char *string, char *toFind) {
  uint8_t slen = strlen(string);
  uint8_t tFlen = strlen(toFind);
  uint8_t found = 0;

  if (slen >= tFlen) {
    for (uint8_t s = 0, t = 0; s < slen; s++) {
      do {

        if (string[s] == toFind[t]) {
          if (++found == tFlen)
            return 1;
          s++;
          t++;
        } else {
          s -= found;
          found = 0;
          t = 0;
        }

      } while (found);
    }
    return 0;
  } else
    return -1;
}
