#ifndef ELEVATE
#define ELEVATE

#include <unistd.h>
#include "src/have.c"

/* determine which privilege elevator program, sudo or doas, is available. checks for sudo first, then doas */
const char* determine_elevator() {
	const char *elevator = NULL;
	if (command_exists("sudo")) { elevator = "sudo"; }
	if (command_exists("doas")) { elevator = "doas"; }
    return elevator;
}

/* examine the input command to see if where to insert the elevator command.
 this is done because maybe the elevator command has to be inserted more than once,
 because the plain command actually runs multiple commands, separated by shell
 operators such as `&&`, `||`, `;`, possibly `|`, `&`, etc. */
const char *examine_command_and_insert_elevator(const char *command) {
	#error unimplemented
}

/* elevate a commmand by inserting sudo or doas where needed */
const char *elevate_command(const char *command) {
	// first: checks
	#error unimplemented checks here

	const char *elevated_command;
	return elevated_command;
}

#endif /* ELEVATE */
