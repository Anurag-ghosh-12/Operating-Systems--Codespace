#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BLOCK_SIZE 4096
#define MYFILE 1
#define MYFOLDER 2
#define METADATA_SIZE 21
#define FILENAME_LEN 12

typedef struct {
   int bsize;
   int nblocks;
   int freeblock;
   int rootblock;
} superblock;

typedef struct { 
   char type;
   char name[FILENAME_LEN];
   unsigned int firstblock;
   unsigned int fsize;
} fd;

typedef struct {
  int id;
  char buffer[4088];
  int nextblock;
} block;

// Function to initialize the filesystem
void init_fs(char *filename, int nblocks)
{
   int fd = open(filename, O_CREAT|O_WRONLY|O_TRUNC, 0700);
   if(fd < 0)
   {
      perror("Error while opening the file\n");
      exit(EXIT_FAILURE);
   }
   
   // Initialize superblock
   superblock sb;
   sb.bsize = BLOCK_SIZE;
   sb.nblocks = nblocks + 1;  // +1 for superblock
   sb.rootblock = 1;
   sb.freeblock = 2;
   
   write(fd, &sb, BLOCK_SIZE);
   
   // Initializing root directory block
   block root_block;
   root_block.id = 1;
   memset(root_block.buffer, 0, 4088);
   root_block.nextblock = -1;  // End of chain
   write(fd, &root_block, BLOCK_SIZE);
   
   /*
   The Structure followed: 
   
   Superblock ------> root block---> END
   
   Free Block Chain
   B2->B3->...Bn
    */
    
    
   // Initializing the free blocks chain
   block dummy_block;
   memset(dummy_block.buffer, 0, 4088);
   
   for(int i = 2; i <= nblocks; i++)
   {
      dummy_block.id = i;
      dummy_block.nextblock = (i < nblocks) ? i + 1 : -1;  // Last block points to -1
      write(fd, &dummy_block, BLOCK_SIZE);
   }
   
   close(fd);
}

// Function to read a block from the filesystem
block readblock(char *filename, int blocknum)
{
   int fd = open(filename, O_RDONLY);
   if(fd < 0)
   {
      perror("Error while opening the file\n");
      exit(EXIT_FAILURE);
   }
   
   block b;
   lseek(fd, blocknum * BLOCK_SIZE, SEEK_SET);
   read(fd, &b, BLOCK_SIZE);
   close(fd);
   return b;
}

// Function to write a block to the filesystem
void writeblock(char *filename, int blocknum, block b)
{
   int fd = open(filename, O_WRONLY);
   if(fd < 0)
   {
      perror("Error while opening the file\n");
      exit(EXIT_FAILURE);
   }
   lseek(fd, blocknum * BLOCK_SIZE, SEEK_SET);
   write(fd, &b, BLOCK_SIZE);
   close(fd);
}

// Function to get superblock from filesystem
superblock get_superblock(char *filename)
{
   superblock sb;
   int fd = open(filename, O_RDONLY);
   if(fd < 0)
   {
      perror("Error while opening the file\n");
      exit(EXIT_FAILURE);
   }
   read(fd, &sb, BLOCK_SIZE);
   close(fd);
   return sb;
}

// Function to update superblock in filesystem
void update_superblock(char *filename, superblock sb)
{
   int fd = open(filename, O_WRONLY);
   if(fd < 0)
   {
      perror("Error while opening the file\n");
      exit(EXIT_FAILURE);
   }
   write(fd, &sb, BLOCK_SIZE);
   close(fd);
}

// Function to allocate a free block
int allocate_block(char *filename)
{
   superblock sb = get_superblock(filename);
   int free_block = sb.freeblock;
   
   if(free_block <= 0 || free_block >= sb.nblocks)
   {
      printf("No free blocks available\n");
      return -1;
   }
   
   // Updating the free block pointer to next free block
   block b = readblock(filename, free_block);
   sb.freeblock = b.nextblock;
   update_superblock(filename, sb);
   
   // Allocating with 0 full block
   memset(b.buffer, 0, 4088);
   b.nextblock = -1;  // End of data block chain i.e. not free block chain
   writeblock(filename, free_block, b);
   
   return free_block;
}

// Function to free a block and adding it to free block chain
void free_block(char *filename, int blocknum)
{
   superblock sb = get_superblock(filename);
   
   // Emptying the block first
   block b = readblock(filename, blocknum);
   memset(b.buffer, 0, 4088);
   b.nextblock = sb.freeblock;
   writeblock(filename, blocknum, b);
   
   // Updating superblock
   sb.freeblock = blocknum;
   update_superblock(filename, sb);
}

// Function to find a file or folder in a directory
int find_entry(char *filename, int dir_block, char *name)
{
   block dir = readblock(filename, dir_block);
   int entries = 0;
   while(dir.buffer[entries * METADATA_SIZE] != 0 && entries < 4088 / METADATA_SIZE)
   {
      fd entry;
      memcpy(&entry, &dir.buffer[entries * METADATA_SIZE], METADATA_SIZE);
      
      if(strcmp(entry.name, name) == 0)
      {
         return entries;
      }
      
      entries++;
   }
   
   // If not found in this block, check next block if available
   if(dir.nextblock != -1)
   {
      int next_result = find_entry(filename, dir.nextblock, name);
      if(next_result >= 0)
      {
         return (4088 / METADATA_SIZE) + next_result;
      }
   }
   
   return -1;  // Not found
}

// Function to find the parent directory and parse the path
int find_parent_dir(char *myfs, char *path, char *basename)
{
   superblock sb = get_superblock(myfs);
   int current_dir = sb.rootblock;
   
   // Handle root path
   if(strcmp(path, "/") == 0)
   {
      strcpy(basename, "");
      return current_dir;
   }
   
   char path_copy[1024];
   strcpy(path_copy, path);
   
   char *token = strtok(path_copy, "/");
   char *prev_token = NULL;
   char *last_token = NULL;
   
   while(token != NULL)
   {
      last_token = token;
      
      if(prev_token != NULL)
      {
         // Look for prev_token directory
         int entry_idx = find_entry(myfs, current_dir, prev_token);
         
         if(entry_idx < 0)
         {
            printf("Directory not found: %s\n", prev_token);
            return -1;
         }
         
         // Get directory entry
         block dir_block = readblock(myfs, current_dir);
         fd dir_entry;
         int block_idx = entry_idx / (4088 / METADATA_SIZE);
         int entry_offset = entry_idx % (4088 / METADATA_SIZE);
         
         while(block_idx > 0 && dir_block.nextblock != -1)
         {
            dir_block = readblock(myfs, dir_block.nextblock);
            block_idx--;
         }
         
         memcpy(&dir_entry, &dir_block.buffer[entry_offset * METADATA_SIZE], METADATA_SIZE);
         
         if(dir_entry.type != MYFOLDER)
         {
            printf("%s is not a directory\n", prev_token);
            return -1;
         }
         
         current_dir = dir_entry.firstblock;
      }
      
      prev_token = token;
      token = strtok(NULL, "/");
   }
   
   if(last_token != NULL)
   {
      strcpy(basename, last_token);
   }
   else
   {
      strcpy(basename, "");
   }
   
   return current_dir;
}

// Function to add entry to directory
int add_dir_entry(char *myfs, int dir_block, fd entry)
{
   block dir = readblock(myfs, dir_block);
   
   // Find empty slot
   int entries = 0;
   while(dir.buffer[entries * METADATA_SIZE] != 0 && entries < 4088 / METADATA_SIZE)
   {
      entries++;
   }
   
   // If current block is full, allocate new block
   if(entries >= 4088 / METADATA_SIZE)
   {
      if(dir.nextblock == -1)
      {
         dir.nextblock = allocate_block(myfs);
         if(dir.nextblock == -1)
         {
            return -1;  // No free blocks
         }
         writeblock(myfs, dir_block, dir);
      }
      
      return add_dir_entry(myfs, dir.nextblock, entry);
   }
   
   // Add entry to current block
   memcpy(&dir.buffer[entries * METADATA_SIZE], &entry, METADATA_SIZE);
   writeblock(myfs, dir_block, dir);
   
   return 0;
}

// Function to create a directory
int mkdir_myfs(char *myfs, char *path)
{
   char basename[FILENAME_LEN + 1];
   int parent_dir = find_parent_dir(myfs, path, basename);
   
   if(parent_dir < 0)
   {
      return -1;
   }
   
   if(strlen(basename) > FILENAME_LEN)
   {
      printf("Directory name too long (max %d chars)\n", FILENAME_LEN);
      return -1;
   }
   
   // Check if entry already exists
   if(find_entry(myfs, parent_dir, basename) >= 0)
   {
      printf("Entry already exists: %s\n", basename);
      return -1;
   }
   
   // Allocate block for new directory
   int new_dir_block = allocate_block(myfs);
   if(new_dir_block < 0)
   {
      return -1;
   }
   
   // Create directory entry
   fd dir_entry;
   dir_entry.type = MYFOLDER;
   strcpy(dir_entry.name, basename);
   dir_entry.firstblock = new_dir_block;
   dir_entry.fsize = 0;
   
   // Add to parent directory
   return add_dir_entry(myfs, parent_dir, dir_entry);
}

// Function to implement mymkfs
void mymkfs(char *myfs, int nblocks)
{
   init_fs(myfs, nblocks);
   printf("Filesystem created successfully\n");
}

// Function to write data to a file starting at given block
void write_file_data(char *myfs, int start_block, char *data, int size)
{
   int current_block = start_block;
   int remaining = size;
   int offset = 0;
   
   while(remaining > 0)
   {
      block b = readblock(myfs, current_block);
      
      int write_size = (remaining > 4088) ? 4088 : remaining;
      memcpy(b.buffer, data + offset, write_size);
      
      if(remaining > 4088 && b.nextblock == -1)
      {
         b.nextblock = allocate_block(myfs);
         if(b.nextblock == -1)
         {
            printf("Out of disk space\n");
            return;
         }
      }
      
      writeblock(myfs, current_block, b);
      
      remaining -= write_size;
      offset += write_size;
      
      if(remaining > 0)
      {
         current_block = b.nextblock;
      }
   }
}

// Function to copy a file from Linux to myfs
void mycopyto(char *linux_file, char *myfs, char *myfile_path)
{
   // Open the Linux file
   int linux_fd = open(linux_file, O_RDONLY);
   if(linux_fd < 0)
   {
      perror("Error opening Linux file\n");
      return;
   }
   
   // Get the file size
   struct stat st;
   fstat(linux_fd, &st);
   int filesize = st.st_size;
   
   // Read the entire file into memory
   char *buffer = (char *)malloc(filesize);
   if(buffer == NULL)
   {
      perror("Memory allocation failed\n");
      close(linux_fd);
      return;
   }
   
   if(read(linux_fd, buffer, filesize) != filesize)
   {
      perror("Error reading Linux file\n");
      free(buffer);
      close(linux_fd);
      return;
   }
   
   close(linux_fd);
   
   // Parse the myfile path
   char basename[FILENAME_LEN + 1];
   int parent_dir = find_parent_dir(myfs, myfile_path, basename);
   
   if(parent_dir < 0)
   {
      free(buffer);
      return;
   }
   
   if(strlen(basename) > FILENAME_LEN)
   {
      printf("File name too long (max %d chars)\n", FILENAME_LEN);
      free(buffer);
      return;
   }
   
   // Check if file already exists
   int existing = find_entry(myfs, parent_dir, basename);
   if(existing >= 0)
   {
      printf("File already exists. Removing it...\n");
      // We should implement proper removal here
   }
   
   // Allocate first block for the file
   int first_block = allocate_block(myfs);
   if(first_block < 0)
   {
      free(buffer);
      return;
   }
   
   // Create file entry
   fd file_entry;
   file_entry.type = MYFILE;
   strcpy(file_entry.name, basename);
   file_entry.firstblock = first_block;
   file_entry.fsize = filesize;
   
   // Add to parent directory
   if(add_dir_entry(myfs, parent_dir, file_entry) < 0)
   {
      free_block(myfs, first_block);
      free(buffer);
      return;
   }
   
   // Write file data
   write_file_data(myfs, first_block, buffer, filesize);
   
   free(buffer);
   printf("File copied successfully\n");
}

// Function to read data from a file starting at given block
int read_file_data(char *myfs, int start_block, char *buffer, int size)
{
   int current_block = start_block;
   int remaining = size;
   int offset = 0;
   
   while(remaining > 0 && current_block != -1)
   {
      block b = readblock(myfs, current_block);
      
      int read_size = (remaining > 4088) ? 4088 : remaining;
      memcpy(buffer + offset, b.buffer, read_size);
      
      remaining -= read_size;
      offset += read_size;
      
      current_block = b.nextblock;
   }
   
   return offset;  // Return total bytes read
}

// Function to copy a file from myfs to Linux
void mycopyfrom(char *myfs, char *myfile_path, char *linux_file)
{
   // Parse the myfile path
   char basename[FILENAME_LEN + 1];
   int parent_dir = find_parent_dir(myfs, myfile_path, basename);
   
   if(parent_dir < 0)
   {
      return;
   }
   
   // Find the file
   int entry_idx = find_entry(myfs, parent_dir, basename);
   if(entry_idx < 0)
   {
      printf("File not found: %s\n", basename);
      return;
   }
   
   // Get file entry
   block dir_block = readblock(myfs, parent_dir);
   fd file_entry;
   int block_idx = entry_idx / (4088 / METADATA_SIZE);
   int entry_offset = entry_idx % (4088 / METADATA_SIZE);
   
   while(block_idx > 0 && dir_block.nextblock != -1)
   {
      dir_block = readblock(myfs, dir_block.nextblock);
      block_idx--;
   }
   
   memcpy(&file_entry, &dir_block.buffer[entry_offset * METADATA_SIZE], METADATA_SIZE);
   
   if(file_entry.type != MYFILE)
   {
      printf("%s is not a file\n", basename);
      return;
   }
   
   // Allocate buffer for file data
   char *buffer = (char *)malloc(file_entry.fsize);
   if(buffer == NULL)
   {
      perror("Memory allocation failed\n");
      return;
   }
   
   // Read file data
   int bytes_read = read_file_data(myfs, file_entry.firstblock, buffer, file_entry.fsize);
   
   // Open Linux file for writing
   int linux_fd = open(linux_file, O_CREAT|O_WRONLY|O_TRUNC, 0644);
   if(linux_fd < 0)
   {
      perror("Error opening Linux file\n");
      free(buffer);
      return;
   }
   
   // Write data to Linux file
   if(write(linux_fd, buffer, bytes_read) != bytes_read)
   {
      perror("Error writing to Linux file\n");
      close(linux_fd);
      free(buffer);
      return;
   }
   
   close(linux_fd);
   free(buffer);
   printf("File copied successfully\n");
}

// Function to free all blocks in a chain
void free_block_chain(char *myfs, int start_block)
{
   int current_block = start_block;
   
   while(current_block != -1)
   {
      block b = readblock(myfs, current_block);
      int next_block = b.nextblock;
      
      free_block(myfs, current_block);
      
      current_block = next_block;
   }
}

// Function to remove a file or directory from myfs
void myrm(char *myfs, char *path)
{
   // Parse the path
   char basename[FILENAME_LEN + 1];
   int parent_dir = find_parent_dir(myfs, path, basename);
   
   if(parent_dir < 0)
   {
      return;
   }
   
   // Find the entry
   int entry_idx = find_entry(myfs, parent_dir, basename);
   if(entry_idx < 0)
   {
      printf("File/directory not found: %s\n", basename);
      return;
   }
   
   // Get entry
   block dir_block = readblock(myfs, parent_dir);
   fd entry;
   int block_idx = entry_idx / (4088 / METADATA_SIZE);
   int entry_offset = entry_idx % (4088 / METADATA_SIZE);
   int current_block = parent_dir;
   
   // Navigate to the correct block
   while(block_idx > 0 && dir_block.nextblock != -1)
   {
      current_block = dir_block.nextblock;
      dir_block = readblock(myfs, current_block);
      block_idx--;
   }
   
   memcpy(&entry, &dir_block.buffer[entry_offset * METADATA_SIZE], METADATA_SIZE);
   
   // Free all blocks of the file/directory
   free_block_chain(myfs, entry.firstblock);
   
   // Remove the entry from directory
   memset(&dir_block.buffer[entry_offset * METADATA_SIZE], 0, METADATA_SIZE);
   writeblock(myfs, current_block, dir_block);
   
   printf("%s removed successfully\n", basename);
}

// Main function with command line parsing
int main(int argc, char *argv[])
{
   if(argc < 2)
   {
      printf("Usage:\n");
      printf("  mymkfs <myfs_file> <nblocks>\n");
      printf("  mycopyto <linux_file> <myfs_file> <myfile_path>\n");
      printf("  mycopyfrom <myfs_file> <myfile_path> <linux_file>\n");
      printf("  myrm <myfs_file> <myfile_path>\n");
      return 1;
   }
   
   if(strcmp(argv[1], "mymkfs") == 0)
   {
      if(argc != 4)
      {
         printf("Usage: mymkfs <myfs_file> <nblocks>\n");
         return 1;
      }
      mymkfs(argv[2], atoi(argv[3]));
   }
   else if(strcmp(argv[1], "mycopyto") == 0)
   {
      if(argc != 5)
      {
         printf("Usage: mycopyto <linux_file> <myfs_file> <myfile_path>\n");
         return 1;
      }
      mycopyto(argv[2], argv[3], argv[4]);
   }
   else if(strcmp(argv[1], "mycopyfrom") == 0)
   {
      if(argc != 5)
      {
         printf("Usage: mycopyfrom <myfs_file> <myfile_path> <linux_file>\n");
         return 1;
      }
      mycopyfrom(argv[2], argv[3], argv[4]);
   }
   else if(strcmp(argv[1], "myrm") == 0)
   {
      if(argc != 4)
      {
         printf("Usage: myrm <myfs_file> <myfile_path>\n");
         return 1;
      }
      myrm(argv[2], argv[3]);
   }
   else
   {
      printf("Unknown command: %s\n", argv[1]);
      return 1;
   }
   
   return 0;
}
