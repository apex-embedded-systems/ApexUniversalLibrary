2014MAY20 - to be revised

librt.a was added to the linker.  Why?

November 4, 2013
This particular version of STX104_FAST is for Borland compiler.  Our goal
is to merge both Borland and Microsoft versions by way of separate libraries
to include within each project, where the library is for a specific compiler.


The location of "aul_hwdef.txt" is relative to the project location
as it is currently setup.  In other words, it is assumed to be in the
same folder as the application executable.


========================================================================
       CONSOLE APPLICATION : stx104_fast
========================================================================

/////////////////////////////////////////////////////////////////////////////
GENERAL

STX104_FAST is intended to be used to perform high speed analog input operations.
This application will write data continuously to a binary file.

The menu enables ADC sampling (software trigger start) and stops ADC sampling
(software trigger stop).  With the rudimentary menu one can set gain, first
and last channel and the analog and digital outputs.  Note that if the card
is actively sampling data, it will not be possible to change channel numbers;
you will need to stop sampling, change the channel values, and then start
sampling.

The output file and sampling rates are hardcoded.  These could be added to 
the menu at some point.

A line at the bottom of the screen is updated to show status and current
board configuration.

Uncommenting the "FIFO_LOG_DATA_AVAILABLE" macro will allow you to log
FIFO data available quantities as well.  This can help debug possible
data throughput issues, should there ever be any.  

The "FILE_OUTPUT_RATE_HZ" macro sets the rate at which data is written to a
file.  This in combination with the macros "AI_MAXIMUM_SAMPLE_BUFFER_SIZE"
and "AI_FRAME_TIME_NS" will dictate how many samples can be written per fwrite().
The software will analyze the settings and let you know if the combinations
of parameters will work.  In any system it is important to balance average
data rate with CPU-readout burst operations (i.e. the bursty CPU reading of 
data from the STX104 FIFO).  OUr experience is that the optimum setting is
based on a rhythmic or periodic CPU-burst reads that balance all of the
CPU loads.  Obvious extreme example is you would not want to read one 
sample at a time at 200KSPS, that would just be too many function calls.  
On the other hand you would not want to read out 1 million samples in 
one call (**) as one might end up missing samples due to FIFO overflow (i.e. 
waited to long to perform a read) and the CPU would then be very occupied 
in moving all that data in one big slug.  Better to find some middle 
ground and this is dependent in part on your overall system.   


(**) There is one case where this might make sense.  This is the case where 
you tell the STX104 you want to collect 1 million samples and stop, then then
perform a post readout of the data.  

/////////////////////////////////////////////////////////////////////////////

stx104_fast.ide
    This file (the project file) contains information at the project level and
    is used to build a single project

stx104_fast.c
    This is the main application source file.

aul_hwdef.txt
	This file is located within the application folder.  It specifies
	hardware information and the base address must match that set in
	the driver.

auldrv_bor.lib
	This file is required for linking.   This file is a copy that
 	was originally found in the auldrv_bor folder.	

auldrv_bor.dll
	This file is part of the driver at the application layer.  It provides
	the API interface.  You can examine it using "dumpbin /export auldrv_bor.dll"
	or Dependency Walker.

auldrv.h
	This file is required for compilation.  This file is a copy that
 	was originally found in the auldrv_bor folder.

cw3230.dll
	This file is required in order to run the Borland application as it will
	reference this dll at run time.



/////////////////////////////////////////////////////////////////////////////
This application was written using Borland V5.02.

/////////////////////////////////////////////////////////////////////////////
Borland Project Settings...

SEE SOURCE FILE FOR DETAILS


/////////////////////////////////////////////////////////////////////////////
Other notes:

1. "TODO_CUSTOMER" indicates parts of the source code you can add to or customize. 



/////////////////////////////////////////////////////////////////////////////
What if I am using interrupts, but nothing seems to be working?

First: compile and run without interrupts.  If it works, then you know you
are experiencing interrupt related problems.

Second: make sure another driver is not using the IRQ line you wish to use.

Third: Check BIOS configuration.  You might try enabling those COM ports even
it might not seem like the right thing to do.  For a DOS application if you 
want IRQ3 or IRQ4 you ought to turn off the COM ports in the BIOS, but in
Windows that may not work.  For windows it might turn out that enabling
the COM ports in BIOS and then disabling the COM port drivers in Windows will
allow you to take advantage of IRQ3 and/or IRQ4.

NOTE: We have seen cases where interrupts work fine in DOS, but then using
windows the interrupts behave in strange ways.  This might be the result
of how the BIOS, Windows and Windows drivers interact.  The best advice is 
to try and manipulate combinations of settings to see if it makes the problem
better or worse, you might actually find out issues with respect to 
hardware and software interactions that you might not expect and might be
good to know as you build up your system.

Fourth: try a different IRQx line that you know is free, even though it may
not be the IRQ line you really wish to use.

Fifth: Contact us and we will help you get to the bottom of the issue.


/////////////////////////////////////////////////////////////////////////////



