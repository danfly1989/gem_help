/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* minishell.c                                        :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/08/05 19:41:24 by daflynn           #+#    #+#             */
/* Updated: 2025/08/10 12:45:00 by daflynn          ###   ########.ft       */
/* */
/* ************************************************************************** */

#include "minishell.h"
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

// This is the global variable, defined in signals.c and declared here.
// It is used to communicate signal status to the main loop.
extern volatile sig_atomic_t	g_signal_status;

// Function prototypes to avoid implicit declaration errors.
char							*ft_strjoin_and_free(char *s1, char *s2);
char							*ft_join_path(char *str1, char *cmd);
void							ft_free_string_array(char **array);
void							ft_cmd_not_found(char *cmd);
int								ft_count_pipes(char **tokens);

/**
 * @brief Frees memory allocated for a single loop iteration.
 * This is crucial to prevent memory leaks as the shell runs.
 * @param data The main data structure.
 */
void	ft_cleanup_loop(t_dat *data)
{
	// Free the token array from the previous command
	if (data->ln)
		ft_free_string_array(data->ln);
	if (data->xln)
		ft_free_string_array(data->xln);
	// Reset pointers to prevent double-free errors
	data->ln = NULL;
	data->xln = NULL;
}

/**
 * @brief Finds an environment variable within a linked list.
 * @param ev_list The head of the environment variable linked list.
 * @param var The name of the variable to find.
 * @return The value of the environment variable, or NULL if not found.
 */
static char	*ft_getenv_from_list(t_va *ev_list, const char *var)
{
	while (ev_list)
	{
		if (ft_strcmp(ev_list->name, var) == 0)
			return (ev_list->value);
		ev_list = ev_list->next;
	}
	return (NULL);
}

/**
 * @brief Converts the environment variable linked list into a
 * standard `char **` array, which is required by `execve`.
 *
 * @param ev_list The head of the environment variable linked list.
 * @return A new `char **` array, or NULL on error.
 */
static char	**ft_convert_list_to_array(t_va *ev_list)
{
	int		i;
	int		count;
	char	**env_array;
	t_va	*current;
	char	*temp_str;

	count = 0;
	current = ev_list;
	while (current)
	{
		count++;
		current = current->next;
	}
	env_array = malloc(sizeof(char *) * (count + 1));
	if (!env_array)
		return (NULL);
	i = 0;
	current = ev_list;
	while (current)
	{
		temp_str = ft_strjoin(current->name, "=");
		env_array[i] = ft_strjoin_and_free(temp_str, current->value);
		if (!env_array[i])
		{
			ft_free_string_array(env_array);
			return (NULL);
		}
		i++;
		current = current->next;
	}
	env_array[i] = NULL;
	return (env_array);
}

/**

	* @brief Finds the full path of a command by checking the PATH environment variable.
 * @param ev_list The head of the environment variable linked list.
 * @param cmd The command to find.
 * @return The full path to the command or NULL if not found.
 */
char	*ft_find_command_path(t_va *ev_list, char *cmd)
{
	char	*path_env;
	char	**paths;
	char	*full_path;
	int		i;

	path_env = ft_getenv_from_list(ev_list, "PATH");
	if (!path_env)
	{
		printf("DEBUG: PATH environment variable not found.\n");
		return (NULL);
	}
	printf("DEBUG: PATH environment variable: %s\n", path_env);
	if (access(cmd, F_OK) == 0)
		return (ft_strdup(cmd));
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		full_path = ft_join_path(paths[i], cmd);
		if (access(full_path, F_OK) == 0)
		{
			ft_free_string_array(paths);
			return (full_path);
		}
		free(full_path);
		i++;
	}
	ft_free_string_array(paths);
	return (NULL);
}

/**
 * @brief Forks a new process to execute an external command.
 * The parent process waits for the child to finish.
 * @param data The main data structure.
 * @param xln The tokenized command and its arguments.
 */
void	ft_exec_cmd_with_fork(t_dat *data, char **xln)
{
	pid_t	pid;
	int		status;
	char	*path_to_cmd;
	char	**env_array;

	printf("DEBUG: Now in ft_exec_cmd_with_fork.\n");
	path_to_cmd = ft_find_command_path(data->ev, xln[0]);
	if (!path_to_cmd)
	{
		ft_cmd_not_found(xln[0]);
		data->last_exit_status = 127;
		return ;
	}
	printf("DEBUG: Command path found: %s\n", path_to_cmd);
	env_array = ft_convert_list_to_array(data->ev);
	if (!env_array)
	{
		free(path_to_cmd);
		data->last_exit_status = 1;
		return ;
	}
	pid = fork();
	if (pid < 0)
	{
		perror("fork failed");
		ft_free_string_array(env_array);
		free(path_to_cmd);
		data->last_exit_status = 1;
	}
	else if (pid == 0)
	{
		printf("DEBUG: In child process, attempting execve with path: %s\n",
			path_to_cmd);
		if (execve(path_to_cmd, xln, env_array) == -1)
		{
			perror("execve failed");
			exit(127);
		}
	}
	else
	{
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			data->last_exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			data->last_exit_status = 128 + WTERMSIG(status);
	}
	ft_free_string_array(env_array);
	free(path_to_cmd);
}
int	main(int argc, char **argv, char **env)
{
	t_dat	data;
	char	*line;

	data = ft_duplicate_input_args(argc, argv, env);
	ft_update_shlvl(&data.ev);
	while (1)
	{
		ft_set_main_signals();
		line = readline("dandav>");
		// This is the new, crucial check.
		if (g_signal_status == 130)
		{
			// Print a single newline and reset the flag.
			write(1, "\n", 1);
			g_signal_status = 0;
			continue ; // Skip the rest of the loop and start over
		}
		if (!line)
		{
			printf("exit\n");
			break ;
		}
		if (ft_strlen(line) > 0)
		{
			add_history(line);
			ft_check_var_assign_and_expand_line(&data, line);
			if (data.xln && data.xln[0])
			{
				ft_exec_cmd_with_fork(&data, data.xln);
			}
		}
		free(line);
		ft_cleanup_loop(&data);
	}
	ft_cleanup_exit(&data, 0);
	return (data.last_exit_status);
}

char	*ft_strjoin_and_free(char *s1, char *s2)
{
	char	*new_str;

	new_str = ft_strjoin(s1, s2);
	free(s1);
	return (new_str);
}

char	*ft_join_path(char *str1, char *cmd)
{
	char	*temp;
	char	*full_path;

	temp = ft_strjoin(str1, "/");
	full_path = ft_strjoin(temp, cmd);
	free(temp);
	temp = NULL;
	return (full_path);
}

int	ft_count_pipes(char **tokens)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (tokens[i])
	{
		if (ft_strcmp(tokens[i], "|") == 0)
			count++;
		i++;
	}
	return (count);
}

void	ft_cmd_not_found(char *cmd)
{
	char *prefix;
	char *suffix;

	prefix = "minishell: ";
	suffix = ": command not found\n";
	write(2, prefix, ft_strlen(prefix));
	write(2, cmd, ft_strlen(cmd));
	write(2, suffix, ft_strlen(suffix));
	exit(127);
}