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

//Search for the first free block and write data to it
int write_to_freeblock(const char *fname, const void *data, size_t data_size) {
    int fd;
    FileHeader header;
    
    // Open the file
    fd = open(fname, O_RDWR);
    if (fd == -1) {
        perror("open() Error");
        return -1;
    }
    
    // Read the header
    if (read(fd, &header, sizeof(header)) != sizeof(header)) {
        perror("read() Error");
        close(fd);
        return -1;
    }
    
    // Check if data size is compatible with block size
    if (data_size > header.s) {
        fprintf(stderr, "Data size exceeds block size\n");
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
        fprintf(stderr, "No free blocks available\n");
        close(fd);
        return -1;
    }
    
    // Update the header
    lseek(fd, 0, SEEK_SET);
    if (write(fd, &header, sizeof(header)) != sizeof(header)) {
        perror("write() header Error");
        close(fd);
        return -1;
    }
    
    // Write data to the block
    char *block_data = (char *)malloc(header.s);
    if (!block_data) {
        perror("Memory allocation failed");
        close(fd);
        return -1;
    }
    
    // Copy the data and padding with zeros if necessary
    memset(block_data, 0, header.s);
    memcpy(block_data, data, data_size);
    
    // Move to the block position and write
    lseek(fd, 4096 + block_index * header.s, SEEK_SET);
    if (write(fd, block_data, header.s) != header.s) {
        perror("Block write error");
        free(block_data);
        close(fd);
        return -1;
    }
    
    free(block_data);
    close(fd);
    return block_index; // Return the block index that was allocated
}

int free_block(const char *fname, int block_index) {
    int fd;
    FileHeader header;
    
    // Open the file
    fd = open(fname, O_RDWR);
    if (fd == -1) {
        perror("open() Error");
        return -1;
    }
    
    // Read the header
    if (read(fd, &header, sizeof(header)) != sizeof(header)) {
        perror("read() Error");
        close(fd);
        return -1;
    }
    
    // Validate block index
    if (block_index < 0 || block_index >= header.n) {
        fprintf(stderr, "Invalid block index\n");
        close(fd);
        return -1;
    }
    
    // Check if block is already free
    int byte_index = block_index / 8;
    int bit_index = block_index % 8;
    
    if (!(header.ub[byte_index] & (1 << bit_index))) {
        fprintf(stderr, "Block %d is already free\n", block_index);
        close(fd);
        return -1;
    }
    
    // Mark the block as free
    header.ub[byte_index] &= ~(1 << bit_index); // Clear the bit
    header.ubn--; // Decrease used block count
    header.fbn++; // Increase free block count
    
    // Update the header
    lseek(fd, 0, SEEK_SET);
    if (write(fd, &header, sizeof(header)) != sizeof(header)) {
        perror("write() header Error");
        close(fd);
        return -1;
    }
    
    // Zero out the block content
    char *block_data = (char *)calloc(header.s, 1);
    if (!block_data) {
        perror("Memory allocation failed");
        close(fd);
        return -1;
    }
    
    lseek(fd, 4096 + block_index * header.s, SEEK_SET);
    if (write(fd, block_data, header.s) != header.s) {
        perror("Block write error");
        free(block_data);
        close(fd);
        return -1;
    }
    
    free(block_data);
    close(fd);
    return 0; // Success
}

// Check filesystem for consistency
int check_fs(const char *fname) {
    int fd;
    FileHeader header;
    int actual_used_blocks = 0;
    
    // Open the file
    fd = open(fname, O_RDONLY);
    if (fd == -1) {
        perror("open() Error");
        return -1;
    }
    
    // Read the header
    if (read(fd, &header, sizeof(header)) != sizeof(header)) {
        perror("read() Error");
        close(fd);
        return -1;
    }
    
    // Count actual used blocks from bitmap
    for (int i = 0; i < header.n; i++) {
        int byte_index = i / 8;
        int bit_index = i % 8;
        if (header.ub[byte_index] & (1 << bit_index)) {
            actual_used_blocks++;
        }
    }
    
    // Check consistency
    printf("Filesystem check for %s:\n", fname);
    printf("Total blocks: %d\n", header.n);
    printf("Block size: %d bytes\n", header.s);
    printf("Header says: %d used blocks, %d free blocks\n", header.ubn, header.fbn);
    printf("Actual count from bitmap: %d used blocks\n", actual_used_blocks);
    
    // Verify if header counts match bitmap counts
    if (header.ubn != actual_used_blocks) {
        printf("ERROR: Inconsistency detected. Header shows %d used blocks but bitmap shows %d used blocks.\n", 
               header.ubn, actual_used_blocks);
        close(fd);
        return -1;
    }
    
    if (header.ubn + header.fbn != header.n) {
        printf("ERROR: Inconsistency detected. Used blocks + Free blocks (%d) doesn't equal total blocks (%d).\n", 
               header.ubn + header.fbn, header.n);
        close(fd);
        return -1;
    }
    
    printf("Filesystem is consistent.\n");
    close(fd);
    return 0; // Consistent
}

int main() {
    const char *filename = "test.bin";
    int block_size = 512;
    int block_number = 10;
    int choice;
    char userData[500];
    int blockIndex;
   
    // Initialize the file system if it doesn't exist
    if (open(filename, O_RDONLY)==-1) {
        printf("File system not found. Creating new file system...\n");
        if (init_File_dd(filename, block_size, block_number) == 0) {
            printf("File '%s' initialized successfully.\n", filename);
        } else {
            printf("Failed to initialize file '%s'.\n", filename);
            return -1;
        }
    } else {
        printf("Using existing file system: %s\n", filename);
    }
    
    do {
        printf("\n==== File System Management ====\n");
        printf("1. Initialize new file system\n");
        printf("2. Allocate a free block\n");
        printf("3. Write data to a free block\n");
        printf("4. Free a specific block\n");
        printf("5. Check file system consistency\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
 
        while (getchar() != '\n');
        
        switch (choice) {
            case 1:
                printf("Enter block size: ");
                scanf("%d", &block_size);
                printf("Enter number of blocks: ");
                scanf("%d", &block_number);
                while (getchar() != '\n'); 
                
                if (init_File_dd(filename, block_size, block_number) == 0) {
                    printf("File '%s' initialized successfully.\n", filename);
                } else {
                    printf("Failed to initialize file '%s'.\n", filename);
                }
                break;
                
            case 2:
                blockIndex = get_freeblock(filename);
                if (blockIndex != -1) {
                    printf("Block allocated: %d\n", blockIndex);
                } else {
                    printf("No free blocks available or error occurred.\n");
                }
                break;
                
            case 3:
                printf("Enter data to write (max 500 chars): ");
                fgets(userData, 500, stdin);
                userData[strcspn(userData, "\n")] = 0; 
                
                blockIndex = write_to_freeblock(filename, userData, strlen(userData) + 1);
                if (blockIndex != -1) {
                    printf("Data written to block %d\n", blockIndex);
                } else {
                    printf("Failed to write data to a block.\n");
                }
                break;
                
            case 4:
                printf("Enter block index to free: ");
                scanf("%d", &blockIndex);
                while (getchar() != '\n'); 
                
                if (free_block(filename, blockIndex) == 0) {
                    printf("Block %d freed successfully.\n", blockIndex);
                } else {
                    printf("Failed to free block %d.\n", blockIndex);
                }
                break;
                
            case 5:
                check_fs(filename);
                break;
                
            case 0:
                printf("Exiting program.\n");
                break;
                
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);
    
    return 0;
}
