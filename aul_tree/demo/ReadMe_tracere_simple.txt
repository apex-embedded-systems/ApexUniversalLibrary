2014MAY20 - to be revised...

November 4, 2013
This particular version of STX104_SIMPLE is for Borland compiler.  Our goal
is to merge both Borland and Microsoft versions by way of separate libraries
to include within each project, where the library is for a specific compiler.


The location of "aul_hwdef.txt" is relative to the project location
as it is currently setup.  In other words, it is assumed to be in the
same folder as the application executable.


========================================================================
       CONSOLE APPLICATION : stx104_simple
========================================================================

/////////////////////////////////////////////////////////////////////////////
GENERAL

STX104_SIMPLE is intended to be used to perform simple I/O actions via keystrokes.
Basically, one can observe each ADC channel one at a time, manipulate analog
outputs and digital outputs, and monitor digital inputs.

It will also output frame counter and the number of samples sitting in the STX104
FIFO.  

/////////////////////////////////////////////////////////////////////////////

stx104_simple.ide
    This file (the project file) contains information at the project level and
    is used to build a single project

stx104_simple.c
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



