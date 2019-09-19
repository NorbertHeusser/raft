/* Byte-level utilities. */

#ifndef BYTE_H_
#define BYTE_H_

#include <stdint.h>
#include <string.h>
#include <unistd.h>

#if defined(__cplusplus)
#    define RAFT__INLINE__ inline
#else
#    if defined(__clang__)
#        define RAFT__INLINE__ static inline __attribute__((unused))
#    else
#        define RAFT__INLINE__ static inline
#    endif
#endif

/* Compile-time endianess detection (best effort). */
#if (defined(__BYTE_ORDER) && (__BYTE_ORDER == __LITTLE_ENDIAN)) || \
    (defined(__ARMEL__) && (__ARMEL__ == 1))
#    define RAFT__LITTLE_ENDIAN__
#elif defined(__BYTE_ORDER) && (__BYTE_ORDER == __BIG_ENDIAN) && \
    defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 8
#    define RAFT__BIG_ENDIAN__
#endif

/* Flip a 32-bit number to network byte order (little endian) */
RAFT__INLINE__ uint32_t byteFlip32(uint32_t v)
{
#if defined(RAFT__LITTLE_ENDIAN__)
    return v;
#elif defined(RAFT__BIG_ENDIAN__)
    return __builtin_bswap32(v);
#else /* Unknown endianess */
    union {
        uint32_t u;
        uint8_t v[4];
    } s;

    s.v[0] = (uint8_t)v;
    s.v[1] = (uint8_t)(v >> 8);
    s.v[2] = (uint8_t)(v >> 16);
    s.v[3] = (uint8_t)(v >> 24);

    return s.u;
#endif
}

/* Flip a 64-bit number to network byte order (little endian) */
RAFT__INLINE__ uint64_t byteFlip64(uint64_t v)
{
#if defined(RAFT__LITTLE_ENDIAN__)
    return v;
#elif defined(RAFT__BIG_ENDIAN__)
    return __builtin_bswap64(v);
#else
    union {
        uint64_t u;
        uint8_t v[8];
    } s;

    s.v[0] = (uint8_t)v;
    s.v[1] = (uint8_t)(v >> 8);
    s.v[2] = (uint8_t)(v >> 16);
    s.v[3] = (uint8_t)(v >> 24);
    s.v[4] = (uint8_t)(v >> 32);
    s.v[5] = (uint8_t)(v >> 40);
    s.v[6] = (uint8_t)(v >> 48);
    s.v[7] = (uint8_t)(v >> 56);

    return s.u;
#endif
}

RAFT__INLINE__ void bytePut8(void **cursor, uint8_t value)
{
    uint8_t **p = (uint8_t **)cursor;
    **p = value;
    *p += 1;
}

RAFT__INLINE__ void bytePut32(void **cursor, uint32_t value)
{
    uint32_t **p = (uint32_t **)cursor;
    **p = byteFlip32(value);
    *p += 1;
}

RAFT__INLINE__ void bytePut64(void **cursor, uint64_t value)
{
    uint64_t **p = (uint64_t **)cursor;
    **p = byteFlip64(value);
    *p += 1;
}

RAFT__INLINE__ void bytePut64Unaligned(void **cursor, uint64_t value)
{
    unsigned i;
    uint64_t flipped = byteFlip64(value);
    for (i = 0; i < sizeof(uint64_t); i++) {
        bytePut8(cursor, ((uint8_t *)(&flipped))[i]);
    }
}

RAFT__INLINE__ void bytePutString(void **cursor, const char *value)
{
    char **p = (char **)cursor;
    strcpy(*p, value);
    *p += strlen(value) + 1;
}

RAFT__INLINE__ uint8_t byteGet8(const void **cursor)
{
    const uint8_t **p = (const uint8_t **)cursor;
    uint8_t value = **p;
    *p += 1;
    return value;
}

RAFT__INLINE__ uint32_t byteGet32(const void **cursor)
{
    const uint32_t **p = (const uint32_t **)cursor;
    uint32_t value = byteFlip32(**p);
    *p += 1;
    return value;
}

RAFT__INLINE__ uint64_t byteGet64(const void **cursor)
{
    const uint64_t **p = (const uint64_t **)cursor;
    uint64_t value = byteFlip64(**p);
    *p += 1;
    return value;
}

RAFT__INLINE__ uint64_t byteGet64Unaligned(const void **cursor)
{
    uint64_t value = 0;
    unsigned i;
    for (i = 0; i < sizeof(uint64_t); i++) {
        ((uint8_t *)(&value))[i] = byteGet8(cursor);
    }
    return byteFlip64(value);
}

RAFT__INLINE__ const char *byteGetString(const void **cursor, size_t max_len)
{
    const char **p = (const char **)cursor;
    const char *value = *p;
    size_t len = 0;
    while (len < max_len) {
        if (*(*p + len) == 0) {
            break;
        }
        len++;
    }
    if (len == max_len) {
        return NULL;
    }
    *p += len + 1;
    return value;
}

/* Add padding to size if it's not a multiple of 8. */
RAFT__INLINE__ size_t bytePad64(size_t size)
{
    size_t rest = size % sizeof(uint64_t);

    if (rest != 0) {
        size += sizeof(uint64_t) - rest;
    }

    return size;
}

/* Calculate the CRC32 checksum of the given data buffer. */
unsigned byteCrc32(const void *buf, size_t size, unsigned init);

#undef RAFT__INLINE__
#undef RAFT__LITTLE_ENDIAN__
#undef RAFT__BIG_ENDIAN__

#endif /* BYTE_H_ */
