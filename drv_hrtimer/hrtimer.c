#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

MODULE_LICENSE("GPL");

#define MS_TO_NS(x)	(x * 1E6L)

static struct hrtimer hr_timer;
static int hr_timer_count = 0;

static const unsigned long hr_delay_in_ms = 1L;

enum hrtimer_restart my_hrtimer_callback( struct hrtimer *timer )
{
	ktime_t ktime;

	printk( "%d: my_hrtimer_callback called (%ld).\n", hr_timer_count, jiffies );

	if ( hr_timer_count < 2 )
	{
		ktime = ktime_set( 0, MS_TO_NS(hr_delay_in_ms) );
		hrtimer_forward_now( &hr_timer, ktime );
		hr_timer_count++;
		return HRTIMER_RESTART;
	}

	return HRTIMER_NORESTART;
}

int init_module( void )
{
  ktime_t ktime;
//  unsigned long delay_in_ms = 200L;

  printk("HR Timer module installing.  HZ = %u\n", HZ );

  hr_timer_count = 0;

  ktime = ktime_set( 0, MS_TO_NS(hr_delay_in_ms) );

  hrtimer_init( &hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );

  hr_timer.function = &my_hrtimer_callback;

  printk( "Starting timer to fire in %ldms (%ld)\n", hr_delay_in_ms, jiffies );

  hrtimer_start( &hr_timer, ktime, HRTIMER_MODE_REL );

  return 0;
}

void cleanup_module( void )
{
  int ret;

  ret = hrtimer_cancel( &hr_timer );
  if (ret) printk("The timer was still in use...\n");

  printk("HR Timer module uninstalling\n");

  return;
}
