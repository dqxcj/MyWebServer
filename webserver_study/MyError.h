#ifndef MYERROR_H
#define MYERROR_H
#include <stdio.h>
#include <stdlib.h>

inline void ErrIf(bool condition, const char *errmsg) {
    if(condition) {
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}

#endif