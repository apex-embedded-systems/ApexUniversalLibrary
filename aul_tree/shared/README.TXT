LIB


-------------------------------------------------------------
GENERAL


This directory houses files that are shared between both the 
kernel driver and the application library.  These files are
required for successful compilation of either the kernel 
driver or application which interfaces to the driver.




-------------------------------------------------------------
DEFINITION OF TERMS

-------------------
kernel driver

This is the driver portion of the code which resides within the kernel space.
					 
-------------------
application library

This is the software on the application side used to interface to the driver.
It provides easy to understand set of APIs.   

The library is typically a DLL in Windows or a static library compiled 
into an application built in linux.



-------------------------------------------------------------
COMMUNICATIONS


application                |     kernel
  space                    |     space
                           |
application <--> library <---> driver
                           |


