/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexander <alexander@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/15 18:41:04 by owmarqui          #+#    #+#             */
/*   Updated: 2025/02/19 10:29:06 by alexander        ###   ########.fr       */
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

static bool	readentry(t_env **envs, t_cmd **cmds)
{
	char	*line;
	char	**tokens;

	*cmds = NULL;
	line = readline("minishell$ ");
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



