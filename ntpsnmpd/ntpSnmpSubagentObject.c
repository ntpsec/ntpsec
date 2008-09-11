/*****************************************************************************
 *
 *  ntpSnmpSubAgentObject.c
 *
 *  This file provides the callback functions for net-snmp and registers the 
 *  serviced MIB objects with the master agent.
 * 
 *  Each object has its own callback function that is called by the 
 *  master agent process whenever someone queries the corresponding MIB
 *  object. 
 * 
 *  At the moment this triggers a full send/receive procedure for each
 *  queried MIB object, one of the things that are still on my todo list:
 *  a caching mechanism that reduces the number of requests sent to the
 *  ntpd process.
 *
 ****************************************************************************/
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "ntpSnmpSubagentObject.h"
#include <libntpq.h>

/* general purpose buffer length definition */
#define NTPQ_BUFLEN 2048

static int      ntpSnmpSubagentObject = 3;


char ntpvalue[NTPQ_BUFLEN];

/*****************************************************************************
 *
 *  read_ntp_value
 *
 *  This function retrieves the value for a given variable, currently
 *  this only supports sysvars. It starts a full mode 6 send/receive/parse
 *  iteration and needs to be optimized, e.g. by using a caching mechanism
 *  
 ****************************************************************************
 * Parameters:
 *	variable	char*	The name of the required variable
 *	rbuffer		char*	The buffer where the value goes
 *	maxlength	int	Max. number of bytes for resultbuf
 *
 * Returns:
 *	u_int		number of chars that have been copied to 
 *			rbuffer 
 ****************************************************************************/

unsigned int read_ntp_value(char *variable, char *rbuffer, unsigned int maxlength)
{
	unsigned int i, sv_len = 0;
	char sv_data[NTPQ_BUFLEN];
	
	memset (sv_data,0, NTPQ_BUFLEN);
	sv_len= ntpq_read_sysvars ( sv_data, NTPQ_BUFLEN );
		
	if ( sv_len )
	{
		i=ntpq_getvar( sv_data, sv_len , variable, rbuffer, maxlength);
		return i;
	} else {
		return 0;
	}

}


/*****************************************************************************
 *
 *  The get_xxx functions
 *
 *  The following function calls are callback functions that will be 
 *  used by the master agent process to retrieve a value for a requested 
 *  MIB object. 
 *
 ****************************************************************************/


int get_ntpEntSoftwareName (netsnmp_mib_handler *handler,
                               netsnmp_handler_registration *reginfo,
                               netsnmp_agent_request_info *reqinfo,
                               netsnmp_request_info *requests)
{
 char ntp_softwarename[NTPQ_BUFLEN];
	
   memset (ntp_softwarename, 0, NTPQ_BUFLEN);
	
   switch (reqinfo->mode) {
   case MODE_GET:
   {
	if ( read_ntp_value("product", ntpvalue, NTPQ_BUFLEN) )
    {
	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                             (u_char *)ntpvalue,
                             strlen(ntpvalue)
                            );
    }
    break;

    if ( read_ntp_value("version", ntpvalue, NTPQ_BUFLEN) )
    {
	ntpq_parsestring(ntp_softwarename, ntpvalue, strlen(ntpvalue), NTPQ_BUFLEN, 'd', 1);
	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                             (u_char *)ntp_softwarename,
                             strlen(ntp_softwarename)
                            );
    } else {
	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                             (u_char *)"N/A",
                             3
                            );
    }
    break;
    
  }


  default:
	  /* If we cannot get the information we need, we will return a generic error to the SNMP client */
        return SNMP_ERR_GENERR;
  }

  return SNMP_ERR_NOERROR;
}


int get_ntpEntSoftwareVersion (netsnmp_mib_handler *handler,
                               netsnmp_handler_registration *reginfo,
                               netsnmp_agent_request_info *reqinfo,
                               netsnmp_request_info *requests)
{

   switch (reqinfo->mode) {
   case MODE_GET:
   {
    
    if ( read_ntp_value("version", ntpvalue, NTPQ_BUFLEN) )
    {
	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                             (u_char *)ntpvalue,
                             strlen(ntpvalue)
                            );
    } else {
	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                             (u_char *)"N/A",
                             3
                            );
    }
    break;
    
  }


  default:
	  /* If we cannot get the information we need, we will return a generic error to the SNMP client */
        return SNMP_ERR_GENERR;
  }

  return SNMP_ERR_NOERROR;
}


int get_ntpEntSoftwareVersionVal (netsnmp_mib_handler *handler,
                               netsnmp_handler_registration *reginfo,
                               netsnmp_agent_request_info *reqinfo,
                               netsnmp_request_info *requests)
{

    int i = 0;
   switch (reqinfo->mode) {
   case MODE_GET:
   {
    
    if ( read_ntp_value("versionval", ntpvalue, NTPQ_BUFLEN) )
    {
	i=atoi(ntpvalue);
	snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                             (u_char *) &i,
                             sizeof (i)
                            );
    } else {
	i = 0;
	snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                             (u_char *) &i,
                             sizeof(i)
                            );
    }
    break;
    
  }


  default:
	  /* If we cannot get the information we need, we will return a generic error to the SNMP client */
        return SNMP_ERR_GENERR;
  }

  return SNMP_ERR_NOERROR;
}



int get_ntpEntSoftwareVendor (netsnmp_mib_handler *handler,
                               netsnmp_handler_registration *reginfo,
                               netsnmp_agent_request_info *reqinfo,
                               netsnmp_request_info *requests)
{

   switch (reqinfo->mode) {
   case MODE_GET:
   {
    
    if ( read_ntp_value("vendor", ntpvalue, NTPQ_BUFLEN) )
    {
	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                             (u_char *)ntpvalue,
                             strlen(ntpvalue)
                            );
    } else {
	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                             (u_char *)"N/A",
                             3
                            );
    }
    break;

  default:
	  /* If we cannot get the information we need, we will return a generic error to the SNMP client */
        return SNMP_ERR_GENERR;
   }
  }
  return SNMP_ERR_NOERROR;
}


int get_ntpEntSystemType (netsnmp_mib_handler *handler,
                               netsnmp_handler_registration *reginfo,
                               netsnmp_agent_request_info *reqinfo,
                               netsnmp_request_info *requests)
{

   switch (reqinfo->mode) {
   case MODE_GET:
   {
    
    if ( read_ntp_value("systemtype", ntpvalue, NTPQ_BUFLEN) )
    {
	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                             (u_char *)ntpvalue,
                             strlen(ntpvalue)
                            );
    }
	   
    if ( read_ntp_value("system", ntpvalue, NTPQ_BUFLEN) )
    {
	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                             (u_char *)ntpvalue,
                             strlen(ntpvalue)
                            );
    } else {
	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                             (u_char *)"N/A",
                             3
                            );
    }
    break;
    
  }


  default:
	  /* If we cannot get the information we need, we will return a generic error to the SNMP client */
        return SNMP_ERR_GENERR;
  }

  return SNMP_ERR_NOERROR;
}

int get_ntpEntTimeResolution (netsnmp_mib_handler *handler,
                               netsnmp_handler_registration *reginfo,
                               netsnmp_agent_request_info *reqinfo,
                               netsnmp_request_info *requests)
{

   switch (reqinfo->mode) {
   case MODE_GET:
   {
    
    if ( read_ntp_value("resolution", ntpvalue, NTPQ_BUFLEN) )
    {
	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                             (u_char *)ntpvalue,
                             strlen(ntpvalue)
                            );
    } else {
	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                             (u_char *)"N/A",
                             3
                            );
    }
    break;
    
  }


  default:
	  /* If we cannot get the information we need, we will return a generic error to the SNMP client */
        return SNMP_ERR_GENERR;
  }

  return SNMP_ERR_NOERROR;
}


int get_ntpEntTimeResolutionVal (netsnmp_mib_handler *handler,
                               netsnmp_handler_registration *reginfo,
                               netsnmp_agent_request_info *reqinfo,
                               netsnmp_request_info *requests)
{

    int i = 0;
   switch (reqinfo->mode) {
   case MODE_GET:
   {
    
    if ( read_ntp_value("resolutionval", ntpvalue, NTPQ_BUFLEN) )
    {
	i=atoi(ntpvalue);
	snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                             (u_char *) &i,
                             sizeof (i)
                            );
    } else {
	i = 0;
	snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                             (u_char *) &i,
                             sizeof(i)
                            );
    }
    break;
    
  }


  default:
	  /* If we cannot get the information we need, we will return a generic error to the SNMP client */
        return SNMP_ERR_GENERR;
  }

  return SNMP_ERR_NOERROR;
}


int get_ntpEntTimePrecision (netsnmp_mib_handler *handler,
                               netsnmp_handler_registration *reginfo,
                               netsnmp_agent_request_info *reqinfo,
                               netsnmp_request_info *requests)
{
   switch (reqinfo->mode) {
   case MODE_GET:
   {
    
    if ( read_ntp_value("precision", ntpvalue, NTPQ_BUFLEN) )
    {
	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                             (u_char *)ntpvalue,
                             strlen(ntpvalue)
                            );
    } else {
	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                             (u_char *)"N/A",
                             3
                            );
    }
    break;
    
  }


  default:
	  /* If we cannot get the information we need, we will return a generic error to the SNMP client */
        return SNMP_ERR_GENERR;
  }

  return SNMP_ERR_NOERROR;
}

int get_ntpEntTimePrecisionVal (netsnmp_mib_handler *handler,
                               netsnmp_handler_registration *reginfo,
                               netsnmp_agent_request_info *reqinfo,
                               netsnmp_request_info *requests)
{

    int i = 0;
   switch (reqinfo->mode) {
   case MODE_GET:
   {
    
    if ( read_ntp_value("precision", ntpvalue, NTPQ_BUFLEN) )
    {
	i=atoi(ntpvalue);
	snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                             (u_char *) &i,
                             sizeof (i)
                            );
    } else {
	i = 0;
	snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                             (u_char *) &i,
                             sizeof(i)
                            );
    }
    break;
    
  }


  default:
	  /* If we cannot get the information we need, we will return a generic error to the SNMP client */
        return SNMP_ERR_GENERR;
  }

  return SNMP_ERR_NOERROR;
}



int get_ntpEntTimeDistance (netsnmp_mib_handler *handler,
                               netsnmp_handler_registration *reginfo,
                               netsnmp_agent_request_info *reqinfo,
                               netsnmp_request_info *requests)
{
   switch (reqinfo->mode) {
   case MODE_GET:
   {
    
    if ( read_ntp_value("rootdelay", ntpvalue, NTPQ_BUFLEN) )
    {
	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                             (u_char *)ntpvalue,
                             strlen(ntpvalue)
                            );
    } else {
	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                             (u_char *)"N/A",
                             3
                            );
    }
    break;
    
  }


  default:
	  /* If we cannot get the information we need, we will return a generic error to the SNMP client */
        return SNMP_ERR_GENERR;
  }

  return SNMP_ERR_NOERROR;
}


/*
 *
 * Initialize sub agent
 * TODO: Define NTP MIB OID (has to be assigned by IANA)
 * At the moment we use a private MIB branch (enterprises.5597.99)
 */

void
init_ntpSnmpSubagentObject(void)
{
	
    /* Register all MIB objects with the agentx master */
	
  _SETUP_OID_RO( ntpEntSoftwareName ,  		NTPV4_OID , 1, 1, 0  );
  _SETUP_OID_RO( ntpEntSoftwareVersion ,  	NTPV4_OID , 1, 2, 0  );
  _SETUP_OID_RO( ntpEntSoftwareVersionVal ,	NTPV4_OID , 1, 3, 0  );
  _SETUP_OID_RO( ntpEntSoftwareVendor ,  	NTPV4_OID , 1, 4, 0  );
  _SETUP_OID_RO( ntpEntSystemType ,  		NTPV4_OID , 1, 5, 0  );
  _SETUP_OID_RO( ntpEntTimeResolution ,  	NTPV4_OID , 1, 6, 0  );
  _SETUP_OID_RO( ntpEntTimeResolutionVal , 	NTPV4_OID , 1, 7, 0  );
  _SETUP_OID_RO( ntpEntTimePrecision ,  	NTPV4_OID , 1, 8, 0  );
  _SETUP_OID_RO( ntpEntTimePrecisionVal ,  	NTPV4_OID , 1, 9, 0  );
  _SETUP_OID_RO( ntpEntTimeDistance ,  		NTPV4_OID , 1,10, 0  );
	
}

