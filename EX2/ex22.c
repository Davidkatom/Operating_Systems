#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#define TIMEOUT 20

void alarm_handler(int signum) {
    printf("Child process timed out\n");
    exit(1);
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

    FILE *output_fp = fopen("temp_output.txt", "w");
    if (!output_fp) {
        perror("Error opening output file\n");
        exit(1);
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

    FILE *input_fp = fopen(input_file, "r");
    if (!input_fp) {
        fprintf(stderr, "Error opening input file (%s): %s\n", input_file, strerror(errno));
        exit(1);
    }

    struct dirent *entry;
    while ((entry = readdir(folder)) != NULL) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char subfolder_path[1024];
            snprintf(subfolder_path, sizeof(subfolder_path), "%.254s/%.254s", folder_path, entry->d_name);
            printf("%s\n\n\n\n\n",subfolder_path);
            if(contains_c_file(subfolder_path) == 0){
                printf("No C file\n");
                continue;
            }
            pid_t pid = fork();
            if (pid == 0) {
                chdir(subfolder_path);

                signal(SIGALRM, alarm_handler);
                alarm(TIMEOUT);

//                char cwd[PATH_MAX];
//                if (getcwd(cwd, sizeof(cwd)) != NULL)
//                    printf("Current working dir: %s\n", cwd);

                char compile_command[1024];
                snprintf(compile_command, sizeof(compile_command), "gcc *.c -o program");

                if (system(compile_command) == 0) {
                    dup2(fileno(output_fp), STDOUT_FILENO);
                    fclose(output_fp);

                    execl("./program", "program", NULL);
                } else {
                    perror("Error compiling C program\n");
                }

                exit(1);
            } else if (pid < 0) {
                perror("Error creating process");
            } else {
                int status;
                waitpid(pid, &status, 0);

                pid_t comp_pid = fork();
                if (comp_pid == 0) {
                    execl("./comp.out", "comp.out", "temp_output.txt", correct_output_file, NULL);
                    perror("Error executing comp.out");
                    exit(1);
                } else if (comp_pid < 0) {
                    perror("Error creating comp.out process");
                } else {
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
                            printf("Program in %s: comp.out returned an unexpected exit status (%d)\n", subfolder_path, exit_status);
                        }
                    }
                }
            }
        }
    }
    closedir(folder);
    return 0;
}
