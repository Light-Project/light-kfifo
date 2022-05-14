/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include "kfifo.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * ARRAY_SIZE - get the number of elements in array.
 * @arr: array to be sized.
 */
#define ARRAY_SIZE(arr) ( \
    sizeof(arr) / sizeof((arr)[0]) \
)

#define TEST_LOOP 16

struct kfifo_test_pdata {
    DECLARE_KFIFO(normal_bytetest, char, TEST_LOOP);
    DECLARE_KFIFO(normal_longtest, long, TEST_LOOP);
    DECLARE_KFIFO_RECORD(record_bytetest, char, TEST_LOOP, 1);
    DECLARE_KFIFO_RECORD(record_longtest, long, TEST_LOOP, 1);
    DECLARE_KFIFO_DYNAMIC(dynamic_bytetest, char);
    DECLARE_KFIFO_DYNAMIC(dynamic_longtest, long);
    DECLARE_KFIFO_DYNAMIC_RECORD(dynamic_record_bytetest, char, 1);
    DECLARE_KFIFO_DYNAMIC_RECORD(dynamic_record_longtest, long, 1);
};

static const char bytetest_table[TEST_LOOP] = {
    'l', 'i', 'g', 'h', 't', 'c', 'o', 'r',
    'e', ',', 'h', 'e', 'l', 'l', 'l', 'o',
};

static const long longtest_table[TEST_LOOP] = {
    (long)0x0000000000000000ULL, (long)0x1111111111111111ULL,
    (long)0x2222222222222222ULL, (long)0x3333333333333333ULL,
    (long)0x4444444444444444ULL, (long)0x5555555555555555ULL,
    (long)0x6666666666666666ULL, (long)0x7777777777777777ULL,
    (long)0x8888888888888888ULL, (long)0x9999999999999999ULL,
    (long)0xaaaaaaaaaaaaaaaaULL, (long)0xbbbbbbbbbbbbbbbbULL,
    (long)0xccccccccccccccccULL, (long)0xddddddddddddddddULL,
    (long)0xeeeeeeeeeeeeeeeeULL, (long)0xffffffffffffffffULL,
};

static int kfifo_testing(struct kfifo_test_pdata *kdata)
{
    char bytevalue[TEST_LOOP];
    long longvalue[TEST_LOOP];
    unsigned int count;
    unsigned long ret;

    for (count = 0; count < TEST_LOOP; ++count) {
        ret = kfifo_put(&kdata->normal_bytetest, bytetest_table[count]);
        printf("kfifo normal_bytetest %u put '%c': ", count, bytetest_table[count]);
        if (!ret || kfifo_len(&kdata->normal_bytetest) != count + 1) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");
    }

    printf("kfifo normal_bytetest check full: ");
    if (!kfifo_check_full(&kdata->normal_bytetest)) {
        printf("failed\n");
        return -EFAULT;
    }
    printf("pass\n");

    for (count = 0; count < TEST_LOOP; ++count) {
        ret = kfifo_get(&kdata->normal_bytetest, bytevalue);
        printf("kfifo normal_bytetest %u get '%c': ", count, *bytevalue);
        if (!ret || *bytevalue != bytetest_table[count]) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");
    }

    printf("kfifo normal_bytetest copy in: ");
    ret = kfifo_in(&kdata->normal_bytetest, bytetest_table, TEST_LOOP);
    if (ret != TEST_LOOP) {
        printf("failed\n");
        return -EFAULT;
    }
    printf("pass\n");

    printf("kfifo normal_bytetest copy out: ");
    ret = kfifo_out(&kdata->normal_bytetest, bytevalue, TEST_LOOP);
    if (ret != TEST_LOOP) {
        printf("failed\n");
        return -EFAULT;
    }
    printf("pass\n");

    printf("kfifo normal_bytetest check copy: ");
    if (memcmp(bytevalue, bytetest_table, sizeof(bytetest_table))) {
        printf("failed\n");
        return -EFAULT;
    }
    printf("pass\n");

    for (count = 0; count < TEST_LOOP; ++count) {
        kfifo_put(&kdata->normal_longtest, longtest_table[count]);
        printf("kfifo normal_longtest %u put %#lx: ", count, longtest_table[count]);
        if (kfifo_len(&kdata->normal_longtest) != count + 1) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");
    }

    printf("kfifo normal_longtest check full: ");
    if (!kfifo_check_full(&kdata->normal_longtest)) {
        printf("failed\n");
        return -EFAULT;
    }
    printf("pass\n");

    for (count = 0; count < TEST_LOOP; ++count) {
        kfifo_get(&kdata->normal_longtest, longvalue);
        printf("kfifo normal_longtest %u get %#lx: ", count, *longvalue);
        if (*longvalue != longtest_table[count]) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");
    }

    printf("kfifo normal_longtest copy in: ");
    ret = kfifo_in(&kdata->normal_longtest, longtest_table, TEST_LOOP);
    if (ret != TEST_LOOP) {
        printf("failed\n");
        return -EFAULT;
    }
    printf("pass\n");

    printf("kfifo normal_longtest copy out: ");
    ret = kfifo_out(&kdata->normal_longtest, longvalue, TEST_LOOP);
    if (ret != TEST_LOOP) {
        printf("failed\n");
        return -EFAULT;
    }
    printf("pass\n");

    printf("kfifo normal_longtest check copy: ");
    if (memcmp(longvalue, longtest_table, sizeof(longtest_table))) {
        printf("failed\n");
        return -EFAULT;
    }
    printf("pass\n");

    for (count = 0; count < TEST_LOOP; ++count) {
        ret = kfifo_put(&kdata->dynamic_bytetest, bytetest_table[count]);
        printf("kfifo dynamic_bytetest %u put '%c': ", count, *bytevalue);
        if (!ret || kfifo_len(&kdata->dynamic_bytetest) != count + 1) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");
    }

    printf("kfifo dynamic_bytetest check full: ");
    if (!kfifo_check_full(&kdata->dynamic_bytetest)) {
        printf("failed\n");
        return -EFAULT;
    }
    printf("pass\n");

    for (count = 0; count < TEST_LOOP; ++count) {
        ret = kfifo_get(&kdata->dynamic_bytetest, bytevalue);
        printf("kfifo dynamic_bytetest %u get '%c': ", count, *bytevalue);
        if (!ret || *bytevalue != bytetest_table[count]) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");
    }

    printf("kfifo dynamic_bytetest copy in: ");
    ret = kfifo_in(&kdata->dynamic_bytetest, bytetest_table, TEST_LOOP);
    if (ret != TEST_LOOP) {
        printf("failed\n");
        return -EFAULT;
    }
    printf("pass\n");

    printf("kfifo dynamic_bytetest copy out: ");
    ret = kfifo_out(&kdata->dynamic_bytetest, bytevalue, TEST_LOOP);
    if (ret != TEST_LOOP)
        return -EFAULT;
    printf("pass\n");

    printf("kfifo dynamic_bytetest check copy: ");
    if (memcmp(bytevalue, bytetest_table, sizeof(bytetest_table))) {
        printf("failed\n");
        return -EFAULT;
    }
    printf("pass\n");

    for (count = 0; count < TEST_LOOP; ++count) {
        kfifo_put(&kdata->dynamic_longtest, longtest_table[count]);
        printf("kfifo dynamic_longtest %u put %#lx: ", count, longtest_table[count]);
        if (kfifo_len(&kdata->dynamic_longtest) != count + 1) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");
    }

    printf("kfifo dynamic_longtest check full: ");
    if (!kfifo_check_full(&kdata->dynamic_longtest)) {
        printf("failed\n");
        return -EFAULT;
    }
    printf("pass\n");

    for (count = 0; count < TEST_LOOP; ++count) {
        kfifo_get(&kdata->dynamic_longtest, longvalue);
        printf("kfifo dynamic_longtest %u get %#lx: ", count, *longvalue);
        if (*longvalue != longtest_table[count]) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");
    }

    printf("kfifo dynamic_longtest copy in: ");
    ret = kfifo_in(&kdata->dynamic_longtest, longtest_table, TEST_LOOP);
    if (ret != TEST_LOOP) {
        printf("failed\n");
        return -EFAULT;
    }
    printf("pass\n");

    printf("kfifo dynamic_longtest copy out: ");
    ret = kfifo_out(&kdata->dynamic_longtest, longvalue, TEST_LOOP);
    if (ret != TEST_LOOP) {
        printf("failed\n");
        return -EFAULT;
    }
    printf("pass\n");

    printf("kfifo normal_longtest check copy: ");
    if (memcmp(longvalue, longtest_table, sizeof(longtest_table))) {
        printf("failed\n");
        return -EFAULT;
    }
    printf("pass\n");

    for (count = 1; count < TEST_LOOP; ++count) {
        printf("kfifo record_bytetest copy %u in: ", count);
        ret = kfifo_in(&kdata->record_bytetest, bytetest_table, count);
        if (ret != count) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");

        printf("kfifo record_bytetest copy %u out: ", count);
        ret = kfifo_out(&kdata->record_bytetest, bytevalue, count);
        if (ret != count) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");

        printf("kfifo record_bytetest check %u copy: ", count);
        if (memcmp(bytevalue, bytetest_table, count)) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");
    }

    for (count = 1; count < TEST_LOOP; ++count) {
        printf("kfifo record_longtest copy %u in: ", count);
        ret = kfifo_in(&kdata->record_longtest, longtest_table, count);
        if (ret != count) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");

        printf("kfifo record_longtest copy %u out: ", count);
        ret = kfifo_out(&kdata->record_longtest, longvalue, count);
        if (ret != count) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");

        printf("kfifo record_longtest check %u copy: ", count);
        if (memcmp(longvalue, longtest_table, count)) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");
    }

    for (count = 1; count < TEST_LOOP; ++count) {
        printf("kfifo dynamic_record_bytetest copy %u in: ", count);
        ret = kfifo_in(&kdata->dynamic_record_bytetest, bytetest_table, count);
        if (ret != count) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");

        printf("kfifo dynamic_record_bytetest copy %u out: ", count);
        ret = kfifo_out(&kdata->dynamic_record_bytetest, bytevalue, count);
        if (ret != count) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");

        printf("kfifo dynamic_record_bytetest check %u copy: ", count);
        if (memcmp(bytevalue, bytetest_table, count)) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");
    }

    for (count = 1; count < TEST_LOOP; ++count) {
        printf("kfifo dynamic_record_longtest copy %u in: ", count);
        ret = kfifo_in(&kdata->dynamic_record_longtest, longtest_table, count);
        if (ret != count) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");

        printf("kfifo dynamic_record_longtest copy %u out: ", count);
        ret = kfifo_out(&kdata->dynamic_record_longtest, longvalue, count);
        if (ret != count) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");

        printf("kfifo dynamic_record_longtest check %u copy: ", count);
        if (memcmp(longvalue, longtest_table, count)) {
            printf("failed\n");
            return -EFAULT;
        }
        printf("pass\n");
    }

    return 0;
}

int main(void)
{
    struct kfifo_test_pdata *kdata;
    int retval;

    kdata = malloc(sizeof(struct kfifo_test_pdata));
    if (!kdata)
        return -ENOMEM;

    kdata->normal_bytetest = KFIFO_INIT(kdata->normal_bytetest);
    kdata->normal_longtest = KFIFO_INIT(kdata->normal_longtest);
    kdata->record_bytetest = KFIFO_INIT(kdata->record_bytetest);
    kdata->record_longtest = KFIFO_INIT(kdata->record_longtest);

    if ((retval = kfifo_alloc(&kdata->dynamic_bytetest, TEST_LOOP)) ||
        (retval = kfifo_alloc(&kdata->dynamic_longtest, TEST_LOOP)) ||
        (retval = kfifo_alloc(&kdata->dynamic_record_bytetest, TEST_LOOP)) ||
        (retval = kfifo_alloc(&kdata->dynamic_record_longtest, TEST_LOOP)))
        return -ENOMEM;

    retval = kfifo_testing(kdata);
    free(kdata);

    return retval;
}
