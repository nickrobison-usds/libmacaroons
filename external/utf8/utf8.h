//
// Created by Nick Robison on 2/25/21.
//

#ifndef LIBMACAROONS_UTF8_H
#define LIBMACAROONS_UTF8_H

/* c */
#include <stdbool.h>

int utf8_range(const unsigned char *data, int len);
int utf8_naive(const unsigned char* data, int len);

bool validate_utf8(const unsigned char *data, size_t len) {
#ifdef __AVX2__
    return utf8_range_avx2(data, len) == 0;
#elif defined USE_NAIVE
    return utf8_naive(data, len) == 0;
#else
    return utf8_range(data, len) == 0;
#endif
}

#endif //LIBMACAROONS_UTF8_H
