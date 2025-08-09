/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 09:39:22 by daflynn           #+#    #+#             */
/*   Updated: 2025/08/07 09:39:28 by daflynn          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Corrected `ft_export_error` to match the prototype in minishell.h
void	ft_export_error(t_dat *data, char *arg, char *message)
{
	// The `data` parameter is now included in the function signature.
	// Since it's not used in this specific function, we can cast it to void
	// to prevent an "unused parameter" warning.
	(void)data;
	ft_printf("minishell: export: `%s': %s\n", arg, message);
}
