/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* directories.c                                      :+:      ::::::::   */
/* +:+ +:+         +:+     */
/* By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/08/05 18:24:37 by daflynn           #+#    #+#             */
/* Updated: 2025/08/05 18:24:51 by daflynn          ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "minishell.h"

/*
 * This function implements the built-in 'pwd' command.
 * It now accepts the shell's data structure to set the exit status.
 * It prints the current working directory or an error message if getcwd fails.
 */
void	ft_pwd(t_dat *data)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (cwd == NULL)
	{
		perror("pwd error");
		data->last_exit_status = 1; // Set status to 1 on failure
		return ;
	}
	printf("%s\n", cwd);
	free(cwd);
	data->last_exit_status = 0; // Set status to 0 on success
}

/*
 * @brief Updates the PWD and OLDPWD environment variables.
 *
 * It takes the old PWD and gets the new PWD to update the environment list.
 * This is called after a successful chdir().
 * @param data Pointer to the main data structure.
 * @param oldpwd The old working directory string (must be freed).
 */
void	ft_update_directories(t_dat *data, char *oldpwd)
{
	char	*newpwd;

	newpwd = getcwd(NULL, 0);
	if (newpwd == NULL)
	{
		perror("getcwd error in ft_update_directories");
		free(oldpwd);
		return ;
	}
	ft_update_env_variable(data, "OLDPWD", oldpwd);
	ft_update_env_variable(data, "PWD", newpwd);
	free(oldpwd);
	free(newpwd);
}

/**
 * @brief Prints a standardized error message for the 'cd' command.
 *
 * @param data Pointer to the main data structure to set the exit status.
 * @param path The path that caused the error.
 */
void	ft_cd_error(t_dat *data, char *path)
{
	ft_printf("minishell: cd: %s: %s\n", path, strerror(errno));
	data->last_exit_status = 1;
}

/**
 * @brief Implements the built-in 'cd' command.
 *
 * Changes the current directory based on the provided path or defaults
 * to the HOME environment variable.
 * @param data Pointer to the main data structure.
 * @param k The index of the command in the token list.
 */
void	ft_change_directory(t_dat *data, size_t k)
{
	char *path;
	char *oldpwd;
	int free_path;

	oldpwd = getcwd(NULL, 0);
	if (oldpwd == NULL)
	{
		perror("getcwd error in ft_change_directory");
		data->last_exit_status = 1;
		return ;
	}
	free_path = 0;
	if (data->xln[k + 1] == NULL || ft_strcmp(data->xln[k + 1], "~") == 0)
	{
		path = ft_get_val_from_list(data->ev, "HOME");
		if (path == NULL)
		{
			ft_printf("minishell: cd: HOME not set\n");
			data->last_exit_status = 1;
			free(oldpwd); // CRITICAL FIX: Free oldpwd before returning
			return ;
		}
		path = ft_strdup(path);
		// NEW: Duplicate the string from the list to be safe
		free_path = 1;
	}
	else
		path = data->xln[k + 1];

	if (chdir(path) == 0)
	{
		ft_update_directories(data, oldpwd);
		data->last_exit_status = 0;
	}
	else
	{
		ft_cd_error(data, path);
		free(oldpwd); // CRITICAL FIX: Free oldpwd on error path
	}

	if (free_path) // NEW: Free the duplicated HOME path if we allocated it
		free(path);
}