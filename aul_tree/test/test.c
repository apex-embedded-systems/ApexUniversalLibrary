
#include "../shared/module.h"
#include <stdio.h>


#if(0)
/*****************************************************************************/
REGISTER_TYPE i8255m0_004_register_table[] = 
{
/* 0 - A - 0 */
	{ REG_CREATE_INFO( REG_WIDTH_BYTES(1),
					  REG_DIR_READ_WRITE,
					  REG_AUTO_INCREMENT(0),   // <-- does not appear to be really used any where.
					  REG_SHADOW(0),
					  REG_APERTURE_BYTES(1) 
				),
	  0, /* param1 */
	  0, /* virtual address */
#ifdef REG_DEBUG
	  "i8255-A"
#endif
	},
/* 1 - B - 1 */
	{ REG_CREATE_INFO( REG_WIDTH_BYTES(1),
					  REG_DIR_READ_WRITE,
					  REG_AUTO_INCREMENT(0),
					  REG_SHADOW(0),
					  REG_APERTURE_BYTES(1) 
				),
	  0, /* param1 */
	  1, /* virtual address */
#ifdef REG_DEBUG
	  "i8255-B"
#endif
	},
/* 2 - C - 2 */
	{ REG_CREATE_INFO( REG_WIDTH_BYTES(1),
					  REG_DIR_READ_WRITE,
					  REG_AUTO_INCREMENT(0),
					  REG_SHADOW(0),
					  REG_APERTURE_BYTES(1) 
				),
	  0, /* param1 */
	  2, /* virtual address */
#ifdef REG_DEBUG
	  "i8255-C"
#endif
	},
/* 3 - DIR - 3 */
	{ REG_CREATE_INFO( REG_WIDTH_BYTES(1),
					  REG_DIR_WRITE,
					  REG_AUTO_INCREMENT(0),
					  REG_SHADOW(0),
					  REG_APERTURE_BYTES(1) 
				),
	  0, /* param1 */
	  3, /* virtual address */
#ifdef REG_DEBUG
	  "i8255-DIR"
#endif
	},

/* 4 TEST 32-bit */
	{ REG_CREATE_INFO( REG_WIDTH_BYTES(4),
					  REG_DIR_READ_WRITE,
					  REG_AUTO_INCREMENT(0),
					  REG_SHADOW(0),
					  REG_APERTURE_BYTES(2) 
				),
	  0, /* param1 */
	  4, /* virtual address */
#ifdef REG_DEBUG
	  "test 32-bit"
#endif
	},

/* 5 TEST 16-bit */
	{ REG_CREATE_INFO( REG_WIDTH_BYTES(2),
					  REG_DIR_READ_WRITE,
					  REG_AUTO_INCREMENT(0),
					  REG_SHADOW(0),
					  REG_APERTURE_BYTES(2) 
				),
	  0, /* param1 */
	  8, /* virtual address */
#ifdef REG_DEBUG
	  "test 16-bit"
#endif
	},

	{ 0, 0, 0, 
#ifdef REG_DEBUG
	NULL 
#endif
	} /* end of register list */
};
#endif


//#include "../components/i8255m0/cmp_i8255m0.h"

//this is a mock device component inventory.
//#define MAIN_COMPONENT_QTY_INCLUDING_NULL  2
//static COMPONENT_TYPE main_inventory[] =
//{
//	CMP_INSTANTIATE( I8255M0_DEFINITION, 0 /* instance */, NULL /* location */, 0 /*offset */ ),
//	CMP_INSTANTIATE( I8255M0_DEFINITION, 1 /* instance */, NULL /* location */, 4 /*offset */ ),
//	CMP_NULL
//};

#if(0)
i8255m0_dataset_type i8255m0_data_set[2] = 
{
	I8255M0_INITIALIZE_DATASET,
	I8255M0_INITIALIZE_DATASET
};
//TODO: initially a prebuilt list at compile time.
component_type main_component_list[3] =
{
//TODO: WOULD BE NICE TO HAVE A SIMPLE MACRO(s)
//  I8255M0_INITIALIZE_COMPONENT_INDIRECT_ADDRESS(instance,link_rw,private_data,offset)
//  I8255M0_INITIALIZE_COMPONENT_DIRECT_ADDRESS(instance,private_data,offset)
//using discovered inventory: I8255M0_INITIALIZE_COMPONENT(instance,offset)
//      I8255M0_INITIALIZE_COMPONENT_NULL
	{	/* definition   */		&component_i8255m0_definition,
		/* physical_id  */		CMP_CREATE_PHYSICAL_ID( COMPONENT_I8255M0, 0 /* instance */ ),
		/* link_rw		 */		NULL,
		/* private_data */		&(i8255m0_data_set[0]),
		/* offset       */		0,
		/* status		 */		CMP_STS_BUILD(0,1,0,0,0)
	},
	{	/* definition   */		&component_i8255m0_definition,
		/* physical_id  */		CMP_CREATE_PHYSICAL_ID( COMPONENT_I8255M0, 1 /* instance */ ),
		/* link_rw		 */		NULL,
		/* private_data */		&(i8255m0_data_set[1]),
		/* offset       */		4,
		/* status		 */		CMP_STS_BUILD(0,1,0,0,0)
	},
	{	/* definition   */		NULL,
		/* physical_id  */		IOC_PHYSICAL_ID_NULL,
		/* link_rw		 */		NULL,
		/* private_data */		0,
		/* offset       */		0,
		/* status		 */		CMP_STS_BUILD(0,0,0,0,0)
	},
};
#endif


//we need to instantiate tracere along with all the parameters here first!


/*****************************************************************************/
static i8255m0_dataset_type i8255m0_data_set[2] = 
{
	I8255M0_INITIALIZE_DATASET,
	I8255M0_INITIALIZE_DATASET
};
/*****************************************************************************/
enum { TRACERE_COMPONENT_QTY = 2 };
/*****************************************************************************/
static component_type component_table[TRACERE_COMPONENT_QTY + 1] =
{
//TODO: WOULD BE NICE TO HAVE A SIMPLE MACRO(s)
//  I8255M0_INITIALIZE_COMPONENT_INDIRECT_ADDRESS(instance,link_rw,private_data,offset)
//  I8255M0_INITIALIZE_COMPONENT_DIRECT_ADDRESS(instance,private_data,offset)
//using discovered inventory: I8255M0_INITIALIZE_COMPONENT(instance,offset)
//      I8255M0_INITIALIZE_COMPONENT_NULL
	{	/* definition   */		&component_i8255m0_definition,
		/* physical_id  */		CMP_CREATE_PHYSICAL_ID( COMPONENT_I8255M0, 0 /* instance */ ),
		/* link_rw		 */		NULL,
		/* private_data */		&(i8255m0_data_set[0]),
		/* offset       */		0,
		/* status		 */		CMP_STS_BUILD(0,1,0,0,0)
	},
	{	/* definition   */		&component_i8255m0_definition,
		/* physical_id  */		CMP_CREATE_PHYSICAL_ID( COMPONENT_I8255M0, 1 /* instance */ ),
		/* link_rw		 */		NULL,
		/* private_data */		&(i8255m0_data_set[1]),
		/* offset       */		4,
		/* status		 */		CMP_STS_BUILD(0,1,0,0,0)
	},
	COMPONENT_LIST_NULL
};
devmgr_device_type tracere =
{
	/* definition		*/		&device_tracere_definition,
	/* physical_id		*/		CMP_CREATE_PHYSICAL_ID( DEVICE_TRACERE, 0 /* instance */ ),
	/* private_data   */		NULL,
	/* offset			*/		0x0300,
	/* component table */	component_table,
	/* component qty	*/		TRACERE_COMPONENT_QTY
};

/*
//TODO: we need a memory allocator to allocate all of the tracere device
//      information.  

	Allocate non-changing data
	allocate dynamic data.  Two layers.



*/

//insmod - or passed in from outside.
static size_t	base_address = 0x0300;

int Tracere_Initialize( void )
{

}

/******************************************************************
Description
This function is used in the driver DRV_Open() function in order
to allocate I/O resources.  This logic repeats in every driver module.

   NOTE:  only 'device' is used.
	if device=255=SYS_DEVICE_ALL_SELECT then
	  this is for the driver or all devices contained with this driver,
	  depending on how the driver will be used.
	end if;

TODO: what if a device has multiple resources?
  Answer: make this a loop with an io_resource[] list.

Updated to accommodate virtual components.
    
WAS:  SYS_Device_Open_Helper_Resources()

*/
int Main_Resources( size_t offset, component_type  component_list[] )
{
	size_t				address_start, address_end, address_len;
	int					error_code;

	error_code = SUCCESS;

	if ( 0 == offset ) 
	{ /* The device has a zero offset which implies that each component will need to be allocated. */
		return( -SYS_EC_ALLOCATE_RESOURCE_ADR_SPACE );
	}
	else
	{
		/* find the address range by finding all registers in the default address space */
		CmpMgr_Offset_Range( component_list,
									&address_start,
									&address_end,
									&address_len
								 );

		if ( address_len > 0 )
		{ /* we have real physical registers to be requested */
			/* add the base address */
			address_start += offset; /* add the base address */

//TODO: this is where we request I/O which must occur at low level!!

			/* request I/O space */
			//dp->io_resource = IOKERN_REGION_REQUEST( address_start, address_len, dp->name ); 
			//if ( !dp->io_resource )
			//{
			//	return( -SYS_EC_ALLOCATE_RESOURCE_ADR_SPACE );
			//}
		}
	}
	return error_code;
}

#include "../array/array.h"
/******************************************************************
Description
This function is used in the driver DRV_Open() function in order
to inventory and allocate components  This logic repeats in every 
driver module.

   NOTE:  only 'device' is used.
	if device=255=SYS_DEVICE_ALL_SELECT then
	  this is for the driver or all devices contained with this driver,
	  depending on how the driver will be used.
	end if;

TODO: what if a device has multiple resources?


WAS: SYS_Device_Open_Helper_Components()

*/
int Main_Components( component_type component_list[]  )
{
	int					error_code;
	size_t				count, count_max;

	error_code = SUCCESS;

	/* encode the array, so that it can be traversed by only knowing
	   an alement within the array.

	   First count the number of elements or components within the array.

	   The reason for all the counting is that if components are inventoried,
	   then then end of the array may not necessarily be known.  The count_max
	   provides a stop so that this loop will guarantee to terminate.
	*/
	count = 0;
//SPECIAL THIS CASE ONLY!!
//	count_max = MAIN_COMPONENT_QTY_INCLUDING_NULL; /* includes null */
	count_max = 3;
	while ( count < count_max )
	{
	/*  Component ID -- physical_id
		Includes both the logical ID and the instantiation, typically
		straight from hardware, or it might be predefined for some 
		components (i.e. i8255) that does not have an ID register.

		<ID: n-bits><instantiation: m-bits >
	*/
		if ( IOC_PHYSICAL_ID_NULL == component_list[count].physical_id )
		{
			break;
		}
		count++;
	}

	//if ( count > 0 )
	//{
	//	error_code = Array_Element_Mark_All( (void *) component_list,
	//										 sizeof( component_type ),
	//										 count, 
	//										 SYS_TYPE_OF(component_type),
	//										 SYS_TYPE_ALLOCATE
	//									   );
	//}

	/* now fixup any remaining private data and linkages with components */
	if ( error_code >= 0 )
	{	/* need private data first before link fix ups so that links can be stored */
		//next line replaced with below
		//Cmp_Private_Data_Fixup(	inventory );


		error_code = CmpMgr_List_Setup(	NULL,  //this assumes all private data has been fixed up!
													0x0300,
													3,		 //list max length is 3
													component_list
												);


		//next line has not been dealt with yet.
		//Cmp_Setup_Final(	inventory, 
		//					CMP_POST_SETUP_ESTABLISH_LINKS,
		//					inventory
		//			   );
	}

	return error_code;
}
/****************************************************************************

Similar to SYS_Command()

*/
int Component_Talk( uint32_t command, void * buf, size_t buf_len )
{
	int					error_code;
	size_t				cmp_cmd;
	size_t				cmp_pid;
	int					cmp_index;
	component_type *	component;
	//size_t				instance;
	size_t				index;
	size_t				cmd;

	cmd         = IOC_GET_COMMAND( command );
	cmp_pid	   = IOC_GET_PHYSICAL_ID( command );
	cmp_index   = CmpMgr_Component_Match_Physical_ID( tracere.component_table, cmp_pid );
	if ( CMPMGR_EC_FAILURE == cmp_index ) return -1; //did not find in component_list[]
	cmp_cmd		= cmd - CMD_COMMON_DEV_TOTAL_QTY; 
	component	= &( tracere.component_table[cmp_index] );
	//instance    = CMP_GET_INSTANCE( cmp_pid );
	index		   = IOC_GET_INDEX( command );
	error_code  = CmpMgr_Command( component,  /* info about the component			*/
											cmp_cmd,	 /* component command for jump table	*/
											index,		 /* index within component				*/
											(char *) buf, 
											buf_len 
										);
	return error_code;
}


/****************************************************************************

Similar to SYS_Command()

*/
int Device_Talk( uint32_t command, void * buf, size_t buf_len )
{
#if(0)
	int					error_code;
	size_t				cmp_cmd;
	size_t				cmp_pid;
	int					cmp_index;
	component_type *	component;
	//size_t				instance;
	size_t				index;
	size_t				cmd;

	cmd         = IOC_GET_COMMAND( command );
	cmp_pid	   = IOC_GET_PHYSICAL_ID( command );
	cmp_index   = CmpMgr_Component_Match_Physical_ID( main_component_list, cmp_pid );
	if ( CMPMGR_EC_FAILURE == cmp_index ) return -1; //did not find in component_list[]
	cmp_cmd		= cmd - SYS_CMD_TOTAL_QTY; 
	component	= &( main_component_list[cmp_index] );
	//instance    = CMP_GET_INSTANCE( cmp_pid );
	index		   = IOC_GET_INDEX( command );
	error_code  = DevMgr_Command( component,  /* info about the component			*/
											cmp_cmd,	 /* component command for jump table	*/
											index,		 /* index within component				*/
											(char *) buf, 
											buf_len 
										);
	return error_code;
#endif
	return SUCCESS;
}
int Main_Sys_Command( int device, uint32_t command, void * buf, size_t buf_len )
{
#if(0)
	size_t				cmd;
	int					error_code;
	size_t				cmp_cmd;
	size_t				cmp_pid;
	int					cmp_index;
	component_type *	component;
	size_t				instance;
	size_t				index;
	size_t				type;
	size_t				device;
//	SYS_DRIVER_TYPE *	driver;

	error_code = SUCCESS;

//Test for valid device.

	type = IOC_GET_TYPE( command );
	if ( SYS_IOC_TYPE == type ) cmd = IOC_GET_COMMAND( command );
	else                        return -SYS_EC_COMMAND_INVALID;

	dev = device_list[device]->device;
	
//Device
	pid       = IOC_GET_PHYSICAL_ID( command );
	cmp_index = CmpMgr_Component_Match_Physical_ID( device_list, pid );
	device    = &(  device_list[cmp_index] );
	channel   = IOC_GET_INDEX( command );
	cmp_cmd   = cmd - CMD_DEV_TOTAL_QTY;
	error_code  = DevMgr_Command( device,  /* info about the device			*/
											cmp,	 /* device command for jump table	*/
											channel,		 /* index within component				*/
											(char *) buf, 
											buf_len 
										);

//Component
	pid       = IOC_GET_PHYSICAL_ID( command );
	cmp_index = CmpMgr_Component_Match_Physical_ID( dev->definition->component_table, pid );
	component = &( dev->definition->component_table[cmp_index] );
	channel   = IOC_GET_INDEX( command );
	cmp_cmd   = cmd - CMD_DEV_TOTAL_QTY;
	error_code  = CmpMgr_Command( component,  /* info about the component			*/
											cmp_cmd,	 /* component command for jump table	*/
											channel,		 /* index within component				*/
											(char *) buf, 
											buf_len 
										);
/* 
1. find the device in our device table.  This will be an array of devices.

2. determine the the command type.

	cmd = IOC_GET_COMMAND( command );
	if ( cmd >= device->jump_table_qty )
	{
		//command is for a device. -- do all the things in Device_Talk()
		DevMgr_Command( 
	}
	else
	{ // command is for component
		cmd = cmd - device->jump_table_qty;
		//find component within the device -- do all the things in Component_Talk()
		//call CmpMgr_Command()
	}

determine device or component within a device.

*/
	//determine module type
#endif
	return SUCCESS;

}
/*****************************************************************************/
int DO_Bit_Set( int device, size_t channel, size_t value )
{
	uint32_t command;
	uint8_t  dout;
	size_t	instance;
	size_t	instance_prev;
	size_t	reg;
	size_t   reg_prev;
	int		error_code;
	int		scratch;
	int		bits;
	int		not_done;
	size_t	index;
/*
If we know something about capabilities, this would be easier, well more clear cut.

If these components were not in order, there would have to be some sort of table or 
mapping in terms of where to look.

...capabilities...for each component....
do.map[0] = first_do_component;
do.map[1] = next  do component;
do.

Once you have a list of components (which are from do[0] to do[n]), then you
call each component and ask if the channel fits into its range.

scratch = channel;
while()
{
component = do_component_list[index];  //mapping of components supporting DOs.
CmpMgr_Range_Channel( component, DO_CHANNELS, &scratch, &bits );
...rest is same as below...
}

REFER TO CMP_I8255M0.C LINE 447.

*/
	scratch  = channel;
	reg      = 0;			instance			= 0;
	reg_prev = reg;		instance_prev	= instance;
	index    = 0;			not_done			= 1;
	while ( not_done )
	{
		switch( reg )
		{
			case 0:	bits = I8255M0_A_BITS;		break;
			case 1:	bits = I8255M0_B_BITS;		break;
			case 2:	bits = I8255M0_C_BITS;		break;
		}
		scratch -= bits;
		if ( scratch < 0 ) 
		{ 
			scratch += bits;	
			not_done = 0;
			//reg = reg_prev;	instance = instance_prev;
		}
		else if ( scratch == 0 )
		{
			not_done = 0;
		}
		else					 
		{	
			reg_prev = reg;		instance_prev	= instance;
			index++;	
			if ( index >= 6 )
			{ error_code = 1;   goto DO_Bit_Set_Error;	}
			reg		= index % 3;  /* 3 == # registers per i8255m0 */
			instance = index / 3;
		}
	}
	// I8255M0_BITS_QTY = I8255M0_A_BITS + I8255M0_B_BITS + I8255M0_C_BITS;
	scratch = channel - instance * I8255M0_BITS_QTY; 
	command = CMP_CMD_SET_INSTANCE( CMD_I8255M0_DO_SET, instance );
//	command = command | reg;
//	command = CMD_I8255M0_DO_SET;
	command = IOC_SET_INDEX( command, scratch );
	//Alternative, but not robust: command = I8255M0_004_CMD_DO_SET | channel;
	if ( value ) dout = 1;
	else         dout = 0;
	return Component_Talk( command, (void *) &dout, 1 );

DO_Bit_Set_Error:
	Error_Log( "main",__func__,__LINE__, error_code );
	return error_code;
}

/*****************************************************************************/
int DO_Port_Set( int device, size_t channel, size_t value )
{
	return SUCCESS;
}

/*****************************************************************************/
int DO_Bit_String_Set( int device, size_t bit_first, size_t width, uint32_t value )
{
	return SUCCESS;
}
/*****************************************************************************/
int DO_Update( int device )
{ /* Has all DO related components update their outputs 
     Could be modified to only do those ports that have an update bit set.
  */
	return SUCCESS;
}


/*****************************************************************************/
int main(int argc, char *argv[], char *envp[])
{
	int error_code;
	//COMPONENT_TYPE * component;
	//size_t   instance;
	//size_t   index;
	uint32_t	command;
	size_t   instance;
	unsigned long revision;
	int     device;
	uint8_t wbuf[16];
	uint8_t rbuf[16];


	//size_t				cmp_cmd;
	//size_t				cmp_pid;
	//int					cmp_index;
	//COMPONENT_TYPE *	component;
	//size_t				instance;
	size_t				index;
	//uint32_t				command;
	//size_t				cmd;
	i8255m0_cfg_type i8255_cfg;
	//size_t				type;


	error_code = Module_Initialize();
	if ( error_code ) return error_code;

	error_code = IO_Open( &device, 0x0300, 16, 0 ); 
	if ( error_code )
	{
		Module_Terminate();
		return error_code;
	}

	IO_Version( &revision );

	printf( "Main: revision = %d\n", revision );

	Mem_Set( wbuf, 0, sizeof( wbuf ) );
   wbuf[0x0] = 0x11; wbuf[0x1] = 0x22;
	wbuf[0x2] = 0x33; wbuf[0x3] = 0x44;
   wbuf[0x4] = 0x55; wbuf[0x5] = 0x66;
	wbuf[0x6] = 0x77; wbuf[0x7] = 0x88;

	wbuf[0x8] = 0x99; wbuf[0x9] = 0xAA;
	wbuf[0xA] = 0xBB; wbuf[0xB] = 0xCC;
	wbuf[0xC] = 0xDD; wbuf[0xD] = 0xEE;


/*** COMPONENT LEVEL */
	/*** private data and offset fix ups 
	
	This will end up being part of initialization of the driver,
	since the offset or base_address is known at that time.
	
	*/

	//initialization
//	Cmp_Offset_Fixup( main_inventory, 0x300 );
//	Cmp_Private_Data_Fixup(	main_inventory );
	//open
	//error_code = Main_Resources( tracere.offset, tracere.component_table );
	//if ( error_code )
	//{
	//printf( "Status code: %s\n", Status_EC_String( error_code ) );
	//}
	////open
	//error_code = Main_Components( tracere.component_table );
	//if ( error_code )
	//{
	//printf( "Status code: %s\n", Status_EC_String( error_code ) );
	//}
//	Cmp_Offset_Fixup( main_inventory, 0x0300 );
//	Cmp_Private_Data_Fixup( main_inventory );

	/*** i8255m0 configuration */
#define DRV_TYPE_OF(type) SYS_TYPE_MAKE(type)
	Mem_Set( &i8255_cfg, 0, sizeof( i8255m0_cfg_type ) );
	i8255_cfg.type					= DRV_TYPE_OF( i8255m0_cfg_type );
	i8255_cfg.port_directions		= I8255M0_PDIR_AOUT_BOUT_COUT;
	i8255_cfg.port[I8255M0_REG_A].polarity		= 0x00;
	i8255_cfg.port[I8255M0_REG_A].init_value	= 0x00;
	i8255_cfg.port[I8255M0_REG_B].polarity		= 0x00;
	i8255_cfg.port[I8255M0_REG_B].init_value	= 0x00;
	i8255_cfg.port[I8255M0_REG_C].polarity		= 0xFF;
	i8255_cfg.port[I8255M0_REG_C].init_value	= 0x00;
	i8255_cfg.simultaneous_update	= false;

	for ( index = 0; index < I8255M0_BITS_QTY; index++ )
	{
		i8255_cfg.bit_map[index] = index;
	}

//	error_code = i8255m0_jump_table_qty();

/*** write the configuration */
	instance = 0;
	command = CMP_CMD_SET_INSTANCE( CMD_I8255M0_CONFIG_SET, instance );
	error_code = Component_Talk( command, (void *) &i8255_cfg, sizeof( i8255_cfg ) ); 
	if ( error_code )
	{
		printf( "Status code: %s\n", Status_EC_String( error_code ) );
	}
/*** write the configuration */
	instance = 1;
	command = CMP_CMD_SET_INSTANCE( CMD_I8255M0_CONFIG_SET, instance );
	error_code = Component_Talk( command, (void *) &i8255_cfg, sizeof( i8255_cfg ) ); 
	if ( error_code )
	{
		printf( "Status code: %s\n", Status_EC_String( error_code ) );
	}
/*** restart - commit configuration */
	instance = 0;
	command = CMP_CMD_SET_INSTANCE( CMD_I8255M0_RESTART, instance );
	error_code = Component_Talk( command, NULL /* RESTART_COLD */, 0 );
	if ( error_code )
	{
		printf( "Status code: %s\n", Status_EC_String( error_code ) );
	}
/*** restart - commit configuration */
	instance = 1;
	command = CMP_CMD_SET_INSTANCE( CMD_I8255M0_RESTART, instance );
	error_code = Component_Talk( command, NULL /* RESTART_COLD */, 0 );
	if ( error_code )
	{
		printf( "Status code: %s\n", Status_EC_String( error_code ) );
	}
/*** write to a port */
	instance = 1;
	command = CMP_CMD_SET_INSTANCE( CMD_I8255M0_PORT_SET, instance );
	command = command | I8255M0_REG_B;
	error_code = Component_Talk( command, (void *) wbuf, 1 );
	if ( error_code )
	{
		printf( "Status code: %s\n", Status_EC_String( error_code ) );
	}
/*** write a bit */
printf("\nDO_Bit_Set() Testing:\n" );
	error_code = DO_Bit_Set( 0,  7, 1 );
	error_code = DO_Bit_Set( 0, 23, 1 );
	error_code = DO_Bit_Set( 0, 23, 0 );

	error_code = DO_Bit_Set( 0, 33, 1 );
	error_code = DO_Bit_Set( 0, 33, 0 );
	error_code = DO_Bit_Set( 0, 47, 1 );
	error_code = DO_Bit_Set( 0, 47, 0 );
	error_code = DO_Bit_Set( 0, 63, 1 );

	if ( error_code )
	{
		printf( "Status code: %s\n", Status_EC_String( error_code ) );
	}
//FAILS because not testing channel range!	error_code = Gen_DOUT_Set( 0, 32, 1 );

#if(0)
/*** REGISTER LEVEL */

	Reg_Write( i8255m0_register_table,
				  0x0300,
				  1,
				  (char *)wbuf,
				  1
				);
	Reg_Write( i8255m0_register_table,
				  0x0300,
				  2,
				  (char *)&(wbuf[1]),
				  1
				);

	Reg_Write( i8255m0_register_table,
				  0x0300,
				  4,
				  (char *)&(wbuf[0]),
				  8  /* writing two elements to same register location */
				);

	Reg_Write( i8255m0_register_table,
				  0x0300,
				  5,
				  (char *)&(wbuf[6]),
				  2
				);

	Mem_Set( rbuf, 0, sizeof( rbuf ) );
	Reg_Read(  i8255m0_register_table,
				  0x0300,
				  2,
				  (char *)&(rbuf[0]),
				  2 /* read register twice */
				);
	Mem_Set( rbuf, 0, sizeof( rbuf ) );
	Reg_Read(  i8255m0_register_table,
				  0x0300,
				  5,
				  (char *)&(rbuf[0]),
				  4 /* read register twice */
				);
	Mem_Set( rbuf, 0, sizeof( rbuf ) );

//this probably ought to post an error somewhere --- perhaps through debug?
//well, this level will only be used by component which it tests its parameters
// fairly thoroughly, I think.
	Reg_Read(  i8255m0_register_table,
				  0x0300,
				  4,
				  (char *)&(rbuf[0]),
				  8 /* read register twice */
				);
	Mem_Set( rbuf, 0, sizeof( rbuf ) );
	Reg_Read(  i8255m0_register_table,
				  0x0300,
				  3,
				  (char *)&(rbuf[0]),
				  1 /* read register */
				);
#endif
	
	wbuf[0]=1;
	error_code = DevMgr_Command( &tracere, CMD_TRACERE_EVENT_ENABLE_BASE, 15, (char *) wbuf, 1 );
	wbuf[0]=1;
	error_code = DevMgr_Command( &tracere, CMD_TRACERE_DIO_BIT_SET_BASE, 15, (char *) wbuf, 1 );
	wbuf[0]=0;
	error_code = DevMgr_Command( &tracere, CMD_TRACERE_DIO_BIT_SET_BASE, 15, (char *) wbuf, 1 );

	IO_Close( device );
	Module_Terminate();

	return error_code;
}