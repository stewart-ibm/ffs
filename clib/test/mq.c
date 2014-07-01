/*
 * Copyright (c) International Business Machines Corp., 2014
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <clib/vector.h>
#include <clib/vector_iter.h>

#include <clib/misc.h>
#include <clib/mq.h>

#define SIZE 2550

int main(void) {
    vector_t a = INIT_VECTOR;
    vector_init(&a, "my_vector", 4, 1024);

    vector_put(&a, 52, (uint32_t[]){52});
    vector_put(&a, 53, (uint32_t[]){53});
    vector_put(&a, 167, (uint32_t[]){167});
    vector_put(&a, 223, (uint32_t[]){223});
    vector_put(&a, 78, (uint32_t[]){78});
    vector_put(&a, 205, (uint32_t[]){205});
    vector_put(&a, 183, (uint32_t[]){183});
    vector_put(&a, 150, (uint32_t[]){150});
    vector_put(&a, 90, (uint32_t[]){90});
    vector_put(&a, 66, (uint32_t[]){66});
    vector_put(&a, 91, (uint32_t[]){91});
    vector_put(&a, 45, (uint32_t[]){45});
    vector_put(&a, 211, (uint32_t[]){211});
    uint32_t arr[] = {55,56,57,58,59,60,61,62,63};
    vector_put(&a, 985, arr, 9);

    vector_iter_t it;
    vector_iter_init(&it, &a, VI_FLAG_FWD);

    uint32_t * j;
    vector_for_each(&it, j) {
        printf("XXX i[%d]\n", *j);
    }

    vector_dump(&a, stdout);

    mqueue_t mq = INIT_MQUEUE;
    mqueue_init(&mq, "dbs");
    mqueue_create(&mq, gettid());

    vector_send(&a, &mq);
    vector_delete(&a);

    sleep(1);

    mqueue_delete(&mq);

    return 0;
}
