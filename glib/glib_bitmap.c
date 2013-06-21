 /*************************************************************************//**
 * @file glib_bitmap.c
 * @brief Energy Micro Graphics Library: Bitmap Drawing Routines
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
*  Draws a bitmap
*
*  Sets up a bitmap that starts at x0,y0 and draws bitmap.
*  The picture is a 24-bit bitmap. It is organized as 8-bit
*  per color per pixel, and the color order is RGB. So picData is supposed to contain
*  at least (width * height * 3) entries. picData has be organized like this:
*  picData = { R, G, B, R, G, B, R, G, B ... }
*
*  @param pContext
*  Pointer to a GLIB_Context in which the bitmap is drawn.
*  @param x
*  Start x-coordinate for bitmap
*  @param y
*  Start y-coordinate for bitmap
*  @param width
*  Width of picture
*  @param height
*  Height of picture
*  @param pixData
*  Bitmap data 24-bit RGB
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/

EMSTATUS GLIB_drawBitmap(const GLIB_Context *pContext, uint16_t x, uint16_t y,
                         uint16_t width, uint16_t height, uint8_t *picData)
{
  EMSTATUS status;

  /* Set display clipping area for bitmap */
  status = DMD_setClippingArea(x, y, width, height);
  if (status != DMD_OK) return status;

  /* Write bitmap to display */
  status = DMD_writeData(0, 0, picData, width * height);
  if (status != DMD_OK) return status;

  /* Reset display clipping area to the whole display */
  GLIB_resetDisplayClippingArea(pContext);
  if (status != GLIB_OK) return status;

  return GLIB_OK;
}
