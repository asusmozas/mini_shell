/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_exec.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexander <alexander@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/15 18:43:18 by owmarqui          #+#    #+#             */
/*   Updated: 2025/02/19 10:17:41 by alexander        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mini_shell.h"

int	ft_check_path(t_cmd *current, char **envp)
{
	int		i;
	char	**env_path;
	char	*path;
	char	*path_cmd;

	i = ft_path(envp);
	env_path = ft_split(envp[i] + 5, ':');
	i = -1;
	while (env_path[++i])
	{
		path = ft_strjoin(env_path[i], "/");
		path_cmd = ft_strjoin(path, current->cmd[0]);
		free(path);
		if (access(path_cmd, X_OK) == 0)
		{
			current->pth_cmd = ft_strdup(path_cmd);
			free(path_cmd);
			ft_free_double(env_path);//revisar si tengo este free
			return (0);
		}
		free(path_cmd);
	}
	ft_free_double(env_path);
	return (1);
}

int	ft_check_cmd(t_cmd *current, char **envp)
{
	if (access(current->cmd[0], X_OK) != 0)
		return (ft_check_path(current, envp));
	else
	{
		current->pth_cmd = ft_strdup(current->cmd[0]);
		if (!current->pth_cmd)
			return (1);
		return (0);
	}
}

void	ft_breack_check(t_cmd *current, char **envp)
{
	if ((current->cmd && !current->cmd[0])
		|| (ft_check_cmd(current, envp) != 0))
		error_st(current->name_cmd, "command not found", 127);
	if (current->infile && (!current->infile[0] || !current->infile[1]))
		perror ("falla el infile");//status = 127;
	if (current->outfile && (!current->outfile[0] || !current->outfile[1]))
		perror ("falla el outfile");//status = 127;
	if (current->infile && current->infile[0][1] == '<'
		&& current->infile[0][0] == '<')
		if (current->dl_hd && !current->dl_hd[0])
			perror ("falla el heredoc, el dl");//status = 127;		
}

void	ft_break_dl(t_cmd *current, int *i)//funciones para heredoc
{
	char	*aux;
	int		j;

	aux = ft_calloc(sizeof(char), 1);
	if (!aux)
		return ;
	j = 0;
	while (current->dl_hd && current->dl_hd[j])
	{
		aux = ft_strjoin_gnl(aux, current->dl_hd[j]);
		aux = ft_strjoin_gnl(aux, " ");
		free(current->dl_hd[j]);
		j++;
	}
	free(current->dl_hd);
	aux = ft_strjoin_gnl(aux, current->args[*i + 1]);
	aux = ft_strjoin_gnl(aux, " ");
	current->dl_hd = ft_split(aux, 32);
	free(aux);
}

//no se para que esta la variable args, corrigelo cuando puedas.
void	ft_break_redir(t_cmd *current, char **args, int *i)
{
	char	*aux;
	int		file;

	aux = ft_strjoin(args[*i], " ");
	aux = ft_strjoin_gnl(aux, args[*i + 1]);
	if (current->args[*i][0] == '<')
	{
		if (current->args[*i][1] == '<')
			ft_break_dl(current, i);
		else if (access(args[*i + 1], F_OK) == -1)
			perror (args[*i]); //Gestionar mejor el error con exitstatus
		current->infile = ft_split(aux, 32);
	}
	if (current->args[*i][0] == '>')
	{
		current->outfile = ft_split(aux, 32);
		file = ft_open(args[*i + 1], O_WRONLY | O_CREAT | O_TRUNC);
		close(file);
	}
	(*i) += 2;
	free(aux);
	if ((current->args[*i]) && (current->args[*i + 1])
		&& (current->args[*i][0] == '<' || current->args[*i][0] == '>'))
		ft_break_redir(current, args, i);
}

void	ft_check_exec(t_cmd *current, char **envp)
{
	char	*aux;
	int		i;

	while (current)
	{
		aux = ft_calloc(sizeof(char), 1);
		if (!aux)
			return ;
		i = 0;
		while (current->args[i])
		{
			if ((current->args[i + 1]) && (current->args[i][0] == '<'
				|| current->args[i][0] == '>'))
				ft_break_redir(current, current->args, &i);
			else
			{
				aux = ft_strjoin_gnl(aux, current->args[i]);
				aux = ft_strjoin_gnl(aux, " ");
				i++;
			}
		}
		current->cmd = ft_split(aux, 32);
		free(aux);
		ft_breack_check(current, envp);
		current = current->next;
	}
}
