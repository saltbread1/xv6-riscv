#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/uthreads.h"

int foo_sub(int c)
{
    yield();
    if (c <= 0) { return 1; }
    return c * foo_sub(c-1);
}

void foo()
{
    int c = 0;
    for (;;)
    {
        printf("foo (tid=%d): %d\n", mytid(), foo_sub(c++));
        yield();
    }
}

void bar()
{
    int c = 0;
    for (;;)
    {
        yield();
        c += 2;
        yield();
        c += 3;
        yield();
        printf("bar (tid=%d): %d\n", mytid(), c);
    }
}

int baz_sub(int c)
{
    yield();
    if (c <= 1) { return 1; }
    return baz_sub(c-1) + baz_sub(c-2);
}

void baz()
{
    int c = 0;
    for (;;)
    {
        printf("baz (tid=%d): %d\n", mytid(), baz_sub(c++));
        yield();
    }
}

int main() {
    make_uthread(foo);
    make_uthread(bar);
    make_uthread(baz);
    start_uthreads();
    exit(0);
}
