#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

extern int errno;

#define DISK_SIZE 4 * 1024 * 1024
#define BLOCK_SIZE 1024

// SUPER BLOCK | INODE | BITMAP | DATA

char DISK[DISK_SIZE];
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

// int wo_read( int fs,  void* buffer, int bytes ) {

// }

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
                    strncpy(((data_block *)free_data_block)->data, (char *)tempBuffer, (count >= 1020 ? 1020 : count));
                    ((inode *)(curr))->data_block_start = target_data_block;
                    ((inode *)(curr))->total_size_of_file += (count >= 1020 ? 1020 : count);
                    count-=(count >= 1020 ? 1020 : count);
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
                            strncpy(((data_block *)free_data_block)->data, (char *)tempBuffer + ((x + 1) * 1020), (count >= 1020 ? 1020 : count));
                            ((inode *)(curr))->total_size_of_file += (count >= 1020 ? 1020 : count);
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
                strncat(((data_block *)data_itr)->data,tempBuffer, ((count <= remaining_size)?count:remaining_size));
                count-= ((count <= remaining_size)?count:remaining_size);
                int size = ((count <= remaining_size)?count:remaining_size);
                ((inode *)(curr))->total_size_of_file += size;
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
                    strncpy(((data_block *)free_data_block)->data, (char *)tempBuffer+remaining_size, (count >= 1020 ? 1020 : count));
                    //((inode *)(curr))->data_block_start = target_data_block;
                    ((inode *)(curr))->total_size_of_file += (count >= 1020 ? 1020 : count);
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
                            strncpy(((data_block *)free_data_block)->data, (char *)tempBuffer+remaining_size + ((x + 1) * 1020), (count >= 1020 ? 1020 : count));
                            ((inode *)(curr))->total_size_of_file += (count >= 1020 ? 1020 : count);
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
            ((inode *)curr)->inUse == 'y';
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
    wo_mount("disk.txt", &DISK);
    wo_unmount(&DISK);
    wo_mount("disk.txt", &DISK);
    int fd = wo_open("ashwin.txt", 1, "current");
    // printf("File Descriptor = %d\n",wo_open("ashwin.txt",1,"current"));

    // printf("File Descriptor = %d\n",wo_open("jayDon.txt",1,"current"));
    // prettyPrintInodes(&DISK);
    //wo_write(fd, "7878756705 1872788848 1869294709 1612511154 9173981851 2132050263 6263897800 2956495174 6822183361 3590392894 6340530864 0499311875 3251064666 2879074650 2783400415 2503024167 2014126294 1736767580 1917339971 4590605251 3563201440 9684122570 5209227966 1426346417 2713378212 1462380458 4235474007 7108232323 9015031133 2560846752 5836258997 9992262986 2221262378 1605474260 8409596018 5685453537 5031873689 2258769781 2545624714 2255890847 0505732741 3782460769 1367345304 8143852954 4685311156 0058508494 2373780014 0277937325 8825058401 7132746705 0380860351 1046724216 9709000501 8094071249 5273465501 4043870340 2717616891 3501501280 6797559641 8939672729 9219553768 0969466036 2627207117 7954265572 7459201469 2997404549 9372363759 3480842599 0482452843 9691253699 2495163464 4887898998 2210733518 5656946606 0088660418 4926216645 9888479073 5129132945 5498637400 8952027811 2461526051 3914870450 1080601120 5355036820 1682085478 9136638949 6989025866 0732249176 1516700029 4917998081 3159278554 2613282188 1571678830 0401507282 2724695435 6453193833 1803509611 6389222224 8766928115 0493738476",sizeof("7878756705 1872788848 1869294709 1612511154 9173981851 2132050263 6263897800 2956495174 6822183361 3590392894 6340530864 0499311875 3251064666 2879074650 2783400415 2503024167 2014126294 1736767580 1917339971 4590605251 3563201440 9684122570 5209227966 1426346417 2713378212 1462380458 4235474007 7108232323 9015031133 2560846752 5836258997 9992262986 2221262378 1605474260 8409596018 5685453537 5031873689 2258769781 2545624714 2255890847 0505732741 3782460769 1367345304 8143852954 4685311156 0058508494 2373780014 0277937325 8825058401 7132746705 0380860351 1046724216 9709000501 8094071249 5273465501 4043870340 2717616891 3501501280 6797559641 8939672729 9219553768 0969466036 2627207117 7954265572 7459201469 2997404549 9372363759 3480842599 0482452843 9691253699 2495163464 4887898998 2210733518 5656946606 0088660418 4926216645 9888479073 5129132945 5498637400 8952027811 2461526051 3914870450 1080601120 5355036820 1682085478 9136638949 6989025866 0732249176 1516700029 4917998081 3159278554 2613282188 1571678830 0401507282 2724695435 6453193833 1803509611 6389222224 8766928115 0493738476"));
    //wo_write(fd, "7878756705 1872788848 1869294709 1612511154 9173981851 2132050263 6263897800 2956495174 6822183361 3590392894 6340530864 0499311875 3251064666 2879074650 2783400415 2503024167 2014126294 1736767580 1917339971 4590605251 3563201440 9684122570 5209227966 1426346417 2713378212 1462380458 4235474007 7108232323 9015031133 2560846752 5836258997 9992262986 2221262378 1605474260 8409596018 5685453537 5031873689 2258769781 2545624714 2255890847 0505732741 3782460769 1367345304 8143852954 4685311156 0058508494 2373780014 0277937325 8825058401 7132746705 0380860351 1046724216 9709000501 8094071249 5273465501 4043870340 2717616891 3501501280 6797559641 8939672729 9219553768 0969466036 2627207117 7954265572 7459201469 2997404549 9372363759 3480842599 0482452843 9691253699 2495163464 4887898998 2210733518 5656946606 0088660418 4926216645 9888479073 5129132945 5498637400 8952027811 2461526051 3914870450 1080601120 5355036820 1682085478 9136638949 6989025866 0732249176 1516700029 4917998081 3159278554 2613282188 1571678830 0401507282 2724695435 6453193833 1803509611 6389222224 8766928115 0493738476",sizeof("7878756705 1872788848 1869294709 1612511154 9173981851 2132050263 6263897800 2956495174 6822183361 3590392894 6340530864 0499311875 3251064666 2879074650 2783400415 2503024167 2014126294 1736767580 1917339971 4590605251 3563201440 9684122570 5209227966 1426346417 2713378212 1462380458 4235474007 7108232323 9015031133 2560846752 5836258997 9992262986 2221262378 1605474260 8409596018 5685453537 5031873689 2258769781 2545624714 2255890847 0505732741 3782460769 1367345304 8143852954 4685311156 0058508494 2373780014 0277937325 8825058401 7132746705 0380860351 1046724216 9709000501 8094071249 5273465501 4043870340 2717616891 3501501280 6797559641 8939672729 9219553768 0969466036 2627207117 7954265572 7459201469 2997404549 9372363759 3480842599 0482452843 9691253699 2495163464 4887898998 2210733518 5656946606 0088660418 4926216645 9888479073 5129132945 5498637400 8952027811 2461526051 3914870450 1080601120 5355036820 1682085478 9136638949 6989025866 0732249176 1516700029 4917998081 3159278554 2613282188 1571678830 0401507282 2724695435 6453193833 1803509611 6389222224 8766928115 0493738476"));
    //wo_write(fd, "7878756705 1872788848 1869294709 1612511154 9173981851 2132050263 6263897800 2956495174 6822183361 3590392894 6340530864 0499311875 3251064666 2879074650 2783400415 2503024167 2014126294 1736767580 1917339971 4590605251 3563201440 9684122570 5209227966 1426346417 2713378212 1462380458 4235474007 7108232323 9015031133 2560846752 5836258997 9992262986 2221262378 1605474260 8409596018 5685453537 5031873689 2258769781 2545624714 2255890847 0505732741 3782460769 1367345304 8143852954 4685311156 0058508494 2373780014 0277937325 8825058401 7132746705 0380860351 1046724216 9709000501 8094071249 5273465501 4043870340 2717616891 3501501280 6797559641 8939672729 9219553768 0969466036 2627207117 7954265572 7459201469 2997404549 9372363759 3480842599 0482452843 9691253699 2495163464 4887898998 2210733518 5656946606 0088660418 4926216645 9888479073 5129132945 5498637400 8952027811 2461526051 3914870450 1080601120 5355036820 1682085478 9136638949 6989025866 0732249176 1516700029 4917998081 3159278554 2613282188 1571678830 0401507282 2724695435 6453193833 1803509611 6389222224 8766928115 0493738476",sizeof("7878756705 1872788848 1869294709 1612511154 9173981851 2132050263 6263897800 2956495174 6822183361 3590392894 6340530864 0499311875 3251064666 2879074650 2783400415 2503024167 2014126294 1736767580 1917339971 4590605251 3563201440 9684122570 5209227966 1426346417 2713378212 1462380458 4235474007 7108232323 9015031133 2560846752 5836258997 9992262986 2221262378 1605474260 8409596018 5685453537 5031873689 2258769781 2545624714 2255890847 0505732741 3782460769 1367345304 8143852954 4685311156 0058508494 2373780014 0277937325 8825058401 7132746705 0380860351 1046724216 9709000501 8094071249 5273465501 4043870340 2717616891 3501501280 6797559641 8939672729 9219553768 0969466036 2627207117 7954265572 7459201469 2997404549 9372363759 3480842599 0482452843 9691253699 2495163464 4887898998 2210733518 5656946606 0088660418 4926216645 9888479073 5129132945 5498637400 8952027811 2461526051 3914870450 1080601120 5355036820 1682085478 9136638949 6989025866 0732249176 1516700029 4917998081 3159278554 2613282188 1571678830 0401507282 2724695435 6453193833 1803509611 6389222224 8766928115 0493738476"));
    //wo_write(fd, "7878756705 1872788848 1869294709 1612511154 9173981851 2132050263 6263897800 2956495174 6822183361 3590392894 6340530864 0499311875 3251064666 2879074650 2783400415 2503024167 2014126294 1736767580 1917339971 4590605251 3563201440 9684122570 5209227966 1426346417 2713378212 1462380458 4235474007 7108232323 9015031133 2560846752 5836258997 9992262986 2221262378 1605474260 8409596018 5685453537 5031873689 2258769781 2545624714 2255890847 0505732741 3782460769 1367345304 8143852954 4685311156 0058508494 2373780014 0277937325 8825058401 7132746705 0380860351 1046724216 9709000501 8094071249 5273465501 4043870340 2717616891 3501501280 6797559641 8939672729 9219553768 0969466036 2627207117 7954265572 7459201469 2997404549 9372363759 3480842599 0482452843 9691253699 2495163464 4887898998 2210733518 5656946606 0088660418 4926216645 9888479073 5129132945 5498637400 8952027811 2461526051 3914870450 1080601120 5355036820 1682085478 9136638949 6989025866 0732249176 1516700029 4917998081 3159278554 2613282188 1571678830 0401507282 2724695435 6453193833 1803509611 6389222224 8766928115 0493738476",sizeof("7878756705 1872788848 1869294709 1612511154 9173981851 2132050263 6263897800 2956495174 6822183361 3590392894 6340530864 0499311875 3251064666 2879074650 2783400415 2503024167 2014126294 1736767580 1917339971 4590605251 3563201440 9684122570 5209227966 1426346417 2713378212 1462380458 4235474007 7108232323 9015031133 2560846752 5836258997 9992262986 2221262378 1605474260 8409596018 5685453537 5031873689 2258769781 2545624714 2255890847 0505732741 3782460769 1367345304 8143852954 4685311156 0058508494 2373780014 0277937325 8825058401 7132746705 0380860351 1046724216 9709000501 8094071249 5273465501 4043870340 2717616891 3501501280 6797559641 8939672729 9219553768 0969466036 2627207117 7954265572 7459201469 2997404549 9372363759 3480842599 0482452843 9691253699 2495163464 4887898998 2210733518 5656946606 0088660418 4926216645 9888479073 5129132945 5498637400 8952027811 2461526051 3914870450 1080601120 5355036820 1682085478 9136638949 6989025866 0732249176 1516700029 4917998081 3159278554 2613282188 1571678830 0401507282 2724695435 6453193833 1803509611 6389222224 8766928115 0493738476"));
    wo_write(fd,"svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX" , sizeof("svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX"));
    wo_write(fd,"svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX" , sizeof("svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX"));
    wo_write(fd,"svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX" , sizeof("svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX"));    

    
    printf("Data BLOCK Content 1 = %s", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4)))->data);
    //"svQJJqEKcdTHidbqDnov ovumrYgNgYxYGrfhIMwz HxwHfMOQDBixYzeJDYIO tTZwGmAYPYUptXRyQptI JJrMmmKBhltWditHXzNY YPwFSGMiVevfeTSKKOUT PSNaQojDlmOpbktDgRKd VVLUqpvEDSjnYOkOElOt TxfiesiFThJMUwuOevgw tGopFkXkfnLeEAbtpaLp QilGngQOgXOqJVGEcwDt PTzSxwqxIeIkFRcuivdy HfbQAWNSTsINhFvuKwzJ JWwUqLZdlTORNGBdddRB aKMxxkkFANLnMcyWmSAy hOHCbxoEcQLBmYnHPcnc PpXZSqfahSAHUwxxExQb lkjraqkfsdIkKEnkEGnX NCiwtOXwSCSRbPvCYlHj JiSgSHNXKkabInYdfAza GDscQGNGvqbluEKrMipH pAPlhurjLSGyBnIXoARJ mcpvBtsLAuJUgMjvRvef dXtZtHoYbGvvLJMfFSkn LzOmPxNydLzNnOjTIeZx UNHmFdSeuNNTGTJAHVnG QcHKddSqCSFQhmFyxtlZ jvhyhmEuoYOeMhXinrlB rGlulbhWzyJXCetMdskB JZbQJQLBNXSsfrkcWUTz WEfrVPxTQXbfzrCWzcNq lzkkxZYsXjSrwuZKLWjw toHGrQDECJDBntOMLkyw DUPhAxmUHUljWCJqWcpu FOZPdicvRMaIHbPlMEqn pkJWymYLEhjuTWSDHLoI UrVBjlOCvFPMmtSndUdw jcemDUbzySKSrRpKNGiu gerWrtKGfoYgprONHJlM KKpKVwwsQAmAlZEdOWgf ecUcsuqMAiSgOIdDCKuW pBPxfKIygRSSZownrrwJ KwXHCqpVxvYPGwQDIybG HHlBYJbhMbNCnpbMGqin DbnGgMZmzTcedXMGhehC cZfdlgDKWoUNYoJhXVgl JeUDZnRXdqtitNLDfVxn QZnHXHeEjICbYimzPBKw geaGjHNWIGAKTfYXZOCL FSzqHTuCGYPuRTLYOdzD vutBIgxbhztmvUfYqrce NfUoWNeTSLwNwNjhqxYg mMeLAdriDEmKLyBqSnlH EwYMHcXiZCzCIqjgqDuK QZytvLgCjVOuLJEBtAwH efVPvDEWyceWlDTTEPPU aUSMRSQUczFVJtTAxXrz kWmYLungAPbzupehyFqN TdlLFYjtMtIUwGlxOEnk gCruvwVoOKgZcxJXngrx GzNVfoTxgHgyXAhvFwoN IzxDmBrXqaNjyRLFbnSx QkPfIKOKbQrotkCNpYTc eLlNNGEehbzJEDgqVsnx ViQGNYsvCGMNrQErUxnV OGFOsGnNqhBUWGOnsuxf TWFKHIRsXSLMSplyOEna FweXOcJhGlvkQnxPqQsS lvNTxdmjLhcolNoTiWXS PBdGLGuizagjFInwJtUq yHKShFUzWntdHgKGIaiD GgNuaLqpgubXUkrayKsS XuAKVfVKyipDGDIsBgYt LVTyOwgUvJMZPOEbhmqU AKnbReQavunqOSZHLWsg kIkKExnvOhCOmHazCwdT QYHFejEaZCUZkMyvUNQi FXOruMdbNOXaLwplAQSE KUYIwnPaIbRbrnBYUOnk DTAdhLGgigJGerZgyJXX lPKaWDOQBjGMVfEoKELF UnMTeFgvAqWIYpDgAPSZ EglLSWOiUotNTtvXJrqm HWYfYPVVvlXwuOkKfsDB zUOjjrNBPNNzlBptOYCr RniKqkSwngfORUmZNewO SbdVhWafPCQQTdACwVYl TlPHNcSHLToJWyCYrHCQ YYhPFXBLkUFFfZYDjcDL ejpBvkKXgONLchUvJgSF pXvSCIiJeFKAQODdeBLs yFVMLdykQfKlckOFNqah pVchusgdYJiZbGLxMZGV MuDBOdBxfqLhgowfSXFs iUagDUUSOfWFXlmgVUmh rWpERJhYxDcPLtOmiQOk aSDBKiUFOQsQSnipFmte hQqxTgGBVLXZRHOBWjkZ GXhGubyapDIOvsDlORfO ZlRSfpIrdQBrhKKbyGOX"
    printf("Data BLOCK Content 2 = %s", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024))->data);
    printf("Data BLOCK Content 3 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 2))->data);
    printf("Data BLOCK Content 4 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 3))->data);
    printf("Data BLOCK Content 5 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 4))->data);
    printf("Data BLOCK Content 6 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 5))->data);
    printf("Data BLOCK Content 7 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 6))->data);
    printf("Data BLOCK Content 8 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 7))->data);
    printf("Data BLOCK Content 9 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 8))->data);
        printf("Data BLOCK Content 10 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 9))->data);
        printf("Data BLOCK Content 11 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 10))->data);
        printf("Data BLOCK Content 12 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 11))->data);
        printf("Data BLOCK Content 13 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 12))->data);
        printf("Data BLOCK Content 14 = %s\n", ((data_block *)(DISK + 1024 + (1024 * 50) + (1024 * 4) + 1024 * 13))->data);

    printf("Total File Size = %d\n", ((inode *)(DISK + 1024))->total_size_of_file);
    wo_unmount(&DISK);
    // prettyPrintSuperBlock(&DISK);
    // prettyPrintBitMap(&DISK);
    // Write to file
}