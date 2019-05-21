
#if(0)
/*---------------------------------------------------------------------------*/
#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>

/*

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdarg.h>
#include <stddef.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <signal.h>
*/
/*---------------------------------------------------------------------------*/
#define SIGTIMER     (SIGRTMAX)
#define ONESHOTTIMER (SIGRTMAX-1)
/*---------------------------------------------------------------------------*/
timer_t SetTimer(int signo, int sec, int mode);
void SignalHandler(int signo, siginfo_t * info, void *context);
/*---------------------------------------------------------------------------*/
timer_t timerid,oneshotTimer;
int   count_timer;
/*---------------------------------------------------------------------------*/
int main()
{
        struct sigaction sigact;

        sigemptyset(&sigact.sa_mask);
        sigact.sa_flags = SA_SIGINFO;
        sigact.sa_sigaction = SignalHandler;

        // Set up sigaction to catch signal
        if (sigaction(SIGTIMER, &sigact, NULL) == -1) {
                perror("sigaction failed");
                return -1;
        }

        if (sigaction(ONESHOTTIMER, &sigact, NULL) == -1) {
                perror("sigaction failed");
                return -1;
        }

        // Establish a handler to catch CTRL+C and use it for exiting
        sigaction(SIGINT, &sigact, NULL);

        count_timer = 0;
        timerid = SetTimer(SIGTIMER, 100, 1);
        oneshotTimer = SetTimer(ONESHOTTIMER, 10000, 0);

        for(;;)
           ;
        return 0;
}
/*---------------------------------------------------------------------------*/
timer_t SetTimer(int signo, int sec, int mode)
{
        struct sigevent sigev;
        timer_t timerid;
        struct itimerspec itval;
        struct itimerspec oitval;

        // Create the POSIX timer to generate signo
        sigev.sigev_notify = SIGEV_SIGNAL;
        sigev.sigev_signo = signo;
        sigev.sigev_value.sival_ptr = &timerid;

        if (timer_create(CLOCK_REALTIME, &sigev, &timerid) == 0)
        {
			itval.it_value.tv_sec = sec / 1000;
			itval.it_value.tv_nsec = (long)(sec % 1000) * (1000000L);

			if (mode == 1)
			{
					itval.it_interval.tv_sec = itval.it_value.tv_sec;
					itval.it_interval.tv_nsec = itval.it_value.tv_nsec;
			}
			else
			{
					itval.it_interval.tv_sec = 0;
					itval.it_interval.tv_nsec = 0;
			}
			if (timer_settime(timerid, 0, &itval, &oitval) != 0)
			{
					perror("time_settime error!");
			}
        }
        else
        {
			perror("timer_create error!");
			return NULL;  /*-1 */
        }
        return timerid;
}
/*---------------------------------------------------------------------------*/
void SignalHandler(int signo, siginfo_t * info, void *context)
{


	if (signo == SIGTIMER)
	{
		count_timer++;
		printf("%3d: Periodic timer\n", count_timer );
	}
	if (signo == ONESHOTTIMER)
	{
		printf("%3d: One-shot timer\n", count_timer );
	}
	if (signo == SIGINT)
	{
		timer_delete(oneshotTimer);
		timer_delete(timerid);
		printf("%3d: Ctrl + C cached!\n", count_timer );
		exit(1);
	}
}
#endif

/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2010.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/

/* Listing 23-5 */

/* ptmr_sigev_signal.c

   This program demonstrates the use of signals as the notification mechanism
   for expirations of a POSIX timer. Each of the program's command-line
   arguments specifies the initial value and interval for a POSIX timer. The
   format of these arguments is defined by the function itimerspecFromStr().

   The program establishes a handler for the timer notification signal, creates
   and arms one timer for each command-line argument, and then pauses. Each
   timer expiration causes the generation of a signal, and, when invoked, the
   signal handler displays information about the timer expiration.

   Kernel support for Linux timers is provided since Linux 2.6. On older
   systems, an incomplete user-space implementation of POSIX timers
   was provided in glibc.
*/
#define _POSIX_C_SOURCE 199309
#include <signal.h>
#include <time.h>
#include "curr_time.h"                  /* Declares currTime() */
#include "itimerspec_from_str.h"        /* Declares itimerspecFromStr() */
#include "tlpi_hdr.h"

#define TIMER_SIG SIGRTMAX              /* Our timer notification signal */

static void
handler(int sig, siginfo_t *si, void *uc)
{
    timer_t *tidptr;

    tidptr = si->si_value.sival_ptr;

    /* UNSAFE: This handler uses non-async-signal-safe functions
       (printf(); see Section 21.1.2) */

    printf("[%s] Got signal %d\n", currTime("%T"), sig);
    printf("    *sival_ptr         = %ld\n", (long) *tidptr);
    printf("    timer_getoverrun() = %d\n", timer_getoverrun(*tidptr));
}

int
main(int argc, char *argv[])
{
    struct itimerspec ts;
    struct sigaction  sa;
    struct sigevent   sev;
    timer_t *tidlist;
    int j;

    if (argc < 2)
        usageErr("%s secs[/nsecs][:int-secs[/int-nsecs]]...\n", argv[0]);

    tidlist = calloc(argc - 1, sizeof(timer_t));
    if (tidlist == NULL)
        errExit("malloc");

    /* Establish handler for notification signal */

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(TIMER_SIG, &sa, NULL) == -1)
        errExit("sigaction");

    /* Create and start one timer for each command-line argument */

    sev.sigev_notify = SIGEV_SIGNAL;    /* Notify via signal */
    sev.sigev_signo = TIMER_SIG;        /* Notify using this signal */

    for (j = 0; j < argc - 1; j++) {
        itimerspecFromStr(argv[j + 1], &ts);

        sev.sigev_value.sival_ptr = &tidlist[j];
                /* Allows handler to get ID of this timer */

        if (timer_create(CLOCK_REALTIME, &sev, &tidlist[j]) == -1)
            errExit("timer_create");
        printf("Timer ID: %ld (%s)\n", (long) tidlist[j], argv[j + 1]);

        if (timer_settime(tidlist[j], 0, &ts, NULL) == -1)
            errExit("timer_settime");
    }

    for (;;)                            /* Wait for incoming timer signals */
        pause();
}



