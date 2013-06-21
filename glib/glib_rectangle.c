 /*************************************************************************//**
 * @file glib_rectangle.c
 * @brief Energy Micro Graphics Library: Rectangle Routines
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

/**************************************************************************//**
*  @brief
*  Checks if the point passed in is in the interior of the rectangle passed in.
*
*  If the point is on the edge of the rectangle the function returns 1.
*
*  @param pRect
*  Pointer to a rectangle structure
*  @param x
*  X-coordinate of point
*  @param y
*  Y-coordinate of point
*  @return
*  - Returns 0 if point is on the outside of rectangle
*  - Returns 1 if point is on the inside of the rectangle
******************************************************************************/

uint32_t GLIB_rectContainsPoint(const GLIB_Rectangle *pRect, uint16_t x, uint16_t y)
{
  /* Check arguments */
  if (pRect == NULL)
    return 0;

  if ((x < pRect->xMin) || (x > pRect->xMax)) return 0;
  if ((y < pRect->yMin) || (y > pRect->yMax)) return 0;
  return 1;
}

/**************************************************************************//**
*  @brief
*  Normalize the rectangle that is passed in.
*  Sets yMin to the minimum value of yMin and yMax.
*  Sets yMax to the maximum value of yMin and yMax.
*  And the same for xMin and xMax
*
*  @param pRect
*  Pointer to a rectangle structure
*
*  @return
*  None.
******************************************************************************/
void GLIB_normalizeRect(GLIB_Rectangle *pRect)
{
  if (pRect->xMin > pRect->xMax)
  {
    uint16_t swap;
    swap        = pRect->xMin;
    pRect->xMin = pRect->xMax;
    pRect->xMax = swap;
  }

  if (pRect->yMin > pRect->yMax)
  {
    uint16_t swap;
    swap        = pRect->yMin;
    pRect->yMin = pRect->yMax;
    pRect->yMax = swap;
  }
}

/**************************************************************************//**
*  @brief
*  Draws a rectangle outline defined by the passed in rectangle
*
*  @param pContext
*  Pointer to a GLIB_Context in which the rectangle is drawn. The rectangle is drawn
*  using the foreground color.
*
*  @param pRect
*  Pointer to a rectangle structure
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_drawRect(const GLIB_Context *pContext,
                       GLIB_Rectangle *pRect)
{
  EMSTATUS status;

  GLIB_normalizeRect(pRect);

  GLIB_Rectangle tmpRectangle;
  tmpRectangle = *pRect;

  /* Clip rectangle if necessary */
  if (tmpRectangle.xMin < pContext->clippingRegion.xMin)
  {
    tmpRectangle.xMin = pContext->clippingRegion.xMin;
  }
  if (tmpRectangle.xMax > pContext->clippingRegion.xMax)
  {
    tmpRectangle.xMax = pContext->clippingRegion.xMax;
  }

  if (tmpRectangle.yMin < pContext->clippingRegion.yMin)
  {
    tmpRectangle.yMin = pContext->clippingRegion.yMin;
  }
  if (tmpRectangle.yMax > pContext->clippingRegion.yMax)
  {
    tmpRectangle.yMax = pContext->clippingRegion.yMax;
  }


  /* Draw a line across the top of the rectangle */
  status = GLIB_drawLineH(pContext, tmpRectangle.xMin, tmpRectangle.yMin, tmpRectangle.xMax);
  if (status != 0) return status;

  /* Check if the rectangle is one pixel tall */
  if (tmpRectangle.yMin == tmpRectangle.yMax)
  {
    return GLIB_OK;
  }

  /* Draw the right side of the rectangle */
  status = GLIB_drawLineV(pContext, tmpRectangle.xMax, tmpRectangle.yMin + 1, tmpRectangle.yMax);
  if (status != 0) return status;

  /* Check if the rectangle is one pixel wide */
  if (tmpRectangle.xMin == tmpRectangle.xMax)
  {
    return GLIB_OK;
  }

  /* Draw a line across the bottom of the rectangle */
  status = GLIB_drawLineH(pContext, tmpRectangle.xMin, tmpRectangle.yMax, tmpRectangle.xMax - 1);
  if (status != 0) return status;

  /* Return if the rectangle is two pixels tall */
  if ((tmpRectangle.yMin + 1) == tmpRectangle.yMax)
  {
    return GLIB_OK;
  }

  /* Draw the left side of the rectangle */
  status = GLIB_drawLineV(pContext, tmpRectangle.xMin, tmpRectangle.yMin + 1, tmpRectangle.yMax - 1);
  if (status != 0) return status;

  return GLIB_OK;
}

/**************************************************************************//**
*  @brief
*  Draws a filled rectangle defined by the passed in rectangle. The filled rectangle
*  is drawn from (xMin, yMin) to (xMax, yMax), inclusive, of the rectangle passed in.
*
*  @param pContext
*  Pointer to a GLIB_Context in which the rectangle is drawn. The rectangle is filled
*  with the foreground color.
*
*  @param pRect
*  Pointer to a rectangle structure
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/

EMSTATUS GLIB_drawRectFilled(const GLIB_Context *pContext,
                             GLIB_Rectangle *pRect)
{
  EMSTATUS status;
  GLIB_normalizeRect(pRect);

  GLIB_Rectangle tmpRectangle;
  tmpRectangle = *pRect;

  /* Clip rectangle if necessary */
  if (tmpRectangle.xMin < pContext->clippingRegion.xMin)
  {
    tmpRectangle.xMin = pContext->clippingRegion.xMin;
  }
  if (tmpRectangle.xMax > pContext->clippingRegion.xMax)
  {
    tmpRectangle.xMax = pContext->clippingRegion.xMax;
  }

  if (tmpRectangle.yMin < pContext->clippingRegion.yMin)
  {
    tmpRectangle.yMin = pContext->clippingRegion.yMin;
  }
  if (tmpRectangle.yMax > pContext->clippingRegion.yMax)
  {
    tmpRectangle.yMax = pContext->clippingRegion.yMax;
  }

  /* Draw filled rectangle */
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  GLIB_colorTranslate24bpp(pContext->foregroundColor, &red, &green, &blue);

  uint16_t width;
  uint16_t height;
  width  = tmpRectangle.xMax - tmpRectangle.xMin + 1;
  height = tmpRectangle.yMax - tmpRectangle.yMin + 1;

  status = DMD_setClippingArea(tmpRectangle.xMin, tmpRectangle.yMin, width, height);
  if (status != 0) return status;

  status = DMD_writeColor(0, 0, red, green, blue, width * height);
  if (status != 0) return status;

  status = GLIB_resetDisplayClippingArea(pContext);
  if (status != 0) return status;

  return GLIB_OK;
}
