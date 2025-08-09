/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* env.c                                              :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/08/05 18:31:35 by daflynn           #+#    #+#             */
/* Updated: 2025/08/05 18:31:57 by daflynn          ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "minishell.h"

void	ft_create_env_variable(t_dat *d, const char *name, const char *value)
{
	t_va	*new_node;

	new_node = malloc(sizeof(t_va));
	if (!new_node)
		return (perror("minishell: malloc error"));
	new_node->name = ft_strdup(name);
	new_node->value = ft_strdup(value);
	new_node->next = d->ev;
	d->ev = new_node;
	if (!new_node->name || !new_node->value)
	{
		free(new_node->name);
		free(new_node->value);
		free(new_node);
		perror("minishell: malloc error");
	}
}

void	ft_update_env_variable(t_dat *d, const char *name, const char *value)
{
	if (!ft_update_existing_var(d->ev, name, value))
		ft_create_env_variable(d, name, value);
}

void	ft_append_env_var(t_dat *data, char *key, char *value)
{
	t_va	*new;
	t_va	*cur;

	new = malloc(sizeof(t_va));
	if (!new)
		return ;
	new->name = ft_strdup(key);
	new->value = ft_strdup(value);
	new->next = NULL;
	cur = data->ev;
	if (!cur)
	{
		data->ev = new;
		return ;
	}
	while (cur->next)
		cur = cur->next;
	cur->next = new;
}

// CRITICAL FIX: This function now accepts 't_dat *data'.
void	ft_print_sorted_env(t_dat *data, t_va *head)
{
	t_va	*sorted;

	sorted = ft_duplicate_list(head);
	ft_sort_list_by_name(&sorted);
	ft_print_export(data, sorted); // CRITICAL FIX: Now passing both arguments.
	ft_free_list(sorted);
}

void	ft_list_to_env_array(t_dat *data)
{
	int i;
	int count;
	t_va *cur;

	i = 0;
	data->tmp1 = NULL;
	count = ft_count_list(data->ev);
	data->evs = malloc((count + 1) * sizeof(char *));
	if (!data->evs)
		return ;
	cur = data->ev;
	while (cur && i < count)
	{
		data->tmp1 = ft_strjoin(cur->name, "=");
		data->evs[i] = ft_strjoin(data->tmp1, cur->value);
		free(data->tmp1);
		data->tmp1 = NULL;
		cur = cur->next;
		i++;
	}
	data->evs[i] = NULL;
}