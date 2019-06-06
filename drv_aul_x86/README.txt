---------------------------------------------------------------------------------
APEX EMBEDDED SYSTEMS
Apex Universal Library, Kernel Module - Bottom x86 I/O support.

---------------------------------------------------------------------------------
CONTACT INFORMATION
APEX EMBEDDED SYSTEMS
116 Owen Road
Monona, WI 53716
(608)-256-0765
customer.service@apexembeddedsystems.com
---------------------------------------------------------------------------------
TITLE

---------------------------------------------------------------------------------
AUTHOR   
$Author: Mike $:
---------------------------------------------------------------------------------
DATE        
$Date: 2015-07-30 20:35:12 -0500 (Thu, 30 Jul 2015) $:

---------------------------------------------------------------------------------
REVISION HISTORY  

---------------------------------------------------------------------------------
SCOPE
This ReadMe applies primarily to drv_aul_x86 and the aul_tree as the drv_aul_x86 do use source files found in aul_tree.  

---------------------------------------------------------------------------------
OVERVIEW
This is used to compile and install the generalized aul driver which
utilizes underlying I/O compatible with x86 machines.

---------------------------------------------------------------------------------
ASSUMPTIONS
1. All file locations referenced within this document are relative to the ReadMe file location.

2. The simulator is simply an array of memory used to behave as I/O space.
There is no intelligence within the simulator.

3. There is no distinction between the driver running the simulator versus
    drivers read/writing directly to hardware.  Later version will implement
    improved status information.

4. The name of the driver "auldrv" must not be changed.  The library currently 
    assumes this name.  Later version will implement source tree enhancement.

5. The driver uses udev.


NOTE: You may wish to create a rule in /etc/udev/rules.d.  For example, we created
a file "99auldrv.rules" and within that (text) file we created the line:

	SUBSYSTEM=="auldrv" ACTION=="add" MODE="0666"

Or, perhaps something like this for a specific group.

	SUBSYSTEM=="auldrv" ACTION=="add" GROUP="my_group" MODE="0666"

This will change the permissions of the devices within /dev folder.


6. Kernel must be 2.6 or later.  Note, if you wish to use the C# demo (Demo_GUI), then you
   will need a Linux kernel that is 3.2.0 or later.

---------------------------------------------------------------------------------
SETUP / INSTALLATION GUIDE
	
1. Installation of kernel-headers.  If kernel headers are not on your
   system that you intend to compile this kernel module on then you must
   download the kernel headers.  Note: if you are logged in as root,
   then you do not need the "sudo" prefix on the command lines below.
   
   Note: If "sudo" does not work, try "su" and enter password.  There after
   you are logged in as super-user.  There maybe better ways of doing this
   to preserve security.  This guide is more about just getting the driver
   installed and functional.
   
1.a. Make sure you have updated version

     sudo apt-get update
     
1.b. Install linux-header package for Debian or any Debian derivative 
     (i.e. Ubuntu, etc)
     
     sudo apt-get install linux-headers-$(uname -r)
     
     
2. If GCC is not installed on your system, you must install it now.
	 apt-get update && apt-get upgrade
	 apt-get install build-essential
	 
	 To verify its installation:
	   whereis gcc make
	   
	 To determine version of gcc and make:
	   gcc -v
	   make -v

3. Compilation of the driver using Eclipse or terminal (command line).  
3.a. Using Eclipse, simply right click on this project and build.
3.b.1. From the terminal (command line), move into this 
        directory (i.e. change directory).
3.b.2. Type "make" (without the quotes) to compile the driver into a 
        mod_auldrv_sim.ko file.
3.b.3. Type "sh auldrv_load" (without the quotes) to install the driver.
        Alternatively, you can simply insmod (copy and paste the line 
	    from the auldrv_load script.

---------------------------------------------------------------------------------
This document discusses the following major topics:

   Makefile
      parameters that can be modified and how to run it.
   
   auldrv.c 
      organization
      
   Module parameters and insmod
      the driver module parameters
      
   Module/Driver installation and removal 
      how to do this
      permissions
      scripts
      
   Helpful Linux commands
    
      
----------------------------------------------------------------------
Makefile:
--------

Please refer to makefile comments especially at the top of the make file.

To compile the driver come to this directory and type the make command at
the prompt.

	 Example:
	 cd aes_eclipse/drv_aul_x86    <-- if you are already there, then you can skip this step.
	 make
	 
	 
----------------------------
DEBUG_ENABLE

Uncommenting _DEBUG allows printk() messages to be posted.  This 
basically enables the "_DEBUG" macro.

----------------------------
NAME

The 'NAME' variable in the makefile is used to set the name of the driver.  
Within /dev folder the driver will appear as the 'NAME'.  The default name
is "auldrv".  

The name of the driver "auldrv" must not be changed.  The library currently 
assumes this name.  Later version will implement source tree enhancement.

----------------------------------------------------------------------
auldrv.c file 
----------------



----------------------------
DEVICE_QTY macro

The maximum number of devices is set using this macro.  For a single board
this macro can simply be set to one (1).


----------------------------------------------------------------------
Module parameters and insmod

Module parameters, as entered via the command line using insmod, for example,
are all applied to local global variables with a "requested_" prefix.  During
initialization, these values are evaluated and then posted into the 
driver information data structure.

The following parameters are recognized by insmod

  major     This defines the major number of the driver.  Typically,
            this will not be used with insmod and the driver will
            ask to have a major number dynamically assigned.
            
            Examples:
              major=0x81
              
            NOTE: we don't recommend assigning a major number unless 
            you have too.

  offset    This is an array of one or more hardware base addresses
            for the Summit board device(s).  
             
            Examples:
                offset=0x300        <-- one board
                offset=0x300,0x310  <-- two boards

            NOTE:  if DEVICE_QTY is set to one (1) and we try
            to insmod two boards, only one board will be initialized.
             
  bytes     This is an array of one or more hardware I/O space 
            required.  In other words contiguous bytes within
            the I/O space. 
             
            Examples:
                bytes=0x10       <-- one board using 16 contiguous I/O bytes.
                bytes=0x10,0x10  <-- two boards
                
 acronym    This is an array of one or names that are used to 'open' then
            device(s). 
             
            Examples:
                acronym="stx104"             <-- one board
                acronym="stx104","tracere"   <-- two boards         

    type    Defines the type of the device.  Possible types
            are: "stx104", "tracere", "summit"
             
            Examples:
                acronym="stx104"             <-- one board
                acronym="stx104","tracere"   <-- two boards    

Complete examples:

Example #1: Single STX104 installed on a stack with base address at 0x300:

	Driver Installation:
	sudo insmod mod_auldrv.ko offset=0x300 bytes=0x10 acronym="mystx104" type="stx104"

	Using the library we open the device as follows:
		int board;
		error_code = AUL_Initialization( NULL );
		if ( error_code ) return error_code;
		error_code = AUL_Device_Open( "mystx104", &board );
		if ( error_code )
		{	printf( "Error code: %s\n", AUL_Last_Error_Get() );
			AUL_Termination();
			return error_code;
		}
		AUL_Device_Close( board );
		AUL_Terminate();
		return SUCCESS;
		
		
Example #2: STX104 board with base address 0x300 and Tracer board with base address 0x310 installed on a stack:

	Driver Installation:
	sudo insmod mod_auldrv.ko offset=0x300,0x310 bytes=0x10,0x10 acronym="mystx104","mytracere" type="stx104","tracere"
	

	Using the library we open the device as follows:
		int board[2];
		error_code = AUL_Initialization( NULL );
		if ( error_code ) return error_code;
		error_code = AUL_Device_Open( "mystx104", &(board[0]) );
		if ( error_code )
		{	printf( "Error code: %s\n", AUL_Last_Error_Get() );
			AUL_Termination();
			return error_code;
		}
		error_code = AUL_Device_Open( "mytracere", &(board[1]) );
		if ( error_code )
		{	printf( "Error code: %s\n", AUL_Last_Error_Get() );
			AUL_Device_Close( board[1] );
			AUL_Termination();
			return error_code;
		}		
		AUL_Device_Close( board[1] );
		AUL_Device_Close( board[0] );
		AUL_Terminate();
		return SUCCESS;
		
	            
----------------------------------------------------------------------                  
Module/Driver Installation and removal
--------------------------------------

----------------------------                
Loading the Module

  Examples (as root):
  
  Example #1:  STX104 at base address 0x300.
  insmod mod_auldrv.ko offset=0x300 bytes=0x10 acronym="stx104" type="stx104"
  
  Example #2:  STX104 at base address 0x300 and Tracer-E at base address 0x310.
  insmod mod_auldrv.ko offset=0x300,0x310 bytes=0x10,0x10 acronym="stx104","tracere" type="stx104","tracere"
  
    
  Examples (as any user):
  
  Use sudo insmod....
    
    You will be asked for the root password.
    
  NOTE: we don't recommend assigning a major number unless 
  you have too.
        
    
----------------------------
Removing the module.  

  Here is an example at the command line.
    Example:
        sudo rmmod mod_auldrv
        You will be asked for the root password.
        
----------------------------------------------------------------------
Auto installation at time of start up.
-----------------------



        
----------------------------
Automated scripts

Running the following scripts will take care of loading and unloading
the mod_auldrv.ko module.

As a non-root user to load or reload the module:
    sudo ./auldrv_load
    
As a non-root user to unload the module:
    sudo ./auldrv_unload
    
    
----------------------------------------------------------------------
Device file nodes
-----------------

The file nodes are automatically generated when the module (driver) is
installed.  

<More info to be added>
    

----------------------------------------------------------------------
Helpful Linux commands 
-----------------------

If you need more information you can use 'man' command in Linux to 
obtain more information or Google.

    dmesg    ==> retrieve printk() messages.  -c flag clears the buffer.
    modinfo  ==> retrieve information regarding the compiled .ko module
		 See example below.  
    insmod   ==> install a module
    lsmod    ==> list modules loaded in the kernel
    rmmod    ==> remove module
    modprobe ==> insmod plus dependencies   
    uname -r ==> display the Linux kernel version (useful in scripts)

----------------------------------------------------------------------
MODINFO
-----------------------
1. Open a terminal

2. Go to the drv_aul_x86 directory where the "mod_auldrv.ko" file resides.

3. Type at the prompt:
	modinfo mod_auldrv.ko

   It will return the following (the exact message below may be different depending on the revision):

filename:       mod_auldrv.ko
license:        GPL
description:    AES Generic Kernel Driver
author:         customer.service@apexembeddedsystems.com
srcversion:     88B4D4233DDB5A0A12DA0A3
depends:        
vermagic:       3.2.0-75-generic-pae SMP mod_unload modversions 686 
parm:           major:This driver ID. Dynamic assignment when major=0 (default). (int)
parm:           offset:region offset array. Ex: offset=0x300 or offset=0x300,0x310 (array of uint)
parm:           bytes:region bytes array. Ex: bytes=0x10 or bytes=0x10,0x10 (array of uint)
parm:           restrict:array of uint
parm:           bit8:region restrict 8-bits. Ex: restrict=0 or restrict=0,1
parm:           acronym:region name array. Ex: acronym="mystx104" or acronym="mystx104","mytracer" (array of charp)
parm:           type:type name array. Ex: type="sxt104" or type="stx104","tracer" (array of charp)


----------------------------------------------------------------------
Additional Testing Notes
-----------------------

This driver was tested using the following distributions:
  Ubuntu 12.04 i386 kernel 3.2.0-38-generic-pae

TODO:  Debian  6.06 i386 kernel 2.6.32-5-686
       Possibly others.



