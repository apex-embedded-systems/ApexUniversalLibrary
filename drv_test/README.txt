
Installing:
sudo insmod mod_test.ko offset=0x300,0x310 bytes=0x10,0x10 acronym="stx104","tracere" type="stx104","tracere" 

remove:
sudo rmmod mod_test

to see debug information:
dmesg

clear debug messages:  sudo dmesg --clear



README SUMMIT DRIVER DOCUMENT.

Apex Embedded Systems
February 19, 2013
Copyright (c) 2012-2013 Apex Embedded Systems.


This document discusses the following major topics:

   Makefile
      parameters that can be modified and how to run it.
   
   drv_summit.c 
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
	 cd ./drv_summit    <-- if you are already there, then you can skip this step.
	 make
	 
	 
----------------------------
DEBUG_ENABLE

Uncommenting DEBUG_ENABLE allows printk() messages to be posted.  This 
basically enables the "DEBUG_DRIVER" macro that is used in the lib_misc.h
file.

This sets the "DEBUG_DRIVER" macro in the software.

----------------------------
SIMULATE_IO

Uncommenting the SIMULATE_IO will suppress hardware I/O transactions (i.e. 
outb, outw, inb) and instead report behavior to printk() statement.  refer
to "io.c" file for details. 

This provides a convient mechanism for testing application and driver on
a desktop or laptop which does not have hardware installed.  

This sets the "IO_SIMULATION" macro in the software.

----------------------------
NAME

The 'NAME' variable in the makefile is used to set the name of the driver and
the prefix name of the device(s).  For example, if NAME=summit, then the 
module file name will become "mod_summit.ko" and the first device name will
be "summit0".  The second device will be named "summit1".  The file nodes,
as shown in /dev will be "summit0", "summit1" and so on.


----------------------------------------------------------------------
drv_summit.c file 
----------------

The overall structure of this file in sequence is as follows: 
  - headers
  - device level data structure definition
  - module parameters (passed in by insmod or modprobe)
  - driver level data structure definition
  - global variables
  - Driver level helper functions
  - Unused and commented out: read() and write() functions
  - ioctl() function
  - Module open() and close() functions
  - Module file_operations data structure.
  - Initialization and release helper functions
  - Initialization function
  - Termination function
  - other module information and macros.


----------------------------
DEVICE_QTY macro

The maximum number of devices is set using this macro.  For a single summit
board install, this macro can simply be set to one (1).


----------------------------------------------------------------------
Module parameters and insmod

Module parameters, as entered via the command line using insmod, for example,
are all applied to local global variables with a "requested_" prefix.  During
initialization, these values are evaluated and then posted into the 
driver information data structure.

The following parameters are recognized by insmod

  io        This is an array of one or more hardware base addresses
            for the Summit board device(s).  The name "io" seems to
            be very popular so we stick with a common name.
             
            Examples:
                io=0x300        <-- one Summit board
                io=0x300,0x310  <-- two Summit boards

            NOTE:  if DEVICE_QTY is set to one (1) and we try
            to insmod two boards, only one board will be initialized.
             
  major     This defines the major number of the driver.  Typically,
            this will not be used with insmod and the driver will
            ask to have a major number dynamically assigned.
            
            Examples:
              major=0x81
              
            NOTE: we don't recommend assigning a major number unless 
            you have too.
              
  dac_mode  This is an array of one or more values (one for each 
            device) that indicates the hard-coded jumper configuration
            on the Summit board to determine how the analog outputs
            will be updated. Below is a summary.
            
                  Jumpers     Mode    Software
                M3 M2 M1 M0   number  Constant
                X  0  0  0      0     DAC6CH_MODE_UPDATE_ANY_READ
                X  0  0  1      1     DAC6CH_MODE_UPDATE_READ_0X0F
                X  0  1  0      2     DAC6CH_MODE_UPDATE_UPON_WRITE
                X  0  1  1      3     DAC6CH_MODE_UPDATE_J5_P22_RE
                X  1  0  0      4     DAC6CH_MODE_UPDATE_J5_P22_FE
                1  X  X  X      5     DAC6CH_MODE_RESERVED          

            Examples:
              Jumpers M[3:0]="X010" <-- this is mode 2
                  dac_mode=2    <-- for a single Summit board
                  dac_mode=2,2, <-- for two Summit boards
                  
----------------------------------------------------------------------                  
Module/Driver Installation and removal
--------------------------------------

----------------------------                
Loading the Module

Putting this all together, here are some examples of command lines
to install the Summit driver.

  Examples (as root):
    insmod mod_summit.ko io=0x300 dac_mode=2
    insmod mod_summit.ko io=0x300 dac_mode=1
    insmod mod_summit.ko io=0x300,0x310 dac_mode=2,2
    insmod mod_summit.ko major=81 io=0x300 dac_mode=2
    
  Examples (as any user):
    sudo insmod mod_summit.ko io=0x300 dac_mode=2
    sudo insmod mod_summit.ko io=0x300 dac_mode=1
    sudo insmod mod_summit.ko io=0x300,0x310 dac_mode=2,2
    sudo insmod mod_summit.ko major=81 io=0x300 dac_mode=2    
    
    You will be asked for the root password.
    
    
  NOTE: we don't recommend assigning a major number unless 
  you have too.

----------------------------    
Device Permissions
    
    We need to set permissions so that any user can use it.
    
    1. Determine if the root group is "wheel" or "staff".  Do a 
       google search to learn more about these group permissions.
       The groups "wheel" or "staff" is different for different
       distributions.
       
        grep '^staff:' /etc/group
        
       If this returns a "staff" entry then use the group "staff",
       otherwise use the group "wheel".  
    
    2. change the group for each device:
      
        chgrp staff /dev/summit0 
        
    3. change the file permission for the device(s).
    
        chmod 666 /dev/summit0
        
    
----------------------------
Removing the module.  

  Here is an example at the command line.
    Example:
        sudo rmmod mod_summit
        You will be asked for the root password.
        
        
----------------------------
Automated scripts

Running the following scripts will take care of loading and unloading
the mod_summit.ko module.

As a non-root user to load or reload the module:
    sudo ./summit_load
    
As a non-root user to unload the module:
    sudo ./summit_unload
    
    
----------------------------------------------------------------------
Device file nodes
-----------------

The file nodes are automatically generated when the module (driver) is
installed.  
    

----------------------------------------------------------------------
Helpful Linux commands 
-----------------------

If you need more information you can use 'man' command in Linux to 
obtain more information or Google.

    dmesg   - retrieve printk() messages.  -c flag clears the buffer.
    modinfo - retrieve information regarding the compiled .ko module
    insmod  - install a module
    lsmod   - list modules loaded in the kernel
    rmmod   - remove module
    modprobe - insmod plus dependencies   

----------------------------------------------------------------------
Additional Testing Notes
-----------------------

This driver was tested using the following distributions:
  Ubuntu 12.04 i286 kernel 3.2.0-35-generic-pae
  Debian  6.06 i386 kernel 2.6.32-5-686

