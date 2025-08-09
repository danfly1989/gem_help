/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* expansion.c                                        :+:      ::::::::   */
/* +:+ +:+         +:+     */
/* By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/08/05 18:07:46 by daflynn           #+#    #+#             */
/* Updated: 2025/08/09 17:30:00 by daflynn          ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "minishell.h"

// NEW: A dedicated function to handle the expansion of the exit status variable '$?'
static void	ft_expand_exit_status(t_dat *data, char **res)
{
	char	*val;
	char	*tmp;

	// Get the string representation of the last exit status.
	// This function will allocate memory for the string.
	val = ft_itoa(data->last_exit_status);
	if (!val)
		return ; // Handle allocation failure
	// Join the exit status to the result string.
	tmp = *res;
	*res = ft_strjoin(*res, val);
	free(tmp);
	free(val); // Free the memory allocated by ft_itoa
}

// Handles the expansion of a regular variable (not '$?')
void	ft_expand_loop(char *token, t_dat *data, char **res, size_t *i)
{
	char	*key;
	char	*val;
	char	*tmp;

	// Extract the variable name (key) from the token.
	// The `i` pointer is updated by this function.
	key = ft_extract_var_key(token, i);
	if (!key)
		return ;
	// Search for the variable in local and then environment lists.
	val = ft_get_var_value(data->lo, key);
	if (val == NULL)
		val = ft_get_var_value(data->ev, key);
	// If a value was found, append it to the result.
	if (val != NULL)
	{
		tmp = *res;
		*res = ft_strjoin(*res, val);
		free(tmp);
	}
	// Clean up memory. The key was allocated by ft_extract_var_key.
	free(key);
}

/*The workhorse function that expands through a single token
character by character.
It checks for '$' and if followed by a valid variable name starter.
It respects single quotes by not expanding variables within them.
*/
char	*ft_expand_token(char *token, t_dat *data, int qt, size_t i)
{
	char	*res;
	char	*tmp;

	if (qt == 1)
		return (ft_strdup(token));
	res = ft_calloc(1, sizeof(char));
	while (token[i])
	{
		if (token[i] == '$' && token[i + 1])
		{
			i++; // Move past the '$'
			// CRITICAL FIX: Handle the special '?' variable separately
			if (token[i] == '?')
			{
				ft_expand_exit_status(data, &res);
				i++; // Move past the '?'
			}
			// Handle regular variables
			else if (ft_isalpha(token[i]) || token[i] == '_')
			{
				ft_expand_loop(token, data, &res, &i);
			}
			else // It's a literal dollar sign, e.g., '$5' or '$$'
			{
				tmp = res;
				res = ft_strjoin_char(res, '$');
				free(tmp);
			}
		}
		else
		{
			tmp = res;
			res = ft_strjoin_char(res, token[i++]);
			free(tmp);
		}
	}
	return (res);
}

char	**ft_expand_tokens(t_dat *d, char **tokens, int *qtypes, int i)
{
	char	**expanded;

	while (tokens[i])
		i++;
	expanded = malloc(sizeof(char *) * (i + 1));
	if (!expanded)
		return (NULL);
	i = 0;
	while (tokens[i])
	{
		expanded[i] = ft_expand_token(tokens[i], d, qtypes[i], 0);
		if (!expanded[i])
		{
			while (--i >= 0)
				free(expanded[i]);
			free(expanded);
			return (NULL);
		}
		i++;
	}
	expanded[i] = NULL;
	return (expanded);
}

/*
** The heart of the shell's expansion logic:
** 1. Tokenizes the input line into tokens with quote type tracking
** 2. Expands variables in tokens (respecting quote rules)
** 3. Strips quote characters from final tokens
** 4. Determines if line contains variable assignments or commands
** 5. Routes to appropriate handler (variable assignment vs command execution)
**
** Sets data->ln (line tokens) and data->xln (expanded line tokens)
*/
void	ft_check_var_assign_and_expand_line(t_dat *data, char *line)
{
	int *quote_types;

	if (!data || !line)
		return ;
	data->ln = ft_tokenize_line(data, line, &quote_types);
	if (!data->ln)
		return ;
	data->xln = ft_expand_tokens(data, data->ln, quote_types, 0);
	if (!data->xln)
	{
		free(quote_types);
		return ;
	}
	ft_strip_quotes_from_xln(data);
	if (ft_all_valid_lvar(data, data->xln))
		ft_update_local_variables(data);
	else if (data->k)
		ft_handle_builtin(data, line, data->k);
	else
		ft_handle_builtin(data, line, 0);
	free(quote_types);
}