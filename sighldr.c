/* opensn0w
 * An open-source jailbreaking utility.
 * Brought to you by rms, acfrazier & Maximus
 * Special thanks to iH8sn0w & MuscleNerd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef _WIN32

#include "sn0w.h"
#include <execinfo.h>
#include <signal.h>
#include <ucontext.h>
#include <unistd.h>

void critical_error_handler(int sig_num, siginfo_t * info, void *ucontext)
{
	void *array[50];
	char **messages;
	int size, i;

	/* Get the address at the time the signal was raised from the EIP (x86) */

	/* fixme: need more platforms */
	printf("%s.\nCallee address: %p\n",
		strsignal(sig_num), info->si_addr);

	size = backtrace(array, 50);

	/* overwrite sigaction with caller's address */
	array[1] = info->si_addr;

	messages = backtrace_symbols(array, size);

	/* skip first stack frame (points here) */
	for (i = 1; i < size && messages != NULL; ++i) {
		printf("Symbol(%d): %s\n", i, messages[i]);
	}

	free(messages);
	printf("\n\nPlease file a bug report with the information above.\n(Please file with full verbose log.)\n"
               "Report opensn0w bugs to rms@velocitylimitless.org.\n");

	exit(EXIT_FAILURE);
}

#endif
