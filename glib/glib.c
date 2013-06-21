 /*************************************************************************//**
 * @file glib.c
 * @brief Energy Micro Graphics Library: General Routines
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

/* GLIB files */
#include "glib.h"

/**************************************************************************//**
*  @brief
*  Initialize the GLIB_Context
*
*  The context is set to default values and gets information about the display
*  from the display driver.
*
*  @param pContext
*  Pointer to a GLIB_Context
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_contextInit(GLIB_Context *pContext)
{
  /* Check arguments */
  if (pContext == NULL) return GLIB_INVALID_ARGUMENT;

  EMSTATUS status;

  /* Sets the default background color */
  pContext->backgroundColor = Black;

  /* Sets the default foreground color */
  pContext->foregroundColor = White;

  /* Sets a pointer to the display geometry struct */
  DMD_DisplayGeometry *pTmpDisplayGeometry;
  status = DMD_getDisplayGeometry(&pTmpDisplayGeometry);
  if (status != 0) return status;

  pContext->pDisplayGeometry = pTmpDisplayGeometry;

  /* Sets the clipping region to the whole display */
  GLIB_Rectangle tmpRect = { 0, 0, pTmpDisplayGeometry->xSize - 1, pTmpDisplayGeometry->ySize - 1 };
  status = GLIB_setClippingRegion(pContext, &tmpRect);
  if (status != 0) return status;

  return GLIB_OK;
}

/**************************************************************************//**
*  @brief
*  Returns the display from sleep mode
*
*  @return
*  Returns DMD_OK on success, or else error code
******************************************************************************/

EMSTATUS GLIB_displayWakeUp()
{
  /* Use display driver's wake up function */
  return DMD_wakeUp();
}

/**************************************************************************//**
*  @brief
*  Sets the display in sleep mode
*
*  @return
*  Returns DMD_OK on success, or else error code
******************************************************************************/

EMSTATUS GLIB_displaySleep()
{
  /* Use Display Driver sleep function */
  return DMD_sleep();
}

/**************************************************************************//**
*  @brief
*  Sets the clippingRegion of the passed in GLIB_Context
*
*  @param pContext
*  Pointer to a GLIB_Context
*  @param pRect
*  Pointer to a GLIB_Rectangle which is the clipping region to be set.
*
*  @return
*  - Returns GLIB_OK on success
*  - Returns GLIB_INVALID_CLIPPINGREGION if invalid coordinates
*  - Returns GLIB_OUT_OF_BOUNDS if clipping region is bigger than display clipping
*  area
******************************************************************************/

EMSTATUS GLIB_setClippingRegion(GLIB_Context *pContext, GLIB_Rectangle *pRect)
{
  /* Check arguments */
  if (pContext == NULL || pRect == NULL) return GLIB_INVALID_ARGUMENT;

  /* Check coordinates against the display region */
  if ((pRect->xMin >= pRect->xMax) || (pRect->yMin >= pRect->yMax)) return GLIB_INVALID_CLIPPINGREGION;
  if (pRect->xMax > pContext->pDisplayGeometry->clipWidth) return GLIB_OUT_OF_BOUNDS;
  if (pRect->yMax > pContext->pDisplayGeometry->clipHeight) return GLIB_OUT_OF_BOUNDS;

  GLIB_Rectangle tmpRect = { pRect->xMin, pRect->yMin, pRect->xMax, pRect->yMax };

  pContext->clippingRegion = tmpRect;

  return GLIB_OK;
}


/**************************************************************************//**
*  @brief
*  Clears the display with the background color of the GLIB_Context
*
*  @param pContext
*  Pointer to a GLIB_Context which holds the backgroundcolor.
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_clear(const GLIB_Context *pContext)
{
  /* Check arguments */
  if (pContext == NULL) return GLIB_INVALID_ARGUMENT;

  EMSTATUS status;

  uint8_t  red;
  uint8_t  green;
  uint8_t  blue;
  /* Divide the 24-color into it's components */
  GLIB_colorTranslate24bpp(pContext->backgroundColor, &red, &green, &blue);

  /* Reset display driver clipping area */
  status = GLIB_resetDisplayClippingArea(pContext);
  if (status != 0) return status;

  uint16_t width  = pContext->pDisplayGeometry->clipWidth;
  uint16_t height = pContext->pDisplayGeometry->clipHeight;
  /* Fill the display with the background color of the GLIB_Context  */

  status = DMD_writeColor(0, 0, red, green, blue, width * height);
  if (status != 0) return status;

  return GLIB_OK;
}

/**************************************************************************//**
*  @brief
*  Reset the display driver clipping area to the whole display
*
*  @param pContext
*  Pointer to a GLIB_Context
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_resetDisplayClippingArea(const GLIB_Context *pContext)
{
  /* Check arguments */
  if (pContext == NULL) return GLIB_INVALID_ARGUMENT;

  EMSTATUS status = DMD_setClippingArea(0, 0, pContext->pDisplayGeometry->xSize,
                                        pContext->pDisplayGeometry->ySize);
  if (status != DMD_OK) return status;

  return GLIB_OK;
}

/**************************************************************************//**
*  @brief
*  Reset the GLIB_Context clipping region to the whole display
*
*  @param pContext
*  Pointer to a GLIB_Context
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_resetClippingRegion(GLIB_Context *pContext)
{
  /* Check arguments */
  if (pContext == NULL) return GLIB_INVALID_ARGUMENT;

  GLIB_Rectangle tmpRect;
  tmpRect.xMin = pContext->pDisplayGeometry->xClipStart;
  tmpRect.yMin = pContext->pDisplayGeometry->yClipStart;
  tmpRect.xMax = tmpRect.xMin + pContext->pDisplayGeometry->clipWidth - 1;
  tmpRect.yMax = tmpRect.yMin + pContext->pDisplayGeometry->clipHeight - 1;

  pContext->clippingRegion = tmpRect;

  return GLIB_OK;
}

/**************************************************************************//**
*  @brief
*  Extracts the color components from the 32-bit color passed and puts them in
*  the passed in 8-bits ints. The color is 24-bit RGB.
*
*  Example: color = 0x00FFFF00 -> red = 0xFF, green = 0xFF, blue = 0x00.
*
*  @param color
*  The color which is to be translated
*  @param red
*  Pointer to a uint8_t holding the red component
*  @param green
*  Pointer to a uint8_t holding the green component
*  @param blue
*  Pointer to a uint8_t holding the blue component
*
*  @return
*  None.
******************************************************************************/

void GLIB_colorTranslate24bpp(uint32_t color, uint8_t *red, uint8_t *green, uint8_t *blue)
{
  *red   = (color & RedMask) >> RedShift;
  *green = (color & GreenMask) >> GreenShift;
  *blue  = (color & BlueMask) >> BlueShift;
}

/**************************************************************************//**
*  @brief
*  Convert 3 uint8_t color components into a 24-bit color
*
*  Example: red = 0xFF, green = 0xFF, blue = 0x00 -> 0x00FFFF00 = Yellow
*
*  @param red
*  Red component
*  @param green
*  Green component
*  @param blue
*  Blue component
*  @return
*  Returns a 32-bit unsigned integer representing the color. The 8 LSB is blue,
*  the next 8 is green and the next 8 is red. 0x00RRGGBB
*
******************************************************************************/

uint32_t GLIB_rgbColor(uint8_t red, uint8_t green, uint8_t blue)
{
  return (red << RedShift) | (green << GreenShift) | (blue << BlueShift);
}

/**************************************************************************//**
*  @brief
*  Draws a pixel at x, y using foregroundColor defined in the GLIB_Context.
*
*  @param pContext
*  Pointer to a GLIB_Context which holds the foreground color
*  @param x
*  X-coordinate
*  @param y
*  Y-coordinate
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_drawPixel(const GLIB_Context *pContext,
                        uint16_t x, uint16_t y)
{
  /* Check arguments */
  if (pContext == NULL) return GLIB_INVALID_ARGUMENT;

  /* Draw pixel */
  return GLIB_drawPixelColor(pContext, x, y, pContext->foregroundColor);
}

/**************************************************************************//**
*  @brief
*  Draws a pixel at x, y with color defined by red, green and blue
*  1 byte per channel.
*
*  Example: To draw a yellow pixel at (10, 10).
*  x = 10, y = 10, red = 0xFF, green = 0xFF, blue = 0x00
*
*  @param x
*  X-coordinate
*  @param y
*  Y-coordinate
*  @param red
*  8-bits red
*  @param green
*  8-bits green
*  @param blue
*  8-bits blue
*
*  @return
*  Returns DMD_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_drawPixelRGB(uint16_t x, uint16_t y, uint8_t red, uint8_t green,
                           uint8_t blue)
{
  /* Call Display driver function */
  return DMD_writeColor(x, y, red, green, blue, 1);
}

/**************************************************************************//**
*  @brief
*  Draws a pixel at x, y with color defined by the 32-bit int
*
*  @param x
*  X-coordinate
*  @param y
*  Y-coordinate
*  @param color
*  32-bit int defining the RGB color. The 24 LSB defines the RGB color like this:
*  RRRRRRRRGGGGGGGGBBBBBBBB. Example: Yellow = 0x00FFFF00
*  @return
*  Returns DMD_OK on success, or else error code
******************************************************************************/
EMSTATUS GLIB_drawPixelColor(const GLIB_Context *pContext, uint16_t x, uint16_t y,
                             uint32_t color)
{
  /* Check arguments */
  if (pContext == NULL) return GLIB_INVALID_ARGUMENT;

  EMSTATUS status;

  /* Draw pixel */
  if (GLIB_rectContainsPoint(&pContext->clippingRegion, x, y) == 1)
  {
    status = GLIB_resetDisplayClippingArea(pContext);
    if (status != GLIB_OK) return status;

    /* Draw pixel */
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    GLIB_colorTranslate24bpp(color, &red, &green, &blue);
    return GLIB_drawPixelRGB(x, y, red, green, blue);
  }

  return GLIB_DID_NOT_DRAW;
}
