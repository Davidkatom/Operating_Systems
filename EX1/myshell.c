//David Shnaiderov 209198308

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_CMD_LENGTH 100
#define MAX_HISTORY 100

char *history[MAX_HISTORY];
int history_count = 0;


void add_to_history(char *cmd, pid_t pid) {
    if (history_count < MAX_HISTORY) {
        char *entry = malloc(MAX_CMD_LENGTH + 20); // Allocate additional space for the PID and formatting
        sprintf(entry, "%d %s", pid, cmd);
        history[history_count++] = entry;
    }
}

void display_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%s\n", history[i]);
    }
}

void add_to_path(const char *path) {
    char *current_path = getenv("PATH");
    char *new_path = malloc(2 + strlen(current_path) + strlen(path));
    strcpy(new_path, current_path);
    strcat(new_path, ":");
    strcat(new_path, path);
    setenv("PATH", new_path, 1);
    free(new_path);
}


int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        add_to_path(argv[i]);
    }

    char cmdcopy[MAX_CMD_LENGTH];
    char cmd[MAX_CMD_LENGTH];

    char *args[MAX_CMD_LENGTH];
    int status;


    while (1) {

        printf("$ ");
        fflush(stdout);
        fgets(cmd, MAX_CMD_LENGTH, stdin);

	strcpy(cmdcopy,cmd);
	cmdcopy[strcspn(cmdcopy, "\n")] = '\0';

        cmd[strcspn(cmd, "\n")] = '\0';
	
	
        char *token = strtok(cmd, " ");
        int i = 0;
        while (token) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
	
	if(i == 0){
	    continue;
	}
        args[i] = NULL;

        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        if (strcmp(args[0], "cd") == 0) {
            if (chdir(args[1]) < 0) {
                perror("cd failed");
            }
            add_to_history(cmdcopy, getpid()); // Add the command and PID to history in the parent process
            continue;
        }

        if (strcmp(args[0], "history") == 0) {
            add_to_history(cmdcopy, getpid()); // Add the command and PID to history in the parent process
            display_history();
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            char error_message[50];

            sprintf(error_message, "%s failed", "fork");
            perror(error_message);
            exit(1);
        } else if (pid == 0) {
            if (execvp(args[0], args) < 0) {
                char error_message[50];
                sprintf(error_message, "%s failed", "exec");
                perror(error_message);
                exit(1);
            }
        } else {
            add_to_history(cmdcopy, pid); // Add the command and PID to history in the parent process
            waitpid(pid, &status, 0);
        }
    }

    for (int i = 0; i < history_count; i++) {
        free(history[i]);
    }

    return 0;
}

