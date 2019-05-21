The CMP_AVAIL.H file tells the core driver
which components are available for use.

The CMP_AVAIL.C file is used to build initialization
and termination lists.

The hardware scanning might take on a different 
approach and might be deprecated.

The sub-folders within are components and within
those components there is a definition of its
registers along with properties of the component.
The component also contains functions to process
data to be read/written to the component.

The names of the folders are built as follows:

	<name>_<logical ID>

Where "name" is the acronym of the component and
the "logical ID" is the logical ID of the component.
The logical ID is a unique identifier that is 
specified and used throughout the coding.

Within a sub-folder a component is composed of four files:
cmp_<name>.c
cmp_<name>.h
reg_<name>.c
reg_<name>.h

The "name" can be an acronym of the component.
The reg_<name>.c and reg_<name>.h are kept as
separate files so that it is easy to auto-
generate these files (i.e. translate from a
spreadsheet (comma delimited) to the associated
source files).


A driver is composed of zero or more devices.

A device is composed of zero or more components.

A component is composed of zero or more registers.

For some components registers can be virtual 
(i.e. just memory) in some cases (I don't mean 
memory I/O, I just mean memory locations to store 
parameters).  this might be useful if you have
a device that needs to talk to another driver
to achieve its I/O, but this device must maintain
certain pieces of information locally.  The 
information could be a form of translation of
data.


TODO:
A template folder is present with code as a
starting point.

