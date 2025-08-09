/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* children.c                                         :+:      ::::::::   */
/* +:+ +:+         +:+     */
/* By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/* and it gets passed */
/* Updated: 2025/08/09 18:00:00 by daflynn          ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Logic for a single child process.
 * This function handles I/O redirection, closes unused pipes,
 * and executes the command for the given segment.
 * @param d Pointer to the main data structure.
 * @param cmd The command and its arguments.
 * @param fd The array of pipe file descriptors.
 * @param i The index of the current command segment.
 */
void	ft_child_process(t_dat *d, char ***cmd, int **fd, size_t i)
{
	t_rdr	r;
	int		len;

	len = 0;
	while (cmd[i][len])
		len++;
	ft_memset(&r, 0, sizeof(r));
	if (!ft_validate_segment(cmd[i], 0, len))
		exit(1);
	ft_setup_io(fd, i, d->tot);
	ft_close_pipes(fd, d->tot);
	ft_parse_redirection(cmd[i], &r);
	if (!ft_apply_redirections(&r, &cmd[i]))
		exit(1);
	ft_exec_command(d, cmd[i]);
}

/**
 * @brief Forks child processes and executes commands.
 * It allocates an array to store the PIDs of all children.
 * Each child process is set up with a default signal handler.
 * @param d Pointer to the main data structure.
 * @param cmd The array of command segments.
 * @param fd The array of pipe file descriptors.
 */
void	ft_fork_children(t_dat *d, char ***cmd, int **fd)
{
	pid_t	pid;
	size_t	i;

	// Allocate pids array based on the total number of commands.
	d->pids = malloc(sizeof(pid_t) * d->tot);
	if (!d->pids)
	{
		perror("malloc");
		ft_cleanup_data(d);
		// CRITICAL FIX: Changed `ft_clear_data` to `ft_cleanup_data`
		exit(1);
	}
	i = 0;
	while (i < d->tot)
	{
		pid = fork();
		if (pid == 0) // Child process
		{
			ft_set_default_signals();
			ft_child_process(d, cmd, fd, i);
		}
		else if (pid > 0) // Parent process
		{
			d->pids[i] = pid; // Store the child's PID
		}
		else // fork() failed
		{
			perror("fork");
			// Here you could add more robust error handling
		}
		i++;
	}
}

/**

	* @brief Waits for all child processes to finish and returns the exit status of the last one.
 * This function correctly uses waitpid for each child and handles normal exits
 * as well as signals. It then frees the pids array.
 * @param d Pointer to the main data structure containing pids and tot.
 * @return The exit status of the last command in the pipeline.
 */
int	ft_wait_children(t_dat *d)
{
	int		status;
	int		last_status;
	size_t	i;

	(void)d; // Fix for unused parameter 'd'
	i = 0;
	last_status = 0; // Initialize last_status to a default value
	while (i < d->tot)
	{
		// Wait for the specific child process stored in the pids array
		waitpid(d->pids[i], &status, 0);
		if (WIFEXITED(status))
			last_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			last_status = 128 + WTERMSIG(status);
		i++;
	}
	// CRITICAL FIX: Free the pids array *after* all waitpid calls are complete.
	free(d->pids);
	d->pids = NULL;
	return (last_status);
}
