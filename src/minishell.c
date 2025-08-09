
#include "minishell.h"

// This is the global variable, defined in signals.c and declared here.
// It is used to communicate signal status to the main loop.
extern volatile sig_atomic_t	g_signal_status;

/**
 * @brief Sets up signal handlers for the main shell loop.
 *
 * It configures SIGINT (Ctrl-C) to be handled by a custom function
 * that sets the global status. SIGQUIT (Ctrl-\) is ignored.
 * `rl_catch_signals(0)` disables readline's default signal handlers.
 */

/**
 * @brief The main function of the minishell.
 *
 * It initializes data structures, enters a read-evaluate-print loop,
 * and handles user input until the program is exited.
 */
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
			ft_external_functions(&data, line);
		}
		free(line);
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

void	ft_external_functions(t_dat *data, char *line)
{
	char	***cmd;
	int		n;

	(void)line;
	if (!data || !data->xln || !data->xln[0])
		return ;
	if (!ft_validate_syntax(data->xln))
		return ;
	ft_list_to_env_array(data);
	n = ft_count_pipes(data->xln);
	if (n > 0)
	{
		cmd = ft_parse_cmd(data, 0, 0, 0);
		if (!cmd)
			return ;
		ft_execute_pipeline(data, cmd);
		ft_clean_cmd(cmd);
	}
	else
		ft_ex_single_cmd(data, NULL);
	ft_free_string_array(data->evs);
}
