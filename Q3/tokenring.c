#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main (int argc, char* argv[]) {
    unsigned int token = 0;
    if (argc < 4) {
        printf("You must enter 3 values (number of processes, probability of locking and locking time).\n");
        return EXIT_FAILURE;
    }
    unsigned int n = atoi(argv[1]);
    float p = atof(argv[2]);
    float t = atof(argv[3]);
    for (int x = 1; x <= n; x++) {
        char pathname[20];
        if (x != n) {
            sprintf(pathname, "pipe%dto%d", x, x+1);
        } else {
            sprintf(pathname, "pipe%dto%d", n, 1);
        }
        if (mkfifo(pathname, 0666) == -1) {
            perror("mkfifo");
            return EXIT_FAILURE;
        }
    }
    unsigned int i;
    for (i = 2; i <= n; i++) {
        int pid = fork();
        if (pid == -1) {
            perror("fork");
            return EXIT_FAILURE;
        }
        if (pid == 0) break;
    }
    int fdw, fdr;
    char readpathname[20];
    char writepathname[20];
    if (i == n) {
        sprintf(readpathname, "pipe%ito%i", n-1, n);
        sprintf(writepathname, "pipe%ito%i", n, 1);
    } else if (i == n+1) {
        i = 1;
        sprintf(readpathname, "pipe%ito1", n);
        sprintf(writepathname, "pipe1to2");
    } else {
        sprintf(readpathname, "pipe%ito%i", i-1, i);
        sprintf(writepathname, "pipe%ito%i", i, i+1);
    }
    if ((fdr = open(readpathname, O_RDWR)) == -1) {
        perror("open");
        return EXIT_FAILURE;
    }
    if ((fdw = open(writepathname, O_RDWR)) == -1) {
        perror("open");
        return EXIT_FAILURE;
    }
    if (i == 1) {
        if (write(fdw, &token, sizeof(int)) == -1) {
        perror("write");
        return EXIT_FAILURE;
        }
    }
    srand(getpid());
    while (1) {
        if (read(fdr, &token, sizeof(int)) == -1) {
            perror("read");
            return EXIT_FAILURE;
        }
        float r = (float)random() / (float)RAND_MAX;
        if (r <= p) {
            printf("[p%i] lock on token (val = %i)\n", i, token);
            usleep(t*1000000);
            printf("[p%i] unlock token\n", i);
        }
        token++;
        if (write(fdw, &token, sizeof(int)) == -1) {
            perror("write");
        }
    }
}
