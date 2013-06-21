 /*************************************************************************//**
 * @file glib_circle.c
 * @brief Energy Micro Graphics Library: Circle Drawing Routines
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

/* GLIB header files */
#include "glib.h"

/* Local function prototypes */
static EMSTATUS GLIB_drawCirclePoints(const GLIB_Context *pContext,
                                      uint16_t xCenter, uint16_t yCenter,
                                      uint16_t x, uint16_t y);

static EMSTATUS GLIB_drawPartialCirclePoints(const GLIB_Context *pContext,
                                             uint16_t xCenter, uint16_t yCenter,
                                             uint16_t x, uint16_t y, uint8_t bitMask);

/**************************************************************************//**
*  @brief
*  Draws a circle with center at x, y, and a radius
*
*  Draws a circle using the Midpoint Circle Algorithm. See Wikipedia for algorithm.
*  Algorithm is optimized to use only integer arithmetic, so no floating
*  point arithmetic is needed.
*
*  @param pContext
*  Pointer to a GLIB_Context in which the circle is drawn. The circle is drawn using
*  the foreground color.
*  @param xCenter
*  Center x-coordinate
*  @param yCenter
*  Center y-coordinate
*  @param radius
*  Radius of the circle
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/

EMSTATUS GLIB_drawCircle(const GLIB_Context *pContext, uint16_t xCenter,
                         uint16_t yCenter, uint16_t radius)
{
  /* Check arguments */
  if (pContext == NULL)
    return GLIB_INVALID_ARGUMENT;

  /* Bounds checking */
  if (GLIB_rectContainsPoint(&(pContext->clippingRegion), xCenter, yCenter) == 0)
    return GLIB_OUT_OF_BOUNDS;

  EMSTATUS status;

  /* Reset clipping area in DMD driver */
  status = GLIB_resetDisplayClippingArea(pContext);
  if (status != 0) return status;

  uint16_t x       = 0;
  uint16_t y       = radius;
  int16_t  d       = 1 - radius;
  uint32_t didDraw = 0;

  /* Draw initial circle points */
  status = GLIB_drawCirclePoints(pContext, xCenter, yCenter, x, y);
  if (status != 0 && status != GLIB_DID_NOT_DRAW) return status;
  if (status == 0) didDraw = 1;

  /* Loops through all points from 0 to 45 degrees of the circle
   * (0 is defined straight upward) */
  while (x < y)
  {
    x++;
    if (d < 0)
    {
      d += 2 * x + 1;
    }
    else
    {
      y--;
      d += 2 * (x - y) + 1;
    }

    /* Draws the circle points using 8-way symmetry */
    status = GLIB_drawCirclePoints(pContext, xCenter, yCenter, x, y);
    if (status != 0 && status != GLIB_DID_NOT_DRAW) return status;
    if (status == 0) didDraw = 1;
  }

  if (didDraw == 0) return GLIB_DID_NOT_DRAW;
  return GLIB_OK;
}

/**************************************************************************//**
*  @brief
*  Draws a partial circle with center at x, y, and a radius
*
*  Draws a partial circle using the Midpoint Circle Algorithm. Algorithm is
*  optimized to use only integer arithmetic, so no floating point arithmetic is
*  needed. The bitMask passed in decides which octant that should be drawn.
*  The octants is numbered 1 to 8 in counterclockwise order.
*
*  Example: bitMask == 4 draws only pixels in 3. octant (00000100).
*  bitMask == 5 draws only pixels in 3. and 1. octant (00000101).
*
*  @param pContext
*  Pointer to a GLIB_Context in which the circle is drawn. The circle is drawn using the foreground color.
*  @param xCenter
*  Center x-coordinate
*  @param yCenter
*  Center y-coordinate
*  @param radius
*  Radius of the circle
*  @param bitMask
*  Bitmask which decides which octants pixels should be drawn.
*  The LSB is 1. octant, and the MSB is 8. octant.
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/

EMSTATUS GLIB_drawPartialCircle(const GLIB_Context *pContext, uint16_t xCenter,
                                uint16_t yCenter, uint16_t radius, uint8_t bitMask)
{
  /* Check arguments */
  if (pContext == NULL)
    return GLIB_INVALID_ARGUMENT;

  /* Bounds checking */
  if (GLIB_rectContainsPoint(&(pContext->clippingRegion), xCenter, yCenter) == 0)
    return GLIB_OUT_OF_BOUNDS;

  EMSTATUS status;

  uint16_t x       = 0;
  uint16_t y       = radius;
  int16_t  d       = 1 - radius;
  uint32_t didDraw = 0;

  /* Draw initial circle points */
  status = GLIB_drawPartialCirclePoints(pContext, xCenter, yCenter, x, y, bitMask);
  if (status != 0 && status != GLIB_DID_NOT_DRAW) return status;
  if (status == 0) didDraw = 1;

  /* Loops through all points from 0 to 45 degrees of the circle
   * (0 is defined straight upward) */
  while (x < y)
  {
    x++;
    if (d < 0)
    {
      d += 2 * x + 1;
    }
    else
    {
      y--;
      d += 2 * (x - y) + 1;
    }

    /* Draws the circle points using 8-way symmetry */
    status = GLIB_drawPartialCirclePoints(pContext, xCenter, yCenter, x, y, bitMask);
    if (status != 0 && status != GLIB_DID_NOT_DRAW) return status;
    if (status == 0) didDraw = 1;
  }

  if (didDraw == 0) return GLIB_DID_NOT_DRAW;
  return GLIB_OK;
}

/**************************************************************************//**
*  @brief
*  Draws a filled circle with center at x, y, and a radius.
*
*  Draws a circle using the Midpoint Circle Algorithm and using horizontal lines.
*  See Wikipedia for algorithm.
*
*  @param pContext
*  Pointer to a GLIB_Context in which the circle is drawn. The circle is drawn using the foreground color.
*  @param xCenter
*  Center x-coordinate
*  @param yCenter
*  Center y-coordinate
*  @param radius
*  Radius of the circle
*
*  @return
*  Returns GLIB_OK on succes.
*  Returns GLIB_DID_NOT_DRAW if none of the points were drawn.
*  Returns error code otherwise.
******************************************************************************/

EMSTATUS GLIB_drawCircleFilled(const GLIB_Context *pContext, uint16_t xCenter,
                               uint16_t yCenter, uint16_t radius)
{
  /* Check arguments */
  if (pContext == NULL)
    return GLIB_INVALID_ARGUMENT;

  /* Bounds checking */
  if (GLIB_rectContainsPoint(&(pContext->clippingRegion), xCenter, yCenter) == 0)
    return GLIB_OUT_OF_BOUNDS;

  EMSTATUS status;

  uint16_t x       = 0;
  uint16_t y       = radius;
  int16_t  d       = 1 - radius;
  uint32_t didDraw = 0;

  /* Draws the initial circle line */
  status = GLIB_drawLineH(pContext, xCenter - y, yCenter, xCenter + y);
  if (status != 0 && status != GLIB_DID_NOT_DRAW) return status;
  if (status == 0) didDraw = 1;

  /* Loops through all points from 0 to 45 degrees of the circle
   * (0 is defined straight upward) */
  while (x < y)
  {
    /* Midpoint Circle algorithm */
    x++;
    if (d < 0)
    {
      d += 2 * x + 1;
    }
    else
    {
      y--;
      d += 2 * (x - y) + 1;
    }

    /* Draws horizontal lines using 4 way symmetry */
    status = GLIB_drawLineH(pContext, xCenter - x, yCenter + y, xCenter + x);
    if (status != 0 && status != GLIB_DID_NOT_DRAW) return status;
    if (!didDraw && status == 0) didDraw = 1;

    status = GLIB_drawLineH(pContext, xCenter - y, yCenter + x, xCenter + y);
    if (status != 0 && status != GLIB_DID_NOT_DRAW) return status;
    if (!didDraw && status == 0) didDraw = 1;

    status = GLIB_drawLineH(pContext, xCenter - x, yCenter - y, xCenter + x);
    if (status != 0 && status != GLIB_DID_NOT_DRAW) return status;
    if (!didDraw && status == 0) didDraw = 1;

    status = GLIB_drawLineH(pContext, xCenter - y, yCenter - x, xCenter + y);
    if (status != 0 && status != GLIB_DID_NOT_DRAW) return status;
    if (!didDraw && status == 0) didDraw = 1;
  }

  if (didDraw == 0) return GLIB_DID_NOT_DRAW;
  return GLIB_OK;
}

/**************************************************************************//**
*  @brief
*  Draws circle points using 8-way symmetry.
*
*  Example: bitMask = 4 draws only pixels in 3. octant (00000100).
*  bitMask = 5 draws only pixels in 3. and 1. octant (00000101).
*
*  @param pContext
*  Pointer to a GLIB_Context in which the circle is drawn. The circle is drawn using the foreground color.
*  @param xCenter
*  Center x-coordinate
*  @param yCenter
*  Center y-coordinate
*  @param x
*  x-coordinate of circle point
*  @param y
*  y-coordinate of circle point
*  @param bitMask
*  Bitmask which decides which octants pixels should be drawn
*  The LSB is 1. octant, and the MSB is 8. octant. The octants are ordered from 1 to 8
*  in counterclockwise order.
*
*  @return
*  - Returns GLIB_OK on succes.
*  - Returns GLIB_DID_NOT_DRAW if none of the points were drawn
*  - Returns error code otherwise
******************************************************************************/
static EMSTATUS GLIB_drawPartialCirclePoints(const GLIB_Context *pContext,
                                             uint16_t xCenter, uint16_t yCenter,
                                             uint16_t x, uint16_t y, uint8_t bitMask)
{
  EMSTATUS status;
  uint32_t didDraw = 0;
  int32_t  i;

  int32_t  xOffsets[8] = { y, x, -x, -y, -y, -x, x, y };
  int32_t  yOffsets[8] = { -x, -y, -y, -x, x, y, y, x };

  /* Draws the circle points using 8-way symmetry */

  for (i = 0; i < 8; ++i)
  {
    /* Pixel in i+1. octant */
    if ((bitMask & 1) != 0)
    {
      status = GLIB_drawPixel(pContext, xCenter + xOffsets[i], yCenter + yOffsets[i]);
      if (status != 0 && status != GLIB_DID_NOT_DRAW) return status;
      if (!didDraw && status == 0) didDraw = 1;
    }

    bitMask >>= 1;
  }

  if (didDraw == 0) return GLIB_DID_NOT_DRAW;
  return GLIB_OK;
}

/**************************************************************************//**
*  @brief
*  Draws circle points using 8-way symmetry
*
*  @param pContext
*  Pointer to a GLIB_Context in which the circle is drawn. The circle is drawn using the foreground color.
*  @param xCenter
*  Center x-coordinate
*  @param yCenter
*  Center y-coordinate
*  @param x
*  x-coordinate of circle point
*  @param y
*  y-coordinate of circle point
*
*  @return
*  - Returns GLIB_OK on succes
*  - Returns GLIB_DID_NOT_DRAW if none of the points were drawn
*  - Returns error code otherwise
******************************************************************************/
static EMSTATUS GLIB_drawCirclePoints(const GLIB_Context *pContext,
                                      uint16_t xCenter, uint16_t yCenter,
                                      uint16_t x, uint16_t y)
{
  EMSTATUS status;
  uint32_t didDraw = 0;
  int32_t  i;

  int32_t  xOffsets[8] = { y, x, -x, -y, -y, -x, x, y };
  int32_t  yOffsets[8] = { -x, -y, -y, -x, x, y, y, x };

  /* Draws the circle points using 8-way symmetry */
  for (i = 0; i < 8; ++i)
  {
    /* Pixel in i+1. octant */
    status = GLIB_drawPixel(pContext, xCenter + xOffsets[i], yCenter + yOffsets[i]);
    if (status != 0 && status != GLIB_DID_NOT_DRAW) return status;
    if (!didDraw && status == 0) didDraw = 1;
  }

  if (didDraw == 0) return GLIB_DID_NOT_DRAW;
  return GLIB_OK;
}


