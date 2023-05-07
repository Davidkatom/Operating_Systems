#include <stdio.h>
#include <ctype.h>

int compare_files(FILE *file1, FILE *file2) {
    int ch1, ch2;
    int identical = 1;
    int similar = 1;

    while ((ch1 = fgetc(file1)) != EOF && (ch2 = fgetc(file2)) != EOF) {
        if (ch1 != ch2) {
            identical = 0;
            while(isspace(ch1)){
                ch1 = fgetc(file1);
            }
            while(isspace(ch2)){
                ch2 = fgetc(file2);
            }
            if (tolower(ch1) != tolower(ch2)){
                similar = 0;
                break;
            }
        }
    }

    // Check if any file has reached EOF and the other file still has characters to read.
    // If so, continue skipping whitespaces for the remaining file.
    if(ch1 != EOF && isspace(ch1))
        return 3;
    if(ch2 != EOF && isspace(ch2))
        return 3;

    if (identical) return 1;
    if (similar && (ch1 == EOF) && (ch2 == EOF)) return 3;
    return 2;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <file1> <file2>\n", argv[0]);
        return 1;
    }

    FILE *file1 = fopen(argv[1], "r");
    FILE *file2 = fopen(argv[2], "r");

    if (!file1 || !file2) {
        perror("Error opening file");
        return 1;
    }

    int result = compare_files(file1, file2);

    fclose(file1);
    fclose(file2);

    return result;
}
