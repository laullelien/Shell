/*****************************************************
 * Copyright Grégory Mounié 2008-2015                *
 *           Simon Nieuviarts 2002-2009              *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "variante.h"
#include "readcmd.h"
#include "execute.h"
#include "time.h"

#ifndef VARIANTE
#error "Variante non défini !!"
#endif

/* Guile (1.8 and 2.0) is auto-detected by cmake */
/* To disable Scheme interpreter (Guile support), comment the
 * following lines.  You may also have to comment related pkg-config
 * lines in CMakeLists.txt.
 */

void terminate(char *line)
{
#if USE_GNU_READLINE == 1
	/* rl_clear_history() does not exist yet in centOS 6 */
	clear_history();
#endif
	if (line)
		free(line);
	printf("exit\n");
	exit(0);
}

void parse_exec(char *line)
{
	struct cmdline *l;

	/* parsecmd free line and set it up to 0 */
	l = parsecmd(&line);

	/* If input stream closed, normal termination */
	if (!l)
	{

		terminate(0);
	}

	if (l->err)
	{
		/* Syntax error, read another command */
		printf("error: %s\n", l->err);
		return;
	}

	if (l->in)
		printf("in: %s\n", l->in);
	if (l->out)
		printf("out: %s\n", l->out);
	if (l->bg)
		printf("background (&)\n");

	/* Display each command of the pipe */
	for (int i = 0; l->seq[i] != 0; i++)
	{
		char **cmd = l->seq[i];
		printf("seq[%d]: ", i);
		for (int j = 0; cmd[j] != 0; j++)
		{
			printf("'%s' ", cmd[j]);
		}
		printf("\n");
	}

	execute(l);

	/* Remove this line when using parsecmd as it will free it */
	free(line);
}

#if USE_GUILE == 1
#include <libguile.h>

int question6_executer(char *line)
{
	parse_exec(line);
	return 0;
}

SCM executer_wrapper(SCM x)
{
	return scm_from_int(question6_executer(scm_to_locale_stringn(x, 0)));
}
#endif

int main()
{
	init_sigaction();
	printf("Variante %d: %s\n", VARIANTE, VARIANTE_STRING);

#if USE_GUILE == 1
	scm_init_guile();
	/* register "executer" function in scheme */
	scm_c_define_gsubr("executer", 1, 0, 0, executer_wrapper);
#endif

	while (1)
	{
		char *line = 0;
		char *prompt = "ensishell>";

		/* Readline use some internal memory structure that
		   can not be cleaned at the end of the program. Thus
		   one memory leak per command seems unavoidable yet */
		line = readline(prompt);
		if (line == 0 || !strncmp(line, "exit", 4))
		{
			terminate(line);
		}

#if USE_GNU_READLINE == 1
		add_history(line);
#endif

#if USE_GUILE == 1
		/* The line is a scheme command */
		if (line[0] == '(')
		{
			char catchligne[strlen(line) + 256];
			sprintf(catchligne, "(catch #t (lambda () %s) (lambda (key . parameters) (display \"mauvaise expression/bug en scheme\n\")))", line);
			scm_eval_string(scm_from_locale_string(catchligne));
			free(line);
			continue;
		}
#endif

		parse_exec(line);
	}
}
