I8255M0 - Intel 8255 Mode 0 implementation.

-----

Why the "cmp_" prefix?  This keeps files nicely organized within a project or 
solution explorer, keeping all the components together in the list.

----------------
file definitions

cmp_i8255m0.c   	-  main core functions
cmp_i8255m0.h

cmp_i8255m0_status.h	-  All error and warning codes.  This will aggregate
			   with all other project status codes.

cmp_i8255m0_regc.	-  Register set definition.  This will be absorbed into
			   cmp_i8255m0.c file above.
cmp_i8255m0_reg.h

cmp_i8255m0_cmd.h	-  IOCTL commands related to this component.  This will
			   aggregate with all other project command codes.


------------------------------
jump table functions, required.
The jump table ought to be for things that occur regularly.  Things that are infrequent
ought to revert to some sort of setup/config function with a command.   The setup/config
function with command will require a packet to be built rather than simple flat approach.

1. Restart command
- Cold: writes config to the hardware and initializes registers as required
- Warm: retrieves current registers
- ALL: restarts FIFOs and buffers

2. Update command
- Could be used for polling
- Or, for purposes of simultaneous updates

3. Power down command
- Not currently used

4. Power up command
- Not currently used

5. Config Set
- Send a configuration data structure.  It can either be queued up awaiting a restart
  for the config to be committed or it can be committed directly.

6. Config Get

7. Read
- Data flow read.  Read data out of a buffer or hardware and return the buffer.

8. Write
- Data flow write.  Write data from buffer to hardware or FIFO.

OPTIONAL:
9. Any custom values


---------------------------------
In order to create a device:

1. We need to have a list of available components that include their 
   definition or component_definiton_type.

2.1. logical_ID
2.2. component_definition_type.  Pointer to this information. 
2.3. must be null terminated list.

2. A list of components that have been inventoried or predefined.  This must include:

2.1. logical_ID
2.2. Instance
2.3. offset
2.4. Address mode ( IO_DIRECT, IO_INDIRECT )
2.5. private_data.  If NULL, it will be created and attached.

2.6. The list must be null terminated.


3. From the two lists we do the following:

index = 0;
while ( LOGICAL_EMPTY != inventory[index].logical_id )
{

}


