/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirecion.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexander <alexander@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 08:20:35 by alexander         #+#    #+#             */
/*   Updated: 2025/02/27 08:31:18 by alexander        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

char	*custom_strtok(char *str, const char *delim)
{
	static char	*saved_str;
	char		*token_start;

	saved_str = NULL;
	if (str)
		saved_str = str;
    if (!saved_str || *saved_str == '\0')
		return (NULL);
	token_start = saved_str;
    while (*saved_str && strchr(delim, *saved_str))
		saved_str++;
	if (*saved_str == '\0')
		return (NULL);
	token_start = saved_str;
	while (*saved_str && !strchr(delim, *saved_str))
		saved_str++;
	if (*saved_str)
	{
		*saved_str = '\0';
		saved_str++;
	}
	return (token_start);
}

void	execute_command(char *command)
{
    char	*args[64];
    char	*input_file = NULL;
    char	*output_file = NULL;
    int		append = 0;
    int		i;
	pid_t	pid;
	int fd;

	i = 0;
	char *token = custom_strtok(command, " ");
	while (token != NULL)
	{
		if (strcmp(token, ">>") == 0)
		{
			append = 1;
			token = custom_strtok(NULL, " ");
			output_file = token;
		}
		else if (strcmp(token, ">") == 0)
		{
			append = 0;
			token = custom_strtok(NULL, " ");
			output_file = token;
		}
		else if (strcmp(token, "<<") == 0)
		{
			token = custom_strtok(NULL, " ");
			input_file = token;
		}
		else if (strcmp(token, "<") == 0) 
		{
			token = custom_strtok(NULL, " ");
			input_file = token;
		}
		else
		{
			args[i++] = token;
		}
		token = custom_strtok(NULL, " ");
	}
	args[i] = NULL;

	pid = fork();
	if (pid == 0) 
	{
		if (output_file)
		{
			fd = open(output_file, O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), 0644);
			if (fd < 0)
			{
				perror("open output file");
				exit(EXIT_FAILURE);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		if (input_file)
		{
			fd = open(input_file, O_RDONLY);
			if (fd < 0)
			{
				perror("open input file");
				exit(EXIT_FAILURE);
			}
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		execve(args[0], args, NULL);
		perror("execve");
		exit(EXIT_FAILURE);
	}
	else if (pid > 0)
	{
		wait(NULL);
	}
	else
	{
		perror("fork");
	}
}

int	main(int argc, char *argv[])
{
	int		i;
	char	command[256];

	if (argc < 2)
	{
		fprintf(stderr, "Uso: %s <comando>\n", argv[0]);
		return (EXIT_FAILURE);
	}
	strcpy(command, argv[1]);
	i = 2;
	while (i < argc)
	{
		strcat(command, " ");
		strcat(command, argv[i]);
		i++;
	}
	execute_command(command);
	return (0);
}