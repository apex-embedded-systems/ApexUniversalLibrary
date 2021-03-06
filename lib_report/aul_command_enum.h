/* Auto Code Generation - AUL library ACGR version 14112610.    */
/* File: aul_command_enum.h    */
/* Date: Thu Jan 15 09:40:11 2015    */

#ifndef AUL_COMMAND_ENUM_H_
#define AUL_COMMAND_ENUM_H_

enum aul_command
{
     CMD_AULDRV_NOP                         =       16837824,
     CMD_AULDRV_ERROR_LAST                  =       16903360,
     CMD_AULDRV_INITIALIZE                  =       16968896,
     CMD_AULDRV_TERMINATE                   =       17034432,
     CMD_AULDRV_VERSION                     =       17099968,
     CMD_AULDRV_ID                          =       17165504,
     CMD_AULDRV_DEVICE_OPEN                 =       17231040,
     CMD_AULDRV_DEVICE_CLOSE                =       17296576,
     CMD_AULDRV_DEVICE_INTERRUPT_ENABLE     =       17558720,
     CMD_AULDRV_DEVICE_INTERRUPT_DISABLE    =       17624256,
     CMD_AULDRV_DEVICE_DEFINITION_AVAILABLE =       17689792,
     CMD_AULDRV_DEVICE_DEFINITION_GET       =       17755328,
     CMD_AULDRV_DEVICE_DEFINITION_SET       =       17820864,
     CMD_AULDRV_NAME                        =       17886400,
     CMD_AULDRV_DEVICE_LIST_GET             =       17951936,
     CMD_AULDRV_DEBUG_ACTIVE_GET            =       18017472,
     CMD_IO_NOP                             =       33614848,
     CMD_IO_ERROR_LAST                      =       33680384,
     CMD_IO_INITIALIZE                      =       33745920,
     CMD_IO_TERMINATE                       =       33811456,
     CMD_IO_VERSION                         =       33876992,
     CMD_IO_ID                              =       33942528,
     CMD_IO_OPEN                            =       34008064,
     CMD_IO_CLOSE                           =       34073600,
     CMD_IO_XFR                             =       34139136,
     CMD_IO_INTERRUPT_ENABLE                =       34204672,
     CMD_IO_INTERRUPT_DISABLE               =       34270208,
     CMD_IO_DEFINITION_AVAILABLE            =       34335744,
     CMD_IO_DEFINITION_GET                  =       34401280,
     CMD_ACGR_NOP                           =       50392256,
     CMD_I8255M0_RESTART                    =     1342237696,
     CMD_I8255M0_UPDATE                     =     1342303232,
     CMD_I8255M0_POWER_UP                   =     1342368768,
     CMD_I8255M0_POWER_DOWN                 =     1342434304,
     CMD_I8255M0_CONFIG_SET                 =     1342499840,
     CMD_I8255M0_CONFIG_GET                 =     1342565376,
     CMD_I8255M0_READ                       =     1342630912,
     CMD_I8255M0_WRITE                      =     1342696448,
     CMD_I8255M0_DI_GET                     =     1342761984,
     CMD_I8255M0_DO_SET                     =     1342827520,
     CMD_I8255M0_PORT_GET                   =     1342893056,
     CMD_I8255M0_PORT_SET                   =     1342958592,
     CMD_I8254_RESTART                      =     1392569344,
     CMD_I8254_UPDATE                       =     1392634880,
     CMD_I8254_POWER_UP                     =     1392700416,
     CMD_I8254_POWER_DOWN                   =     1392765952,
     CMD_I8254_CONFIG_SET                   =     1392831488,
     CMD_I8254_CONFIG_GET                   =     1392897024,
     CMD_I8254_READ                         =     1392962560,
     CMD_I8254_WRITE                        =     1393028096,
     CMD_I8254_CT_XFR                       =     1393093632,
     CMD_I8254_CT_XFR_ARRAY                 =     1393159168,
     CMD_INDIRECT_RESTART                   =     1426123776,
     CMD_INDIRECT_UPDATE                    =     1426189312,
     CMD_INDIRECT_POWER_UP                  =     1426254848,
     CMD_INDIRECT_POWER_DOWN                =     1426320384,
     CMD_INDIRECT_CONFIG_SET                =     1426385920,
     CMD_INDIRECT_CONFIG_GET                =     1426451456,
     CMD_INDIRECT_READ                      =     1426516992,
     CMD_INDIRECT_WRITE                     =     1426582528,
     CMD_TRACERE_NOP                        =      335604800,
     CMD_TRACERE_INITIALIZE                 =      335670336,
     CMD_TRACERE_TERMINATE                  =      335735872,
     CMD_TRACERE_POWER_UP                   =      335801408,
     CMD_TRACERE_POWER_DOWN                 =      335866944,
     CMD_TRACERE_OPEN                       =      335932480,
     CMD_TRACERE_CLOSE                      =      335998016,
     CMD_TRACERE_CONFIG_SET                 =      336063552,
     CMD_TRACERE_CONFIG_GET                 =      336129088,
     CMD_TRACERE_WRITE                      =      336194624,
     CMD_TRACERE_READ                       =      336260160,
     CMD_TRACERE_REG_WRITE                  =      336325696,
     CMD_TRACERE_REG_READ                   =      336391232,
     CMD_TRACERE_RESTART                    =      336456768,
     CMD_TRACERE_DIO_BIT_SET                =      336522304,
     CMD_TRACERE_DIO_BIT_GET                =      336587840,
     CMD_TRACERE_EVENT_ENABLE               =      336653376,
     CMD_TRACERE_EVENT_DISABLE              =      336718912,
     CMD_TRACERE_DIO_PORT_SET               =      336784448,
     CMD_TRACERE_DIO_PORT_GET               =      336849984,
     CMD_TRACERE_DIO_DIRECTION              =      336915520,
     CMD_TRACERE_DIO_UPDATE                 =      336981056,
     CMD_TRACERE_LED_SET                    =      337046592,
     CMD_TRACERE_LED_GET                    =      337112128,
     CMD_STX104_NOP                         =      335604864,
     CMD_STX104_INITIALIZE                  =      335670400,
     CMD_STX104_TERMINATE                   =      335735936,
     CMD_STX104_POWER_UP                    =      335801472,
     CMD_STX104_POWER_DOWN                  =      335867008,
     CMD_STX104_OPEN                        =      335932544,
     CMD_STX104_CLOSE                       =      335998080,
     CMD_STX104_CONFIG_SET                  =      336063616,
     CMD_STX104_CONFIG_GET                  =      336129152,
     CMD_STX104_WRITE                       =      336194688,
     CMD_STX104_READ                        =      336260224,
     CMD_STX104_REG_WRITE                   =      336325760,
     CMD_STX104_REG_READ                    =      336391296,
     CMD_STX104_RESTART                     =      336456832,
     CMD_STX104_DOUT_BIT_SET                =      336522368,
     CMD_STX104_DIN_BIT_GET                 =      336587904,
     CMD_STX104_EVENT_ENABLE                =      336653440,
     CMD_STX104_EVENT_DISABLE               =      336718976,
     CMD_STX104_DIO_PORT_SET                =      336784512,
     CMD_STX104_DIO_PORT_GET                =      336850048,
     CMD_STX104_DIO_UPDATE                  =      336915584,
};
typedef enum aul_command AUL_COMMAND_ENUM;

#endif  /* AUL_COMMAND_ENUM_H_*/

