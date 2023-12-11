#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/uthreads.h"

void foo()
{
    int c = 0;
    for (;;)
    {
        printf("foo (tid=%d): %d\n", mytid(), c);
        if (c%3 == 0)
        {
            uthread_wait(foo);
        }
        c += 1;
        yield();
    }
}

void bar()
{
    int c = 0;
    for (;;)
    {
        printf("bar (tid=%d): %d\n", mytid(), c);
        if (c%10 == 0) { uthread_notify(0, foo); }
        yield();
        c += 2;
    }
}

int main()
{
    make_uthread(foo);
    make_uthread(bar);
    start_uthreads();
    exit(0);
}
