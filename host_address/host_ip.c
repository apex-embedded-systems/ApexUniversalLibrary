#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>

/* paddr: print the IP address in a standard decimal dotted format */
void paddr(unsigned char *a)
{
        printf("  %d.%d.%d.%d\n", a[0], a[1], a[2], a[3]);
}

int main(int argc, char **argv)
{
        struct hostent * 	hp[2];
        char * 				host[2] = { "google.com", "apexembedded.com" };
        int i;

        /* get IP addresses for host #0 */
        hp[0] = gethostbyname(host[0]);
        if (!hp[0]) {
                fprintf(stderr, "could not obtain address of %s\n", host[0]);
                return 0;
        }

        /* print out IP addresses for host #0 */
        printf( "\n%s:\n", host[0] );
        for (i=0; hp[0]->h_addr_list[i] != 0; i++)
        {
        	paddr((unsigned char*) hp[0]->h_addr_list[i]);
        }


        /* get IP addresses for host #1 */
        hp[1] = gethostbyname(host[1]);
        if (!hp[1]) {
                fprintf(stderr, "could not obtain address of %s\n", host[1]);
                return 0;
        }

/* cannot have the printout for first host here!, otherwise the IP address reported are identical, this must be because
 * gethostbyname must be re-using internal space to report information.  In other words, it is not reentrant!
 */


        /* print out IP addresses for host #1 */
        printf( "\n%s:\n", host[1] );
        for (i=0; hp[1]->h_addr_list[i] != 0; i++)
        {
        	paddr((unsigned char*) hp[1]->h_addr_list[i]);
        }


        exit(0);
}
