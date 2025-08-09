/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* signals.c                                          :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/08/10 10:00:00 by daflynn           #+#    #+#             */
/* Updated: 2025/08/10 15:30:00 by daflynn          ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "minishell.h"

// This is a global status variable, used to communicate signal status
// between the signal handler and the main program loop.
// It is set to 130 (128 + SIGINT) on Ctrl+C.
volatile sig_atomic_t	g_signal_status = 0;

// This is a very minimal signal handler. Its only job is to set a global flag
// and tell readline to exit its current loop,
// We must only call signal-safe functions here.
void	ft_sigint_handler(int sig)
{
	(void)sig;              // Avoid unused parameter warning
	g_signal_status = 130;  // 128 + SIGINT (2), a common exit status
	write(1, "\n", 1);      // Print a newline to move to a new line
	rl_replace_line("", 0); // Clear the current line buffer
	rl_redisplay();         // Redraw the prompt on the new line
}

/**
 * @brief Sets up signal handlers for the main shell loop.
 *
 * It configures SIGINT (Ctrl-C) to be handled by a custom function
 * that simply sets a global flag and tells `readline` to stop.
 * SIGQUIT (Ctrl-\) is ignored to prevent it from exiting the shell.
 */
void	ft_set_main_signals(void)
{
	// Disable readline's internal signal handling.
	rl_catch_signals = 0;
	signal(SIGINT, ft_sigint_handler);
	signal(SIGQUIT, SIG_IGN);
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

	* waiting for a child process. This prevents a `Ctrl-C` from killing the parent.
 */
void	ft_set_signals_waiting(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}