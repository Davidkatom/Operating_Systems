#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define TIMEOUT 5

void concat_strings(char *dest, size_t dest_size, const char *src) {
    size_t dest_len = strlen(dest);
    size_t src_len = strlen(src);
    size_t to_copy = (dest_size > dest_len + src_len) ? src_len : (dest_size - dest_len - 1);
    strncpy(dest + dest_len, src, to_copy);
    dest[dest_len + to_copy] = '\0';
}

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
char *contains_c_file(const char *folder_path, char *c_file, size_t c_file_size) {
    printf("checking if there is c file in %s\n:", folder_path);
    DIR *folder = opendir(folder_path);
    if (!folder) {
        perror("Error in: opendir");
        return NULL;
    }

    struct dirent *entry;
    char *c_file_found = NULL;
    while ((entry = readdir(folder)) != NULL) {
        if (entry->d_type == DT_REG) {
            char *dot = strrchr(entry->d_name, '.');
            if (dot && strcmp(dot, ".c") == 0) {
                snprintf(c_file, c_file_size, "%s", entry->d_name);
                c_file_found = c_file;
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
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char subfolder_path[1024];
            snprintf(subfolder_path, sizeof(subfolder_path), "%.254s/%.254s", folder_path, entry->d_name);

            // Check if the subfolder contains a C source file
            char c_file_name[1024];
            if (contains_c_file(subfolder_path, c_file_name, sizeof(c_file_name)) == NULL) {
                append_result(entry->d_name, 0, "NO_C_FILE");
                printf("No c file");
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

                //chdir(subfolder_path);

                pid_t monitor_pid = fork();
                if (monitor_pid > 0) {
                    // Parent of the monitor process
                    int s;
                    signal(SIGALRM, alarm_handler);
                    alarm(TIMEOUT);
                    waitpid(monitor_pid, &s, 0);
                    if (access("program", F_OK) != -1) {
                        // File "program" exists, delete it
                        if (remove("program") != 0) {
                            perror("Error in: remove");
                        }
                    }

                } else {
                    // Monitor process
                    //char compile_command[1024];
                    char compile_command[1024] = "gcc ";
                    char full_c_file_path[1024];

                    strncpy(full_c_file_path, subfolder_path, sizeof(full_c_file_path) - 1);
                    strncat(full_c_file_path, "/", sizeof(full_c_file_path) - strlen(full_c_file_path) - 1);
                    strncat(full_c_file_path, c_file_name, sizeof(full_c_file_path) - strlen(full_c_file_path) - 1);

                    concat_strings(compile_command, sizeof(compile_command), full_c_file_path);
                    concat_strings(compile_command, sizeof(compile_command), " -o program");

                    printf("compile command = &d\n",system(compile_command) == 0);
                    if (system(compile_command) == 0) {
                        dup2(output_fd, STDOUT_FILENO);

                        close(output_fd);

                        char *exec_argv[] = {"program", NULL};
                        execvp("./program", exec_argv);
                    } else {
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
                    printf("exit status - %d\n", exit_status);
                    if (exit_status == 3) {
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
                    //execl("./comp.out", "comp.out", "temp_output.txt", correct_output_file, NULL);
                    char *exargv[] = {"comp.out", "temp_output.txt", correct_output_file, NULL};
                    execvp("./comp.out", exargv);
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
                        int comp = WEXITSTATUS(comp_status);
                        //printf("exit status - %d\n",comp);
                        if (comp == 1) {
                            append_result(entry->d_name, 100, "EXCELLENT");
                        } else if (comp == 3) {
                            append_result(entry->d_name, 75, "SIMILAR");
                        } else if (comp == 2) {
                            append_result(entry->d_name, 50, "WRONG");
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

