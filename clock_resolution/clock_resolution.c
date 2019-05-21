/*
 *
 *
 * http://stackoverflow.com/questions/922492/use-of-clock-getres-newbie-linux-c
 * http://www.qnx.com/developers/docs/6.3.2/neutrino/lib_ref/c/clock_getres.html
 */



/* clocks.c: find clock resolution */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
/*---------------------------------------------------------------------------*/
void * Periodic_Thread_Task( void * data )
{
  int counter;
/*  int interval = *( (int *) data );*/
  counter = 0;
  printf( "hello %d\n", counter );
  counter++;
  if ( counter > 2 )
  {
  	pthread_exit( NULL );
  }
  usleep(1);
/*  for (;;)
  {
    usleep(10000);
  }*/
  return NULL;
}
/*---------------------------------------------------------------------------*/
void Periodic_Test( void )
{
	pthread_t thread;
	int interval = 5000000;

	pthread_create( &thread, NULL, Periodic_Thread_Task, &interval );

	pthread_join( thread, NULL ); /* wait for thread to complete */
}

/*---------------------------------------------------------------------------*/
void Another_Test( void )
{
	  clockid_t types[] = { CLOCK_REALTIME, CLOCK_MONOTONIC, CLOCK_PROCESS_CPUTIME_ID, CLOCK_THREAD_CPUTIME_ID, (clockid_t) - 1 };

	  struct timespec spec;
	  int i;
	  for ( i = 0; types[i] != (clockid_t) - 1; i++ )
	  {
	    if ( clock_getres( types[i], &spec ) != 0 )
	    {
	      printf( "Timer %d not supported.\n", types[i] );
	    }
	    else
	    {
	      printf( "Timer: %d, Seconds: %ld Nanos: %ld\n", i, spec.tv_sec, spec.tv_nsec );
	    }
	  }
}
/*---------------------------------------------------------------------------*/
int main ( void )
{
    struct timespec res;


    if ( clock_getres (CLOCK_REALTIME, &res) == -1 )
	{
    	perror( "clock_getres: CLOCK_REALTIME" );
	}
    else
    {
    	printf ( "CLOCK_REALTIME: %ld s, %ld ns\n", res.tv_sec, res.tv_nsec );
    }

    if ( clock_getres (CLOCK_MONOTONIC, &res) == -1 ) perror ( "clock_getres: CLOCK_MONOTONIC" );
    else        printf ("CLOCK_MONOTONIC: %ld s, %ld ns\n", res.tv_sec, res.tv_nsec);

    if (clock_getres (CLOCK_MONOTONIC_RAW, &res) == -1)
        perror ("clock_getres: CLOCK_MONOTONIC_RAW");
    else
        printf ( "CLOCK_MONOTONIC_RAW: %ld s, %ld ns\n", res.tv_sec, res.tv_nsec );

    if (clock_getres (CLOCK_PROCESS_CPUTIME_ID, &res) == -1)
        perror ("clock_getres: CLOCK_PROCESS_CPUTIME_ID");
    else
        printf ( "CLOCK_PROCESS_CPUTIME_ID: %ld s, %ld ns\n", res.tv_sec, res.tv_nsec );

    if (clock_getres (CLOCK_THREAD_CPUTIME_ID, &res) == -1)
        perror ("clock_getres: CLOCK_THREAD_CPUTIME_ID");
    else
        printf ( "CLOCK_THREAD_CPUTIME_ID: %ld s, %ld ns\n", res.tv_sec, res.tv_nsec );


    Another_Test();

    Periodic_Test();


    return 0;
}
