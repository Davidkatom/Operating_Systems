#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>


int compare_files(int fd1, int fd2) {
    char ch1, ch2;
    int identical = 1;
    char buf1, buf2;

    while (read(fd1, &buf1, 1) > 0 && read(fd2, &buf2, 1) > 0) {
        ch1 = buf1;
        ch2 = buf2;
        if (ch1 != ch2) {
            identical = 0;
            while (isspace(ch1)) {
                read(fd1, &buf1, 1);
                ch1 = buf1;
            }
            while (isspace(ch2)) {
                read(fd2, &buf2, 1);
                ch2 = buf2;
            }
            if (tolower(ch1) != tolower(ch2)) {
                return 2;
            }
        }
    }
    while(read(fd1, &buf1, 1)>0) {
        if(isspace(buf1))
            continue;
        ch1 = buf1;
        if (ch1 != ch2)
            identical = 0;
        if (tolower(ch1) != tolower(ch2))
            return 2;
    }
    while(read(fd2, &buf2, 1)>0) {
        if(isspace(buf2))
            continue;
        ch2 = buf2;
        if (ch1 != ch2)
            identical = 0;
        if (tolower(ch1) != tolower(ch2))
            return 2;
    }

    if (identical) return 1;
    return 3;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        return 1;
    }

    int fd1 = open(argv[1], O_RDONLY);
    int fd2 = open(argv[2], O_RDONLY);

    if (fd1 == -1 || fd2 == -1) {
        perror("Error in: open");
        return 1;
    }

    int result = compare_files(fd1, fd2);

    close(fd1);
    close(fd2);

    return result;
}
