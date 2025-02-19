/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_shell.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alexander <alexander@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 15:11:06 by owmarqui          #+#    #+#             */
/*   Updated: 2025/02/19 10:30:54 by alexander        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# define MINI_SHELL_H

# include "Libft/libft.h"
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>
# include <errno.h>
# include <string.h>
# include <stdbool.h>
# include <stddef.h>
# include <dirent.h>
# define HEREDOC_FILE ".heredoc"

# define WRITE	1
# define READ	0
# define BUFFER	4096

typedef struct s_cmd
{
	char			**envp;
	char			**args;
	char			**cmd;		//bloque cmd
	char			**infile;	//bloque de entrada
	char			**outfile;	//bloque de salida
	char			**dl_hd;	//Delmitadores el heredoc
	char			*pth_hd;	//nombre dle archivo tmp
	char			*pth_cmd;
	char			*name_cmd;	//no lo uso
	bool			has_pipe;	//no lo uso, las variables de abajo
	int				cmd_count;
	struct s_cmd	*next;
}	t_cmd;

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_builtin
{
	char	*name;
	int		(*func)(t_cmd *cmd, t_env **envs);
}	t_builtin;

typedef struct s_minishell
{
	bool	force_exit;
	bool	heredoc; //para que tu quieres eso
	int		signal;
	int		exit_status;
	t_env	*envs;
}	t_minishell;

extern t_minishell	g_minishell;

//util bueno para la libft
long long ft_atoll(const char *str);

t_cmd	*init_cmds(char **tokens);

t_cmd	*new_cmd(char **tokens, size_t start, size_t end);
void	add_cmd(t_cmd **cmds, t_cmd *new);
char	*last_cmd_arg(t_cmd *cmds);
void	free_cmds(t_cmd *cmds);

void	ft_check_exec(t_cmd *current, char **envp);
void	ft_break_redir(t_cmd *current, char **args, int *i);
void	ft_break_dl(t_cmd *current, int *i);
void	ft_breack_check(t_cmd *current, char **envp);
int		ft_check_cmd(t_cmd *current, char **envp);
int		ft_check_path(t_cmd *current, char **envp);

char	**format_env(t_env *envs);
size_t	count_envs(t_env *envs);
int		ft_path(char **env);

int		count_cmd_nodes(t_cmd *cmds);
int		ft_open(char *file, int flags);
void	ft_infile(struct s_cmd *ps, int std);
void	ft_outfile(struct s_cmd *ps, int std);
pid_t	ft_fork(void);
void	ft_pipe(int fd[2]);
void	ft_execute(t_cmd *current, char **envp, int infile, int outfile);
int		ft_pipex(t_cmd *cmd, char **envp, int file);
void	ft_wait_for_childs(void);
void	ft_init_exec(t_cmd **cmds, t_env **env);

char	**token_split(char **tokens, size_t *i, bool *split_token, int k);

bool	loop_get_next_token(char *line, int *quote, size_t *i);
char	*get_next_token(char **line, t_env *envs, bool *split_token, char *tokenpre);
int		loop_count_tokens(char *line, size_t *i, size_t *count);
size_t	count_tokens(char *line);
char	**tokenize(char *line, t_env *envs, char *pretoken);

int		skip_quotes(char *line);
int		isinquote(char *token, size_t *i);
int		check_end_by_quote(char *line, int i);
char	*trimm_token_quote(char **token);
bool	handle_quotes(char *line, size_t *i);

char	*ft_strjoin_gnl(char *stash, char *buff);
void	cmds_has_pipes(t_cmd *cmds);

char	*join_path(char *s1, char *s2);
char	*resolve_path(char *file, t_env *envs, int perms);

void	skip_spaces(char *line, size_t *i);
bool	is_space(char c);
void	increase_token_index(size_t *count, size_t *i);
bool	special_char(char c);

//utils parsing
bool	check_tokens(char **tokens, int i);
bool	check_newline(char **tokens);
bool	handle_unexpected(char ***tokens);

//funciones de errores
void	error_unexpected(char *token, size_t len);
void	error(char *msg, char *more);
void	error_st(char *msg, char *more, int exit_status);
void	error_e(char *msg, char *more, int exit_status);
void	error_numerical_arg(char *name, char *arg);
void	error_write(char *cmd);

//frees

void	ft_free_double(char **str);
void	ft_free_cmd(t_cmd **cmds, char **envp);
void	free_tokens(char **tokens);

void    free_cmds(t_cmd *cmds);

// funciones de env
t_env	*get_env(t_env *envs, char *key);
t_env	*new_env(char *key, char *value);
t_env	*set_env(t_env **envs, char *key, char *value);
t_env	*remove_env(t_env **envs, char *key);
void	free_env(t_env *env);

//fucion de env con $
char	*ft_replace_env_var(t_env *envs, char *token, bool *split_token, char *tokenpre);

// funcion de heredoc en proceso recordar hacerlo he implementarlo en el makefile
void	ft_init_heredoc(t_cmd *current, t_env **envs);


//------full señales apañate como hacerlo---- gestionar cat -e cuando aparece C^ puede que se duplique

void	main_signal(int signal);
void	sig_heredoc(void);
void	sig_child(void);
void	sig_parent(void);
void	sig_ignore(void);

int		builtin_pwd(t_cmd *cmd, t_env **envs);//pwd
int		builtin_unset(t_cmd *cmd, t_env **envs);//unset
//export
void	error_identifier(char *identifier);
int		builtin_export(t_cmd *cmd, t_env **envs);
// utils de export el maldito bash se tiene que ordenar de una forma
// concreta esto directamente lo he sacado de un github mucha paja
void	handle_argument_with_equals(char *arg, t_env **envs);
void	handle_argument_without_equals(char *arg, t_env **envs);
t_env	*find_env_node(t_env **env, char *key);
void	add_env_node(t_env **env, t_env *new_node);
void	free_env_node(t_env *node);
t_env	*sort_env_list(t_env **env);
int		cmp_env_nodes(t_env *tmp1, t_env *tmp2);
void	swap_env_nodes(t_env *tmp1, t_env *tmp2);
t_env	*create_new_env_node(char *key_value);
//exit
void	error_numerical_arg2(char *arg);
int		verify_args(char **args);
bool	is_overflowing(const char *s);
void	exit_arg(t_cmd *cmd);
int		builtin_exit(t_cmd *cmd, t_env **envs);
//env
bool	is_special_env(char *key);
int		builtin_env(t_cmd *cmd, t_env **envs);
//echo
int		handle_options(char **args, bool *n_option);
int		builtin_echo(t_cmd *cmd, t_env **envs);
//cd
void	error_invalid(char *path);
char	*home_path(t_env **envs);
char	*get_path(t_cmd *cmd, t_env **envs);
void	tilted_path(t_env **envs, char **path);
int		builtin_cd(t_cmd *cmd, t_env **envs);
//building general
void	ft_echo_env_pwd(t_cmd *cmd, t_env **env);
void	ft_cd_exit_export_unset(t_cmd *cmd, t_env **env);
int		ft_execute_built(t_cmd *cmd, t_env **env);
int		ft_builtin(t_cmd *cmd, t_env **env, int len);
int		is_builtin(t_cmd *cmd);
#endif
