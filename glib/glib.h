 /*************************************************************************//**
 * @file glib.h
 * @brief Energy Micro Graphics Library
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

#ifndef __GLIB_H
#define __GLIB_H

/* C-header files */
#include <stdint.h>
#include <stdlib.h>

/* GLIB header files */
#include "glib_color.h"

/* Display Driver header files */
#include "dmd/ssd2119/dmd_ssd2119.h"

#include "em_types.h"

#define ECODE_GLIB_BASE    0x00000000

/* Error codes */
/** Successful call */
#define GLIB_OK                        0x00000000
/** Function did not draw */
#define GLIB_DID_NOT_DRAW              (ECODE_GLIB_BASE | 0x0001)
/** Invalid char */
#define GLIB_INVALID_CHAR              (ECODE_GLIB_BASE | 0x0002)
/** Coordinates out of bounds */
#define GLIB_OUT_OF_BOUNDS             (ECODE_GLIB_BASE | 0x0003)
/** Invalid coordinates (ex. xMin > xMax) */
#define GLIB_INVALID_CLIPPINGREGION    (ECODE_GLIB_BASE | 0x0004)
/** Invalid argument */
#define GLIB_INVALID_ARGUMENT          (ECODE_GLIB_BASE | 0x0005)
/** Out of memory */
#define GLIB_OUT_OF_MEMORY             (ECODE_GLIB_BASE | 0x0006)
/** File not supported */
#define GLIB_FILE_NOT_SUPPORTED        (ECODE_GLIB_BASE | 0x0007)
/** General IO Error */
#define GLIB_IO_ERROR                  (ECODE_GLIB_BASE | 0x0008)
/** Invalid file */
#define GLIB_INVALID_FILE              (ECODE_GLIB_BASE | 0x0009)

/** @struct __GLIB_Rectangle
 *  @brief Rectangle structure
 */
typedef struct __GLIB_Rectangle
{
  /** Minimum x-coordinate */
  uint16_t xMin;
  /** Minimum y-coordinate */
  uint16_t yMin;
  /** Maximum x-coordinate */
  uint16_t xMax;
  /** Maximum y-coordinate */
  uint16_t yMax;
} GLIB_Rectangle;

/** @struct __GLIB_Context
 *  @brief GLIB Drawing Context
 *  (Multiple instances of GLIB_Context can exist)
 */
typedef struct __GLIB_Context
{
  /** Pointer to the dimensions of the display */
  const DMD_DisplayGeometry *pDisplayGeometry;

  /** Background color */
  uint32_t                  backgroundColor;

  /** Foreground color */
  uint32_t                  foregroundColor;

  /** Clipping rectangle */
  GLIB_Rectangle            clippingRegion;
} GLIB_Context;

/* Prototypes for graphics library functions */
EMSTATUS GLIB_contextInit(GLIB_Context *pContext);

EMSTATUS GLIB_displayWakeUp(void);

EMSTATUS GLIB_displaySleep(void);

EMSTATUS GLIB_clear(const GLIB_Context *pContext);

EMSTATUS GLIB_resetDisplayClippingArea(const GLIB_Context *pContext);

EMSTATUS GLIB_resetClippingRegion(GLIB_Context *pContext);

void GLIB_colorTranslate24bpp(uint32_t color, uint8_t *red, uint8_t *green, uint8_t *blue);

uint32_t GLIB_rgbColor(uint8_t red, uint8_t green, uint8_t blue);

uint32_t GLIB_rectContainsPoint(const GLIB_Rectangle *pRect, uint16_t xCenter, uint16_t yCenter);

void GLIB_normalizeRect(GLIB_Rectangle *pRect);

EMSTATUS GLIB_setClippingRegion(GLIB_Context *pContext, GLIB_Rectangle *pRect);

EMSTATUS GLIB_drawCircle(const GLIB_Context *pContext, uint16_t x, uint16_t y,
                         uint16_t radius);

EMSTATUS GLIB_drawCircleFilled(const GLIB_Context *pContext, uint16_t x, uint16_t y,
                               uint16_t radius);

EMSTATUS GLIB_drawPartialCircle(const GLIB_Context *pContext, uint16_t xCenter,
                                uint16_t yCenter, uint16_t radius, uint8_t bitMask);

EMSTATUS GLIB_drawString(const GLIB_Context *pContext, char* pString, uint16_t sLength,
                         uint16_t x0, uint16_t y0, uint32_t opaque);

EMSTATUS GLIB_drawChar(const GLIB_Context *pContext, char myChar, uint16_t x,
                       uint16_t y, uint32_t opaque);

EMSTATUS GLIB_drawBitmap(const GLIB_Context* pContext, uint16_t x, uint16_t y,
                         uint16_t width, uint16_t height, uint8_t *picData);

EMSTATUS GLIB_drawLine(const GLIB_Context *pContext, uint16_t x1, uint16_t y1,
                       uint16_t x2, uint16_t y2);

EMSTATUS GLIB_drawLineH(const GLIB_Context *pContext, uint16_t x1, uint16_t y1,
                        uint16_t x2);

EMSTATUS GLIB_drawLineV(const GLIB_Context *pContext, uint16_t x1, uint16_t y1,
                        uint16_t y2);

EMSTATUS GLIB_drawRect(const GLIB_Context *pContext, GLIB_Rectangle *pRect);

EMSTATUS GLIB_drawRectFilled(const GLIB_Context *pContext,
                             GLIB_Rectangle *pRect);

EMSTATUS GLIB_drawPolygon(const GLIB_Context *pContext,
                          uint32_t numPoints, uint16_t *polyPoints);

EMSTATUS GLIB_drawPixelRGB(uint16_t x, uint16_t y,
                           uint8_t red, uint8_t green, uint8_t blue);

EMSTATUS GLIB_drawPixel(const GLIB_Context *pContext, uint16_t x, uint16_t y);

EMSTATUS GLIB_drawPixelColor(const GLIB_Context *pContext, uint16_t x, uint16_t y,
                             uint32_t color);
#endif
