#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGUMENTS 100

char history[MAX_ARGUMENTS][MAX_COMMAND_LENGTH];
int history_index = 0;

void print_prompt() {
    printf("$ ");
    fflush(stdout);
}

void print_history() {
    for (int i = 0; i < history_index; i++) {
        printf("%d %s\n", i+1, history[i]);
    }
}

void change_directory(char* dir) {
    if (chdir(dir) != 0) {
        perror("chdir() failed");
    }
}

int execute_command(char** args) {
    if (strcmp(args[0], "history") == 0) {
        print_history();
    } else if (strcmp(args[0], "cd") == 0) {
        change_directory(args[1]);
    } else if (strcmp(args[0], "exit") == 0) {
        return 1;
    } else {
        pid_t pid = fork();
        if (pid == 0) {
            execvp(args[0], args);
            perror("execvp() failed");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("fork() failed");
        } else {
            waitpid(pid, NULL, 0);
        }
    }
    return 0;
}

int parse_command(char* input, char** args) {
    int argc = 0;
    char* token = strtok(input, " ");
    while (token != NULL) {
        args[argc++] = token;
        token = strtok(NULL, " ");
    }
    args[argc] = NULL;
    return argc;
}

int main() {
    char input[MAX_COMMAND_LENGTH];
    char* args[MAX_ARGUMENTS];
    int exit_shell = 0;

    while (!exit_shell) {
        print_prompt();
        fgets(input, MAX_COMMAND_LENGTH, stdin);
        input[strcspn(input, "\n")] = '\0'; // remove trailing newline

        // save command to history
        strcpy(history[history_index], input);
        history_index++;

        int argc = parse_command(input, args);
        if (argc > 0) {
            exit_shell = execute_command(args);
        }
    }

    return 0;
}
