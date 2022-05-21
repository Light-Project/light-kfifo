/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include "kfifo.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define TEST_LOOP 16
DEFINE_KFIFO(normal_bytetest, char, TEST_LOOP);

static const char test_table[TEST_LOOP] = {
    'l', 'i', 'g', 'h', 't', '-', 'f', 'i',
    'f', 'o', ' ', 't', 'e', 's', 't', '\n',
};

static void *kfifo_production(void *unused)
{
    static unsigned int count = 0;
    char ch;

    for (;;) {
        if (!kfifo_check_full(&normal_bytetest)) {
            ch = test_table[(TEST_LOOP - 1) & count++];
            kfifo_put(&normal_bytetest, ch);
        }
        sched_yield();
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t thread;
    char ch;

    pthread_create(&thread, NULL, kfifo_production, NULL);
    for (;;) {
        if (!kfifo_check_empty(&normal_bytetest)) {
            kfifo_get(&normal_bytetest, &ch);
            printf("%c", ch);
        }
        sched_yield();
    }

    return 0;
}
