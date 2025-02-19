/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extra_token.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexander <alexander@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 10:24:55 by alexander         #+#    #+#             */
/*   Updated: 2025/02/19 10:26:16 by alexander        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mini_shell.h"

char	**token_split(char **tokens, size_t *i, bool *split_token, int k)
{
	char		**new_tokens;
	size_t 		j;

	new_tokens = (char **)malloc(sizeof (char *) * (k + 2));
	if (!new_tokens)
		return (0);
	j = 0;
	while (j < (*i) - 1)
	{
		new_tokens[j] = tokens[j];
		j++;
	}
	k = 0;
	while (!is_space(tokens[(*i) - 1][k]))
		k++;
	new_tokens[j++] = ft_substr(tokens[(*i) - 1], 0, k);
	new_tokens[j] = ft_strdup(tokens[(*i) - 1] + k + 1);
	free(tokens[(*i) - 1]);
	free(tokens);
	(*i)++;
	*split_token = false;
	return (new_tokens);
}
