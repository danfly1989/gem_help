/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* piping.c                                           :+:      ::::::::   */
/* +:+ +:+         +:+     */
/* By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/* and it gets passed */
/* Updated: 2025/08/09 19:19:00 by daflynn          ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "minishell.h"

int	ft_check_pipe(char **tokens, int i)
{
	if (i == 0 || !tokens[i + 1] || ft_is_redirection(tokens[i + 1]))
		return (ft_syntax_error(tokens[i + 1]));
	return (1);
}

void	ft_execute_pipeline(t_dat *d, char ***cmd)
{
	int	**fd;

	fd = init_fd_array(d->tot);
	if (!fd || !ft_create_pipes(fd, d->tot))
	{
		if (fd)
			ft_free_fd(fd);
		d->last_exit_status = 1; // Set status on pipe failure
		return ;
	}
	ft_fork_children(d, cmd, fd); // This calls the function from children.c
	ft_close_pipes(fd, d->tot);
	// Call the updated ft_wait_children and store the result
	d->last_exit_status = ft_wait_children(d);
		// This calls the function from children.c
	ft_free_fd(fd);
}

int	ft_create_pipes(int **fd, int tot)
{
	int	i;

	i = 0;
	while (i < tot - 1)
	{
		if (pipe(fd[i]) == -1)
		{
			perror("pipe");
			while (i-- > 0)
				free(fd[i]);
			free(fd);
			return (0);
		}
		i++;
	}
	return (1);
}

void	ft_close_pipes(int **fd, int tot)
{
	int	i;

	i = 0;
	while (i < tot - 1)
	{
		close(fd[i][0]);
		close(fd[i][1]);
		i++;
	}
}
