

-------------------------------------------------------
-------------------------------------------------------
COMPONENT FILE NAMING CONVENTION
-------------------------------

The .H file tells the core driver which components are available for use.

The CMP_AVAIL.C file is used to build initialization and termination lists.

The hardware scanning might take on a different approach and might be deprecated.


------------------------------------------------------- 
------------------------------------------------------- 
COMPONENT FILE NAMING CONVENTION

1. The top level files found in the "components" directory are used to link all of the components together.

The sub-folders within are components and within those components there is a definition of its registers along with properties of the component. The component also contains functions to process configuration and data flow information to/from to the component.

The names of the individual component folders are built as follows:

	<name>

Where "name" is the acronym of the component.  The acronym will be used throughout the component code base.   When functions, constants, enums, structs and so on are found in a sorted list this naming technique will group all of the same component information together.  

Within a sub-folder a component is composed of four files: cmp_<name>_cmd.h cmp_<name>_reg.h cmp_<name>_status.h cmp_<name>.h cmp_<name>.h

------------------------------------------------------- ------------------------------------------------------- DESIGN CONCEPT






















A driver is composed of zero or more devices.

A device is composed of zero or more components.

A component is composed of zero or more registers.

For some components registers can be virtual  (i.e. just memory) in some cases (I don't mean  memory I/O, I just mean memory locations to store  parameters).  this might be useful if you have a device that needs to talk to another driver to achieve its I/O, but this device must maintain certain pieces of information locally.  The  information could be a form of translation of data.


TODO: A template folder is present with code as a starting point.

