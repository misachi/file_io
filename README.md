# file_io
Trivial demo code to test I/O performance degradation due to write stalls when overwriting data. This can occur when writing data that is not page aligned e.g writing 2000 bytes instead of the "normal" page size on linux platforms, 4096. When stable page writes is enabled, attempting to perform a write during writeback can block leading to stalls. Also, a stalled write can block other reads or writes of the same page.

This is demo'd by having two threads, one is writing to a file while another is constantly fsync'ing dirty pages from the same file.

This was tested on an Ubuntu jammy VM with Linux v5.15.0-121-generic.

I could not replicate the same on a machine installed with Linux v6.5.0-45-generic.

Unaligned page write size 2000
![Demo](assets/Screenshot%202024-09-23%20233527.png)


Aligned page write size 4096
![Demo](<assets/Screenshot 2024-09-23 234200.png>)

Ensure to drop page cache between runs, to prevent reading/writing to cache.
