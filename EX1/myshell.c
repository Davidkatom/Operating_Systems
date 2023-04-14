#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define MAX_CMD_LENGTH 100
#define MAX_HISTORY 100

char *history[MAX_HISTORY];
int history_count = 0;

void add_to_history(char *cmd) {
    if (history_count < MAX_HISTORY) {
        history[history_count++] = strdup(cmd);
    }
}

void display_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%s\n", history[i]);
    }
}

int main() {
    char cmd[MAX_CMD_LENGTH];
    char *args[MAX_CMD_LENGTH];
    int status;

    while (1) {
        printf("$ ");
        fflush(stdout);
        fgets(cmd, MAX_CMD_LENGTH, stdin);
        cmd[strcspn(cmd, "\n")] = '\0';

        add_to_history(cmd);

        char *token = strtok(cmd, " ");
        int i = 0;
        while (token) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        if (strcmp(args[0], "cd") == 0) {
            if (chdir(args[1]) < 0) {
                perror("chdir failed");
            }
            continue;
        }

        if (strcmp(args[0], "history") == 0) {
            display_history();
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(1);
        } else if (pid == 0) {
            if (execvp(args[0], args) < 0) {
                perror("execvp failed");
                exit(1);
            }
        } else {
            waitpid(pid, &status, 0);
        }
    }

    for (int i = 0; i < history_count; i++) {
        free(history[i]);
    }

    return 0;
}

