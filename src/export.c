/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* export.c                                           :+:      ::::::::   */
/* +:+ +:+         +:+     */
/* By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/08/05 18:26:58 by daflynn           #+#    #+#             */
/* Updated: 2025/08/05 18:27:09 by daflynn          ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "minishell.h"

void	ft_export_type2(t_dat *data, char *str)
{
	char	*name;
	char	*val;
	t_va	*cur;

	val = NULL;
	name = str;
	cur = data->lo;
	while (cur)
	{
		if (ft_strncmp(cur->name, name, ft_strlen(name)) == 0)
		{
			val = cur->value;
			break ;
		}
		cur = cur->next;
	}
	if (val)
	{
		ft_append_env_var(data, name, val);
		data->last_exit_status = 0; // NEW: Set status to 0 on success
	}
	else
		data->last_exit_status = 1;
	// NEW: Set status to 1 on failure to find value
}

t_va	*ft_export_type1_ext(char *name, char *val)
{
	t_va	*new;

	new = malloc(sizeof(t_va));
	if (!new)
	{
		// Handle malloc failure if needed, though this is a helper.
		return (NULL);
	}
	new->name = name;
	new->value = ft_strdup(val);
	new->next = NULL;
	return (new);
}

void	ft_export_type1(t_dat *data, t_va **head, char *s, char *name,
		char *val)
{
	t_va	*cur;
	t_va	*new;
	t_va	*prev;

	name = ft_extract_var_name(s);
	val = ft_strchr(s, '=') + 1;
	cur = *head;
	while (cur)
	{
		if (ft_strcmp(cur->name, name) == 0)
		{
			free(cur->value);
			cur->value = ft_strdup(val);
			free(name);
			data->last_exit_status = 0; // NEW: Set status to 0 on success
			return ;
		}
		prev = cur;
		cur = cur->next;
	}
	new = ft_export_type1_ext(name, val);
	if (!new)
	{
		data->last_exit_status = 1;
		// NEW: Set status to 1 on memory allocation failure
		return ;
	}
	if (prev)
		prev->next = new;
	else
		*head = new;
	data->last_exit_status = 0; // NEW: Set status to 0 on success
}

void	ft_print_export(t_dat *data, t_va *head)
{
	t_va	*cur;

	cur = head;
	while (cur != NULL)
	{
		printf("declare -x %s=\"%s\"\n", cur->name, cur->value);
		cur = cur->next;
	}
	data->last_exit_status = 0; // NEW: Set status to 0 on success
}

void	ft_export_multi_var(t_dat *data, size_t k)
{
	char	*message;
	int		i;

	message = "not a valid identifier";
	if (data->xln[k + 1] == NULL)
	{
		ft_print_export(data, data->ev);
		// Changed to call the new ft_print_export
		return ;
	}
	i = 1;
	while (data->xln[k + i] != NULL)
	{
		if (ft_valid_var(data->xln[k + i]) == 1)
		{
			ft_export_type1(data, &data->ev, data->xln[k + i], NULL, NULL);
			// NEW: Pass data
			ft_add_local_var(data, data->xln[k + i]);
		}
		else if (ft_var_name_only(data->xln[k + i]) == 1)
		{
			ft_export_type2(data, data->xln[k + i]); // NEW: Pass data
		}
		else
		{
			ft_export_error(data, data->xln[k + i], message); // NEW: Pass data
		}
		i++;
	}
}
