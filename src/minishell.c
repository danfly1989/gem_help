/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* minishell.c                                        :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/08/05 19:41:24 by daflynn           #+#    #+#             */
/* Updated: 2025/08/09 17:30:00 by daflynn          ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "minishell.h"

// This is the global variable, defined in signals.c and declared here.
// It is used to communicate signal status to the main loop.
extern volatile sig_atomic_t	g_signal_status;

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

int	main(int argc, char **argv, char **env)
{
	t_dat	data;
	char	*line;

	data = ft_duplicate_input_args(argc, argv, env);
	ft_update_shlvl(&(data.ev));
	// Set up the event hook to handle signals within readline.
	rl_event_hook = ft_handle_signal_in_readline;
	while (1)
	{
		ft_set_main_signals();
		line = readline("dandav>");
		if (!line) // EOF (Ctrl-D)
		{
			printf("exit\n");
			break ;
		}
		if (ft_strlen(line) > 0)
		{
			add_history(line);
			// This is the correct, existing function from your expansion.c
			// It will tokenize, expand, and then route the command for you.
			ft_check_var_assign_and_expand_line(&data, line);
		}
		free(line);
		// Always clean up after each loop to avoid memory leaks.
		ft_cleanup_loop(&data);
	}
	ft_cleanup_exit(&data, 0);
	return (data.last_exit_status);
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
	char	*prefix;
	char	*suffix;

	prefix = "minishell: ";
	suffix = ": command not found\n";
	write(2, prefix, ft_strlen(prefix));
	write(2, cmd, ft_strlen(cmd));
	write(2, suffix, ft_strlen(suffix));
	exit(127);
}
