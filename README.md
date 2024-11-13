# file_io
Trivial demo code to test I/O performance degradation due to write stalls when overwriting data in non page-aligned sizes.

This is demo'd by having two threads, one is writing to a file while another is constantly fsync'ing dirty pages from the same file.

This was tested on an Ubuntu jammy VM with Linux v5.15.0-121-generic.

For page aligned test, turn on `#define IS_ALIGNED_PAGE 1`. Turn off `#define IS_ALIGNED_PAGE 0` for unaligned page test.

Unaligned page write size 1024
![Demo](assets/unaligned_write.png)


Aligned page write size 4096
![Demo](<assets/aligned_write.png>)

Ensure to drop page cache between runs, to prevent reading/writing to page cache.
 ```
 echo  3 | sudo tee -a '/proc/sys/vm/drop_caches'
 ```