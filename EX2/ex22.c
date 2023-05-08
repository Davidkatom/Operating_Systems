#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#define TIMEOUT 5


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>

//TODO not allowed to use fclose
//TODO change execl to execvp
//TODO remove program after launch
//TODO check if errors are in file
void append_result(const char *name, int grade, const char *reason) {
    // Check if results.csv exists
    int file_fd = open("results.csv", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (file_fd == -1) {
        perror("Error opening/creating results.csv");
        exit(1);
    }

    // Append the line to the CSV file
    char buffer[1024];
    int length = snprintf(buffer, sizeof(buffer), "%s,%d,%s\n", name, grade, reason);
    if (length < 0) {
        perror("Error formatting CSV entry");
        close(file_fd);
        exit(1);
    }
    write(file_fd, buffer, length);

    // Close the file
    close(file_fd);
}
// Timeout handler for the alarm signal
void alarm_handler(int signo) {
    //printf("Child process timed out - signal %d\n", signo);
    exit(4);
}

// Checks if the given folder contains a C source file
int contains_c_file(const char *folder_path) {
    DIR *folder = opendir(folder_path);
    if (!folder) {
        //fprintf(stderr, "Error opening folder (%s): %s\n", folder_path, strerror(errno));
        perror("Error in: opendir");
        return 0;
    }

    struct dirent *entry;
    int c_file_found = 0;
    while ((entry = readdir(folder)) != NULL) {
        if (entry->d_type == DT_REG) {
            char *dot = strrchr(entry->d_name, '.');
            if (dot && strcmp(dot, ".c") == 0) {
                c_file_found = 1;
                break;
            }
        }
    }

    closedir(folder);
    return c_file_found;
}

int main(int argc, char *argv[]) {
    // Check for correct number of arguments
    if (argc != 2) {
        //printf("Usage: %s <configuration_file>\n", argv[0]);
        return 1;
    }
    int errors_fd = open("errors.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (errors_fd == -1) {
        perror("Error in: open\n");
        exit(1);
    }
    dup2(errors_fd, STDERR_FILENO);
    close(errors_fd);

    int config_fd = open(argv[1], O_RDONLY);
    if (config_fd == -1) {
        perror("Error in: open\n");
        return 1;
    }

    char buffer[4096];
    ssize_t read_len = read(config_fd, buffer, sizeof(buffer) - 1);
    if (read_len <= 0) {
        perror("Error reading configuration file\n");
        close(config_fd);
        return 1;
    }
    buffer[read_len] = '\0'; // Null terminate the string

    close(config_fd);

    char folder_path[1024], input_file[1024], correct_output_file[1024];

    char *token = strtok(buffer, "\n");
    if (token) {
        strncpy(folder_path, token, sizeof(folder_path) - 1);
        folder_path[sizeof(folder_path) - 1] = '\0';
    } else {
        perror("Error extracting folder_path\n");
        return 1;
    }

    token = strtok(NULL, "\n");
    if (token) {
        strncpy(input_file, token, sizeof(input_file) - 1);
        input_file[sizeof(input_file) - 1] = '\0';
    } else {
        perror("Error extracting input_file\n");
        return 1;
    }

    token = strtok(NULL, "\n");
    if (token) {
        strncpy(correct_output_file, token, sizeof(correct_output_file) - 1);
        correct_output_file[sizeof(correct_output_file) - 1] = '\0';
    } else {
        perror("Error extracting correct_output_file\n");
        return 1;
    }

    // Open the main folder
    DIR *folder = opendir(folder_path);
    if (!folder) {
        //perror("Error opening folder\n");
        perror("Error in: open");

        return 1;
    }

    struct dirent *entry;

    // Iterate through the subfolders
    while ((entry = readdir(folder)) != NULL) {
        int exit_status = 0;

        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char subfolder_path[1024];
            snprintf(subfolder_path, sizeof(subfolder_path), "%.254s/%.254s", folder_path, entry->d_name);

            // Check if the subfolder contains a C source file
            if (contains_c_file(subfolder_path) == 0) {
                continue;
            }


            // Create a child process to run the program
            pid_t pid = fork();
            if (pid == 0) {
                // Child process
                // Redirect input, output, and error streams
                int input_fd = open(input_file, O_RDONLY);
                if (input_fd == -1) {
                    perror("Error in: open");
                    exit(1);
                }
                dup2(input_fd, STDIN_FILENO);
                close(input_fd);

                int output_fd = open("temp_output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (output_fd == -1) {
                    perror("Error in: open");
                    exit(1);
                }

                chdir(subfolder_path);

                pid_t monitor_pid = fork();
                if (monitor_pid > 0) {
                    // Parent of the monitor process
                    int s;
                    signal(SIGALRM, alarm_handler);
                    alarm(TIMEOUT);
                    waitpid(monitor_pid, &s, 0);
                } else {
                    // Monitor process
                    char compile_command[1024];
                    snprintf(compile_command, sizeof(compile_command), "gcc *.c -o program");

                    if (system(compile_command) == 0) {
                        dup2(output_fd, STDOUT_FILENO);

                        close(output_fd);

                        execl("./program", "program", NULL);
                    } else {
                        //perror("Error compiling C program\n");
                        exit(3);
                    }
                }
                int status;
                waitpid(monitor_pid, &status, 0);
                exit(WEXITSTATUS(status));
            } else if (pid < 0) {
                //perror("Error creating process");
                perror("Error in: fork");

            } else {
                // Parent process
                int status;
                waitpid(pid, &status, 0);

                if (WIFEXITED(status)) {
                    int exit_status = WEXITSTATUS(status);
                    if (exit_status == 3) {
                        //printf("No C file\n");
                        append_result(entry->d_name, 10, "COMPILATION_ERROR");

                        continue; // Skip this iteration and move on to the next directory
                    }
                    if (exit_status == 4) {
                        append_result(entry->d_name, 20, "TIMEOUT");
                        continue; // Skip this iteration and move on to the next directory
                    }
                }

                pid_t comp_pid = fork();
                if (comp_pid == 0) {
                    // Child process to execute comp.out
                    execl("./comp.out", "comp.out", "temp_output.txt", correct_output_file, NULL);
                    //perror("Error executing comp.out");
                    perror("Error in: execvp");
                    exit(1);
                } else if (comp_pid < 0) {
                    //perror("Error creating comp.out process");
                    perror("Error in: fork");
                } else {
                    // Parent process for comp.out
                    int comp_status;
                    waitpid(comp_pid, &comp_status, 0);

                    if (WIFEXITED(comp_status)) {
                        int exit_status = WEXITSTATUS(comp_status);
                        if (exit_status == 1) {
                            append_result(entry->d_name, 100, "EXCELLENT");
                        } else if (exit_status == 3) {
                            append_result(entry->d_name, 75, "SIMILAR");
                        } else if (exit_status == 2) {
                            append_result(entry->d_name, 100, "WRONG");
                        } else {
                            exit(1);
                        }
                    }
                }
            }
        }
    }
    closedir(folder);
    return 0;
}

