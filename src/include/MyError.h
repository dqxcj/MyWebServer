#ifndef SRC_INCLUDE_MYERROR_H_
#define SRC_INCLUDE_MYERROR_H_
#include <stdio.h>
#include <stdlib.h>

inline void ErrIf(bool condition, const char *errmsg) {
  if (condition) {
    perror(errmsg);
    exit(EXIT_FAILURE);
  }
}

#endif  // SRC_INCLUDE_MYERROR_H_
