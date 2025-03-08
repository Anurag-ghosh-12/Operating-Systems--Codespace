#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int n;      // Number of blocks
    int s;      // Block size
    int ubn;    // Used block number
    int fbn;    // Free block number
    unsigned char ub[512]; // Bitmap for used blocks (supports up to 4096 blocks)
} FileHeader;

int init_File_dd(const char *fname, int bsize, int bno) {
    int fd;
    FileHeader header;
    size_t file_size = 4096 + bsize * bno;
    char *buffer;

    // Open the file (create if it doesn't exist)
    fd = open(fname, O_CREAT | O_RDWR, 0700);
    if (fd == -1) {
        perror("open() Error:");
        return -1;
    }

    // Initialize the header
    header.n = bno;
    header.s = bsize;
    header.ubn = 0;
    header.fbn = bno;
    memset(header.ub, 0, sizeof(header.ub)); // All bits are 0 (all blocks free)

    // Write the header (4096 bytes)
    if (write(fd, &header, sizeof(header)) != sizeof(header)) {
        perror("write() Error:");
        close(fd);
        return -1;
    }

    // Allocate and write the remaining space (zeroed-out blocks)
    buffer = (char *)calloc(bsize * bno, sizeof(char));
    if (!buffer) {
        perror("Memory allocation failed");
        close(fd);
        return -1;
    }
    if (write(fd, buffer, bsize * bno) != (ssize_t)(bsize * bno)) {
        perror("Data write error");
        free(buffer);
        close(fd);
        return -1;
    }

    free(buffer);
    close(fd);
    return 0;
}

int get_freeblock(const char *fname) {
    int fd;
    FileHeader header;

    // Open the file
    fd = open(fname, O_RDWR);
    if (fd == -1) {
        perror("open() Error:");
        return -1;
    }

    // Read the header
    if (read(fd, &header, sizeof(header)) != sizeof(header)) {
        perror("read() Error:");
        close(fd);
        return -1;
    }

    // Find the first free block
    int block_index = -1;
    for (int i = 0; i < header.n; i++) {
        int byte_index = i / 8;
        int bit_index = i % 8;

        if (!(header.ub[byte_index] & (1 << bit_index))) { // Check if bit is 0 (free)
            block_index = i;
            header.ub[byte_index] |= (1 << bit_index); // Mark block as used in bit pattern
            header.ubn++; // Increase used block count
            header.fbn--; // Decrease free block count
            break;
        }
    }

    if (block_index == -1) {
        close(fd);
        return -1;
    }
    lseek(fd, 0, SEEK_SET);
    if (write(fd, &header, sizeof(header)) != sizeof(header)) {
        perror("write() Error:");
        close(fd);
        return -1;
    }
    // Filling the allocated block with 1's
    char *block_data = (char *)malloc(header.s);
    if (!block_data) {
        perror("Memory allocation failed");
        close(fd);
        return -1;
    }
    memset(block_data, 1, header.s);
    lseek(fd, 4096 + block_index * header.s, SEEK_SET);
    if (write(fd, block_data, header.s) != header.s) {
        perror("Block write error");
        free(block_data);
        close(fd);
        return -1;
    }
    free(block_data);
    close(fd);
    return block_index;
}

int main() {
    const char *filename = "test.bin";
    int block_size = 512;
    int block_number = 10;
    if (init_File_dd(filename, block_size, block_number) == 0) {
        printf("File '%s' initialized successfully.\n", filename);
    } else {
        printf("Failed to initialize file '%s'.\n", filename);
        return -1;
    }
    int x=0;
    //testing for 7 iterations
    while(x++<7){
    int free_block = get_freeblock(filename);
    if (free_block != -1) {
        printf("First free block index: %d\n", free_block);
    } else {
        printf("No free block found or error occurred.\n");
    }
    }
    return 0;
}

