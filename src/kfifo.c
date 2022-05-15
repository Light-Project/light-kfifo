/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include "kfifo.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define KFIFO_GENERIC_COPY(copy1, copy2, fold1, fold2) do { \
    unsigned long size = kfifo->mask + 1;                   \
    unsigned long esize = kfifo->esize;                     \
    unsigned long llen;                                     \
                                                            \
    offset &= kfifo->mask;                                  \
    if (esize != 1) {                                       \
        offset *= esize;                                    \
        size *= esize;                                      \
        len *= esize;                                       \
    }                                                       \
                                                            \
    llen = min(len, size - offset);                         \
    memcpy(copy1, copy2, llen);                             \
    memcpy(fold1, fold2, len - llen);                       \
} while (0)

static __always_inline void
kfifo_out_copy(struct kfifo *kfifo, void *buff, unsigned long len, unsigned long offset)
{
    KFIFO_GENERIC_COPY(
        buff, kfifo->data + offset,
        buff + llen, kfifo->data
    );
}

static __always_inline void
kfifo_in_copy(struct kfifo *kfifo, const void *buff, unsigned long len, unsigned long offset)
{
    KFIFO_GENERIC_COPY(
        kfifo->data + offset, buff,
        kfifo->data, buff + llen
    );
}

static __always_inline unsigned long
kfifo_out_peek(struct kfifo *kfifo, unsigned long recsize)
{
    unsigned long mask = kfifo->mask;
    unsigned long offset = kfifo->out;
    unsigned long length = 0;
    uint8_t *data = kfifo->data;

    if (kfifo->esize != 1) {
        offset *= kfifo->esize;
        mask *= kfifo->esize;
        mask += kfifo->esize - 1;
    }

    while (recsize--) {
        length <<= 8;
        length |= data[offset & mask];
        offset += kfifo->esize;
    }

    return length;
}

static __always_inline void
kfifo_in_poke(struct kfifo *kfifo, unsigned long len, unsigned long recsize)
{
    unsigned long mask = kfifo->mask;
    unsigned long offset = kfifo->out;
    uint8_t *data = kfifo->data;

    if (kfifo->esize != 1) {
        offset *= kfifo->esize;
        mask *= kfifo->esize;
        mask += kfifo->esize - 1;
    }

    while (recsize--) {
        data[offset & mask] = (uint8_t)len;
        offset += kfifo->esize;
        len >>= 8;
    }
}

static inline bool kfifo_empty(struct kfifo *kfifo)
{
    return kfifo->in == kfifo->out;
}

static inline unsigned long kfifo_valid(struct kfifo *kfifo)
{
    return kfifo->in - kfifo->out;
}

static inline unsigned long kfifo_unused(struct kfifo *kfifo)
{
    return (kfifo->mask + 1) - (kfifo->in - kfifo->out);
}

unsigned long kfifo_peek_flat(struct kfifo *kfifo, void *buff, unsigned long len)
{
    unsigned long valid;

    valid = kfifo_valid(kfifo);
    min_adj(len, valid);
    kfifo_out_copy(kfifo, buff, len, kfifo->out);

    return len;
}

unsigned long kfifo_out_flat(struct kfifo *kfifo, void *buff, unsigned long len)
{
    unsigned long llen;

    llen = kfifo_peek_flat(kfifo, buff, len);
    kfifo->out += llen;

    return llen;
}

unsigned long kfifo_in_flat(struct kfifo *kfifo, const void *buff, unsigned long len)
{
    unsigned long unused;

    unused = kfifo_unused(kfifo);
    min_adj(len, unused);
    kfifo_in_copy(kfifo, buff, len, kfifo->in);
    kfifo->in += len;

    return len;
}

unsigned long kfifo_peek_record(struct kfifo *kfifo, void *buff,
                                unsigned long len, unsigned long record)
{
    unsigned long datalen;

    if (kfifo_empty(kfifo))
        return 0;

    datalen = kfifo_out_peek(kfifo, record);
    min_adj(len, datalen);
    kfifo_out_copy(kfifo, buff, len, kfifo->out + record);

    return len;
}

unsigned long kfifo_out_record(struct kfifo *kfifo, void *buff,
                               unsigned long len, unsigned long record)
{
    unsigned long datalen;

    if (kfifo_empty(kfifo))
        return 0;

    datalen = kfifo_out_peek(kfifo, record);
    min_adj(len, datalen);
    kfifo_out_copy(kfifo, buff, len, kfifo->out + record);
	kfifo->out += datalen + record;

    return len;
}

unsigned long kfifo_in_record(struct kfifo *kfifo, const void *buff,
                              unsigned long len, unsigned long record)
{
    if (len + record > kfifo_unused(kfifo))
        return 0;

    kfifo_in_poke(kfifo, len, record);
    kfifo_in_copy(kfifo, buff, len, kfifo->in + record);
    kfifo->in += len + record;

    return len;
}

int kfifo_dynamic_alloc(struct kfifo *kfifo, size_t esize, size_t size)
{
    size = roundup_pow2(size);
    if (size < 2)
        return -EINVAL;

    kfifo->in = 0;
    kfifo->out = 0;
    kfifo->mask = size - 1;
    kfifo->esize = esize;

    kfifo->data = calloc(size, esize);
    if (!kfifo->data) {
        kfifo->mask = 0;
        return -EINVAL;
    }

    return 0;
}

void kfifo_dynamic_free(struct kfifo *kfifo)
{
    free(kfifo->data);
    kfifo->in = 0;
    kfifo->out = 0;
    kfifo->mask = 0;
    kfifo->esize = 0;
    kfifo->data = NULL;
}
