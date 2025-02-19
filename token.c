#include "mini_shell.h"

bool	loop_get_next_token(char *line, int *quote, size_t *i)
{
	while (line[*i] && !is_space(line[*i]))
	{
		if (line[*i] == '\'' || line[*i] == '"')
		{
			*quote = 1;
			if (!handle_quotes(line, i))
				return (error("unclosed quotes ", 0), false);
			if (line[(*i)] == '|')
				return (true);
		}
		else if ((line[*i] == '<' || line[*i] == '>'))
		{
			if ((*i) > 0 && !is_space(line[(*i) - 1]))
				break ;
			(*i)++;
			if (line[(*i)] == line[*i - 1])
				(*i)++;
			break ;
		}
		else if (is_space(line[*i]) || line[*i] == '|' || line[(*i) + 1] == '|')
			return ((*i)++, true);
		else
			(*i)++;
	}
	return (true);
}

char *get_next_token(char **line, t_env *envs, bool *split_token, char *tokenpre)
{
	char	*token;
	int		quote;
	size_t	i;

	if (!*line)
		return (NULL);
	i = 0;
	skip_spaces(*line, &i);
	*line += i;
	quote = 0;
	if (!loop_get_next_token((*line), &quote, &i))
		return (NULL);
	token = ft_substr(*line, 0, i);
	if (!token)
		return (NULL);
	if (ft_strchr(token, '$'))
		token = ft_replace_env_var(envs, token, split_token, tokenpre);
	if (quote)
		token = trimm_token_quote(&token);
	skip_spaces(*line, &i);
	*line += i;
	return (token);
}

int	loop_count_tokens(char *line, size_t *i, size_t *count)
{
	if (line[(*i)] == '\'' || line[(*i)] == '"')
	{
		if (!handle_quotes(line, i))
			return (error("unclosed quotes ", 0), 0);
	}
	else if (line[(*i)] == '<' || line[(*i)] == '>')
	{
		if ((*i) > 0 && !(is_space(line[(*i) - 1])) && line != line + (*i))
			(*count)++;
		increase_token_index(count, i);
		if ((*i) > 0 && line[(*i) - 1] == line[(*i)]) //analizar esta linea
			(*i)++;
		skip_spaces(line, i);
	}
	else if (is_space(line[(*i)]) || line[(*i)] == '|')
	{
		if (line[(*i)] == '|' && (*i) > 0 && !(is_space(line[(*i) - 1]))
			&& line[(*i) - 1] != '<' && line[(*i) - 1] != '>' )
			(*count)++;
		increase_token_index(count, i);
		skip_spaces(line, i);
	}
	else
		(*i)++;
	return (1);
}

size_t	count_tokens(char *line)
{
	size_t	i;
	size_t	count;

	i = 0;
	count = 0;
	skip_spaces(line, &i);
	while (line[i])
	{
		if (!loop_count_tokens(line, &i, &count))
			return (0);
	}
	if ((line[i] == '\0' && !is_space(line[i - 1])
			&& line[i - 1] != '<' && line[i - 1] != '>')
		|| (check_end_by_quote(line, i) && count == 0))
		count++;
	return (count);
}

char	**tokenize(char *line, t_env *envs, char *pretoken)
{
	size_t 	i;
	size_t 	j;
	size_t 	numers_tokens;
	char	**tokens;
	bool	split_token;

	split_token = false;
	i = 0;
	j = 0;
	numers_tokens = count_tokens(line);
	if (numers_tokens <= 0)
		return (NULL);
	tokens = (char **)malloc(sizeof(char *) * (numers_tokens + 1));
	if (!tokens)
		return (NULL);
	while (i++ < numers_tokens)
	{
		tokens[j++] = get_next_token(&line, envs, &split_token, pretoken);
		if (split_token)
			tokens = token_split(tokens, &j, &split_token, numers_tokens);
		pretoken = tokens[j - 1];
	}
	tokens[j] = NULL;
	return (handle_unexpected(&tokens), tokens);
}