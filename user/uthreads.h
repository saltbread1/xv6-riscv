int make_uthread(void (*fun)());
void start_uthreads();
void yield();
int mytid();
void uthread_exit();
void uthread_wait(void *a);
void uthread_notify(int tid, void *a);
void uthread_notify_all(void *a);
