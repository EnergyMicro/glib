 /*************************************************************************//**
 * @file glib_string.c
 * @brief Energy Micro Graphics Library: String Drawing Routines
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
#include "glib_font.h"
#include "glib_color.h"

/* Defines */
/* Sets how many pixels it should be between each line */
#define SPACE_BETWEEN_LINES    (2)
#define FONT_ROW_OFFSET        (100)

/**************************************************************************//**
*  @brief
*  Draws a char using the font supplied with the library.
*
*  @param pContext
*  Pointer to the GLIB_Context
*
*  @param myChar
*  Char to be drawn
*
*  @param x0
*  Start x-coordinate for the char (Upper left corner)
*
*  @param y0
*  Start y-coordinate for the char (Upper left corner)
*
*  @param opaque
*  Determines whether to show the background or color it with the background
*  color specified by the GLIB_Context. If opaque == 1, the background color is used.
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/

EMSTATUS GLIB_drawChar(const GLIB_Context *pContext, char myChar, uint16_t x, uint16_t y,
                       uint32_t opaque)
{
  /* Check arguments */
  if (pContext == NULL)
    return GLIB_INVALID_ARGUMENT;

  /* Check input char */
  if ((myChar < ' ') || (myChar > '~'))
  {
    /* Invalid char */
    return GLIB_INVALID_CHAR;
  }

  /* Check against clipping region */
  if (GLIB_rectContainsPoint(&pContext->clippingRegion, x, y) == 0)
  {
    /* Do not draw if the char is outside of clipping region */
    return GLIB_DID_NOT_DRAW;
  }

  /* Index for fontData */
  uint16_t fontIdx;

  /* Sets the index in the font array */
  fontIdx = myChar - ' ';

  uint16_t row;
  /* Loop through the rows and draw the char */
  for (row = 0; row < font_height; row++)
  {
    /* Draw current row */
    uint8_t currentRow;
    currentRow = fontBits[fontIdx];

    uint16_t xOffset;
    for (xOffset = 0; xOffset < font_width; ++xOffset)
    {
      /* Bit 1 means draw, Bit 0 means do not draw */
      if ((currentRow & 1) == 1)
      {
        /* Draw pixel */
        GLIB_drawPixel(pContext, x + xOffset, y + row);
      }
      else if (opaque == 1)
      {
        /* Draw background pixel */
        GLIB_drawPixelColor(pContext, x + xOffset, y + row, pContext->backgroundColor);
      }

      currentRow >>= 1;
    }

    /* fontIdx offset for a new row */
    fontIdx += FONT_ROW_OFFSET;
  }

  return GLIB_OK;
}

/**************************************************************************//**
*  @brief
*  Draws a string using the font supplied with the library.
*
*  @param pContext
*  Pointer to a GLIB_Context
*
*  @param pString
*  Pointer to the string that is drawn
*
*  @param x0
*  Start x-coordinate for the string (Upper left corner)
*
*  @param y0
*  Start y-coordinate for the string (Upper left corner)
*
*  @param opaque
*  Determines whether to show the background or color it with the background
*  color specified by the GLIB_Context. If opaque == 1, the background color is used.
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/

EMSTATUS GLIB_drawString(const GLIB_Context *pContext, char* pString, uint16_t sLength,
                         uint16_t x0, uint16_t y0, uint32_t opaque)
{
  /* Check arguments */
  if (pContext == NULL || pString == NULL)
    return GLIB_INVALID_ARGUMENT;

  EMSTATUS tmpStatus;
  uint32_t didDraw = 0;

  /* Index for string */
  uint16_t idx;

  uint16_t x, y;
  x = x0;
  y = y0;

  /* Loops through the string and prints char for char */
  for (idx = 0; idx < sLength; idx++)
  {
    /* Special case: Newline char */
    if (pString[idx] == '\n')
    {
      x = x0;
      y = y + font_height + SPACE_BETWEEN_LINES;
      continue;
    }

    /* Prints the current char */
    tmpStatus = GLIB_drawChar(pContext, pString[idx], x, y, opaque);
    if (tmpStatus != 0 && tmpStatus != GLIB_DID_NOT_DRAW) return tmpStatus;
    if (!didDraw && tmpStatus == 0) didDraw = 1;

    /* Adjust x and y coordinate */
    x += font_width;
  }

  if (didDraw == 0) return GLIB_DID_NOT_DRAW;
  return GLIB_OK;
}
