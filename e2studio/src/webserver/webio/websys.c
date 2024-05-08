/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/* websys.c
 *
 * Part of the Webio Open Source lightweight web server.
 *
 * Copyright (c) 2007 by John Bartas
 * Portions copyright Renesas Electronics Corporation
 * All rights reserved.
 *
 * Use license: Modified from standard BSD license.
 * 
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation, advertising 
 * materials, Web server pages, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by John Bartas. The name "John Bartas" may not be used to 
 * endorse or promote products derived from this software without 
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include <fcntl.h>
#include <unistd.h>

#include "bsp_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "queue.h"

#include "websys.h"     /* port dependant system files */
#include "webio.h"
#include <ctype.h>
//#include "command.h"

/* This file contains the routines which change from OS to OS 
 * These are:
 *
 * wi_getdate - to format a string with the time and date
 * strnicmp - case insensitive string compare with length parameter
 * stricmp - case insensitive string compare
 * cticks - Function to return the tick count
 */

static char gpszDate[64] = "00";

/*****************************************************************************
Function Name: wi_getdate
Description:   Function to format a date string
Arguments:     IN  sess - Pointer to the web interface session data
Return value:  Pointer to a 
*****************************************************************************/
char * wi_getdate(wi_sess * sess)
{
    (void)sess;

    return gpszDate;
}
/*****************************************************************************
End of function  wi_getdate
******************************************************************************/

/*****************************************************************************
Function Name: pathCompare
Description:   Function to perform a slash and case insensitive string compare
Arguments:     IN  pszS1 - Pointer to the first string
               IN  pszS2 - Pointer to the second string
               IN  stLengthS2 - The length of S1
Return value:  If the two strings are equivalent the return is zero.
*****************************************************************************/
int pathCompare(const char   *pszS1, const char   *pszS2)
{
    /* Drop the first slash at the front of either string */
    if ((*pszS1 == '\\')
    ||  (*pszS1 == '/'))
    {
        pszS1++;
    }
    if ((*pszS2 == '\\')
    ||  (*pszS2 == '/'))
    {
        pszS2++;
    }

    /* Until the end of either sting */
    while ((*pszS1) && (*pszS2))
    {
        /* Do a case insensitive compare */
        if (((*pszS1) | 0x20) != ((*pszS2) | 0x20))
        {
            if ((((*pszS1) == '\\') || ((*pszS1) == '/'))
            &&  (((*pszS2) == '\\') || ((*pszS2) == '/')))
            {
                /* Only different by the slash is OK */
            }
            else
            {
                break;
            }
        }

        /* Bump the pointers */
        pszS1++;
        pszS2++;

        /* Both strings must be the same length */
        if (('\0' == *pszS1)
        &&  ('\0' == *pszS2))
        {
            return 0;
        }
    }
    return 1;
}
/*****************************************************************************
End of function  pathCompare
******************************************************************************/

/*****************************************************************************
Function Name: cticks
Description:   Function to get the current system "tick"
Arguments:     none
Return value:  The number of mS since the timer was opened
*****************************************************************************/
uint32_t cticks(void)
{
    uint32_t    ulClockTicks = 0UL;
    int         iClockTicks = 0;


    iClockTicks = (int)(xTaskGetTickCount());

    if (iClockTicks >= 0 )
    {
    	ulClockTicks = (uint32_t)iClockTicks;
    }

    return ulClockTicks;
}
/*****************************************************************************
End of function  cticks
******************************************************************************/

