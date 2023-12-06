#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_THREAD 4
#define STACK_DEPTH 512
#define UT_EMPTY 0
#define UT_RUNNABLE 1
#define UT_RUNNING 2
#define UT_SLEEP 3

struct context {
    uint64 ra;
    uint64 sp;

    // callee-saved
    uint64 s0;
    uint64 s1;
    uint64 s2;
    uint64 s3;
    uint64 s4;
    uint64 s5;
    uint64 s6;
    uint64 s7;
    uint64 s8;
    uint64 s9;
    uint64 s10;
    uint64 s11;
};

struct uthread
{
    int tid;
    int state;
    uint64 stack[STACK_DEPTH];
    struct context context;
    void (*fun)();
};

void swtch(struct context*, struct context*);

struct uthread threads[MAX_THREAD];
struct uthread *curr_thread;
int tnum;
struct context scontext;
uint64 sstack[STACK_DEPTH];

int make_uthread(void (*fun)())
{
    struct uthread *t;

    for (t = threads; t < threads + MAX_THREAD; t++)
    {
        if (t->state == UT_EMPTY)
        {
            t->tid = tnum++;
            t->state = UT_RUNNABLE;
            t->context.ra = (uint64)fun;
            t->context.sp = (uint64)(t->stack + STACK_DEPTH);
            t->fun = fun;
            return t->tid;
        }
    }
    // The number of threads has already reached the limit.
    return -1;
}

void start_uthreads()
{
    curr_thread = threads;
    curr_thread->state = UT_RUNNING;

    //curr_thread->fun();
    scontext.ra = (uint64)start_uthreads;
    scontext.sp = (uint64)(sstack + STACK_DEPTH);
    swtch(&scontext, &curr_thread->context);
}

// schedule
// Not switch if there is only one thread.
void sched()
{
    struct uthread *t, *next_thread;

    for (t = curr_thread + 1; t <= curr_thread + MAX_THREAD; t++)
    {
        next_thread = t;
        if (next_thread >= threads + MAX_THREAD)
        {
            next_thread -= MAX_THREAD;
        }
        if (next_thread->state == UT_RUNNABLE)
        {
            next_thread->state = UT_RUNNING;
            if (next_thread != curr_thread)
            {
                t = curr_thread;
                curr_thread = next_thread;
                swtch(&t->context, &curr_thread->context);
            }
            return;
        }
    }

    // There are no active thread.
    swtch(&curr_thread->context, &scontext);
}

void yield()
{
    curr_thread->state = UT_RUNNABLE;
    sched();
}

int mytid()
{
    return curr_thread->tid;
}

void uthread_exit()
{
    curr_thread->state = UT_EMPTY;
    sched();
}

void foo() {
    int c = 0;
    for (;;) {
        printf("foo (tid=%d): %d\n", mytid(), c);
        c += 1;
        if (c > 8) { uthread_exit(); }
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
        if (c > 12) { uthread_exit(); }
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
        if (c > 15) { uthread_exit(); }
    }
}

int main() {
    make_uthread(foo);
    make_uthread(bar);
    make_uthread(baz);
    start_uthreads();
    exit(0);
}
