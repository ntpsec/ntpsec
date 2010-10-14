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
#include <ntp_snmp.h>
#include <ctype.h>
#include <ntp.h>
#include <libntpq.h>

/* general purpose buffer length definition */
#define NTPQ_BUFLEN 2048

char ntpvalue[NTPQ_BUFLEN];


/*****************************************************************************
 *
 * ntpsnmpd_parse_string
 *
 *  This function will parse a given NULL terminated string and cut it
 *  into a fieldname and a value part (using the '=' as the delimiter. 
 *  The fieldname will be converted to uppercase and all whitespace 
 *  characters are removed from it.
 *  The value part is stripped, e.g. all whitespace characters are removed
 *  from the beginning and end of the string.
 *  If the value is started and ended with quotes ("), they will be removed
 *  and everything between the quotes is left untouched (including 
 *  whitespace)
 *  Example:
 *     server host name =   hello world!
 *  will result in a field string "SERVERHOSTNAME" and a value
 *  of "hello world!".
 *     My first Parameter		=		"  is this!    "
  * results in a field string "MYFIRSTPARAMETER" and a value " is this!    "
 ****************************************************************************
 * Parameters:
 *	src		char *	The name of the source string variable
 *				NOTE: must be NULL terminated!
 *	field		char *	The name of the string which takes the
 *				fieldname
 *	fieldsize	size_t	The maximum size of the field name
 *	value		char *	The name of the string which takes the
 *				value part
 *	valuesize	size_t	The maximum size of the value string
 *
 * Returns:
 *	size_t			length of value string 
 ****************************************************************************/

size_t ntpsnmpd_parse_string(char *src, char *field, size_t fieldsize, char *value, size_t valuesize)
{
	char string[2048];
	int i = 0;
	int j = 0;
	int l = 0;
	size_t str_cnt;
	size_t val_cnt;

	strncpy(string, src, min(sizeof(string), valuesize));

	str_cnt = strlen(string);

	/* Parsing the field name */
	for (i = 0; l == 0; i++) {
		if (i >= str_cnt) {
			l=1;
		} else {
			switch (string[i]) {
			case '\t': 	/* Tab */
			case '\n':	/* LF */
			case '\r':	/* CR */
			case ' ':  	/* Space */
				break;

			case '=':
				l=1;
				break;

			default:
				if (j >= fieldsize)
					break;
				if (isalpha(string[i]))
					field[j++] = toupper(string[i]);
				else
					field[j++] = string[i]; 
			}
		}
	}

	field[j] = '\0';
	j = 0; 
	value[0] = '\0';


	/* Now parsing the value */
	for (l = 0; i < str_cnt; i++) {
		if (string[i] > 0x0D && string[i] != ' ')
			l = j + 1;
		
		if (value[0] != 0 ||
		    (string[i] > 0x0D && string[i] != ' ')) {
			if (j < valuesize)
				value[j++] = string[i];
		}
	}

	value[l] = '\0';

	if (value[0] == '"')
		strncpy(value, &value[1], valuesize);

	val_cnt = strlen(value);
	if (value[val_cnt - 1] == '"') {
		val_cnt--;
		value[val_cnt] = '\0';
	}

	return (val_cnt);
}


/*****************************************************************************
 *
 * ntpsnmpd_cut_string
 *
 *  This function will parse a given NULL terminated string and cut it
 *  into fields using the specified delimiter character. 
 *  It will then copy the requested field into a destination buffer
 *  Example:
 *     ntpsnmpd_cut_string(read:my:lips:fool, RESULT, ':', 2, sizeof(RESULT))
 *  will copy "lips" to RESULT.
 ****************************************************************************
 * Parameters:
 *	src			char*	The name of the source string variable
 *						NOTE: must be NULL terminated!
 *	dest			char*	The name of the string which takes the
 *						requested field content
 * 	delim			char	The delimiter character
 *	fieldnumber		int		The number of the required field
 *						(start counting with 0)
 *	maxsize			int		The maximum size of dest
 *
 * Returns:
 *	int			length of resulting dest string 
 ****************************************************************************/

size_t ntpsnmpd_cut_string(char *src, char *dest, const char delim, int fieldnumber, size_t maxsize)
{
	char string[2048];
	size_t i;
	size_t j;
	int l;
	size_t a;

	strncpy(string, src, sizeof(string));
	a = strlen(string);
	j = 0;
	memset(dest, 0, maxsize);

	/* Parsing the field name */
	for (i = 0, l = 0; l <= fieldnumber; i++) {
		if (i >= a)
			break;	/* terminate loop */
		if (string[i] == delim)
			l++;	/* next field */
		else if (l == fieldnumber && j < maxsize)
			dest[j++] = string[i]; 
	}

	return j;
}


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

size_t
read_ntp_value(
	char *	variable,
	char *	rbuffer,
	size_t	maxlength)
{
	size_t	sv_len;
	char	sv_data[NTPQ_BUFLEN];
	
	memset(sv_data, 0, sizeof(sv_data));
	sv_len = ntpq_read_sysvars(sv_data, sizeof(sv_data));

	if (0 == sv_len)
		return 0;
	else
		return ntpq_getvar(sv_data, sv_len, variable, rbuffer,
				   maxlength);
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
    else  if ( read_ntp_value("version", ntpvalue, NTPQ_BUFLEN) )
    {
	ntpsnmpd_cut_string(ntpvalue, ntp_softwarename, ' ', 0, sizeof(ntp_softwarename)-1);
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
 */

void
init_ntpSnmpSubagentObject(void)
{
	/* Register all MIB objects with the agentx master */
	NTP_OID_RO( ntpEntSoftwareName,		1, 1, 1, 0);
	NTP_OID_RO( ntpEntSoftwareVersion,	1, 1, 2, 0);
	NTP_OID_RO( ntpEntSoftwareVendor,	1, 1, 3, 0);
	NTP_OID_RO( ntpEntSystemType,		1, 1, 4, 0);
	NTP_OID_RO( ntpEntTimeResolution,	1, 1, 5, 0);
	NTP_OID_RO( ntpEntTimePrecision,	1, 1, 6, 0);
	NTP_OID_RO( ntpEntTimeDistance,		1, 1, 7, 0);
}

