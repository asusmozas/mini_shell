/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_exec.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexander <alexander@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 10:26:41 by alexander         #+#    #+#             */
/*   Updated: 2025/02/19 10:26:43 by alexander        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mini_shell.h"

void	ft_free_double(char **str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		free(str[i]);
		i++;
	}
	free(str);
}

void	ft_free_cmd(t_cmd **cmds, char **envp)
{
	t_cmd	*ps;

	ps = *cmds;
	if (envp)
		ft_free_double(envp);
	while (ps)
	{
		/* if (ps->args)
			ft_free_double(ps->args) */;
		if (ps->cmd)
			ft_free_double(ps->cmd);
		if (ps->infile)
			ft_free_double(ps->infile);
		if (ps->outfile)
			ft_free_double(ps->outfile);
		if (ps->dl_hd)
			ft_free_double(ps->dl_hd);
		if (ps->pth_hd)
		{
			unlink(ps->pth_hd);
			free(ps->pth_hd);
		}
		/* if (ps->name_cmd)
			free(ps->name_cmd); */
		ps = ps->next;
	}
}
