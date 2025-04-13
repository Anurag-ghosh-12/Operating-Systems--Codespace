#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BLOCK_SIZE 4096
#define METADATA_BLOCKS 8
#define DATA_BLOCKS 2048
#define METADATA_SIZE 16
#define FILENAME_LEN 12
int is_text_content(char *, int );
struct Metadata {
  char name[FILENAME_LEN];
  int size;
};

void display_metadata(char *dd) {
    int fd = open(dd, O_RDONLY);
    if (fd < 0) {
        perror("Error opening the file\n");
        exit(EXIT_FAILURE);
    }
    struct Metadata meta;
    printf("Metadata Blocks Information:\n");
    printf("%-6s %-12s %-10s\n", "Block", "Filename", "Size (bytes)");
    printf("---------------------------\n");
    
    for (int i = 0; i < DATA_BLOCKS; i++) {
        lseek(fd, i * METADATA_SIZE, SEEK_SET);
        read(fd, &meta, METADATA_SIZE);
        
        
        if ((int)meta.name[0] == 0) {
            //not printing otherwise very long output
            //printf("%-12s %-10s\n", "empty", "-");
        } else {
            printf("%-6d ", i);
            printf("%-12s %-10d\n", meta.name, meta.size);
        }
    }
    
    close(fd);
}

void display_used_blocks(char *dd) {
    int fd = open(dd, O_RDONLY);
    if (fd < 0) {
        perror("Error opening the file\n");
        exit(EXIT_FAILURE);
    }
    
    struct Metadata meta;
    printf("\nUsed Data Blocks Content:\n");
    printf("=============================================\n");
    
    char buffer[BLOCK_SIZE];
    int used_blocks = 0;
    
    for (int i = 0; i < DATA_BLOCKS; i++) {
        lseek(fd, i * METADATA_SIZE, SEEK_SET);
        read(fd, &meta, METADATA_SIZE);
        
        if ((int)meta.name[0] != 0) {
            used_blocks++;
            
            // Read the actual data block
            lseek(fd, (METADATA_BLOCKS + i) * BLOCK_SIZE, SEEK_SET);
            read(fd, buffer, meta.size);
            
            // Ensure null termination for string printing
            buffer[meta.size] = '\0';
            
            printf("Block %d: %s (%d bytes)\n", i, meta.name, meta.size);
            printf("---------------------------------------------\n");
            
            // For text files, printing as text
            if (is_text_content(buffer, meta.size)) {
                printf("Content:\n%s\n", buffer);
            } else {
                // For binary files, printing hex representation
                printf("Content: Not a Text File \n");
            }
            printf("=============================================\n");
        }
    }
    
    printf("\nTotal used blocks: %d\n", used_blocks);
    close(fd);
}

// Helper function to determine if content is printable text checking from ASCII
int is_text_content(char *buffer, int size) {
    for (int i = 0; i < size; i++) {
        
        if ((buffer[i] < 32 || buffer[i] > 126) && 
            buffer[i] != '\n' && buffer[i] != '\r' && 
            buffer[i] != '\t' && buffer[i] != '\0') {
            return 0; // Not text content
        }
    }
    return 1; // Is text content
}
/*
-------mymkfs () takes dd as the name of the file
       and makes it ready for storing files
       - 8 blocks of metadata and 2048 blocks of data ( containing different
files)
       - each block is of size BLOCK_SIZE i.e. 4096 bytes
*/
void mymkfs(char *dd) {
  int fd = open(dd, O_CREAT | O_WRONLY, 0700);
  if (fd < 0) {
    perror("Error while opening the file\n");
    exit(EXIT_FAILURE);
  }
  char buffer[BLOCK_SIZE];// = {0};
  memset(buffer, '\0', BLOCK_SIZE);
  for (int i = 0; i < METADATA_BLOCKS + DATA_BLOCKS; i++) {
    if (write(fd, buffer, BLOCK_SIZE) < BLOCK_SIZE) {
      perror("Error while allocating blocks\n");
      exit(EXIT_FAILURE);
    } // writing 4096 bytes buffer repetedly in 8+2048 blocks
  }
  close(fd);
  printf("File %s created successfully.\n",dd);
}

void mycopyto(char *linuxfile, char *dd) {
  int fd = open(dd, O_RDWR);
  if (fd < 0) {
    perror("Error opening the device file\n");
    exit(EXIT_FAILURE);
  }
  
  // Checking if the file already exists in our filesystem
  struct Metadata meta;
  for (int i = 0; i < DATA_BLOCKS; i++) {
    lseek(fd, i * METADATA_SIZE, SEEK_SET);
    read(fd, &meta, METADATA_SIZE);
    
    if ((int)meta.name[0] != 0 && strcmp(meta.name, linuxfile) == 0) {
      printf("Error: File '%s' already exists in the filesystem.\n", linuxfile);
      close(fd);
      exit(EXIT_FAILURE);
    }
  }
  
  // Check file size using stat
  struct stat file_stat;
  if (stat(linuxfile, &file_stat) != 0) {
    perror("Error getting file stats\n");
    close(fd);
    exit(EXIT_FAILURE);
  }
  
  if (file_stat.st_size > BLOCK_SIZE) {
    printf("Error: File size (%ld bytes) exceeds maximum allowed size (%d bytes).\n", 
           file_stat.st_size, BLOCK_SIZE);
    close(fd);
    exit(EXIT_FAILURE);
  }
  
  int lf = open(linuxfile, O_RDONLY);
  if (lf < 0) {
    perror("Error opening the source file\n");
    close(fd);
    exit(EXIT_FAILURE);
  }
  
  int index = -1;
  for (int i = 0; i < DATA_BLOCKS; i++) {
    lseek(fd, i * METADATA_SIZE, SEEK_SET);
    read(fd, &meta, METADATA_SIZE);
    if ((int)meta.name[0] == 0) {
      index = i;
      break;
    }
  }
  
  if (index == -1) {
    printf("The file system is full. Please try later.\n");
    close(lf);
    close(fd);
    exit(EXIT_FAILURE);
  }
  
  // Copy the linux file to a buffer
  char buffer[BLOCK_SIZE] = {0};
  int bytes_read = read(lf, &buffer, BLOCK_SIZE);
  strncpy(meta.name, linuxfile, FILENAME_LEN);
  meta.size = bytes_read;
  
  // Modify the metadata block
  lseek(fd, index * METADATA_SIZE, SEEK_SET);
  write(fd, &meta, METADATA_SIZE);
  
  // Modify the data block
  lseek(fd, (METADATA_BLOCKS + index) * BLOCK_SIZE, SEEK_SET);
  int bytes = write(fd, buffer, bytes_read);
  
  printf("Copied linux file successfully to mymkfs file at data block %d\n", index);
  printf("File size: %d bytes\n", bytes_read);
  
  close(lf);
  close(fd);
}

void mycopyfrom(char *linuxfile, char *dd) {
  int fd = open(dd, O_RDONLY);
  if (fd < 0) {
    perror("Error opening the file\n");
    exit(EXIT_FAILURE);
  }

  int index = -1;
  struct Metadata meta;
  for (int i = 0; i < DATA_BLOCKS; i++) {
    lseek(fd, i * METADATA_SIZE, SEEK_SET);
    read(fd, &meta, METADATA_SIZE);
    if (strncmp(meta.name, linuxfile,12) == 0) { // the file names matched !!
      index = i;
      break;
    }
  }

  if (index == -1) {
    printf("No such file with %s name exists." ,linuxfile);
    close(fd);
    exit(EXIT_FAILURE);
  }
  char buffer[BLOCK_SIZE] = {0};
  lseek(fd, (METADATA_BLOCKS + index)*BLOCK_SIZE, SEEK_SET);
  int bytes_read = read(fd, buffer, BLOCK_SIZE);

  int lf = open(linuxfile, O_CREAT | O_WRONLY, 0666);
  if (lf < 0) {
    perror("Error opening the file\n");
    close(fd); // as it is no longer required
    exit(EXIT_FAILURE);
  }
  write(lf, buffer, meta.size);
  printf("File %s copied from %s.\n", linuxfile, dd);

  close(fd);
  close(lf);
}

void myrm(char *linuxfile, char * dd)
{
  int fd = open(dd, O_RDWR);
  if (fd < 0) {
    perror("Error opening the file\n");
    exit(EXIT_FAILURE);
  }

  int index = -1;
  struct Metadata meta;
  for (int i = 0; i < DATA_BLOCKS; i++) {
    lseek(fd, i * METADATA_SIZE, SEEK_SET);
    read(fd, &meta, METADATA_SIZE);
    if (strncmp(meta.name, linuxfile ,12) == 0) { // the file names matched !!
      index = i;
      break;
    }
  }

  if (index == -1) {
    printf("No such file with %s name exists.", linuxfile);
    close(fd);
    exit(EXIT_FAILURE);
  }
  
  lseek(fd,index*METADATA_SIZE,SEEK_SET);
  memset(&meta,0,METADATA_SIZE);
  write(fd,&meta,METADATA_SIZE);
  close(fd);
  
  printf("Successfully removed file and modified metadata sub-block%d\n",index);
}
int main(int argc, char* argv[])
{
    if (argc < 3) {
        printf("Usage: ./myfs <command> <args>\n");
        printf("commands are mymkfs, mycopyTo, mycopyFrom, myrm, showMetadata, showUsedBlocks\n");
        return 1;
    }
    
    if (strcmp(argv[1], "mymkfs") == 0) {
        mymkfs(argv[2]);
    } 
    else if (strcmp(argv[1], "mycopyTo") == 0) 
    {
        if (argc != 4) 
        {
            printf("Usage:  mycopyTo <linux file> <dd1>\n");
            return 1;
        }
        mycopyto(argv[2], argv[3]);
    } 
    else if (strcmp(argv[1], "mycopyFrom") == 0)
    {
    if (argc != 3) 
    {
        printf("Usage: mycopyFrom <linuxfile@dd1>\n");
        return 1;
    }
    
    char parameter[256];
    strcpy(parameter, argv[2]); // Make a copy since strtok modifies the string
    
    char *linuxfile = strtok(parameter, "@");
    char *dd1 = strtok(NULL, "@");
    
    if (linuxfile == NULL || dd1 == NULL) 
    {
        printf("Error: Invalid format. Use <linuxfile@dd1>\n");
        return 1;
    }
    
    mycopyfrom(linuxfile, dd1);
	}
	else if (strcmp(argv[1], "myrm") == 0)
	{
	    if (argc != 3) 
	    {
		printf("Usage: myrm <linuxfile@dd1>\n");
		return 1;
	    }
	    
	    char parameter[256];
	    strcpy(parameter, argv[2]); // Make a copy since strtok modifies the string
	    
	    char *linuxfile = strtok(parameter, "@");
	    char *dd1 = strtok(NULL, "@");
	    
	    if (linuxfile == NULL || dd1 == NULL) 
	    {
		printf("Error: Invalid format. Use <linuxfile@dd1>\n");
		return 1;
	    }
	    
	    myrm(linuxfile, dd1);
	}
    else if (strncmp(argv[1], "showMetadata", 12) == 0) 
    {
        if (argc != 3) 
        {
            printf("Usage: showMetadata <dd1>\n");
            return 1;
        }
        display_metadata(argv[2]);
    }
    else if (strncmp(argv[1], "showUsedBlocks", 14) == 0) 
    {
        if (argc != 3) 
        {
            printf("Usage: showUsedBlocks <dd1>\n");
            return 1;
        }
        display_used_blocks(argv[2]);
    }
    else
    {
       printf("Invalid command !!!");
       return 1;
    }
    return 0;
}
