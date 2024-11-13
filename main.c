#include <stdio.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <threads.h>
#include <unistd.h>

#define IS_ALIGNED_PAGE 1
#define NUM_LOOPS 1000000UL
#define PAGE_SIZE 4096
#define APPEND_FILE 0

#if IS_ALIGNED_PAGE
char buf[PAGE_SIZE];
#else
char buf[1 << 10];
#endif

typedef struct func_args
{
    bool sync_meta;
    int fd;
    int len;
    char *buf;
} func_args;

int write_file(void *arg)
{
    func_args *args = (func_args *)arg;
    for (size_t i = 0; i < NUM_LOOPS; i++)
    {
        /* Ensure unaligned page write touches a new page for every write */
        lseek(args->fd, i * PAGE_SIZE, SEEK_SET);

        write(args->fd, args->buf, args->len);
    }
    return EXIT_SUCCESS;
}

int sync_file(void *arg)
{
    func_args *args = (func_args *)arg;

    if (args->sync_meta)
    {
        for (size_t i = 0; i < NUM_LOOPS; i++)
        {
            fsync(args->fd);
        }
    }
    else
    {
        for (size_t i = 0; i < NUM_LOOPS; i++)
        {
            fdatasync(args->fd);
        }
    }

    return EXIT_SUCCESS;
}

void work(int fd, bool sync_meta)
{
    thrd_t threads[2];
    func_args args;

    args.fd = fd;
    args.len = sizeof(buf);
    args.buf = buf;
    args.sync_meta = sync_meta;

    thrd_create(&threads[0], write_file, (void *)&args); /* Start thread1 */
    thrd_create(&threads[1], sync_file, (void *)&args);  /* Start thread2 */

    thrd_join(threads[0], NULL); /* Wait thread1 */
    thrd_join(threads[1], NULL); /* Wait thread2 */
}

int main(int argc, char *argv[])
{
    clock_t start, end;
    double cpu_time_used;
    char path[PATH_MAX];
    int fd;

    /* I have to run the application in one terminal process and profiler in another
     * terminal process. Sometimes the application process
     * terminates before the profiler can start. This is to delay so that I'm able
     * to get off-cpu profile with bcc tools. I'm sure there is a better way to do it
     */
    sleep(5);

    snprintf(path, PATH_MAX, "%s/%s", getenv("HOME"), ".tmp/file_io/stalls.txt");

#if APPEND_FILE
    fd = open(path, O_CREAT | O_APPEND, 0777);
#else
    fd = open(path, O_CREAT | O_WRONLY, 0777);
#endif
    if (!fd)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    memset(buf, 'K', sizeof(buf));

    start = clock();
    work(fd, 1);
    end = clock();

    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    close(fd);
    printf("Time Taken: %f seconds\n", cpu_time_used);
    exit(EXIT_SUCCESS);
}
