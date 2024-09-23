#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <threads.h>
#include <unistd.h>

#define PAGE_IS_ALIGNED 1
#define NUM_LOOPS 1000000UL
#define PAGE_SIZE 4096
#define APPEND_FILE 0


#if PAGE_IS_ALIGNED
char buf[PAGE_SIZE];
#else
char buf[1000];
#endif

static int thread_num = 2;

typedef struct func_args
{
    char write_val;
    int fd;
    int len;
    char *buf;
} func_args;

static int write_file(void *arg) {
    func_args *args = (func_args *)arg;
    memset(args->buf, args->write_val, 1000);
    for (size_t i = 0; i < NUM_LOOPS; i++)
    {
        write(args->fd, args->buf, args->len);
    }
    return EXIT_SUCCESS;
}

static int sync_file(void *arg) {
    func_args *args = (func_args *)arg;
    for (size_t i = 0; i < NUM_LOOPS; i++)
    {
        fsync(args->fd);
    }
    return EXIT_SUCCESS;
}


int main(int argc, char *argv[]) {
    clock_t start, end;
    double cpu_time_used;
    int fd;
    size_t size;
    thrd_t threads[thread_num];  
    func_args args; 

    start = clock();
#if APPEND_FILE
    fd = open("/tmp/file_io/stalls.txt", O_APPEND);
#else
    fd = open("/tmp/file_io/stalls.txt", O_WRONLY);
#endif
    if (!fd)
    {
       perror("open");
       exit(EXIT_FAILURE);
    }

    size = sizeof(buf);
    args.write_val = 'K';
    args.fd = fd;
    args.len = size;
    args.buf = buf;
    thrd_create(&threads[0], write_file, (void *)&args);
    thrd_create(&threads[1], sync_file, (void *)&args);

    thrd_join(threads[0], NULL);
    thrd_join(threads[1], NULL);

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time Taken: %f seconds\n", cpu_time_used);
    exit(EXIT_SUCCESS);
}
