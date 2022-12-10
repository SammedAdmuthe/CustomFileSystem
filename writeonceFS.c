#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

extern int errno;

#define DISK_SIZE 4 * 1024 * 1024
#define BLOCK_SIZE 1024

// SUPER BLOCK | INODE | BITMAP | DATA

char* DISK;
char block_map[1024 * 4];
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
    // Data info
    // int data_start;
    short next;
    char data[1020];

} data_block;

typedef struct
{
    int id;
    char filename[200];
    int total_size_of_file;
    int data_block_start;
    char inUse;
    int permission;
} inode;

// typedef struct
// {
//     char block_map[1024*4];
// }

int wo_mount(char *filename, void *address)
{
    // FILE* ptr;
    DISK = address;
    int ptr = open(filename, O_RDWR | O_CREAT, 0777);
    if (ptr != -1)
    {
        // int size = ftell(ptr);
        // write(ptr, "Not PUMPED\n", strlen("Not PUMPED\n"));
        int currLine = lseek(ptr, 0, SEEK_END);

        printf("CurrLine - % d\n", currLine);
        if (currLine == 0)
        {
            printf("file is empty\n");
            // initialize superblock with number of inodes.
            //  ((super_block *)address)->data_start = address+(1024 + 50*1024)+1;
            ((super_block *)address)->no_data_block = 4045;
            ((super_block *)address)->no_inodes = 50;
            ((super_block *)address)->latest_inode = 0;

            address = address + 1024;

            for (int i = 1; i <= 50; i++)
            {
                ((inode *)address)->id = i;
                ((inode *)address)->inUse = 'n';
                ((inode *)address)->data_block_start = -1;
                ((inode *)address)->total_size_of_file = 0;
                // ((inode *)address)->filename = NULL;
                address = address + 1024;
            }

            for (int i = 0; i < 4096; i++)
            {
                *(char *)address = 'n';
                address = address + sizeof(char);
            }
            // initialize data block
            for (int i = 0; i < 4041; i++)
            {
                ((data_block *)address)->next = -1;
                address = address + 1024;
            }
        }
        else
        {
            lseek(ptr, 0, SEEK_SET);
            printf("READ LINE - %d\n", read(ptr, DISK, DISK_SIZE));
            // printf("Address = %p\n", &DISK);
            // printf("DISK Content = %d\n", ((inode *)(address + 1024))->id);
        }
        close(ptr);
    }
}

void prettyPrintInodes(void *address)
{
    // superblock
    address = address + 1024;
    for (int i = 1; i <= 50; i++)
    {
        printf("DISK Content = %d\n", ((inode *)address)->id);
        printf("File Name = %s\n", ((inode *)address)->filename);
        printf("In USE %c\n", ((inode *)address)->inUse);
        printf("In File Size %d\n", ((inode *)address)->total_size_of_file);
        address += 1024;
    }
}

void prettyPrintBitMap(void *address)
{
    // superblock
    address = address + 1024 + 1024 * 50;
    for (int i = 1; i <= 4096; i++)
    {
        printf("DISK Content = %c\n", *(char *)address);
        address++;
    }
}

void prettyPrintSuperBlock(void *address)
{
    // superblock
    printf("======================================SUPER BLOCK ===========================================\n");
    printf("DISK Content = %d\n", ((super_block *)(address))->no_inodes);
    printf("DISK Content = %d\n", ((super_block *)(address))->no_data_block);
    printf("DISK Content = %d\n", ((super_block *)(address))->latest_inode);
}

void test()
{
    int ptr = open("disk1.txt", O_RDWR | O_CREAT, 0777);
    char test[3], test1[3];
    test[0] = 'a';
    test[1] = 'b';
    test[2] = 'c';
    printf(" fd %d\n", ptr);

    int len = write(ptr, test, sizeof(test));
    printf(" File SIZE = %d\n", lseek(ptr, 0, SEEK_SET));
    printf("LEN - %d\n", read(ptr, test1, 3));
    printf("test - %s\n", test1);
    close(ptr);

    // printf(" here %d\n", len);
}

int wo_unmount(void *address)
{
    // FILE* ptr;
    int fd = open("disk.txt", O_RDWR | O_CREAT | O_TRUNC, 0777);

    write(fd, address, DISK_SIZE);
    close(fd);
}

char* my_strncat(char *dest, char *src, size_t n)
{
    size_t i;
    // printf(" SRC = %s \n",src);
    //&& src[i] != '\0'
    for (i = 0; i < n && src[i] != '\0' ; i++)
        dest[i] = src[i];
    dest[n] = '\0';    
        // printf(" DEST AFTER = %s \n",dest);

    return dest;
}

int getMin(int num1, int num2) {
    return num1 > num2 ? num2 : num1;
}

int wo_read( int fd,  void* buffer, int bytes ) {
    void* curr = DISK + 1024;
    // buffer = (char*)malloc(bytes);
    int indx = 0;
    int fdFound = -1;
    char* checkTest = (char*)buffer;
    for(int i=0;i<50;i++) {
        if(((inode *)curr)->id == fd) {
            fdFound = fd;
            //File found start reading!!
            int start = ((inode *)curr)->data_block_start;
            char* head = DISK + 1024 + 1024 * 50 + 4 * 1024 + 1024 * (start);
            int dptr = 0;
            int inodeFileSize = ((inode *)curr)->total_size_of_file;
            int count = getMin(bytes, inodeFileSize);

            //What if first block's next == -1 i.e file is empty.
            // while(((data_block *)head)->next == -1) {
                int temp =0;
                while(count>0) {
                    my_strncat((char* )buffer, ((data_block *)head)->data, getMin(count, 1020));
                    // printf(" HEAD DATA = %s\n", ((data_block *)head)->data);
                    
                    printf("========================================================\n");
                    printf("BUFFER KONTENT= %s\n",buffer);
                    printf("========================================================\n");
                    buffer = buffer + getMin(count, 1020);
                    
                    count-= getMin(count, 1020);

                    // if(((data_block *)head)->next == -1) {
                    //     break;
                    // }
                    temp++;
                    head = DISK + 1024 + 1024 * 50 + 4 * 1024 + 1024 * (((data_block *)head)->next);
                }
                
                printf("TEMP = %d\n", temp);
                printf("TEMPBYTES = %d\n", bytes);
                // char* addr = ((data_block *)head)->data;
                // while(indx < bytes) {
                //     buffer[indx] = addr[dptr++];
                //     indx++;
                // }
            // }
            // while(count > 0)
            // {
            //     my_strncat((char* )buffer, ((data_block *)head)->data, (count >= 1020? 1020: count));
            //     count-= (count >= 1020? 1020: count);
            // }
                // printf(" File content buffer = %s\n", buffer);
            
            return bytes;
        }
        
        curr+=1024;
    }
    if(fdFound == -1) {
        printf("File Not Found !!\n");
        return -1;
    }
    return bytes;
}

// int wo_close( int <fd> ){

// }
short getFreeDataBlockIndx()
{
    short target_data_block = -1;
    char *data_itr = DISK + 1024 + (50 * 1024);
    for (short j = 0; j < 4041; j++)
    {
        char c = *(char *)data_itr;
        if (c == 'n')
        {
            *(char *)data_itr = 'y';
            target_data_block = j;
            break;
        }
        data_itr++;
    }

    return target_data_block;
}
int wo_write(int fd, void *buffer, int bytes)
{
    char *curr = DISK + 1024;
    short target_data_block = -1;
    char tempBuffer[bytes];
    strcpy(tempBuffer, buffer);
    for (int i = 0; i < 50; i++)
    {
        //  printf("FILE IIIIIXXXXX NAME - %s\n", (((inode *)(DISK+((1+i) * 1024)))->filename));
        if (((inode *)(curr))->id == fd)
        {
            if (((inode *)(curr))->data_block_start == -1)
            {
                // char* data_itr = DISK + 1024 + (50 * 1024);
                //  for(int j=0;j<4041;j++) {
                //      char c = *(char *)data_itr;
                //      printf("c = %c", c);
                //      if(c == 'n') {
                //          target_data_block = j;
                //          break;
                //      }
                //      data_itr ++;
                //  }
                int count = bytes;
                // char doOnlyOnce = 't';
                int previous_data_block = -1;
                int howManyTimes = bytes / 1020;
                target_data_block = getFreeDataBlockIndx();
                if (target_data_block != -1)
                {
                    previous_data_block = target_data_block;
                    char *free_data_block = DISK + 1024 + (50 * 1024) + (4 * 1024) + (target_data_block * 1024);
                    strncpy(((data_block *)free_data_block)->data, (char *)tempBuffer, getMin(count, 1020));
                    printf("===============DATA =================\n %s\n",((data_block *)free_data_block)->data);
                    ((inode *)(curr))->data_block_start = target_data_block;
                    ((inode *)(curr))->total_size_of_file += getMin(count, 1020);
                    printf("Total FILE SIZE1 = %d\n", ((inode *)(curr))->total_size_of_file);
                    count-=getMin(count, 1020);
                }
                else
                {
                    printf("No free data block available!\n");
                    return -1;
                }
                if (howManyTimes > 0)
                {

                    for (int x = 0; x < howManyTimes && count > 0; x++)
                    {
                        target_data_block = getFreeDataBlockIndx();
                        if (target_data_block != -1)
                        {
                            char *prev_free_data_block = DISK + 1024 + (50 * 1024) + (4 * 1024) + (previous_data_block * 1024);
                            ((data_block *)prev_free_data_block)->next = target_data_block;
                            previous_data_block = target_data_block;
                            char *free_data_block = DISK + 1024 + (50 * 1024) + (4 * 1024) + (target_data_block * 1024);
                            strncpy(((data_block *)free_data_block)->data, (char *)tempBuffer + ((x + 1) * 1020), getMin(count, 1020));
                            ((inode *)(curr))->total_size_of_file += getMin(count, 1020);
                            printf("Total FILE SIZE2 = %d\n", ((inode *)(curr))->total_size_of_file);
                            ((data_block *)free_data_block)->next = -1;
                        }
                        else
                        {
                            printf("No free data block available!\n");
                            return -1;
                        }
                        
                        count -= 1020;
                    }
                }

                // while(count > 0) {
                //     target_data_block = getFreeDataBlockIndx();
                //     if(target_data_block!=-1){
                //         char* free_data_block = DISK + 1024 + (50 * 1024) + (4*1024) + (target_data_block * 1024)+8;
                //         strncpy(((data_block *)free_data_block)->data, (char *)tempBuffer, 1016);
                //         if(doOnlyOnce == 't'){
                //             ((inode *)(curr))->data_block_start = free_data_block-8;
                //             doOnlyOnce = 'f';
                //         }
                //         count-=1016;
                //     }else{
                //         printf("No free data block available!\n");
                //         return -1;
                //     }
                // }
                // target_data_block = getFreeDataBlockIndx();
                // if(target_data_block != -1) {
                //     char* free_data_block = DISK + 1024 + (50 * 1024) + (4*1024) + (target_data_block * 1024)+8;
                //     strncpy(((data_block *)free_data_block)->data, (char *)buffer, bytes);
                //     ((inode *)(curr))->data_block_start = free_data_block;
                //     // printf("DATA = %s\n", ((data_block *)free_data_block)->data);
                // }
                // else {
                //     printf("HERERERERERERERER\n");
                // }
            }
            else
            {
                // go to location where we left writing first time
                // traverse to the correct data block
                printf("BYTES = %d\n", bytes);
                int file_size = 0;
                int previous_data_block = -1;
                file_size = (((inode *)curr)->total_size_of_file);
                void *data_itr = DISK + 1024 + 1024 * 50 + 1024 * 4 + (((inode *)curr)->data_block_start) * 1024;
                // Iterate Linked List
                //printf("hehe : %d\n",((data_block *)(data_itr))->next);
                while (((data_block *)(data_itr))->next != -1)
                {
                    //printf("lol");
                    //previous_data_block = ((data_block *)(data_itr))->next;
                    file_size -= 1020;
                    data_itr = DISK + 1024 + (1024 * 50) + (1024 * 4) + (((data_block *)(data_itr))->next) * 1024;
                }
                //printf("Conctent : %s\n",((data_block*)data_itr)->data);
                // data_itr= data_itr + (file_size)+4;
                // data_itr may point to full or partially filled block.
                char tempContent[1020];
                int remaining_size = 1020 - file_size;
                int count = bytes;
                printf("File Size : %d\n",file_size);
                strncat(((data_block *)data_itr)->data,tempBuffer, getMin(count, remaining_size));
                printf("##33333333333333# %d , %d\n",count, remaining_size);
                ((inode *)(curr))->total_size_of_file += getMin(count, remaining_size);
                printf("Total FILE SIZE3 = %d\n", ((inode *)(curr))->total_size_of_file);
                count-= getMin(count, remaining_size);
                // if (remaining_size > 0)
                // {
                //     //((count <= remaining_size)?count:remaining_size)
                    
                //     printf("Are we here?\n");
                //     strncat(tempContent,tempBuffer, size);
                //     printf("Are we here?\n");
                //     printf("File + File size : %d\n",file_size+size);
                //     strncpy(((data_block *)data_itr)->data,(char*)tempContent,(size+file_size));
                //     printf("File + File size : %d\n",file_size+size);
                //     count-= ((count <= remaining_size)?count:remaining_size);
                //     ((inode *)(curr))->total_size_of_file += size;
                // }
                
                if(count <= 0){
                    return bytes;
                }

                int howManyTimes = count / 1020;
                target_data_block = getFreeDataBlockIndx();
                //char *data = DISK + 1024 + (50 * 1024) + (4 * 1024) + (target_data_block * 1024);
                ((data_block *)data_itr)->next = target_data_block;

                if (target_data_block != -1)
                {
                    previous_data_block = target_data_block;
                    char *free_data_block = DISK + 1024 + (50 * 1024) + (4 * 1024) + (target_data_block * 1024);
                    strncpy(((data_block *)free_data_block)->data, (char *)tempBuffer+remaining_size, getMin(count, 1020));
                    //((inode *)(curr))->data_block_start = target_data_block;
                    ((inode *)(curr))->total_size_of_file += getMin(count, 1020);
                    printf("Total FILE SIZE4 = %d\n", ((inode *)(curr))->total_size_of_file);
                    // count-=1016;
                }
                else
                {
                    printf("No free data block available!\n");
                    return -1;
                }
                if (howManyTimes > 0)
                {

                    for (int x = 0; x < howManyTimes; x++)
                    {
                        count -= 1020;
                        target_data_block = getFreeDataBlockIndx();
                        if (target_data_block != -1)
                        {
                            char *prev_free_data_block = DISK + 1024 + (50 * 1024) + (4 * 1024) + (previous_data_block * 1024);
                            ((data_block *)prev_free_data_block)->next = target_data_block;
                            previous_data_block = target_data_block;
                            char *free_data_block = DISK + 1024 + (50 * 1024) + (4 * 1024) + (target_data_block * 1024);
                            strncpy(((data_block *)free_data_block)->data, (char *)tempBuffer+remaining_size + ((x + 1) * 1020), getMin(count, 1020));
                            ((inode *)(curr))->total_size_of_file += getMin(count, 1020);
                            printf("Total FILE SIZE5 = %d\n", ((inode *)(curr))->total_size_of_file);
                        }
                        else
                        {
                            printf("No free data block available!\n");
                            return -1;
                        }
                    }
                }
                printf("Returnning bytes %d \n",bytes);
                return bytes;
                
            }
            break;
        }
        curr = curr + 1024;
    }
    return bytes;
}

// int wo_open( char* <filename>, <flags> ){

// }
int wo_open(char *filename, int flags, char *mode)
{
    char *curr = DISK + 1024;
    char file_name[200];
    strcpy(file_name, filename);
    // printf("=======================DISK ADDRESS - %d\n", ((((super_block *)(DISK)))->no_data_block));
    for (int i = 0; i < 50; i++)
    {
        //  printf("FILE IIIIIXXXXX NAME - %s\n", (((inode *)(DISK+((1+i) * 1024)))->filename));
        if (strcmp((((inode *)(curr))->filename), file_name) == 0)
        {
            printf("Can't create! File already exists\n");
            return -1;
        }
        curr = curr + 1024;
    }
    curr = DISK + 1024;
    int fileDescriptor = -1;
    for (int i = 0; i < 50; i++)
    {
        if (((inode *)curr)->inUse == 'n')
        {
            strcpy(((inode *)curr)->filename, file_name);
            fileDescriptor = ((inode *)curr)->id;
            ((inode *)curr)->inUse = 'y';
            break;
        }
        curr = curr + 1024;
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
    // address of free data block = curr+(block_start*i);
}
int main()
{

    // test();
    char* c = (char*)malloc(DISK_SIZE);
    wo_mount("disk.txt", c);
        wo_unmount(c);

        wo_mount("disk.txt", c);
    int fd = wo_open("ashwin.txt", 1, "current");
    // printf("File Descriptor = %d\n",wo_open("ashwin.txt",1,"current"));

    // printf("File Descriptor = %d\n",wo_open("jayDon.txt",1,"current"));
    // prettyPrintInodes(&DISK);
    //wo_write(fd, "7878756705 1872788848 1869294709 1612511154 9173981851 2132050263 6263897800 2956495174 6822183361 3590392894 6340530864 0499311875 3251064666 2879074650 2783400415 2503024167 2014126294 1736767580 1917339971 4590605251 3563201440 9684122570 5209227966 1426346417 2713378212 1462380458 4235474007 7108232323 9015031133 2560846752 5836258997 9992262986 2221262378 1605474260 8409596018 5685453537 5031873689 2258769781 2545624714 2255890847 0505732741 3782460769 1367345304 8143852954 4685311156 0058508494 2373780014 0277937325 8825058401 7132746705 0380860351 1046724216 9709000501 8094071249 5273465501 4043870340 2717616891 3501501280 6797559641 8939672729 9219553768 0969466036 2627207117 7954265572 7459201469 2997404549 9372363759 3480842599 0482452843 9691253699 2495163464 4887898998 2210733518 5656946606 0088660418 4926216645 9888479073 5129132945 5498637400 8952027811 2461526051 3914870450 1080601120 5355036820 1682085478 9136638949 6989025866 0732249176 1516700029 4917998081 3159278554 2613282188 1571678830 0401507282 2724695435 6453193833 1803509611 6389222224 8766928115 0493738476",sizeof("7878756705 1872788848 1869294709 1612511154 9173981851 2132050263 6263897800 2956495174 6822183361 3590392894 6340530864 0499311875 3251064666 2879074650 2783400415 2503024167 2014126294 1736767580 1917339971 4590605251 3563201440 9684122570 5209227966 1426346417 2713378212 1462380458 4235474007 7108232323 9015031133 2560846752 5836258997 9992262986 2221262378 1605474260 8409596018 5685453537 5031873689 2258769781 2545624714 2255890847 0505732741 3782460769 1367345304 8143852954 4685311156 0058508494 2373780014 0277937325 8825058401 7132746705 0380860351 1046724216 9709000501 8094071249 5273465501 4043870340 2717616891 3501501280 6797559641 8939672729 9219553768 0969466036 2627207117 7954265572 7459201469 2997404549 9372363759 3480842599 0482452843 9691253699 2495163464 4887898998 2210733518 5656946606 0088660418 4926216645 9888479073 5129132945 5498637400 8952027811 2461526051 3914870450 1080601120 5355036820 1682085478 9136638949 6989025866 0732249176 1516700029 4917998081 3159278554 2613282188 1571678830 0401507282 2724695435 6453193833 1803509611 6389222224 8766928115 0493738476"));
    //wo_write(fd, "7878756705 1872788848 1869294709 1612511154 9173981851 2132050263 6263897800 2956495174 6822183361 3590392894 6340530864 0499311875 3251064666 2879074650 2783400415 2503024167 2014126294 1736767580 1917339971 4590605251 3563201440 9684122570 5209227966 1426346417 2713378212 1462380458 4235474007 7108232323 9015031133 2560846752 5836258997 9992262986 2221262378 1605474260 8409596018 5685453537 5031873689 2258769781 2545624714 2255890847 0505732741 3782460769 1367345304 8143852954 4685311156 0058508494 2373780014 0277937325 8825058401 7132746705 0380860351 1046724216 9709000501 8094071249 5273465501 4043870340 2717616891 3501501280 6797559641 8939672729 9219553768 0969466036 2627207117 7954265572 7459201469 2997404549 9372363759 3480842599 0482452843 9691253699 2495163464 4887898998 2210733518 5656946606 0088660418 4926216645 9888479073 5129132945 5498637400 8952027811 2461526051 3914870450 1080601120 5355036820 1682085478 9136638949 6989025866 0732249176 1516700029 4917998081 3159278554 2613282188 1571678830 0401507282 2724695435 6453193833 1803509611 6389222224 8766928115 0493738476",sizeof("7878756705 1872788848 1869294709 1612511154 9173981851 2132050263 6263897800 2956495174 6822183361 3590392894 6340530864 0499311875 3251064666 2879074650 2783400415 2503024167 2014126294 1736767580 1917339971 4590605251 3563201440 9684122570 5209227966 1426346417 2713378212 1462380458 4235474007 7108232323 9015031133 2560846752 5836258997 9992262986 2221262378 1605474260 8409596018 5685453537 5031873689 2258769781 2545624714 2255890847 0505732741 3782460769 1367345304 8143852954 4685311156 0058508494 2373780014 0277937325 8825058401 7132746705 0380860351 1046724216 9709000501 8094071249 5273465501 4043870340 2717616891 3501501280 6797559641 8939672729 9219553768 0969466036 2627207117 7954265572 7459201469 2997404549 9372363759 3480842599 0482452843 9691253699 2495163464 4887898998 2210733518 5656946606 0088660418 4926216645 9888479073 5129132945 5498637400 8952027811 2461526051 3914870450 1080601120 5355036820 1682085478 9136638949 6989025866 0732249176 1516700029 4917998081 3159278554 2613282188 1571678830 0401507282 2724695435 6453193833 1803509611 6389222224 8766928115 0493738476"));
    //wo_write(fd, "7878756705 1872788848 1869294709 1612511154 9173981851 2132050263 6263897800 2956495174 6822183361 3590392894 6340530864 0499311875 3251064666 2879074650 2783400415 2503024167 2014126294 1736767580 1917339971 4590605251 3563201440 9684122570 5209227966 1426346417 2713378212 1462380458 4235474007 7108232323 9015031133 2560846752 5836258997 9992262986 2221262378 1605474260 8409596018 5685453537 5031873689 2258769781 2545624714 2255890847 0505732741 3782460769 1367345304 8143852954 4685311156 0058508494 2373780014 0277937325 8825058401 7132746705 0380860351 1046724216 9709000501 8094071249 5273465501 4043870340 2717616891 3501501280 6797559641 8939672729 9219553768 0969466036 2627207117 7954265572 7459201469 2997404549 9372363759 3480842599 0482452843 9691253699 2495163464 4887898998 2210733518 5656946606 0088660418 4926216645 9888479073 5129132945 5498637400 8952027811 2461526051 3914870450 1080601120 5355036820 1682085478 9136638949 6989025866 0732249176 1516700029 4917998081 3159278554 2613282188 1571678830 0401507282 2724695435 6453193833 1803509611 6389222224 8766928115 0493738476",sizeof("7878756705 1872788848 1869294709 1612511154 9173981851 2132050263 6263897800 2956495174 6822183361 3590392894 6340530864 0499311875 3251064666 2879074650 2783400415 2503024167 2014126294 1736767580 1917339971 4590605251 3563201440 9684122570 5209227966 1426346417 2713378212 1462380458 4235474007 7108232323 9015031133 2560846752 5836258997 9992262986 2221262378 1605474260 8409596018 5685453537 5031873689 2258769781 2545624714 2255890847 0505732741 3782460769 1367345304 8143852954 4685311156 0058508494 2373780014 0277937325 8825058401 7132746705 0380860351 1046724216 9709000501 8094071249 5273465501 4043870340 2717616891 3501501280 6797559641 8939672729 9219553768 0969466036 2627207117 7954265572 7459201469 2997404549 9372363759 3480842599 0482452843 9691253699 2495163464 4887898998 2210733518 5656946606 0088660418 4926216645 9888479073 5129132945 5498637400 8952027811 2461526051 3914870450 1080601120 5355036820 1682085478 9136638949 6989025866 0732249176 1516700029 4917998081 3159278554 2613282188 1571678830 0401507282 2724695435 6453193833 1803509611 6389222224 8766928115 0493738476"));
    //wo_write(fd, "7878756705 1872788848 1869294709 1612511154 9173981851 2132050263 6263897800 2956495174 6822183361 3590392894 6340530864 0499311875 3251064666 2879074650 2783400415 2503024167 2014126294 1736767580 1917339971 4590605251 3563201440 9684122570 5209227966 1426346417 2713378212 1462380458 4235474007 7108232323 9015031133 2560846752 5836258997 9992262986 2221262378 1605474260 8409596018 5685453537 5031873689 2258769781 2545624714 2255890847 0505732741 3782460769 1367345304 8143852954 4685311156 0058508494 2373780014 0277937325 8825058401 7132746705 0380860351 1046724216 9709000501 8094071249 5273465501 4043870340 2717616891 3501501280 6797559641 8939672729 9219553768 0969466036 2627207117 7954265572 7459201469 2997404549 9372363759 3480842599 0482452843 9691253699 2495163464 4887898998 2210733518 5656946606 0088660418 4926216645 9888479073 5129132945 5498637400 8952027811 2461526051 3914870450 1080601120 5355036820 1682085478 9136638949 6989025866 0732249176 1516700029 4917998081 3159278554 2613282188 1571678830 0401507282 2724695435 6453193833 1803509611 6389222224 8766928115 0493738476",sizeof("7878756705 1872788848 1869294709 1612511154 9173981851 2132050263 6263897800 2956495174 6822183361 3590392894 6340530864 0499311875 3251064666 2879074650 2783400415 2503024167 2014126294 1736767580 1917339971 4590605251 3563201440 9684122570 5209227966 1426346417 2713378212 1462380458 4235474007 7108232323 9015031133 2560846752 5836258997 9992262986 2221262378 1605474260 8409596018 5685453537 5031873689 2258769781 2545624714 2255890847 0505732741 3782460769 1367345304 8143852954 4685311156 0058508494 2373780014 0277937325 8825058401 7132746705 0380860351 1046724216 9709000501 8094071249 5273465501 4043870340 2717616891 3501501280 6797559641 8939672729 9219553768 0969466036 2627207117 7954265572 7459201469 2997404549 9372363759 3480842599 0482452843 9691253699 2495163464 4887898998 2210733518 5656946606 0088660418 4926216645 9888479073 5129132945 5498637400 8952027811 2461526051 3914870450 1080601120 5355036820 1682085478 9136638949 6989025866 0732249176 1516700029 4917998081 3159278554 2613282188 1571678830 0401507282 2724695435 6453193833 1803509611 6389222224 8766928115 0493738476"));
    // wo_write(fd,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaX" , sizeof("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
    // wo_write(fd,"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb" , sizeof("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
    // wo_write(fd,"svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX" , sizeof("svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX"));    
    // wo_write(fd,"svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX",sizeof("svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX"));

    wo_write(fd, "wvumxwqhpr vjywxxzjws fvyfydbjdd umxduvnoyc glskrgqemx bkiogtrkca uxlypfdobw yelzyofmxl vysndgjwlb prpylsbxgr lvpcbildvp pmjhupxmqq nydkmaysbb wiwiefbjth lkuhjahvas xxbxivogen pdsdqvrztz vkgfxepbnl cfucbwrrvp zinujpwxvr wvjwmjegws euiigjklxx kbneondgfl idvlkliagn efqjdqkrrh ranhfsmzqq ezpxfnddpt wadqvltlrs sijoajindx zkodcryiyi iqufdzxmic sqcmkbtvej tvyngzfmfr asnfdbiimk ajinaavraw gprbanubxy emcjlwufgw ydpsypklui hbildwxrzs phmejtcyxc gnlxedgdfu iuytmyirkx dowdueensn hjfxdawzom zwleshnivb jymjqtqqcu yesastidhj tsdkatdnll puogcvawvs qugaopjwjx tjqzkqwvwb wcravlquuv mcrkuudcec iccufyqzrc ygtadfmsbl epamotrgqq khqksespsq mvuscypxgs zskogpzgfr gbbdvuvddx wfyhfwzcjc ifuejhqhqh zdfopydxqi wtqvtlmvhn unseguggap xxyzcjomwp troxoquvkd khmmlmzhnb onxkvwpsor mshpdynqtu cuhiggrnqk cbawnxwnfn agpztyflpo jbbvbvzlfy neyjzjoexd tkjffzbgwn qiyomjfxga uvoemerczx cqwxfwafwi btavrrvjmf pgnnrncqwb uahqsepoxg qkldsaljsr dugofbokxr vbpexyqysj kkppkakwlf nkpqeuhwhk zxxpmrthek ihsfhacsib peouxdqovf jtimcyfujw ekjwjyfplz kvibysqaX", 1021);
    wo_write(fd, "wvumxwqhpr vjywxxzjws fvyfydbjdd umxduvnoyc glskrgqemx bkiogtrkca uxlypfdobw yelzyofmxl vysndgjwlb prpylsbxgr lvpcbildvp pmjhupxmqq nydkmaysbb wiwiefbjth lkuhjahvas xxbxivogen pdsdqvrztz vkgfxepbnl cfucbwrrvp zinujpwxvr wvjwmjegws euiigjklxx kbneondgfl idvlkliagn efqjdqkrrh ranhfsmzqq ezpxfnddpt wadqvltlrs sijoajindx zkodcryiyi iqufdzxmic sqcmkbtvej tvyngzfmfr asnfdbiimk ajinaavraw gprbanubxy emcjlwufgw ydpsypklui hbildwxrzs phmejtcyxc gnlxedgdfu iuytmyirkx dowdueensn hjfxdawzom zwleshnivb jymjqtqqcu yesastidhj tsdkatdnll puogcvawvs qugaopjwjx tjqzkqwvwb wcravlquuv mcrkuudcec iccufyqzrc ygtadfmsbl epamotrgqq khqksespsq mvuscypxgs zskogpzgfr gbbdvuvddx wfyhfwzcjc ifuejhqhqh zdfopydxqi wtqvtlmvhn unseguggap xxyzcjomwp troxoquvkd khmmlmzhnb onxkvwpsor mshpdynqtu cuhiggrnqk cbawnxwnfn agpztyflpo jbbvbvzlfy neyjzjoexd tkjffzbgwn qiyomjfxga uvoemerczx cqwxfwafwi btavrrvjmf pgnnrncqwb uahqsepoxg qkldsaljsr dugofbokxr vbpexyqysj kkppkakwlf nkpqeuhwhk zxxpmrthek ihsfhacsib peouxdqovf jtimcyfujw ekjwjyfplz kvibysqa", 1020);
    wo_write(fd, "wvumxwqhpr vjywxxzjws fvyfydbjdd umxduvnoyc glskrgqemx bkiogtrkca uxlypfdobw yelzyofmxl vysndgjwlb prpylsbxgr lvpcbildvp pmjhupxmqq nydkmaysbb wiwiefbjth lkuhjahvas xxbxivogen pdsdqvrztz vkgfxepbnl cfucbwrrvp zinujpwxvr wvjwmjegws euiigjklxx kbneondgfl idvlkliagn efqjdqkrrh ranhfsmzqq ezpxfnddpt wadqvltlrs sijoajindx zkodcryiyi iqufdzxmic sqcmkbtvej tvyngzfmfr asnfdbiimk ajinaavraw gprbanubxy emcjlwufgw ydpsypklui hbildwxrzs phmejtcyxc gnlxedgdfu iuytmyirkx dowdueensn hjfxdawzom zwleshnivb jymjqtqqcu yesastidhj tsdkatdnll puogcvawvs qugaopjwjx tjqzkqwvwb wcravlquuv mcrkuudcec iccufyqzrc ygtadfmsbl epamotrgqq khqksespsq mvuscypxgs zskogpzgfr gbbdvuvddx wfyhfwzcjc ifuejhqhqh zdfopydxqi wtqvtlmvhn unseguggap xxyzcjomwp troxoquvkd khmmlmzhnb onxkvwpsor mshpdynqtu cuhiggrnqk cbawnxwnfn agpztyflpo jbbvbvzlfy neyjzjoexd tkjffzbgwn qiyomjfxga uvoemerczx cqwxfwafwi btavrrvjmf pgnnrncqwb uahqsepoxg qkldsaljsr dugofbokxr vbpexyqysj kkppkakwlf nkpqeuhwhk zxxpmrthek ihsfhacsib peouxdqovf jtimcyfujw ekjwjyfplz kvibysqaX", 1021);
    wo_write(fd, "wvumxwqhpr vjywxxzjws fvyfydbjdd umxduvnoyc glskrgqemx bkiogtrkca uxlypfdobw yelzyofmxl vysndgjwlb prpylsbxgr lvpcbildvp pmjhupxmqq nydkmaysbb wiwiefbjth lkuhjahvas xxbxivogen pdsdqvrztz vkgfxepbnl cfucbwrrvp zinujpwxvr wvjwmjegws euiigjklxx kbneondgfl idvlkliagn efqjdqkrrh ranhfsmzqq ezpxfnddpt wadqvltlrs sijoajindx zkodcryiyi iqufdzxmic sqcmkbtvej tvyngzfmfr asnfdbiimk ajinaavraw gprbanubxy emcjlwufgw ydpsypklui hbildwxrzs phmejtcyxc gnlxedgdfu iuytmyirkx dowdueensn hjfxdawzom zwleshnivb jymjqtqqcu yesastidhj tsdkatdnll puogcvawvs qugaopjwjx tjqzkqwvwb wcravlquuv mcrkuudcec iccufyqzrc ygtadfmsbl epamotrgqq khqksespsq mvuscypxgs zskogpzgfr gbbdvuvddx wfyhfwzcjc ifuejhqhqh zdfopydxqi wtqvtlmvhn unseguggap xxyzcjomwp troxoquvkd khmmlmzhnb onxkvwpsor mshpdynqtu cuhiggrnqk cbawnxwnfn agpztyflpo jbbvbvzlfy neyjzjoexd tkjffzbgwn qiyomjfxga uvoemerczx cqwxfwafwi btavrrvjmf pgnnrncqwb uahqsepoxg qkldsaljsr dugofbokxr vbpexyqysj kkppkakwlf nkpqeuhwhk zxxpmrthek ihsfhacsib peouxdqovf jtimcyfujw ekjwjyfplz kvibysqa", 1020);
    // wo_unmount(&DISK);

    //     wo_mount("disk.txt", &DISK);

    // wo_write(fd, "wvumxwqhpr vjywxxzjws fvyfydbjdd umxduvnoyc glskrgqemx bkiogtrkca uxlypfdobw yelzyofmxl vysndgjwlb prpylsbxgr lvpcbildvp pmjhupxmqq nydkmaysbb wiwiefbjth lkuhjahvas xxbxivogen pdsdqvrztz vkgfxepbnl cfucbwrrvp zinujpwxvr wvjwmjegws euiigjklxx kbneondgfl idvlkliagn efqjdqkrrh ranhfsmzqq ezpxfnddpt wadqvltlrs sijoajindx zkodcryiyi iqufdzxmic sqcmkbtvej tvyngzfmfr asnfdbiimk ajinaavraw gprbanubxy emcjlwufgw ydpsypklui hbildwxrzs phmejtcyxc gnlxedgdfu iuytmyirkx dowdueensn hjfxdawzom zwleshnivb jymjqtqqcu yesastidhj tsdkatdnll puogcvawvs qugaopjwjx tjqzkqwvwb wcravlquuv mcrkuudcec iccufyqzrc ygtadfmsbl epamotrgqq khqksespsq mvuscypxgs zskogpzgfr gbbdvuvddx wfyhfwzcjc ifuejhqhqh zdfopydxqi wtqvtlmvhn unseguggap xxyzcjomwp troxoquvkd khmmlmzhnb onxkvwpsor mshpdynqtu cuhiggrnqk cbawnxwnfn agpztyflpo jbbvbvzlfy neyjzjoexd tkjffzbgwn qiyomjfxga uvoemerczx cqwxfwafwi btavrrvjmf pgnnrncqwb uahqsepoxg qkldsaljsr dugofbokxr vbpexyqysj kkppkakwlf nkpqeuhwhk zxxpmrthek ihsfhacsib peouxdqovf jtimcyfujw ekjwjyfplz kvibysqaX", 1021);
    // wo_write(fd, "wvumxwqhpr vjywxxzjws fvyfydbjdd umxduvnoyc glskrgqemx bkiogtrkca uxlypfdobw yelzyofmxl vysndgjwlb prpylsbxgr lvpcbildvp pmjhupxmqq nydkmaysbb wiwiefbjth lkuhjahvas xxbxivogen pdsdqvrztz vkgfxepbnl cfucbwrrvp zinujpwxvr wvjwmjegws euiigjklxx kbneondgfl idvlkliagn efqjdqkrrh ranhfsmzqq ezpxfnddpt wadqvltlrs sijoajindx zkodcryiyi iqufdzxmic sqcmkbtvej tvyngzfmfr asnfdbiimk ajinaavraw gprbanubxy emcjlwufgw ydpsypklui hbildwxrzs phmejtcyxc gnlxedgdfu iuytmyirkx dowdueensn hjfxdawzom zwleshnivb jymjqtqqcu yesastidhj tsdkatdnll puogcvawvs qugaopjwjx tjqzkqwvwb wcravlquuv mcrkuudcec iccufyqzrc ygtadfmsbl epamotrgqq khqksespsq mvuscypxgs zskogpzgfr gbbdvuvddx wfyhfwzcjc ifuejhqhqh zdfopydxqi wtqvtlmvhn unseguggap xxyzcjomwp troxoquvkd khmmlmzhnb onxkvwpsor mshpdynqtu cuhiggrnqk cbawnxwnfn agpztyflpo jbbvbvzlfy neyjzjoexd tkjffzbgwn qiyomjfxga uvoemerczx cqwxfwafwi btavrrvjmf pgnnrncqwb uahqsepoxg qkldsaljsr dugofbokxr vbpexyqysj kkppkakwlf nkpqeuhwhk zxxpmrthek ihsfhacsib peouxdqovf jtimcyfujw ekjwjyfplz kvibysqa", 1020);
 
    // wo_write(fd, "wvumxwqhpr vjywxxzjws fvyfydbjdd umxduvnoyc glskrgqemx bkiogtrkca uxlypfdobw yelzyofmxl vysndgjwlb prpylsbxgr lvpcbildvp pmjhupxmqq nydkmaysbb wiwiefbjth lkuhjahvas xxbxivogen pdsdqvrztz vkgfxepbnl cfucbwrrvp zinujpwxvr wvjwmjegws euiigjklxx kbneondgfl idvlkliagn efqjdqkrrh ranhfsmzqq ezpxfnddpt wadqvltlrs sijoajindx zkodcryiyi iqufdzxmic sqcmkbtvej tvyngzfmfr asnfdbiimk ajinaavraw gprbanubxy emcjlwufgw ydpsypklui hbildwxrzs phmejtcyxc gnlxedgdfu iuytmyirkx dowdueensn hjfxdawzom zwleshnivb jymjqtqqcu yesastidhj tsdkatdnll puogcvawvs qugaopjwjx tjqzkqwvwb wcravlquuv mcrkuudcec iccufyqzrc ygtadfmsbl epamotrgqq khqksespsq mvuscypxgs zskogpzgfr gbbdvuvddx wfyhfwzcjc ifuejhqhqh zdfopydxqi wtqvtlmvhn unseguggap xxyzcjomwp troxoquvkd khmmlmzhnb onxkvwpsor mshpdynqtu cuhiggrnqk cbawnxwnfn agpztyflpo jbbvbvzlfy neyjzjoexd tkjffzbgwn qiyomjfxga uvoemerczx cqwxfwafwi btavrrvjmf pgnnrncqwb uahqsepoxg qkldsaljsr dugofbokxr vbpexyqysj kkppkakwlf nkpqeuhwhk zxxpmrthek ihsfhacsib peouxdqovf jtimcyfujw ekjwjyfplz kvibysqaX", 1021);
    // wo_write(fd, "wvumxwqhpr vjywxxzjws fvyfydbjdd umxduvnoyc glskrgqemx bkiogtrkca uxlypfdobw yelzyofmxl vysndgjwlb prpylsbxgr lvpcbildvp pmjhupxmqq nydkmaysbb wiwiefbjth lkuhjahvas xxbxivogen pdsdqvrztz vkgfxepbnl cfucbwrrvp zinujpwxvr wvjwmjegws euiigjklxx kbneondgfl idvlkliagn efqjdqkrrh ranhfsmzqq ezpxfnddpt wadqvltlrs sijoajindx zkodcryiyi iqufdzxmic sqcmkbtvej tvyngzfmfr asnfdbiimk ajinaavraw gprbanubxy emcjlwufgw ydpsypklui hbildwxrzs phmejtcyxc gnlxedgdfu iuytmyirkx dowdueensn hjfxdawzom zwleshnivb jymjqtqqcu yesastidhj tsdkatdnll puogcvawvs qugaopjwjx tjqzkqwvwb wcravlquuv mcrkuudcec iccufyqzrc ygtadfmsbl epamotrgqq khqksespsq mvuscypxgs zskogpzgfr gbbdvuvddx wfyhfwzcjc ifuejhqhqh zdfopydxqi wtqvtlmvhn unseguggap xxyzcjomwp troxoquvkd khmmlmzhnb onxkvwpsor mshpdynqtu cuhiggrnqk cbawnxwnfn agpztyflpo jbbvbvzlfy neyjzjoexd tkjffzbgwn qiyomjfxga uvoemerczx cqwxfwafwi btavrrvjmf pgnnrncqwb uahqsepoxg qkldsaljsr dugofbokxr vbpexyqysj kkppkakwlf nkpqeuhwhk zxxpmrthek ihsfhacsib peouxdqovf jtimcyfujw ekjwjyfplz kvibysqa", 1020);

    // wo_write(fd,"vzhfwacwmphlucvimzup ppaaubctbkzmvanvvfov nvzkpcwjmdgcbbgplldw xrpfimzboknzkyevwpfh khdrnhtktygbpzoydifw tlyqtgyayordwpuzqrfm psfcfnpxtyywaqoaaodr kzoghczilqkffjmspolq yrkksvvdbjjrjzfphhjx qwobjbvytzunoacpgmcg odmdypjqrgwgitlfyssf rlqeleodeqrlsnqqikgq ijkyjkncynmusayvpgbw gxvoketsxtuqaxmpglim jbciaayyzulbltgaypht iwhtcbbnltqcrfueynve pdmabpwkzqtebtasvfxa clnqpuiowduvbhnwevqa ettkpgeetuweidznmlow rftrquroclwiuurwjpma enlqljyqyogsgrthropn zhqprhwvtajxnquycumm qcdgcilveumgyrmdsfnc ijtaazwkzjkbcwdytsnb mrruoyvcwcoeajjhazhh pgekonawixipdrjecgjz siqzqwevxqstpcpuxodl ihoacowwcfladopythht kisxtzwivstntxpvpbte pzfpnqhqiigrupmppyir pzbxcvxmpeonjjgaqjji izikmkrynzsoltcmgccg fexcpathxcxledpohrsn xmelglhvcmnfongrbkzc hpyouwrmyotibjwhykdo ibbkyeriungyskiridmy ugrloalmmohuxdcbaufw hzogbzjexqlzqjizjbmg xqpqvainbumntgbajkry cfsrgysxiuthipaljrai jpabzhxehzeifymwnkrp srwvfuktgwkkofnnpvlg cpwfqolwsqrlapbujapd bvsamztrxpghpmdypola dfpxlkbttfvelyvlqmlz sxarymikpqoctiwwppoz tqhvdzurcblpnikdbuxc kvkynnrlcbxfsldkbzap qzaltucbvyjfuvvtfodo uypqtkbpcdpjjyxkzpep puibovgsibjvijozazbu psheixxydjhafjbpizfv pfaepbejomgavhyjjsvn ojizfqezsfhztthjhpgu myyodfifbwzdjktxpgnh ycpebqkmmukabkxmgdrc nrtpfbiejfzbuqhzeriu icuwgyuhyqssphagkfkt iwiahnebibixetuvrvmn mfbomvjnwwoukenkshpa ulyacqygmzcmxmaotbfj ocvmcprlwzfrgqfdkgdm ugexabtarriloflzikaw imttfojksuggfsipvbfc ulukunkzjndtajivkbzr gxnqgefpxrvjnwxirjht wuxwpedkhivpdodvmaeq gqsdkwpxsjyixrboofky iqbhbspljrmfoqhuleab cdynttgbvpsafzlssbgj hqrntwuvamkveofbfrpa lcnykcgcncmduhcdteyq nqljlcdjayfxsuakcfvi ovkmtorvtdfpbleysolp cokcucgbgxessoizfmhj lofmvpekkcjdzfwgxmdf baclhqnmstdjbyjspnec ducbtwyransgbjfvorog sonbwiqlsozqjikuuyor jvshgrzoemmtdmnvwllk kbuaqksprxtfddcqwzcq kgtqsemiayiyhothbkix gavybvgxpndjaanilgcd wroruuqkpttpwmjwhjxs dojdwamirwsevopwbeyc ieuucqaijhlsxoxdmdvq vtzygbfeirbkvmuzdynv nhfmgpgtoksrolcmjlwx ofwhsoddjcidgbwnlwkc ssvpyexjnrwxnqzhordr kzxucsdpyqqzmlcwwwdb ajxnbcgmfjbthnwkaljd iovqoqkpryytiqzbneie ejdiffpfgzaqftbwcgqm xnfgxoohodgjqdcsfvaz tlhbydyrnvxfoxpodroz euqdslnoxsegieoveqcd jyqqblcawugazbsieanu sgiooskzjwdvyvyxvtrv qtghtytuwsesqrlgqyzq",sizeof("vzhfwacwmphlucvimzup ppaaubctbkzmvanvvfov nvzkpcwjmdgcbbgplldw xrpfimzboknzkyevwpfh khdrnhtktygbpzoydifw tlyqtgyayordwpuzqrfm psfcfnpxtyywaqoaaodr kzoghczilqkffjmspolq yrkksvvdbjjrjzfphhjx qwobjbvytzunoacpgmcg odmdypjqrgwgitlfyssf rlqeleodeqrlsnqqikgq ijkyjkncynmusayvpgbw gxvoketsxtuqaxmpglim jbciaayyzulbltgaypht iwhtcbbnltqcrfueynve pdmabpwkzqtebtasvfxa clnqpuiowduvbhnwevqa ettkpgeetuweidznmlow rftrquroclwiuurwjpma enlqljyqyogsgrthropn zhqprhwvtajxnquycumm qcdgcilveumgyrmdsfnc ijtaazwkzjkbcwdytsnb mrruoyvcwcoeajjhazhh pgekonawixipdrjecgjz siqzqwevxqstpcpuxodl ihoacowwcfladopythht kisxtzwivstntxpvpbte pzfpnqhqiigrupmppyir pzbxcvxmpeonjjgaqjji izikmkrynzsoltcmgccg fexcpathxcxledpohrsn xmelglhvcmnfongrbkzc hpyouwrmyotibjwhykdo ibbkyeriungyskiridmy ugrloalmmohuxdcbaufw hzogbzjexqlzqjizjbmg xqpqvainbumntgbajkry cfsrgysxiuthipaljrai jpabzhxehzeifymwnkrp srwvfuktgwkkofnnpvlg cpwfqolwsqrlapbujapd bvsamztrxpghpmdypola dfpxlkbttfvelyvlqmlz sxarymikpqoctiwwppoz tqhvdzurcblpnikdbuxc kvkynnrlcbxfsldkbzap qzaltucbvyjfuvvtfodo uypqtkbpcdpjjyxkzpep puibovgsibjvijozazbu psheixxydjhafjbpizfv pfaepbejomgavhyjjsvn ojizfqezsfhztthjhpgu myyodfifbwzdjktxpgnh ycpebqkmmukabkxmgdrc nrtpfbiejfzbuqhzeriu icuwgyuhyqssphagkfkt iwiahnebibixetuvrvmn mfbomvjnwwoukenkshpa ulyacqygmzcmxmaotbfj ocvmcprlwzfrgqfdkgdm ugexabtarriloflzikaw imttfojksuggfsipvbfc ulukunkzjndtajivkbzr gxnqgefpxrvjnwxirjht wuxwpedkhivpdodvmaeq gqsdkwpxsjyixrboofky iqbhbspljrmfoqhuleab cdynttgbvpsafzlssbgj hqrntwuvamkveofbfrpa lcnykcgcncmduhcdteyq nqljlcdjayfxsuakcfvi ovkmtorvtdfpbleysolp cokcucgbgxessoizfmhj lofmvpekkcjdzfwgxmdf baclhqnmstdjbyjspnec ducbtwyransgbjfvorog sonbwiqlsozqjikuuyor jvshgrzoemmtdmnvwllk kbuaqksprxtfddcqwzcq kgtqsemiayiyhothbkix gavybvgxpndjaanilgcd wroruuqkpttpwmjwhjxs dojdwamirwsevopwbeyc ieuucqaijhlsxoxdmdvq vtzygbfeirbkvmuzdynv nhfmgpgtoksrolcmjlwx ofwhsoddjcidgbwnlwkc ssvpyexjnrwxnqzhordr kzxucsdpyqqzmlcwwwdb ajxnbcgmfjbthnwkaljd iovqoqkpryytiqzbneie ejdiffpfgzaqftbwcgqm xnfgxoohodgjqdcsfvaz tlhbydyrnvxfoxpodroz euqdslnoxsegieoveqcd jyqqblcawugazbsieanu sgiooskzjwdvyvyxvtrv qtghtytuwsesqrlgqyzq"));
    
    // wo_write(fd,"svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX",sizeof("svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX"));
    // wo_write(fd,"svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX",sizeof("svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX"));

    
    printf("Data BLOCK Content 1 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4)))->data);
        printf("Data BLOCK Content 1 = %d\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4)))->next);
    //"svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX"
    printf("Data BLOCK Content 2 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024))->data);
            printf("Data BLOCK Content 2 = %d\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024))->next);
    printf("Data BLOCK Content 3 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 2))->data);
                printf("Data BLOCK Content 3 = %d\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 2))->next);

    printf("Data BLOCK Content 4 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 3))->data);
    printf("Data BLOCK Content 5 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 4))->data);
    printf("Data BLOCK Content 6 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 5))->data);
    // printf("Data BLOCK Content 7 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 6))->data);
    // printf("Data BLOCK Content 8 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 7))->data);
    // printf("Data BLOCK Content 9 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 8))->data);
    // printf("Data BLOCK Content 10 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 9))->data);
    // printf("Data BLOCK Content 11 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 10))->data);
    // printf("Data BLOCK Content 12 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 11))->data);
    // printf("Data BLOCK Content 13 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 12))->data);
    // printf("Data BLOCK Content 14 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 13))->data);

    printf("Total File Size = %d\n", ((inode *)(DISK + 1024))->total_size_of_file);
    char res[4082];
    wo_read(fd,&res,4082);
    printf(" File content = %s\n", res);
    wo_unmount(c);

    // prettyPrintInodes(&DISK);
    // prettyPrintSuperBlock(&DISK);
    // prettyPrintBitMap(&DISK);
    // Write to file
}