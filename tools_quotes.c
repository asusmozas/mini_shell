/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools_quotes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexander <alexander@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/15 18:42:38 by owmarqui          #+#    #+#             */
/*   Updated: 2025/02/19 10:40:07 by alexander        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mini_shell.h"

int	skip_quotes(char *line)
{
	int		i;
	char	quote;

	i = 1;
	quote = line[0];
	while (line[i] && line[i] != quote)
		i++;
	if (line[i] == quote)
		return (i + 1);
	return (-1);
}

int	isinquote(char *token, size_t *i)
{
	size_t	j;

	j = 0;
	while (j < (*i))
	{
		if (token[j] == '"')
		{
			handle_quotes(token, &j);
			if (j > (*i))
				return (1);
		}
		j++;
	}
	return (0);
}

int	check_end_by_quote(char *line, int i)
{
	i--;
	while (i > 0 && is_space(line[i]))
		i--;
	if (line[i] == '"' || line[i] == '\'')
		return (1);
	return (0);
}


char	*trimm_token_quote(char **token)
{
	char	quote;
	char	*str;
	size_t	i;
	size_t	str_index;

	str = (char *)malloc(sizeof(char) * ft_strlen(*token) + 1);
	if (!str)
		return (NULL);
	i = 0;
	str_index = 0;
	quote = '\0';
	while (i < ft_strlen(*token))
	{
		if (!quote && (((*token)[i]) == '\'' || (*token)[i] == '\"'))
			quote = (*token)[i++];
		else if (quote && (*token)[i] == quote)
		{
			quote = '\0';
			i++;
		}
		else
			str[str_index++] = (*token)[i++];
	}
	str[str_index] = '\0';
	return (free(*token), tr);
}

bool	handle_quotes(char *line, size_t *i)
{
	int	skip;

	skip = skip_quotes(line + (*i));
	if (skip == -1)
		return (false);
	(*i) += skip;
	return (true);
}
