/*****************************************************************************
 *
 *  ntpSnmpSubAgentObject.h
 *
 *	Definitions and macros for ntpSnmpSubAgentObject.c
 *
 ****************************************************************************/


#ifndef NTPSNMPSUBAGENTOBJECT_H
#define NTPSNMPSUBAGENTOBJECT_H

/* Function Prototypes */


/* Initialization */
void            init_ntpSnmpSubagentObject(void);

/* MIB Section 1 Callback Functions*/
Netsnmp_Node_Handler get_ntpEntSoftwareName;
Netsnmp_Node_Handler get_ntpEntSoftwareVersion;
Netsnmp_Node_Handler get_ntpEntSoftwareVersionVal;
Netsnmp_Node_Handler get_ntpEntSoftwareVendor;
Netsnmp_Node_Handler get_ntpEntSystemType;
Netsnmp_Node_Handler get_ntpEntTimeResolution;
Netsnmp_Node_Handler get_ntpEntTimeResolutionVal;
Netsnmp_Node_Handler get_ntpEntTimePrecision;
Netsnmp_Node_Handler get_ntpEntTimePrecisionVal;
Netsnmp_Node_Handler get_ntpEntTimeDistance;

/* MIB Section 2 Callback Functions (TODO) */
Netsnmp_Node_Handler get_ntpEntStatusCurrentMode;
Netsnmp_Node_Handler get_ntpEntStatusCurrentModeVal;
Netsnmp_Node_Handler get_ntpEntStatusStratum;
Netsnmp_Node_Handler get_ntpEntStatusActiveRefSourceId;
Netsnmp_Node_Handler get_ntpEntStatusActiveRefSourceName;
Netsnmp_Node_Handler get_ntpEntStatusActiveOffset;

/* TODO: This needs to be changed as soon as the official OID has been registered with IANA */
#define NTPV4_OID 1,3,6,1,4,1,5597,99


/* The following two macros simplify the registration of the callback functions 
 * and allow to easily specify the name and OID of either read-only (RO) or read-write (RW) functions
 */
 
#define _SETUP_OID_RO( _oidname, ... )  \
    static oid _oidname##_oid [] = { __VA_ARGS__ };                                                                             \
   {                                                                                                                                                                            \
         netsnmp_register_read_only_instance(netsnmp_create_handler_registration                        \
                                        ("#_oidname",                                                                                                             \
                                         get_##_oidname,                                                                                                        \
                                         _oidname##_oid,                                                                                                        \
                                         OID_LENGTH                                                                                                             \
                                         ( _oidname##_oid ),                                                                                                    \
                                         HANDLER_CAN_RONLY));                                                                                           \
   }

#define _SETUP_OID_RW( _oidname, ... )                                                                                                          \
    static oid _oidname##_oid [] = { __VA_ARGS__ };                                                                             \
   {                                                                                                                                                                            \
         netsnmp_register_instance(netsnmp_create_handler_registration                                          \
                                        ("#_oidname",                                                                                                             \
                                         do_##_oidname,                                                                                                         \
                                         _oidname##_oid,                                                                                                        \
                                         OID_LENGTH                                                                                                             \
                                         ( _oidname##_oid ),                                                                                                    \
                                         HANDLER_CAN_RWRITE));                                                                                          \
   }


#endif
