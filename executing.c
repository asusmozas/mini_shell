/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executing.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexander <alexander@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 14:41:20 by owmarqui          #+#    #+#             */
/*   Updated: 2025/02/19 10:24:33 by alexander        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


int	count_cmd_nodes(t_cmd *cmds)
{
	int	count;

	count = 0;
	while (cmds)
	{
		count++;
		cmds = cmds->next;
	}
	return (count);
}

int	ft_open(char *file, int flags)
{
	int	fd;

	fd = open(file, flags, 0644);
	if (fd < 0)
		perror(file);
	return (fd);
}

void	ft_infile(struct s_cmd *ps, int std)
{
	if (ps->infile && ps->infile[0][0] == '<')
	{
		if (ps->infile && ps->infile[0][1] == '<' && ps->infile[0][0] == '<')
			std = ft_open(ps->pth_hd, O_RDONLY);
		else
			std = ft_open(ps->infile[1], O_RDONLY);
		dup2(std, STDIN_FILENO);
		close(std);
	}
	else if (std != STDIN_FILENO)
	{
		dup2(std, STDIN_FILENO);
		close(std);
	}
}

void	ft_outfile(struct s_cmd *ps, int std)
{
	if (ps->outfile && ps->outfile[0][0] == '>')
	{
		if (ps->outfile[0][1] == '>' && ps->outfile[0][0] == '>')
			std = ft_open(ps->outfile[1], O_WRONLY | O_CREAT | O_APPEND);
		else
			std = ft_open(ps->outfile[1], O_WRONLY | O_CREAT | O_TRUNC);
		dup2(std, STDOUT_FILENO);
		close(std);
	}
	else if (std != STDOUT_FILENO)
	{
		dup2(std, STDOUT_FILENO);
		close(std);
	}
}

pid_t	ft_fork(void)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
		perror("fork");
	return (pid);
}

void	ft_pipe(int fd[2])
{
	int	p;

	p = pipe(fd);
	if (p < 0)
		perror("pipe");
}

void	ft_execute(t_cmd *current, char **envp, int infile, int outfile)
{
	pid_t	pid;

	pid = ft_fork();
	if (pid == 0)
	{
		ft_infile(current, infile);
		ft_outfile(current, outfile);
		if (execve(current->pth_cmd, current->cmd, envp) == -1)
			exit(0);
	}
}

int	ft_pipex(t_cmd *cmd, char **envp, int file)
{
	int	pipex[2];

	ft_pipe(pipex);
	ft_execute(cmd, envp, file, pipex[WRITE]);
	close(pipex[WRITE]);
	if (file != STDIN_FILENO)
		close(file);
	return (pipex[READ]);
}

void	ft_wait_for_childs(void)
{
	while (1)
	{
		if (waitpid(-1, NULL, 0) == -1)
			break ;
	}
}

void	ft_init_exec(t_cmd **cmds, t_env **env)
{
	t_cmd	*current;
	int		file;
	int		len;
	char	**envp;

	current = *cmds;
	len = count_cmd_nodes(*cmds);
	envp = format_env(*env);
	ft_check_exec(current, envp);
	//ft_init_heredoc(current, env); // apañetelas 
	file = STDIN_FILENO;
	while (current)
	{
		if (is_builtin(current))
			file = ft_builtin(current, env, len);
		if (!current->next)
			ft_execute(current, envp, file, STDOUT_FILENO);
		else
			file = ft_pipex(current, envp, file);
		current = current->next;
	}
	if (file != STDIN_FILENO)
		close(file);
	ft_wait_for_childs();
	ft_free_cmd(cmds, envp);
}
