/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_file.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexander <alexander@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 10:40:40 by alexander         #+#    #+#             */
/*   Updated: 2025/02/19 10:40:51 by alexander        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mini_shell.h"
// parte curiosa de ver luego si tengo fuerzas te explico
char	*join_path(char *s1, char *s2)
{
	char	*output;
	size_t	s1_len;
	size_t	s2_len;

	if (!s1 || !s2)
		return (NULL);
	s1_len = ft_strlen(s1);
	if (s1[s1_len - 1] == '/')
		s1_len--;
	s2_len = ft_strlen(s2);
	if (s2[0] == '/')
		s2_len--;
	output = malloc(sizeof(char) * (s1_len + s2_len + 2));
	if (!output)
		return (NULL);
	ft_memcpy(output, s1, s1_len);
	output[s1_len] = '/';
	ft_memcpy(output + s1_len + 1, s2, s2_len);
	output[s1_len + s2_len + 1] = '\0';
	return (output);
}

char	*resolve_path(char *file, t_env *envs, int perms)
{
	t_env	*path;
	char	**paths;
	size_t	i;
	char	*output;

	if (!file || !file[0])
		return (NULL);
	if (access(file, perms) >= 0)
		return (ft_strdup(file));
	path = get_env(envs, "PATH");
	if (!path)
		return (NULL);
	paths = ft_split(path->value, ':');
	i = 0;
	while (paths[i])
	{
		output = join_path(paths[i], file);
		if (access(output, perms) >= 0)
			break ;
		free(output);
		output = NULL;
		free(paths[i++]);
	}
	free(paths);
	return (output);
}
