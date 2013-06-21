 /*************************************************************************//**
 * @file glib_polygon.c
 * @brief Energy Micro Graphics Library: Polygon Drawing Routines
 * @author Energy Micro AS
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2012 Energy Micro AS, http://www.energymicro.com</b>
 ******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 4. The source and compiled code may only be used on Energy Micro "EFM32"
 *    microcontrollers and "EFR4" radios.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 *****************************************************************************/

/* C-header files */
#include <stdint.h>

/* EM types */
#include "em_types.h"

/* GLIB header files */
#include "glib.h"

/**************************************************************************//**
*  @brief
*  Draws a polygon using Bresnham's Midpoint Line Algorithm.
*
*  This function draws a line between all points outlining the polygon.
*  The first and last point doesn't have to be the same. The function automatically
*  draws a line from the start point to the end point.
*
*  @param pContext
*  Pointer to a GLIB_Context which the line is drawn. The lines are drawn using the
*  foreground color.
*  @param numPoints
*  Number of points in the polygon ( Has to be greater than 1 )
*  @param polyPoints
*  Pointer to array of polygon points. The points are laid out like this: polyPoints = {x1,y1,x2,y2 ... }
*  Polypoints has to contain at least (numPoints * 2) entries
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/

EMSTATUS GLIB_drawPolygon(const GLIB_Context *pContext,
                          uint32_t numPoints, uint16_t *polyPoints)
{
  /* Check arguments */
  if (pContext == NULL || polyPoints == NULL || numPoints < 2)
    return GLIB_INVALID_ARGUMENT;

  EMSTATUS status;
  uint32_t didDraw = 0;

  uint32_t counter;
  uint16_t firstX;
  uint16_t firstY;
  uint16_t startX;
  uint16_t startY;
  uint16_t endX;
  uint16_t endY;

  startX = *polyPoints++;
  startY = *polyPoints++;
  firstX = startX;
  firstY = startY;

  /* Loop through the points by moving the pointer */
  for (counter = 1; counter < numPoints; counter++)
  {
    endX = *polyPoints++;
    endY = *polyPoints++;

    /* Draw a line between each pair of points */
    status = GLIB_drawLine(pContext, startX, startY, endX, endY);
    if (status != 0 && status != GLIB_DID_NOT_DRAW) return status;
    if (status == 0) didDraw = 1;

    startX = endX;
    startY = endY;
  }

  /* Draw a line from last point to first point */
  if ((endX != firstX) || (endY != firstY))
  {
    status = GLIB_drawLine(pContext, firstX, firstY, endX, endY);
    if (status != 0 && status != GLIB_DID_NOT_DRAW) return status;
    if (status == 0) didDraw = 1;
  }

  if (didDraw == 0) return GLIB_DID_NOT_DRAW;
  return GLIB_OK;
}
