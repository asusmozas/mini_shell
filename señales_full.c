/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   señales_full.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexander <alexander@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 10:31:21 by alexander         #+#    #+#             */
/*   Updated: 2025/02/19 10:31:23 by alexander        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mini_shell.h"

void	main_signal(int signal)
{
	g_minishell.signal = signal;
	if (signal == SIGINT)
	{
		if (!g_minishell.heredoc)
			write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
		set_env(&g_minishell.envs, "?",
			ft_itoa(128 + g_minishell.signal));
	}
}

// sacado de un repo para heredoc
void	sig_heredoc(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
}

void	sig_child(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

void	sig_parent(void)
{
	signal(SIGINT, main_signal);
	signal(SIGQUIT, SIG_IGN);
}

void	sig_ignore(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

// asociada a pwd
int	builtin_pwd(t_cmd *cmd, t_env **envs)
{
	char	path[1024];

	(void)envs;
	(void)cmd;
	if (!getcwd(path, 1024))
	{
		perror("pwd");
		return (EXIT_FAILURE);
	}
	if (!ft_putendl_fd(path, STDOUT_FILENO))
		return (error_write("pwd"), EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

// asociado a unset
int	builtin_unset(t_cmd *cmd, t_env **envs)
{
	size_t	i;

	i = 1;
	while (cmd->args[i])
	{
		if (!remove_env(envs, cmd->args[i]))
			return (0);
		i++;
	}
	return (EXIT_SUCCESS);
}

//asociada a export
void	error_identifier(char *identifier)
{
	ft_putstr_fd("minishell: export: `", STDERR_FILENO);
	ft_putstr_fd(identifier, STDERR_FILENO);
	ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
}

int	builtin_export(t_cmd *cmd, t_env **envs)
{
	size_t	i;
	size_t	j;

	i = 1;
	while (cmd->args && cmd->args[i])
	{
		j = 0;
		while (cmd->args[i][j] && cmd->args[i][j] != '=')
		{
			if ((cmd->args[i][0] >= '0' && cmd->args[i][0] <= '9')
				|| special_char(cmd->args[i][j]))
				return (error_identifier(cmd->args[i]), EXIT_FAILURE);
			j++;
		}
		if (j == 0)
			return (error_identifier(&cmd->args[i][j]), EXIT_FAILURE);
		if (cmd->args[i][j])
		{
			cmd->args[i][j] = '\0';
			if (!set_env(envs, cmd->args[i], ft_strdup(&cmd->args[i][j + 1])))
				return (EXIT_FAILURE);
		}
		i++;
	}
	return (EXIT_SUCCESS);
}

void	handle_argument_with_equals(char *arg, t_env **envs)
{
	t_env	*new_node;

	new_node = create_new_env_node(arg);
	if (new_node)
		add_env_node(envs, new_node);
}

void	handle_argument_without_equals(char *arg, t_env **envs)
{
	t_env	*found_env;
	t_env	*new_node;

	found_env = find_env_node(envs, arg);
	if (!found_env)
	{
		new_node = create_new_env_node(arg);
		if (new_node)
			add_env_node(envs, new_node);
	}
}

t_env	*find_env_node(t_env **env, char *key)
{
	t_env	*tmp;
	size_t	key_len;

	tmp = *env;
	key_len = ft_strlen(key);
	while (tmp)
	{
		if (ft_strncmp(tmp->key, key, key_len) == 0
			&& tmp->key[key_len] == '\0')
			return (tmp);
		tmp = tmp->next;
	}
	return (NULL);
}

void	add_env_node(t_env **env, t_env *new_node)
{
	t_env	*tmp;
	size_t	new_node_key_len;

	tmp = *env;
	new_node_key_len = ft_strlen(new_node->key) + 1;
	while (tmp)
	{
		if (ft_strncmp(tmp->key, new_node->key, new_node_key_len) == 0)
		{
			free(tmp->value);
			tmp->value = ft_strdup(new_node->value);
			free_env_node(new_node);
			return ;
		}
		if (tmp->next == NULL)
			break ;
		tmp = tmp->next;
	}
	tmp->next = new_node;
}

void	free_env_node(t_env *node)
{
	if (node)
	{
		free(node->key);
		free(node->value);
		free(node);
	}
}

t_env	*sort_env_list(t_env **env)
{
	t_env	*tmp1;
	t_env	*tmp2;
	int		swapped;

	if (!env)
		return (NULL);
	swapped = 1;
	while (swapped)
	{
		swapped = 0;
		tmp1 = *env;
		while (tmp1->next != NULL)
		{
			tmp2 = tmp1->next;
			if (cmp_env_nodes(tmp1, tmp2))
			{
				swap_env_nodes(tmp1, tmp2);
				swapped = 1;
			}
			tmp1 = tmp1->next;
		}
	}
	return (*env);
}

int	cmp_env_nodes(t_env *tmp1, t_env *tmp2)
{
	size_t	max_length;

	if (ft_strlen(tmp1->key) > ft_strlen(tmp2->key))
		max_length = ft_strlen(tmp1->key);
	else
		max_length = ft_strlen(tmp2->key);
	if (ft_strncmp(tmp1->key, tmp2->key, max_length) > 0)
		return (1);
	else
		return (0);
}

void	swap_env_nodes(t_env *tmp1, t_env *tmp2)
{
	char	*tmp_key;
	char	*tmp_value;

	tmp_key = tmp1->key;
	tmp_value = tmp1->value;
	tmp1->key = tmp2->key;
	tmp1->value = tmp2->value;
	tmp2->key = tmp_key;
	tmp2->value = tmp_value;
}

t_env	*create_new_env_node(char *key_value)
{
	t_env	*new_node;
	char	*equal_sign;

	new_node = (t_env *)malloc(sizeof(t_env));
	if (!new_node)
		return (NULL);
	equal_sign = ft_strchr(key_value, '=');
	if (equal_sign)
	{
		*equal_sign = '\0';
		new_node->key = ft_strdup(key_value);
		new_node->value = ft_strdup(equal_sign + 1);
		*equal_sign = '=';
	}
	else
	{
		new_node->key = ft_strdup(key_value);
		new_node->value = ft_strdup("");
	}
	new_node->next = NULL;
	return (new_node);
}


void	error_numerical_arg2(char *arg)
{
	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
}

int	verify_args(char **args)
{
	int	i;
	int	j;

	i = 1;
	while (args[i])
	{
		j = 0;
		if (args[i][0] == '-' || args[i][0] == '+')
			j++;
		while (args[i][j])
		{
			if (!ft_isdigit(args[i][j]))
				return (error_numerical_arg2(args[i]), 255);
			j++;
		}
		if (args[i + 1])
		{
			g_minishell.force_exit = false;
			return (error("exit", "too many arguments"), EXIT_FAILURE);
		}
		i++;
	}
	return (EXIT_SUCCESS);
}
// vainas para joder  creo
bool	is_overflowing(const char *s)
{
	bool	is_negative;

	is_negative = false;
	if (s && *s == '-')
		is_negative = true;
	if (ft_strlen(s) - is_negative > 19)
		return (true);
	if (is_negative && ft_strcmp(s + 1, "9223372036854775808") > 0)
		return (true);
	else if (!is_negative && ft_strcmp(s, "9223372036854775807") > 0)
		return (true);
	return (false);
}

void	exit_arg(t_cmd *cmd)
{
	long long	exit_code;
	t_env		*tmp;

	exit_code = ft_atoll(cmd->args[1]);
	free_cmds(cmd);
	while (g_minishell.envs)
	{
		tmp = g_minishell.envs;
		g_minishell.envs = g_minishell.envs->next;
		free_env(tmp);
	}
	exit(exit_code);
}

int	builtin_exit(t_cmd *cmd, t_env **envs)
{
	int		verify_status;

	(void)envs;
	g_minishell.force_exit = true;
	if (!cmd->has_pipe)
		ft_putstr_fd("exit\n", STDERR_FILENO);
	verify_status = verify_args(cmd->args);
	if (verify_status != EXIT_SUCCESS)
		return (verify_status);
	if (cmd->args[1] && is_overflowing(cmd->args[1]))
		return (error_numerical_arg2(cmd->args[1]), 255);
	if (cmd->args[1])
		exit_arg(cmd);
	return (EXIT_SUCCESS);
}

bool	is_special_env(char *key)
{
	char	*specials[1];
	size_t	i;

	specials[0] = "?";
	i = 0;
	while (i < 1)
	{
		if (!ft_strcmp(specials[i], key))
			return (true);
		i++;
	}
	return (false);
}

int	builtin_env(t_cmd *cmd, t_env **envs)
{
	t_env	*env;

	(void)cmd;
	env = *envs;
	while (env)
	{
		if (!is_special_env(env->key) && env->value)
		{
			if (!ft_putstr_fd(env->key, STDOUT_FILENO)
				|| !ft_putstr_fd("=", STDOUT_FILENO)
				|| !ft_putendl_fd(env->value, STDOUT_FILENO))
				return (error_write("env"), EXIT_FAILURE);
		}
		env = env->next;
	}
	return (EXIT_SUCCESS);
}

int	handle_options(char **args, bool *n_option)
{
	size_t	i;
	size_t	j;

	*n_option = false;
	i = 1;
	while (args && args[i])
	{
		if (args[i][0] != '-')
			break ;
		j = 1;
		while (args[i][j])
		{
			if (args[i][j] != 'n')
			{
				if (i == 1)
					*n_option = false;
				return (i - 1);
			}
			*n_option = true;
			j++;
		}
		i++;
	}
	return (i - 1);
}

int	builtin_echo(t_cmd *cmd, t_env **envs)
{
	bool	n_option;
	size_t	i;

	(void)envs;
	i = 1 + handle_options(cmd->args, &n_option);
	while (cmd && cmd->args && cmd->args[i])
	{
		if (cmd->args[i][0])
		{
			if (!ft_putstr_fd(cmd->args[i], STDOUT_FILENO))
				return (error_write("echo"), EXIT_FAILURE);
			if (cmd->args[i + 1])
				if (!ft_putstr_fd(" ", STDOUT_FILENO))
					return (error_write("echo"), EXIT_FAILURE);
		}
		i++;
	}
	if (!n_option)
		if (!ft_putstr_fd("\n", STDOUT_FILENO))
			return (error_write("echo"), EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

void	error_invalid(char *path)
{
	struct stat	sb;

	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd("cd: ", STDERR_FILENO);
	ft_putstr_fd(path, STDERR_FILENO);
	if (stat(path, &sb) == -1)
		ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
	else if (!S_ISDIR(sb.st_mode))
		ft_putstr_fd(": Not a directory\n", STDERR_FILENO);
	else
		ft_putstr_fd(": Unknown error\n", STDERR_FILENO);
}

char	*home_path(t_env **envs)
{
	t_env	*env;

	env = get_env(*envs, "HOME");
	if (!env)
		return (error("cd", "HOME not set"), NULL);
	return (env->value);
}

char	*get_path(t_cmd *cmd, t_env **envs)
{
	size_t	i;
	char	*path;
	char	*home;

	i = 1;
	path = NULL;
	while (cmd->args[i])
	{
		if (cmd->args[i][0])
		{
			if (path)
				return (error("cd", "too many arguments"), NULL);
			path = cmd->args[i];
		}
		i++;
	}
	if (!path)
	{
		home = home_path(envs);
		if (home)
			return (ft_strdup(home));
		return (NULL);
	}
	return (ft_strdup(path));
}

void	tilted_path(t_env **envs, char **path)
{
	char	*home;
	char	*tmp;

	home = home_path(envs);
	if (home)
	{
		tmp = ft_strjoin(home, *path + 1);
		free(*path);
		*path = tmp;
	}
}

int	builtin_cd(t_cmd *cmd, t_env **envs)
{
	char		*path;
	char		current[1024];

	path = get_path(cmd, envs);
	if (path && path[0] == '~')
		tilted_path(envs, &path);
	if (!path)
		return (0);
	if (path[0])
	{
		if (chdir(path) == -1)
		{
			error_invalid(path);
			return (free(path), EXIT_FAILURE);
		}
		if (getcwd(current, 1024))
			set_env(envs, "PWD", ft_strdup(current));
	}
	free(path);
	return (EXIT_SUCCESS);
}

void	ft_echo_env_pwd(t_cmd *cmd, t_env **env)
{
	if (ft_strcmp(cmd->name_cmd, "echo") == 0)
		g_minishell.exit_status = builtin_echo(cmd, env);
	else if (ft_strcmp(cmd->name_cmd, "env") == 0)
		g_minishell.exit_status = builtin_env(cmd, env);
	else if (ft_strcmp(cmd->name_cmd, "pwd") == 0)
		g_minishell.exit_status = builtin_pwd(cmd, env);
	/* if(g_minishell.exit_status == EXIT_FAILURE) //si meten opt en lo comando los tira como bien
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd->name_cmd, 2);
		ft_putstr_fd(" command not found\n", 2);
	} */
}

void	ft_cd_exit_export_unset(t_cmd *cmd, t_env **env)
{
	if (ft_strcmp(cmd->name_cmd, "cd") == 0)
		g_minishell.exit_status = builtin_cd(cmd, env);
	else if (ft_strcmp(cmd->name_cmd, "exit") == 0)
		g_minishell.exit_status = builtin_exit(cmd, env);
	else if (ft_strcmp(cmd->name_cmd, "export") == 0)
		g_minishell.exit_status = builtin_export(cmd, env);
	else if (ft_strcmp(cmd->name_cmd, "unset") == 0)
		g_minishell.exit_status = builtin_unset(cmd, env);
	/* if(g_minishell.exit_status == EXIT_FAILURE) //si meten opt en lo comando los tira como bien
	{
		perror(cmd->name_cmd);
	} */
}

int	ft_execute_built(t_cmd *cmd, t_env **env)
{
	int	pipex[2];

	ft_pipe(pipex);
	if (!ft_fork())
	{
		close(pipex[READ]);
		if (cmd->outfile)
			ft_outfile(cmd, STDOUT_FILENO);
		else
			dup2(pipex[WRITE], STDOUT_FILENO);
		close(pipex[WRITE]);
		ft_echo_env_pwd(cmd, env);
		exit(0);
	}
	close(pipex[WRITE]);
	return (pipex[READ]);
}

int	ft_builtin(t_cmd *cmd, t_env **env, int len)
{
	if (ft_strcmp(cmd->name_cmd, "echo") == 0 || \
		ft_strcmp(cmd->name_cmd, "env") == 0 || \
		ft_strcmp(cmd->name_cmd, "pwd") == 0)
	{
		if ((len == 1 || !cmd->next) && (cmd->outfile == NULL))
			ft_echo_env_pwd(cmd, env);
		else
			return (ft_execute_built(cmd, env));
	}
	else if (len == 1)
		ft_cd_exit_export_unset(cmd, env);
	return (STDIN_FILENO);
}

int	is_builtin(t_cmd *cmd)
{
	if (cmd->name_cmd == NULL)
		return (0);
	else if (ft_strcmp(cmd->name_cmd, "cd") == 0)
		return (1);
	else if (ft_strcmp(cmd->name_cmd, "echo") == 0)
		return (1);
	else if (ft_strcmp(cmd->name_cmd, "env") == 0)
		return (1);
	else if (ft_strcmp(cmd->name_cmd, "exit") == 0)
		return (1);
	else if (ft_strcmp(cmd->name_cmd, "export") == 0)
		return (1);
	else if (ft_strcmp(cmd->name_cmd, "pwd") == 0)
		return (1);
	else if (ft_strcmp(cmd->name_cmd, "unset") == 0)
		return (1);
	return (0);
}