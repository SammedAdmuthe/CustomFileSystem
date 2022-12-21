#include "writeonceFS.c"
int main(void) {
        char disk[5000000];
    char buffer[1500000];
    printf("\n\n\n");
    memset(buffer, 0, 1500000);

        wo_mount("dfile1", &disk);
        printf("ch41\n");

    // int fd = wo_open("test2.txt", "WO_RDWR", "WO_CREAT");
        int fd = wo_create("test2.txt", WO_RDWR);
        // int fd = wo_create("test2.txt", WO_RDWR, WO_CREAT);
        // int fd = wo_create("test2.txt");
        printf("ch42\n");

    for (int i = 0; i < 1500000; i++) {
            char c = i;
        buffer[i] = c;
    }
    buffer[1500000] = '\0';
        printf("ch5\n");

    wo_write(fd, &buffer, 1500000);
        char buffer2[1500000];
        memset(buffer2, 0, 1500000);
        printf("here\n");
        fd = wo_open("test2.txt", WO_RDONLY);
        // fd = wo_open("test2.txt", "WO_RDONLY", "WO_RDONLY");
        printf("ch6\n");
        wo_read(fd, buffer2, 1500000);
        if (strcmp(buffer, buffer2)) {
                printf("Failed to write and read 1.5MB file [-15pts]\n");
        }
        printf("DONE\n");
        wo_unmount(&disk);
}