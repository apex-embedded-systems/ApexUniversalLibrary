
This is a high-level discussion regarding the windows we are
building.

=======================
Notes about concepts here.

1. All items within are a starting point...nothing is fixed.

2. All forms tend to have the same pattern which leads me to 
   believe that it is very possible to re-use a lot of code
   here.

3. All this is embodied within the "test" device.  Later it may 
   splinter/duplicate to other "test" devices.  For example,
   we will end up with both a board called Yukon RevF, part number
   980039. There will be two software devices, production and 
   test versions.
   "yukon", "yukontest" might be the device names.

=======================
Definitions

Slot:   A slot is a command/instruction/data given at a certain 
        time interval.  

Frame:  A frame is composed of one or more slots.  

=======================
How to proceed in the design.

Suggest focusing on SPI-Cheetah first because all other forms will 
reuse a lot of what is there.

=========================================================
Engineering Notation
Text boxes where we input values.

We need to support engineering notation.
		http://en.wikipedia.org/wiki/Engineering_notation

We need to generalize this so that we can reuse everywhere.  The question will
be what is the expected output and simply need to scale to that.

Example:  If an input text box is asking for a value in Hertz (Hz) which is
          a frequency, then we need to support:

		  20M or 20000KHz or 20MHz or 20e6 which all mean 20,000,000 Hz

		  if we are looking for an output in KHz, then 

				20M or 20000KHz or 20MHz or 20e6 which all mean 20,000 KHz.

		  and so on.

My suggestion is convert to the units, then scale to the output.  Example:

Input      units              expected output format
20MHz --> 20,000,000 Hz -->   desire KHz  so it will be 20,000 KHz
20uS  --> 20e-6 seconds -->   desire nS   so it will be 20,000 nS

When we rescale to the expected output format we can deal with rounding
there.

12345 Hz  -->  desire KHz as integer becomes   12 KHz.

typically "u" is micro".


One possibility:

Conversion_Function( double in_value, NOTATION_ENUM in_metric, 
					 NOTATION_ENUM out_metric,
					 double * out_value
					)
{
	double in_multiplier;
	double value_units;

	switch( in_metric )
	{   // case statement from mega (M) to pico (p)
		// determine in_multipler
		...
	}
	value_units = in_value * in_multipler;

	switch( out_metric )
	{   // case statement from mega (M) to pico (p)
		// determine out_multiplier
		...
	}
	*out_value = value_units * out_multipler;
} 

=========================================================
Range Testing
Text boxes where we input values.

We need to support range testing.  In other words, the value entered
   must pass a range test.

Perhaps something like this:

CLAMP_ENUM Range_IsClamped( double in_value, double min, double max, double * out_value )
{
	CLAMP_ENUM clamp;
	if ( in_value < min ) 
	{
		*out_value = min;
		clamp = CLAMP_MIN; // = -1
	}
	else if ( in_value > max ) 
	{
		*out_value = max;
		clamp = CLAMP_MAX; // = -2
	}
	else
	{
		*out_vlaue = in_value;
		clamp = CLAMP_NONE;  // = 0
	}
	return clamp;
}

if ( Range_IsClamped( my_value, 100KHz, 50MHz, &my_value ) ) 
{
	// post the clamped "my_value" back into the text box
}



=======================
SPI - CHEETAH  (spich)

This exercises the SPI interface supported by third party hardware.
This screen is intended to exercise that hardware and also provide
a scheme for initial testing of hardware that has not been defined
otherwise.

Note that there will be other types of SPI interfaces, this is just 
the first one, but from the GUI standpoint they will all be the same.

Two forms:   spich_flow.cs  and   spich_config.cs.

=======================
Espec_Ect2  Temperature chamber.

Here we build a list of setpoint temperatures over time.  

The chamber or controller written in the library will go through
a series of steps or modes:  setpoint-match, soak time, measurement
window.

Setpoint-match: if the chamber temperature starts at 22C (room temp) and we
and we set a new set point of -40C, it will take time for it to reach 
the -40C temperature.  Typically, that might take 1/2 hour or 
so to achieve.  Once the temperature of the chamber is measured to 
be -40C (for at least a couple samples), we then go into soak mode.

Soak time:  this time interval is set such that all the stuff inside
the chamber as reached/settled to the set temperature.  Typically,
15 minutes seems to be about right.  Once the soak time has elapsed,
we then go into measurement-window mode.

Measurement-Window:  This is the time we give other parts of the 
system time to take necessary measurements.  You can think of this
as a gate that is openned and measurement data is allowed to flow.
At the end of this interval we issue a new setpoint and we revert
back to Setpoint-match mode.   This time interval is set based on
the kinds of measurements we need to perform.  It could be as little
as one second or as long as 24 hours.


Library API functions:



=======================
LTC2449

The LTC2449 is an analog-to-digital converter (ADC) with a SPI interface.  There
are lots of ADCs and chips in general that have SPI interfaces.  The problem is
how to move data and analyze the properties of the device.  This GUI is a way for
us to do that directly (well as direct as we can get).

Two forms:   ltc2449_flow.cs  and   ltc2449_cfg.cs.

For configuration, the only thing we need to set is the SPI clock.

=======================

 