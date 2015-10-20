/*
 * Copyright (C) 2004, 2007  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

/* Principal Authors: DCL */

#include <config.h>

#include <windows.h>

#include <isc/once.h>
#include <isc/assertions.h>
#include <isc/util.h>

isc_result_t
isc_once_do(isc_once_t *controller, void(*function)(void)) {
	REQUIRE(controller != NULL && function != NULL);

	if (controller->status == ISC_ONCE_INIT_NEEDED) {

		if (InterlockedDecrement(&controller->counter) == 0) {
			if (controller->status == ISC_ONCE_INIT_NEEDED) {
				function();
				controller->status = ISC_ONCE_INIT_DONE;
			}
		} else {
			while (controller->status == ISC_ONCE_INIT_NEEDED) {
				/*
				 * Sleep(0) indicates that this thread 
				 * should be suspended to allow other 
				 * waiting threads to execute.
				 */
				Sleep(0);
			}
		}
	}

	return (ISC_R_SUCCESS);
}
