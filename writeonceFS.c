#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>

extern int errno;

#define DISK_SIZE 4*1024*1024
#define BLOCK_SIZE 1024

char DISK[DISK_SIZE];

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
    int data_block_start;
} inode;

int wo_mount( char* filename, void* address ) {
    // FILE* ptr;
    int ptr = open(filename, O_RDWR | O_CREAT, 0777);
    if (ptr!= -1) {
        // int size = ftell(ptr);
        // write(ptr, "Not PUMPED\n", strlen("Not PUMPED\n"));
                int currLine = lseek (ptr,0, SEEK_END);

        printf("CurrLine - % d\n", currLine);
        if (currLine == 0) {
            printf("file is empty\n");
            //initialize superblock with number of inodes.
            // ((super_block *)address)->data_start = address+(1024 + 50*1024)+1;
            // address = address + sizeof(int);
            printf("Address = %p\n", address);
            ((super_block *)address)->no_data_block = 4045;
            
            //address = address + sizeof(int);
            printf("Address = %p\n", address);
            ((super_block *)address)->no_inodes = 50;
            //address = address + sizeof(int);
            printf("Address = %p\n", address);
            ((super_block *)address)->latest_inode = 0;
            //address = address + sizeof(int);
            printf("Address = %p\n", address);

        }
        else {
            lseek(ptr,0,SEEK_SET);
            read(ptr, DISK, DISK_SIZE);
            printf("Address = %p\n", &DISK);
            printf("DISK Content = %d\n", ((super_block *)&DISK)->no_data_block);
        }
        close(ptr);
    }
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

int main()
{
    // test();
    wo_mount("disk.txt",&DISK);
    wo_unmount(&DISK);
    wo_mount("disk.txt",&DISK);
    //Write to file

}