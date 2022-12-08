#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>

extern int errno;

#define DISK_SIZE 4*1024*1024
#define BLOCK_SIZE 1024


//SUPER BLOCK | INODE | BITMAP | DATA

char DISK[DISK_SIZE];
char block_map[1024*4];
typedef struct
{
    // Data info
    // int data_start;
    int no_inodes;
    int no_data_block;
    int latest_inode;
} super_block;

typedef struct
{
    int id;
    char filename[200];
    int data_block_start;
    char inUse;
} inode;

// typedef struct
// {
//     char block_map[1024*4];
// }

int wo_mount( char* filename, void* address ) {
    // FILE* ptr;
    int ptr = open(filename, O_RDWR | O_CREAT, 0777);
    printf("FIle dasdadada descriptor = %d\n", ptr);
    if (ptr!= -1) {
        // int size = ftell(ptr);
        // write(ptr, "Not PUMPED\n", strlen("Not PUMPED\n"));
                int currLine = lseek (ptr,0, SEEK_END);

        printf("CurrLine - % d\n", currLine);
        if (currLine == 0) {
            printf("file is empty\n");
            //initialize superblock with number of inodes.
            // ((super_block *)address)->data_start = address+(1024 + 50*1024)+1;
            ((super_block *)address)->no_data_block = 4045;
            ((super_block *)address)->no_inodes = 50;
            ((super_block *)address)->latest_inode = 0;
            
            address = address + 1024;

            for(int i=1;i<=50;i++) {
                ((inode *)address)->id = i;
                ((inode *)address)->inUse = 'n';
                // ((inode *)address)->filename = NULL;
                address = address + 1024;
            }
            
            for(int i=0;i<4096;i++) {
                *(char*)address = 'n';
                address = address+sizeof(char);
            }
            
        }
        else {
            lseek(ptr,0,SEEK_SET);
            printf("READ LINE - %d\n",read(ptr, DISK, DISK_SIZE));
            // printf("Address = %p\n", &DISK);
                    // printf("DISK Content = %d\n", ((inode *)(address + 1024))->id);
        }
        close(ptr);
    }
}

void prettyPrintInodes(void* address ) {
    //superblock
    address = address + 1024;
    for(int i = 1;i<=50;i++) {
        printf("DISK Content = %d\n", ((inode *)address)->id);
        printf("File Name = %s\n", ((inode *)address)->filename);
        address += 1024;
    }
}

void prettyPrintBitMap(void* address ) {
    //superblock
    address = address + 1024 +  1024 * 50;
    for(int i = 1;i<=4096;i++) {
        printf("DISK Content = %c\n", *(char*)address);
        address ++;
    }
}


void prettyPrintSuperBlock(void* address ) {
    //superblock
    printf("======================================SUPER BLOCK ===========================================\n");
    printf("DISK Content = %d\n", ((super_block *)(address))->no_inodes);
    printf("DISK Content = %d\n", ((super_block *)(address))->no_data_block);
    printf("DISK Content = %d\n", ((super_block *)(address))->latest_inode);

}

void test() {
    int ptr = open("disk1.txt", O_RDWR | O_CREAT, 0777);
    char test[3], test1[3];
    test[0] ='a';
    test[1] = 'b';
    test[2] = 'c';
    printf(" fd %d\n", ptr);

    int len = write(ptr, test, sizeof(test));
    printf(" File SIZE = %d\n", lseek(ptr,0,SEEK_SET));
    printf("LEN - %d\n", read(ptr, test1, 3));
    printf("test - %s\n" , test1);
    close(ptr);

    // printf(" here %d\n", len);
}

int wo_unmount(void* address) {
    // FILE* ptr;
    int fd = open("disk.txt", O_RDWR | O_CREAT | O_TRUNC, 0777);

    write(fd, address, DISK_SIZE);
    close(fd);
}

// int wo_read( int <fd>,  void* <buffer>, int <bytes> ) {

// }

// int wo_read( int <fd>,  void* <buffer>, int <bytes> ) {

// }

// int wo_close( int <fd> ){

// }

// int wo_write( int <fd>,  void* <buffer>, int <bytes> ) {
    
// }

// int wo_open( char* <filename>, <flags> ){

// }
int wo_open( char* filename, int flags, char* mode ) {
    char* curr = DISK + 1024;
    char file_name[200];
    strcpy(file_name,filename);
    // printf("=======================DISK ADDRESS - %d\n", ((((super_block *)(DISK)))->no_data_block));
    for(int i=0;i<50;i++) {
        //  printf("FILE IIIIIXXXXX NAME - %s\n", (((inode *)(DISK+((1+i) * 1024)))->filename));
        if(strcmp((((inode *)(curr))->filename), file_name)==0) {
            printf("Can't create! File already exists\n");
            return -1;
        }
        curr = curr + 1024;
    }
    curr = DISK+1024;
    int fileDescriptor = -1;
    for(int i=0;i<50;i++) {
        if(((inode *)curr) ->inUse == 'n')  {
            strcpy(((inode *)curr) ->filename, file_name);
            fileDescriptor = ((inode *)curr) ->id;
            ((inode *)curr) ->inUse == 'y';
            break;
        }
        curr = curr+1024;
    }

    return fileDescriptor;
    // int block_start = -1;
    // for(int i=0;i<4096;i++) {
    //     if(*(char *)curr == 'n') {
    //         block_start = i;
    //         break;
    //     }
    //     curr ++;
    // }
    //address of free data block = curr+(block_start*i);
}
int main()
{
    // test();
    wo_mount("disk.txt",&DISK);
    wo_unmount(&DISK);
    wo_mount("disk.txt",&DISK);
    printf("File Descriptor = %d\n",wo_open("ashwin.txt",1,"current"));
        printf("File Descriptor = %d\n",wo_open("ashwin.txt",1,"current"));

    // printf("File Descriptor = %d\n",wo_open("jayDon.txt",1,"current"));
    prettyPrintInodes(&DISK);

    wo_unmount(&DISK);
    // prettyPrintSuperBlock(&DISK);
    // prettyPrintBitMap(&DISK);
    //Write to file

}