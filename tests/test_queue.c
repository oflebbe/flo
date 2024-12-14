#include <stdio.h>

#define FLO_QUEUE_IMPLEMENTATION
#include "../flo_queue.h"
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>

int f_worker(void *arg)
{
    flo_queue_t *work_queue = (flo_queue_t *)arg;
    int sum = 0;
    int *payload;
    do
    {
        int buf;
        payload = flo_queue_pop_block(work_queue, &buf);

        if (payload)
        {
            sum += *payload;
        }
    } while (payload);
    return sum;
}

void test_queue_single()
{
    const int queue_size = 10;

    flo_queue_t *q = flo_queue_create(queue_size, sizeof(int));
    for (int i = 0; i < queue_size - 1; i++)
    {
        flo_queue_push_block(q, &i);
    }

    for (int i = 0; i < queue_size - 1; i++)
    {
        int ret;
        int *ptr = flo_queue_pop_block(q, &ret);
        assert(*ptr == i);
    }

    for (int i = 0; i < queue_size - 1; i++)
    {
        flo_queue_push_block(q, &i);
    }
    for (int i = 0; i < queue_size - 1; i++)
    {
        int ret;
        int *ptr = flo_queue_pop_block(q, &ret);
        assert(*ptr == i);
    }
    flo_queue_free(q);
}

void test_queue_multi()
{
    const int queue_size = 100;
    const int worker = 10;
    const int problem = 100000;

    thrd_t *threads = calloc( worker, sizeof( thrd_t));
    
    flo_queue_t *q = flo_queue_create(queue_size, sizeof(int));
    for (int i = 0; i < worker; i++) {
        thrd_create( &threads[i], f_worker, q);
    }
    
    for (int i = 0; i < problem; i++)
    {
        int p = 1;
        flo_queue_push_block(q, &p);
    }
    flo_queue_close( q);
    
    int sum = 0;
    for (int i = 0; i < worker; i++) {
        int res = 0;
        thrd_join( threads[i], &res);
        sum += res;
    }
    assert( sum == problem);
    flo_queue_free( q);
    free(threads);
}

int main()
{
    test_queue_single();
    test_queue_multi();
    printf("END\n");
}