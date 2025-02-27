/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirecion.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexander <alexander@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 08:20:35 by alexander         #+#    #+#             */
/*   Updated: 2025/02/27 10:58:50 by alexander        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

char *custom_strtok(char *str, const char *delim) {
    static char *saved_str = NULL;
    if (str) saved_str = str;
    if (!saved_str || *saved_str == '\0') return NULL;
    
    char *token_start = saved_str;
    while (*saved_str && strchr(delim, *saved_str)) saved_str++;
    if (*saved_str == '\0') return NULL;
    
    token_start = saved_str;
    while (*saved_str && !strchr(delim, *saved_str)) saved_str++;
    
    if (*saved_str) {
        *saved_str = '\0';
        saved_str++;
    }
    
    return token_start;
}

char *find_executable(char *command) {
    if (access(command, X_OK) == 0) return command;
    
    char *path = getenv("PATH");
    if (!path) return NULL;
    
    char *path_copy = strdup(path);
    char *dir = custom_strtok(path_copy, ":");
    static char full_path[256];
    
    while (dir) {
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);
        if (access(full_path, X_OK) == 0) {
            free(path_copy);
            return full_path;
        }
        dir = custom_strtok(NULL, ":");
    }
    free(path_copy);
    return NULL;
}

void execute_command(char *command) {
    char *args[64];
    char *input_file = NULL;
    char *output_file = NULL;
    int append = 0;
    int i = 0;
    
    char *token = custom_strtok(command, " ");
    while (token != NULL) {
        if (strcmp(token, ">>") == 0) {
            append = 1;
            token = custom_strtok(NULL, " ");
            output_file = token;
        } else if (strcmp(token, ">") == 0) {
            append = 0;
            token = custom_strtok(NULL, " ");
            output_file = token;
        } else if (strcmp(token, "<<") == 0) {
            token = custom_strtok(NULL, " ");
            input_file = token;
        } else if (strcmp(token, "<") == 0) {
            token = custom_strtok(NULL, " ");
            input_file = token;
        } else {
            args[i++] = token;
        }
        token = custom_strtok(NULL, " ");
    }
    args[i] = NULL;
    
    char *exec_path = find_executable(args[0]);
    if (!exec_path) {
        fprintf(stderr, "Comando no encontrado: %s\n", args[0]);
        return;
    }
    
    pid_t pid = fork();
    if (pid == 0) { // Proceso hijo
        if (output_file) {
            int fd = open(output_file, O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), 0644);
            if (fd < 0) {
                perror("open output file");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        if (input_file) {
            int fd = open(input_file, O_RDONLY);
            if (fd < 0) {
                perror("open input file");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        execve(exec_path, args, NULL);
        perror("execve");
        exit(EXIT_FAILURE);
    } else if (pid > 0) { // Proceso padre
        wait(NULL);
    } else {
        perror("fork");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <comando>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    char command[256];
    strcpy(command, argv[1]);
    for (int i = 2; i < argc; i++) {
        strcat(command, " ");
        strcat(command, argv[i]);
    }
    
    execute_command(command);
    return 0;
}

