/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexander <alexander@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/15 18:41:04 by owmarqui          #+#    #+#             */
/*   Updated: 2025/02/26 10:48:24 by alexander        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mini_shell.h"

t_minishell g_minishell;
/*
static void printtokens(t_cmd **cmds)
{
    t_cmd *aux = *cmds;
    int i = 0;
    while (aux){
	printf("--------------\n");
	printf("cabeza = %s\n", aux->name_cmd);
    i = 0;
	while(aux->args[i])
	{
		printf("argumentos[%d] = %s\n", i, aux->args[i]);
		i++;
	}
	printf("--------------\n");
	i++;
    aux = aux->next;
    }
}*/

t_cmd	*init_cmds(char **tokens)
{
	t_cmd	*cmds;
	size_t	start;
	size_t	i;

	cmds = NULL;
	start = 0;
	i = 0;
	while (tokens[i])
	{
		if (tokens[i][0] == '|')
		{
			add_cmd(&cmds, new_cmd(tokens, start, i));
			start = i + 1;
		}
		i++;
	}
	if (tokens[start])
		add_cmd(&cmds, new_cmd(tokens, start, i));
	if (cmds && cmds->next)
		cmds_has_pipes(cmds);
	return (cmds);
}

t_env	*init_envs(char **envp)
{
	t_env	*env;
	char	*name;
	size_t	i;
	char	path[1024];

	env = NULL;
	while (*envp)
	{
		i = 0;
		while ((*envp)[i] != '=')
			i++;
		name = ft_substr(*envp, 0, i);
		set_env(&env, name, ft_strdup(getenv(name)));
		free(name);
		envp++;
	}
	set_env(&env, "?", ft_strdup("0"));
	if (getcwd(path, 1024))
		set_env(&env, "PWD", ft_strdup(path));
	return (env);
}

char	*concat_strings(const char *str1, const char *str2)
{
	size_t	len1;
	size_t	len2;
	char	*result;
	size_t	i;
	size_t	j;

	if (!str1 || !str2)
		return (NULL);
	len1 = strlen(str1);
	len2 = strlen(str2);
	result = (char *)malloc(len1 + len2 + 1);
	if (!result)
		return (NULL);
	i = 0;
	while (i < len1)
	{
		result[i] = str1[i];
		i++;
	}
	j = 0;
	while (j < len2)
	{
		result[len1 + j] = str2[j];
		j++;
	}
	result[len1 + len2] = '\0';
	return (result);
}

char	*expand_variable_2(const char *input)
{
	size_t	len;
	char	*var_name;
	char	*value;
	char	*result;

	if (!input || strlen(input) < 4 || input[0] != '$' || input[1] != '('
		|| input[strlen(input) - 1] != ')')
		return (NULL);
	len = strlen(input) - 3;
	var_name = (char *)malloc(len + 1);
	if (!var_name)
		return (NULL);
	strncpy(var_name, input + 2, len);
	var_name[len] = '\0';
	value = getenv(var_name);
	free(var_name);
	if (value)
	{
		result = (char *)malloc(strlen(value) + 1);
		if (result)
		{
			strcpy(result, value);
		}
		return (result);
	}
	return (NULL);
}

char	*get_hostname(void)
{
	int		fd;
	size_t	buffer_size;
	char	*buffer;
	ssize_t	total_read;
	ssize_t	bytes_read;
	char	*new_buffer;
	int		i;

	fd = open("/etc/hostname", O_RDONLY);
	if (fd < 0)
	{
		perror("Error al abrir /etc/hostname");
		return (NULL);
	}
	buffer_size = 64;
	buffer = (char *)malloc(buffer_size);
	if (!buffer)
	{
		perror("Error al asignar memoria");
		close(fd);
		return (NULL);
	}
	total_read = 0;
	bytes_read = 0;
	bytes_read = read(fd, buffer + total_read, buffer_size - total_read - 1);
	while (bytes_read > 0)
	{
		total_read += bytes_read;
		if (total_read >= buffer_size - 1)
		{
			buffer_size *= 2;
			new_buffer = (char *)realloc(buffer, buffer_size);
			if (!new_buffer)
			{
				perror("Error al reasignar memoria");
				free(buffer);
				close(fd);
				return (NULL);
			}
			buffer = new_buffer;
		}
		bytes_read = read(fd, buffer + total_read, buffer_size - total_read - 1);
	}
	if (bytes_read < 0)
	{
		perror("Error al leer /etc/hostname");
		free(buffer);
		close(fd);
		return (NULL);
	}
	buffer[total_read] = '\0';
	i = 0;
	while (i < total_read)
	{
		if (buffer[i] == '\n')
		{
			buffer[i] = '\0';
			break ;
		}
		i++;
	}
	close(fd);
	return (buffer);
}

static	bool	readentry(t_env **envs, t_cmd **cmds)
{
	char	*line;
	char	**tokens;
	char	*userr;
	char	*hostname;

	*cmds = NULL;
	*userr = expand_variable_2("$(USER)");
	*hostnamee = get_hostname();
	prompt =  concat_strings("\033[1;32m", concat_strings(userr, concat_strings("@", concat_strings(hostnamee, "\033[1;35m:~$ \033[0m"))));
	line = readline(prompt);
	if (!line)
		return (false);
	add_history(line);
	if (line[0] == '\0')
		return (free(line), true);
	tokens = tokenize(line, *envs, NULL);
	free(line);
	if (!tokens)
	{
		set_env(envs, "?", ft_strdup("2"));
		return (true);
	}
	*cmds = init_cmds(tokens);
	free_tokens(tokens);
	return (true);
}

/*int	main(void)
{
	char	*line;
	char    *prompt;
	char    *expanded;

	signal(SIGINT, handle_sigint); //ctrl-c
	signal(SIGQUIT, SIG_IGN);
	line = NULL;
	while (1)
	{
        //expand_variable_2("$(USER)");
		int parse_error;
		char *userr = expand_variable_2("$(USER)");
		char *hostnamee = get_hostname();
        //printf("%s", hostnamee);
		prompt =  concat_strings("\033[1;32m", concat_strings(userr, concat_strings("@", concat_strings(hostnamee, "\033[1;35m:~$ \033[0m"))));
		line = readline(prompt);*/

static int	program(t_cmd **cmds, t_env **envs)
{
	while (1)
	{
		sig_parent();
		if (!readentry(envs, cmds))
			break ;
		if (*cmds)
		{
			set_env(envs, "_", ft_strdup(last_cmd_arg(*cmds)));
			//printtokens(cmds);
			// retorne el exit status capaz que tenga que hacer parte o implementalos atraves de una estrcutura
			ft_init_exec(cmds, envs);
		}
		if (g_minishell.signal > 0)
			g_minishell.exit_status = 128 + g_minishell.signal;
		set_env(envs, "?", ft_itoa(g_minishell.exit_status));
		if (g_minishell.force_exit /* || is_child_process(*cmds) */)
			return (free_cmds(*cmds), g_minishell.exit_status);
		free_cmds(*cmds);
		g_minishell.signal = 0;
	}
	return (g_minishell.exit_status);
}

int	main(int argc, char **argv, char **envp)
{
	t_cmd	*cmds;
	t_env	*tmp;

	(void)argc;
	(void)argv;
	g_minishell.force_exit = false;
	g_minishell.signal = 0;
	g_minishell.heredoc = false;
	g_minishell.envs = init_envs(envp);
	g_minishell.exit_status = program(&cmds, &g_minishell.envs);
	if (g_minishell.signal > 0)// vainas que no entiendo ya me podre a ver
		g_minishell.exit_status = 128 + g_minishell.signal;
	rl_clear_history();
	while (g_minishell.envs)
	{
		tmp = g_minishell.envs;
		g_minishell.envs = g_minishell.envs->next;
		free_env(tmp);
	}
	return (g_minishell.exit_status);
}
