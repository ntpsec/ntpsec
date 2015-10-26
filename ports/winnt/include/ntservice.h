/*
 * Copyright (C) 1999-2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_NTSERVICE_H
#define GUARD_NTSERVICE_H

#define NTP_DISPLAY_NAME "NetworkTimeProtocol"
#define NTP_SERVICE_NAME "ntpd"

void ntservice_init(void);
void UpdateSCM(DWORD);
void WINAPI ServiceControl(DWORD dwCtrlCode);
void ntservice_shutdown(void);
BOOL ntservice_systemisshuttingdown(void);
BOOL WINAPI OnConsoleEvent(DWORD dwCtrlType);

#endif	/* GUARD_NTSERVICE_H */
