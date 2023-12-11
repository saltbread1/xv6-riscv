#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/uthreads.h"

void foo() {
    int c = 0;
    for (;;) {
        printf("foo (tid=%d): %d\n", mytid(), c);
        c += 1;
        yield();
    }
}

void bar() {
    int c = 0;
    for (;;)
    {
        printf("bar (tid=%d): %d\n", mytid(), c);
        yield();
        c += 2;
    }
}

void baz_sub(int *cp) {
    printf("baz (tid=%d): %d\n", mytid(), *cp);
    yield();
    *cp += 3;
}

void baz() {
    int c = 0;
    for (;;) {
        baz_sub(&c);
        baz_sub(&c);
    }
}

int main() {
    make_uthread(foo);
    make_uthread(bar);
    make_uthread(baz);
    start_uthreads();
    exit(0);
}
