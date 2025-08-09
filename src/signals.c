/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* signals.c                                          :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/08/10 10:00:00 by daflynn           #+#    #+#             */
/* Updated: 2025/08/10 10:00:00 by daflynn          ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal_status = 0;

// This function is the signal handler for SIGINT (Ctrl-C) at the main prompt.
// It sets the global status variable to 130 and properly redraws the prompt.
void	ft_sigint_handler(int sig)
{
	(void)sig;              // Avoid unused parameter warning
	g_signal_status = 130;  // 128 + SIGINT, common convention
	write(1, "\n", 1);      // Print a newline to move to the next line
	rl_replace_line("", 0); // Clear the current readline buffer
	rl_on_new_line();       // Tell readline to prepare for a new line
	rl_redisplay();         // Redraw the new prompt
}

// This function is called by `readline` periodically. It checks our global flag
// to see if a signal was received. If so, it cleans up and tells readline
// to finish the current input line.
int	ft_handle_signal_in_readline(void)
{
	if (g_signal_status == 130)
	{
		rl_done = 1;
		g_signal_status = 0; // Reset the flag
	}
	return (0);
}

/**
 * @brief Sets up signal handlers for the main shell loop.
 *
 * It configures SIGINT (Ctrl-C) to be handled by a custom function
 * that sets the global status. SIGQUIT (Ctrl-\) is ignored.
 * `rl_catch_signals(0)` disables readline's default signal handlers.
 * `rl_event_hook` is set to handle the signal properly within readline's loop.
 */
void	ft_set_main_signals(void)
{
	// Disable readline's internal signal handling.
	rl_catch_signals = 0;
	signal(SIGINT, ft_sigint_handler);
	signal(SIGQUIT, SIG_IGN);
	rl_event_hook = ft_handle_signal_in_readline;
}

/**
 * @brief Sets up signal handlers for child processes.
 *
 * It restores the default signal handlers for both SIGINT and SIGQUIT.
 * This ensures that a `Ctrl-C` or `Ctrl-\` will properly terminate the
 * child process without affecting the parent shell.
 */
void	ft_set_default_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

/**
 * @brief Resets the signal handlers to ignore signals while waiting.
 *
 * It configures both SIGINT and SIGQUIT to be ignored while the parent is

	* waiting for a child process. This is the crucial part that prevents the parent
 * from exiting when a `Ctrl-C` is sent to a running foreground process.
 */
void	ft_set_signals_waiting(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}
