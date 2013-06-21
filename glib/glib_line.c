 /*************************************************************************//**
 * @file glib_line.c
 * @brief Energy Micro Graphics Library: Line Drawing Routines
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

/* Standard C header files */
#include <stdint.h>

/* EM types */
#include "em_types.h"

/* GLIB Header files */
#include "glib.h"

/* Local function prototypes */
static uint8_t GLIB_getClipCode(const GLIB_Context *pContext, uint16_t x, uint16_t y);
static uint32_t GLIB_clipLine(const GLIB_Context *pContext, uint16_t *pX1, uint16_t *pY1,
                              uint16_t *pX2, uint16_t *pY2);

/**************************************************************************//**
*  @brief
*  Draws a horizontal line from x1, y1 to x2, y2
*
*  @param pContext
*  Pointer to a GLIB_Context in which the line is drawn. The line is drawn using the
*  foreground color.
*
*  @param x1
*  Start x-coordinate
*  @param y1
*  Start y-coordinate
*  @param x2
*  End x-coordinate
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_drawLineH(const GLIB_Context *pContext, uint16_t x1, uint16_t y1,
                        uint16_t x2)
{
  /* Check arguments */
  if (pContext == NULL)
    return GLIB_INVALID_ARGUMENT;

  /* Check if line is outside of clipping region */
  if ((y1 < pContext->clippingRegion.yMin) || (y1 > pContext->clippingRegion.yMax))
  {
    return GLIB_DID_NOT_DRAW;
  }

  EMSTATUS status;

  /* Reset clipping area in DMD driver */
  status = GLIB_resetDisplayClippingArea(pContext);
  if (status != 0) return status;

  /* Swap the coordinates if x1 is larger than x2 */
  if (x1 > x2)
  {
    int16_t swap;
    swap = x1;
    x1   = x2;
    x2   = swap;
  }

  /* Check if entire line is outside clipping region */
  if ((x1 > pContext->clippingRegion.xMax) || (x2 < pContext->clippingRegion.xMin))
  {
    /* Nothing to draw */
    return GLIB_DID_NOT_DRAW;
  }

  /* Clip the line if necessary */
  if (x1 < pContext->clippingRegion.xMin)
  {
    x1 = pContext->clippingRegion.xMin;
  }

  if (x2 > pContext->clippingRegion.xMax)
  {
    x2 = pContext->clippingRegion.xMax;
  }

  /* Draw line using display driver */
  uint8_t  red;
  uint8_t  green;
  uint8_t  blue;
  uint32_t length = x2 - x1 + 1;
  GLIB_colorTranslate24bpp(pContext->foregroundColor, &red, &green, &blue);

  return DMD_writeColor(x1, y1, red, green, blue, length);
}

/**************************************************************************//**
*  @brief
*  Draws a vertical line from x1, y1 to x1, y2
*
*  @param pContext
*  Pointer to a GLIB_Context which the line is drawn. The line is drawn using the
*  foreground color.
*  @param x1
*  Start x-coordinate
*  @param y1
*  Start y-coordinate
*  @param y2
*  End y-coordinate
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/

EMSTATUS GLIB_drawLineV(const GLIB_Context *pContext, uint16_t x1, uint16_t y1,
                        uint16_t y2)
{
  /* Check arguments */
  if (pContext == NULL)
    return GLIB_INVALID_ARGUMENT;

  /* Check if line is outside of clipping region */
  if ((x1 < pContext->clippingRegion.xMin) || (x1 > pContext->clippingRegion.xMax))
  {
    return GLIB_DID_NOT_DRAW;
  }

  EMSTATUS status;

  /* Swap the coordinates if y1 is larger than y2 */
  if (y1 > y2)
  {
    uint16_t swap;
    swap = y1;
    y1   = y2;
    y2   = swap;
  }

  /* Check if entire line is outside clipping region */
  if ((y1 > pContext->clippingRegion.yMax) || (y2 < pContext->clippingRegion.yMin))
  {
    /* Nothing to draw */
    return GLIB_DID_NOT_DRAW;
  }

  /* Clip the line if necessary */
  if (y1 < pContext->clippingRegion.yMin)
  {
    y1 = pContext->clippingRegion.yMin;
  }

  if (y2 > pContext->clippingRegion.yMax)
  {
    y2 = pContext->clippingRegion.yMax;
  }

#if 0
  /* Draw line using a for loop */
  uint16_t y;
  for (y = y1; y < y2; y++)
  {
    status = GLIB_drawPixel(pContext, x1, y);
    if (status != 0) return status;
  }

#else
  uint16_t length = y2 - y1 + 1;
  status = DMD_setClippingArea(x1, y1, 1, length);
  if (status != DMD_OK) return status;

  uint8_t red;
  uint8_t green;
  uint8_t blue;

  GLIB_colorTranslate24bpp(pContext->foregroundColor, &red, &green, &blue);

  status = DMD_writeColor(0, 0, red, green, blue, length);
  if (status != DMD_OK) return status;

  status = GLIB_resetDisplayClippingArea(pContext);
  if (status != GLIB_OK) return status;
#endif


  return GLIB_OK;
}

/**************************************************************************//**
*  @brief
*  Gets the clip code for the point that is used by Cohen-Sutherland algorithm
*
*  @param pContext
*  Pointer to the GLIB_Context which holds the clipping region
*  @param x
*  Start x-coordinate
*  @param y
*  Start y-coordinate
*
*  @return
*  Returns the 4-bit clip code in a uint8_t
*
******************************************************************************/

static uint8_t GLIB_getClipCode(const GLIB_Context *pContext, uint16_t x, uint16_t y)
{
  uint8_t code;

  /* Set the clipping code to zero */
  code = 0;

  /* Check if the point is to the left of the clipping region */
  if (x < pContext->clippingRegion.xMin)
  {
    code |= 1;
  }
  /* Check if the point is to the right of the clipping region */
  if (x > pContext->clippingRegion.xMax)
  {
    code |= 2;
  }
  /* Check if the point is below the clipping region */
  if (y > pContext->clippingRegion.yMax)
  {
    code |= 4;
  }
  /* Check if point is above clipping region */
  if (y < pContext->clippingRegion.yMin)
  {
    code |= 8;
  }

  return code;
}

/**************************************************************************//**
*  @brief
*  Clips the line if necessary, using the Cohen-Sutherland algorithm for clipping
*  lines. See Wikipedia for algorithm.
*
*  The points are moved using the linear equations:
*  y=y0+slope*(x-x0),x=x0+(1/slope)*(y-y0)
*
*  @param pContext
*  Pointer to the GLIB_Context which holds the clipping region
*  @param pX1
*  Pointer to the start x-coordinate
*  @param pY1
*  Pointer to the start y-coordinate
*  @param pX2
*  Pointer to the end-x coordinate
*  @param pY2
*  Pointer to the end-y coordinate
*
*  @return
*  Returns 1 if the line is inside the clipping region, otherwise return 0
******************************************************************************/
static uint32_t GLIB_clipLine(const GLIB_Context *pContext, uint16_t *pX1,
                              uint16_t *pY1, uint16_t *pX2, uint16_t *pY2)
{
  uint8_t  currentCode, code1, code2;
  uint16_t x=0, y=0;

  /* Compute the clipping code for the two points */
  code1 = GLIB_getClipCode(pContext, *pX1, *pY1);
  code2 = GLIB_getClipCode(pContext, *pX2, *pY2);

  while (1)
  {
    /* Case 1: Check if the points is inside the clipping rectangle */
    if ((code1 | code2) == 0) return 1;

    /* Case 2: Check if the points can be trivially rejected */
    if (code1 & code2) return 0;

    /* Case 3: Move the points so they can be either trivially accepted or rejected */
    /* Choose one of the points that are outside of the clipping region */
    if (code1) currentCode = code1;
    else currentCode = code2;

    /* Check if currentCode is to the left of the clipping region */
    if (currentCode & 1)
    {
      /* Move the point to the left edge of the clipping region */
      y = *pY1 + ((*pY2 - *pY1) * (pContext->clippingRegion.xMin - *pX1)) / (*pX2 - *pX1);
      x = pContext->clippingRegion.xMin;
    }

    /* Check if currentCode is to the right of the clipping region */
    else if (currentCode & 2)
    {
      /* Move the point to the right edge of the clipping region */
      x = pContext->clippingRegion.xMax;
      y = *pY1 + ((*pY2 - *pY1) * (pContext->clippingRegion.xMax - *pX1)) / (*pX2 - *pX1);
    }

    /* Check if currentCode is below the clipping region */
    else if (currentCode & 4)
    {
      /* Move the point to the bottom of the clipping region */
      y = pContext->clippingRegion.yMax;
      x = *pX1 + ((*pX2 - *pX1) * (pContext->clippingRegion.yMax - *pY1)) / (*pY2 - *pY1);
    }

    /* Check if currentCode is above the clipping region */
    else if (currentCode & 8)
    {
      /* Move the point to the top of the clipping region */
      y = pContext->clippingRegion.yMin;
      x = *pX1 + ((*pX2 - *pX1) * (pContext->clippingRegion.yMin - *pY1)) / (*pY2 - *pY1);
    }

    /* Determine which point is moved and set the new coordinates */
    if (code1)
    {
      *pX1 = x;
      *pY1 = y;
      /* Compute new clipCode */
      code1 = GLIB_getClipCode(pContext, x, y);
    }
    else
    {
      *pX2 = x;
      *pY2 = y;
      /* Compute new clipCode */
      code2 = GLIB_getClipCode(pContext, x, y);
    }
  }
}

/**************************************************************************//**
*  @brief
*  Draws a line from x1,y1 to x2, y2
*
*  Draws a straight line using the Bresnham's Midpoint Line Algorithm.
*  Checks for vertical and horizontal line.
*
*  @param x1
*  Start x-coordinate
*  @param y1
*  Start y-coordinate
*  @param x2
*  End x-coordinate
*  @param y2
*  End y-coordinate
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/

EMSTATUS GLIB_drawLine(const GLIB_Context *pContext, uint16_t x1, uint16_t y1,
                       uint16_t x2, uint16_t y2)
{
  /* Check arguments */
  if (pContext == NULL)
    return GLIB_INVALID_ARGUMENT;

  EMSTATUS status;

  /* Check if the line is vertical */
  if (x1 == x2)
  {
    /* Use algorithm for vertical line */
    return GLIB_drawLineV(pContext, x1, y1, y2);
  }

  /* Check if the line is horizontal */
  if (y1 == y2)
  {
    /* Use algorithm for horizontal line */
    return GLIB_drawLineH(pContext, x1, y1, x2);
  }

  /* Variables that help drawing the line using only integer arithmetic */
  int16_t error;
  int16_t deltaX;
  int16_t deltaY;

  /* Y-direction for line */
  int16_t yStep = 1;

  /* Holds whether the line is steep */
  uint32_t steep = 0;

  /* Reset clipping area in DMD driver */
  status = GLIB_resetDisplayClippingArea(pContext);
  if (status != 0) return status;

  /* Clip the line against the clipping region */
  if (GLIB_clipLine(pContext, &x1, &y1, &x2, &y2) == 0)
  {
    return GLIB_DID_NOT_DRAW;
  }

  /* Determine if steep or not steep
   * (Steep means more motion in Y-direction than X-direction) */
  uint16_t yMotion = (y2 > y1) ? (y2 - y1) : (y1 - y2);
  uint16_t xMotion = (x2 > x1) ? (x2 - x1) : (x1 - x2);
  if (yMotion > xMotion)
  {
    /* If line is steep, swap x and y values */
    steep = 1;

    error = x1;
    x1    = y1;
    y1    = error;

    error = x2;
    x2    = y2;
    y2    = error;
  }

  /* Place the leftmost point in x1, y1 */
  if (x2 < x1)
  {
    /* Swap x-values */
    error = x1;
    x1    = x2;
    x2    = error;

    /* Swap y-values */
    error = y1;
    y1    = y2;
    y2    = error;
  }

  /* Compute the differences between the points */
  deltaX = x2 - x1;
  deltaY = (y2 > y1) ? (y2 - y1) : (y1 - y2);

  /* Set error to negative half deltaX ? */
  error = -deltaX / 2;

  /* Determine which direction to step in */
  if (y2 < y1) yStep = -1;

  /* Loop through all points along the x-axis */
  for (; x1 <= x2; x1++)
  {
    if (steep == 1)
    {
      /* If steep, swap x and y coordinates */
      status = GLIB_drawPixel(pContext, y1, x1);
    }
    else
    {
      status = GLIB_drawPixel(pContext, x1, y1);
    }

    if (status != 0) return status;

    error += deltaY;

    if (error > 0)
    {
      y1    += yStep;
      error -= deltaX;
    }
  }

  return GLIB_OK;
}
