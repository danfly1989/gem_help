/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* clear.c                                            :+:      ::::::::   */
/* +:+ +:+         +:+     */
/* By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/08/07 09:20:32 by daflynn           #+#    #+#             */
/* Updated: 2025/08/07 09:20:41 by daflynn          ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "minishell.h"

/*
 * Frees a 3D array of strings used for parsed commands.
 */
char	***ft_clean_cmd(char ***cmd)
{
	int	i;

	if (!cmd)
		return (NULL);
	i = 0;
	while (cmd[i])
	{
		ft_free_string_array(cmd[i]);
		i++;
	}
	free(cmd);
	return (NULL);
}

/*
 * Frees a 2D array of file descriptors.
 */
void	ft_free_fd(int **fd)
{
	int	i;

	if (!fd)
		return ;
	i = 0;
	while (fd[i])
	{
		free(fd[i]);
		i++;
	}
	free(fd);
}

/*
 * Performs a full cleanup of the shell's data and exits.
 */
void	ft_cleanup_exit(t_dat *data, int flag)
{
	ft_cleanup_data(data);
	rl_clear_history();
	exit(flag);
}

/*
 * Prints an error message for an invalid identifier for the 'unset' built-in.
 */
static void	ft_unset_error(char *arg)
{
	char	*prefix;
	char	*suffix;

	prefix = "minishell: unset: `";
	suffix = "': not a valid identifier\n";
	write(2, prefix, ft_strlen(prefix));
	write(2, arg, ft_strlen(arg));
	write(2, suffix, ft_strlen(suffix));
}

/*
 * Implements the 'unset' built-in command.
 * It now sets the last_exit_status to 0 on success or 1 if any argument
 * is an invalid identifier.
 */
void	ft_unset_multi_var(t_dat *d, size_t k)
{
	int i;
	int status;

	status = 0;
	i = 1;
	while (d->xln[k + i] != NULL)
	{
		// This assumes ft_valid_var exists and works like in export.c
		if (ft_valid_var(d->xln[k + i]) == 1)
		{
			d->ev = ft_remove_variable_node(d->xln[k + i], d->ev, NULL);
			d->lo = ft_remove_variable_node(d->xln[k + i], d->lo, NULL);
		}
		else
		{
			ft_unset_error(d->xln[k + i]);
			status = 1; // Set status to 1 but continue processing other args
		}
		i++;
	}
	d->last_exit_status = status;
}