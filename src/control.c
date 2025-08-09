/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* control.c                                          :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/08/08 10:15:30 by daflynn           #+#    #+#             */
/* Updated: 2025/08/09 19:40:00 by daflynn          ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "minishell.h"
#include <limits.h>

/**
 * @brief Duplicates the command-line arguments and environment variables.
 * Initializes the main data structure for the shell.
 * @param argc The argument count.
 * @param argv The argument vector.
 * @param env The environment variables.
 * @return The initialized t_dat structure.
 */
t_dat	ft_duplicate_input_args(int argc, char **argv, char **env)
{
	t_dat	data;

	(void)argc;
	data.av = NULL;
	data.ev = NULL;
	data.lo = NULL;
	data.ln = NULL;
	data.xln = NULL;
	data.tmp1 = NULL;
	data.tmp2 = NULL;
	data.i = 0;
	data.j = 0;
	data.k = 0;
	data.tot = 0;
	data.avs = NULL;
	data.evs = NULL;
	data.av = create_lst_frm_arr(argv + 1, NULL, 0, ft_create_node);
	data.ev = create_lst_frm_arr(env, NULL, 0, ft_create_var_node);
	ft_update_shlvl(&data.ev);
	return (data);
}

/**
 * @brief Frees all dynamically allocated memory within the t_dat structure.
 * This is crucial for preventing memory leaks.
 * @param data The main data structure to clean up.
 */
void	ft_cleanup_data(t_dat *data)
{
	if (data->ev != NULL)
		ft_free_list(data->ev);
	if (data->av != NULL)
		ft_free_list(data->av);
	if (data->lo != NULL)
		ft_free_list(data->lo);
	if (data->ln != NULL)
		ft_free_string_array(data->ln);
	if (data->xln != NULL)
		ft_free_string_array(data->xln);
	if (data->tmp1 != NULL)
		free(data->tmp1);
	if (data->tmp2 != NULL)
		free(data->tmp2);
}

/**
 * @brief Implements the 'exit' built-in command.
 * This version properly handles numeric arguments, too many arguments,
 * and non-numeric arguments according to shell specifications.
 * @param data Pointer to the main data structure.
 * @param k The index of the 'exit' command in the parsed command line.
 */
void	ft_exit(t_dat *data, size_t k)
{
	long long	exit_code;
	char		*arg;
	size_t		i;

	// A normal shell will print "exit" to stderr, not stdout.
	ft_putstr_fd("exit\n", 2);
	// If there are arguments
	if (data->xln[k + 1])
	{
		arg = data->xln[k + 1];
		i = 0;
		// Check for leading sign
		if (arg[i] == '-' || arg[i] == '+')
			i++;
		// Check if the rest of the argument is numeric
		while (arg[i])
		{
			if (!ft_isdigit(arg[i]))
			{
				ft_putstr_fd("minishell: exit: ", 2);
				ft_putstr_fd(arg, 2);
				ft_putstr_fd(": numeric argument required\n", 2);
				ft_cleanup_exit(data, 255);
			}
			i++;
		}
		// If there's more than one argument, print an error but don't exit
		if (data->xln[k + 2])
		{
			ft_putstr_fd("minishell: exit: too many arguments\n", 2);
			data->last_exit_status = 1;
			return ;
		}
		// Convert the argument to a long long and exit
		exit_code = ft_atoi(arg);
		ft_cleanup_exit(data, exit_code);
	}
	else
	{
		// No arguments, just exit with the last status
		ft_cleanup_exit(data, data->last_exit_status);
	}
}

/**
 * @brief Handles the execution of built-in commands.
 * It checks the command name and calls the corresponding function.
 * @param data Pointer to the main data structure.
 * @param line The original command line string.
 * @param k The index of the command in the parsed array.
 * @return 1 if the shell should exit, 0 otherwise.
 */
int	ft_handle_builtin(t_dat *data, char *line, size_t k)
{
	(void)line;
	if (data == NULL || data->xln == NULL)
		return (0);
	if (ft_strcmp(data->xln[k], "pwd") == 0)
		ft_pwd(data);
	else if (ft_strcmp(data->xln[k], "cd") == 0)
		ft_change_directory(data, k);
	else if (ft_strcmp(data->xln[k], "echo") == 0)
		ft_echo(data, k);
	else if (ft_strcmp(data->xln[k], "exit") == 0)
	{
		ft_exit(data, k);
		return (1); // Tell the main loop to exit
	}
	else if (ft_strcmp(data->xln[k], "env") == 0)
		ft_env(data);
	else if (ft_strcmp(data->xln[k], "unset") == 0)
		ft_unset_multi_var(data, k);
	else if (ft_strcmp(data->xln[k], "export") == 0)
		ft_export_multi_var(data, k);
	else
		return (0);
	return (0); // Tell the main loop to continue
}