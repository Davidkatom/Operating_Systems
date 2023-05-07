#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#define TIMEOUT 5

void alarm_handler(int signo) {
    printf("Child process timed out - singal %d\n", signo);
    //killpg(getpid(), SIGKILL);
    exit(4);
}

int contains_c_file(const char *folder_path) {
    DIR *folder = opendir(folder_path);
    if (!folder) {
        fprintf(stderr, "Error opening folder (%s): %s\n", folder_path, strerror(errno));
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
    if (argc != 2) {
        printf("Usage: %s <configuration_file>\n", argv[0]);
        return 1;
    }

    FILE *config_file = fopen(argv[1], "r");
    if (!config_file) {
        perror("Error opening configuration file\n");
        return 1;
    }


    char folder_path[1024], input_file[1024], correct_output_file[1024];
    fgets(folder_path, sizeof(folder_path), config_file);
    folder_path[strcspn(folder_path, "\n")] = 0;

    fgets(input_file, sizeof(input_file), config_file);
    input_file[strcspn(input_file, "\n")] = 0;

    fgets(correct_output_file, sizeof(correct_output_file), config_file);
    correct_output_file[strcspn(correct_output_file, "\n")] = 0;

    fclose(config_file);

    //printf("folder - %s\n", folder_path);
    //printf("input - %s\n", input_file);
    //printf("output - %s\n", correct_output_file);


    DIR *folder = opendir(folder_path);
    if (!folder) {
        perror("Error opening folder\n");
        return 1;
    }


    struct dirent *entry;


    while ((entry = readdir(folder)) != NULL) {
        int exit_status = 0;


        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char subfolder_path[1024];
            snprintf(subfolder_path, sizeof(subfolder_path), "%.254s/%.254s", folder_path, entry->d_name);
            printf("%s\n\n\n\n\n", subfolder_path);
            if (contains_c_file(subfolder_path) == 0) {
                printf("No C file\n");
                continue;
            }

            pid_t pid = fork();
            if (pid == 0) {
                int exit_s = 0;


                FILE *input_fp = fopen(input_file, "r");
                if (!input_fp) {
                    fprintf(stderr, "Error opening input file (%s): %s\n", input_file, strerror(errno));
                    exit(1);
                }
                dup2(fileno(input_fp), STDIN_FILENO);
                fclose(input_fp);

                FILE *output_fp = fopen("temp_output.txt", "w");
                if (!output_fp) {
                    perror("Error opening output file\n");
                    exit(1);
                }

                FILE *errors_fp = fopen("errors.txt", "a");
                if (!errors_fp) {
                    perror("Error opening errors.txt");
                    exit(1);
                }
                dup2(fileno(errors_fp), STDERR_FILENO);
                fclose(errors_fp);
                chdir(subfolder_path);

                pid_t monitor_pid = fork();
                if (monitor_pid > 0) {
                    int s;
                    signal(SIGALRM, alarm_handler);
                    alarm(TIMEOUT);
                    waitpid(monitor_pid, &s, 0);
                    //if (WIFSIGNALED(s) && WTERMSIG(s) == SIGALRM) {
                    //    compare_output = 0; // Set the flag to 0 in case of a timeout
                    //}
                } else {


                    char compile_command[1024];
                    //TODO no printf
                    snprintf(compile_command, sizeof(compile_command), "gcc *.c -o program");

                    if (system(compile_command) == 0) {
                        dup2(fileno(output_fp), STDOUT_FILENO);
                        fclose(output_fp);

                        execl("./program", "program", NULL);
                    } else {
                        perror("Error compiling C program\n");
                        exit_status = 3; // Set the exit_status to 4 in case of a timeout
                        exit(3);
                    }

                }
                printf("status = %d", exit_status);
            } else if (pid < 0) {
                perror("Error creating process");
            } else {
                int status;
                waitpid(pid, &status, 0);

                if (WIFEXITED(status)) {
                    int exit_status = WEXITSTATUS(status);
                    printf("exit status - %d\n", exit_status);
                    if (exit_status == 3) {
                        printf("Program in %s failed to compile\n", subfolder_path);
                        continue; // Skip this iteration and move on to the next directory
                    }
                }

                pid_t comp_pid = fork();
                if (comp_pid == 0) {
                    execl("./comp.out", "comp.out", "temp_output.txt", correct_output_file, NULL);
                    perror("Error executing comp.out");
                    exit(1);
                } else if (comp_pid < 0) {
                    perror("Error creating comp.out process");
                } else {
                    if(WIFEXITED(status) == 0)
                        continue;
                    int comp_status;
                    waitpid(comp_pid, &comp_status, 0);
                    if (WIFEXITED(comp_status)) {
                        int exit_status = WEXITSTATUS(comp_status);
                        if (exit_status == 1) {
                            printf("Program in %s passed the test (identical output)\n", subfolder_path);
                        } else if (exit_status == 3) {
                            printf("Program in %s failed the test (similar output)\n", subfolder_path);
                        } else if (exit_status == 2) {
                            printf("Program in %s failed the test (different output)\n", subfolder_path);
                        } else {
                            printf("Program in %s: comp.out returned an unexpected exit status (%d)\n", subfolder_path,
                                   exit_status);
                        }
                    }
                }
            }
        }
    }
    closedir(folder);
    return 0;
}
