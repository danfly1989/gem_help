/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* echo.c                                             :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/08/05 18:40:22 by daflynn           #+#    #+#             */
/* Updated: 2025/08/05 18:40:40 by daflynn          ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "minishell.h"

/*
 * This function implements the built-in 'echo' command.
 * It now accepts the t_dat struct to set the exit status.
 * 'echo' is a simple command that almost always succeeds, so the exit status is
 * set to 0.
 */
void	ft_echo(t_dat *d, size_t k)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	while (d->xln[k + i] != NULL && ft_strncmp(d->xln[k + i], "-n", 2) == 0)
	{
		newline = 0;
		i++;
	}
	while (d->xln[k + i] != NULL)
	{
		printf("%s", d->xln[k + i]);
		i++;
		if (d->xln[k + i] != NULL)
			printf(" ");
	}
	if (newline)
		printf("\n");
	d->last_exit_status = 0; // NEW: Set status to 0 on success
}

/*
 * This function implements the built-in 'env' command.
 * It now accepts the t_dat struct to set the exit status.
 * 'env' is a simple command that almost always succeeds, so the exit status is
 * set to 0.
 */
/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* env.c                                              :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/08/07 09:28:44 by daflynn           #+#    #+#             */
/* Updated: 2025/08/09 18:30:00 by daflynn          ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "minishell.h"

/*
 * This function handles the built-in 'env' command.
 * It iterates through the environment list and prints each variable.
 * Sets the exit status to 0 on success.
 */
void	ft_env(t_dat *data)
{
	t_va	*current;

	current = data->ev;
	while (current)
	{
		if (current->value)
			ft_printf("%s=%s\n", current->name, current->value);
		current = current->next;
	}
	data->last_exit_status = 0;
}

/*
 * This function handles the built-in 'export' command with no arguments.
 * It duplicates the environment list, sorts it, and then prints it.
 */
void	ft_export_no_args(t_dat *data)
{
	t_va	*sorted;

	sorted = ft_duplicate_list(data->ev);
	if (!sorted)
	{
		data->last_exit_status = 1;
		return ;
	}
	ft_sort_list_by_name(&sorted);
	ft_print_export(data, sorted); // CRITICAL FIX: Pass 'data' here.
	ft_free_list(sorted);
}

// ... other functions in env.c ...
