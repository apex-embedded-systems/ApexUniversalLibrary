
Apex Embedded Systems
July 6, 2014



=======================
PURPOSE

This GUI is intended for low level hardware validation and testing.

This GUI along with a test device defined in the AUL Library/Driver
can be used to completely bypass all other hardware and software 
layers for directly low level access.  It can also be used to bypass
internal FPGA VHDL layers as needed.

Can also be used for initial verification of the hardware directly.

=======================
DEFINITIONS

1. Frame.  The outbound list is considered a single frame.  List and frame
are used interchangably here.

2. Slot.  A frame is composed of one or more slots or items.

=======================
BUSINESS LOGIC

The bulk of the business logic shall be embodied within this GUI unless
stated otherwise.

This GUI is intended for internal use only.  Other A/D converters will 
have similar sort of implementation so plan for some amount of reuse.

NOTE: hoping that we will see a large amount of reuse as most A/D converters
will operate in a similar fashion.  The slot build process will vary amongst
A/D converters.

Somehow, we need to present human readable information in the ListBox and
at the same time keep machine readable slot information in an array.  The
array is a complete frame.  If "Frame" checkbox is checked, then the entire
frame is submitted to the library for transmission, otherwise, we submit a 
slot at a time (either manually or by time interval).



=======================
LIBRARY FUNCTIONS

Below is a list of library functions provided:



=======================
OPERATIONS

0.  Cycling
0.1. "Frames" checkbox.  If checked then "Cycle Manual" or "Cycle Rate" is used
     to control delivery of frames.  Otherwise, it is used to control delivery
	 of slots.
0.2. Cycling - Manual (rdb_cycle_manual): Requires use of "Cycle Manual" button
0.3. Cycling - Single (rdb_cycle_single): only one frame is sent.
0.4. Cycling - Free-run (rdb_cycle_free_run): frames/slots sent at manual or Cycle rate
     rates.

1.	 Enter slot (op-code) information.   

1.a. Press "Add to Frame" to transfer to outbound list or frame.  It will be added
	 or inserted just at the "Outbound Opcode Frame" cursor.  If no cursor
	 then added to the end.

	 If "Delay" is a non-zero value, then it will be appended to the code.  Internally,
	 within the library it will compute center-to-center slot time so that it matches
	 the delay time as best as it can.

	 The range of delay is 0 nS to 2 sec in nanosecond resolution.  The values
	 entered in the text box can be for example:

		100 n  -->  100e-9 seconds, or 100 nanoseconds, or 100 ns
		1 m    -->   1e-3  seconds, or 1 millisecond,   or 1,000,000 ns
		1      -->    1    seconds, or 1 second         or 1,000,000,000 ns
		1 u    -->    1e-6 seconds, or 1 microsecond    or 1,000 ns

		In other words, classical engineering notation.
		http://en.wikipedia.org/wiki/Engineering_notation

2. Slot Remove (item).  Any line in "Outbound Opcode Frame" that is highlighted
   will be removed.  Only one Item at a time (for simplicity).

3. Frame Clear (list).  Will completely remove the list in "Outbound Opcode Frame".

4. Frame Save.  Saves the frame to a file.  Prefer CSV so that any software can
   pick it up and/or produce the same file.

4.1. CSV format:
4.1.1. First line defines columns:  "SLOT","IN_POS","IN_NEG","OSR", "TWOX", "DELAY_NS"

5. Frame Load.  Retrieves a CSV file.
5.1. Passes information onto Ltc2449_Slot_Build() to build the slot op-codes as necessary.
5.2. When each row is read in, it basically would replicate the manual entry of a slot.


6.	Cycle Start. 
6.a.  Disables all other buttons except for "Cycle Manual" where applicable.
6.b. "Cycle Start" button disabled too.
6.c. The list processing begins at the beginning of the list.
6.d. Note 1: the inbound data is only processed once we have wrapped and performed
     the first item in the frame/list.
6.e. Note 2: If the commit list is timing, then the list will continue to 
     be cycled continuously until a "Cycle Stop" request is pressed.  It will
	 start at the highlighted row in the list.  

7. Cycle Stop
7.a. Cycle stop is a request to stop. 

8. Once the list (or frame) has been completely sent, then we can retrieve the
   inbound data and present it.  

9. "Cycle Rate"  This is the rate at which the slots/frames are sent to the hardware.
    The number entered is in Hertz and is limited from 0.1 Hz to 100 Hz.

10. Frame Log.
10.1 Logs a CSV where each row is a frame  (if file is *.csv).  Raw values.
10.2. Logs a binary raw values sequently (if file is *.bin).  Raw values.
