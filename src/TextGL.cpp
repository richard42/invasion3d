/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - TextGL.cpp                                               *
 *   Homepage: http://code.google.com/p/invasion3d/                        *
 *   Copyright (C) 2005-2011 Richard Goedeken                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/****************************************************************************
**                              04/26/05                                   **
**                                                                         **
** TextGL.cpp - contains implementation of CTextGL class, used to print    **
**              text in 3-space using OpenGL                               **
**                                                                         **
****************************************************************************/

#ifdef WIN32
  #include <windows.h>
#endif
#if defined(__APPLE__)
  #include <OpenGL/gl.h>
#else
  #include <GL/gl.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TextGL.h"
#include "GameMain.h" // for m_bAlphaSupported flag
#include "Package.h"

// static data declaration
bool          CTextGL::m_bInitialized = false;
unsigned int  CTextGL::m_uiNumFonts = 0;
sFont         CTextGL::m_asFonts[16];
unsigned int  CTextGL::m_uiAsciiConv[256];
unsigned char CTextGL::m_uchCharTable[88] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz',.-/=;\"<>?+:1234567890!@#$%^&*()[]"};
unsigned int  CTextGL::m_uiMaxWidth;

/////////////////////////////////////////////////////////////////////////////
// CTextGL public static functions

bool CTextGL::InitFonts(CPackage *pcMedia)
{
  // first, clear the conversion table
  for (int i = 0; i < 256; i++) m_uiAsciiConv[i] = 0;

  // then inverse-map the char table into the conversion table
  for (int j = 0; j < 87; j++) m_uiAsciiConv[m_uchCharTable[j]] = j + 1;

  // find the maximum texture size of this OpenGL implementation
  GLint iMaxSize;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &iMaxSize);
  if (iMaxSize < 512)
    {
    printf("CTextGL::InitFonts Error! Maximum OpenGL texture size is %i! I need 512 pixel textures for fonts.\n", iMaxSize);
    return false;
    }
  m_uiMaxWidth = iMaxSize;

  if (!CreateFontGL(pcMedia, E_FONT_6809,         "font-6809chargen.bmp",  15)) return false;
  if (!CreateFontGL(pcMedia, E_FONT_EUPHORIGENIC, "font-euphorigenic.bmp", 10)) return false;
  if (!CreateFontGL(pcMedia, E_FONT_CREDIT_RIVER, "font-creditriver.bmp",  10)) return false;
  if (!CreateFontGL(pcMedia, E_FONT_VECTROID,     "font-vectroid.bmp",     15)) return false;
  if (!CreateFontGL(pcMedia, E_FONT_BERYLLIUM,    "font-beryllium.bmp",    10)) return false;

  m_bInitialized = true;
  return true;
}

void CTextGL::DestroyFonts(void)
{
  if (m_bInitialized)
    {
    for (unsigned int ui = 0; ui < m_uiNumFonts; ui++)
      {
      glDeleteTextures(1, &(m_asFonts[ui].glCharTexture));
      }
    m_bInitialized = false;
    }

}

/////////////////////////////////////////////////////////////////////////////
// CTextGL constructor and destructor

CTextGL::CTextGL()
{
  m_fPositionX  = 0.0f;
  m_fPositionY  = 0.0f;
  m_fPositionZ  = 0.0f;
  m_fHeight     = 1.0f;
  m_bAlphaBlend = false;
  m_uchColor[0] = 0xff;
  m_uchColor[1] = 0xff;
  m_uchColor[2] = 0xff;

  m_eJustify   = E_LEFT;
  m_eFont      = E_FONT_6809;

  // set pointer to NULL
  m_pchString      = NULL;
  m_uiBufferLength = 0;
}

CTextGL::~CTextGL()
{
  if (m_pchString)
    {
    free(m_pchString);
    m_pchString = NULL;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CTextGL accessor functions

float CTextGL::GetLineWidth(const char *pch, int iLength) const
{
  unsigned int uiLineWidth = 0;

  for (int i = 0; i < iLength; i++, pch++)
    {
    if (*pch == ' ')     // handle space
      {
      uiLineWidth += m_asFonts[m_eFont].uiSpaceWidth;
      continue;
      }
    int iGlyph = m_uiAsciiConv[*pch];
    if (iGlyph != 0)
      {
      uiLineWidth += m_asFonts[m_eFont].uiCharWidth[iGlyph-1] + 1;
      }
    }

  return (float) uiLineWidth * m_fHeight / (float) m_asFonts[m_eFont].uiFontHeight;
}

void CTextGL::Draw(void) const
{
  float fRatio = m_fHeight / (float) m_asFonts[m_eFont].uiFontHeight;

  // check input state
  if (m_fHeight <= 0.0f || m_pchString == NULL) return;

  // setup initial position
  float fX = m_fPositionX;
  float fY = m_fPositionY;
  float fZ = m_fPositionZ;

  // disable lighting
  glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  if (m_bAlphaBlend && CGameMain::m_bAlphaSupported)
    {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    }
  // if our color is white, we don't need the texture environment to be set to MODULATE
  bool bWhite = m_uchColor[0] == 255 && m_uchColor[1] == 255 && m_uchColor[2] == 255;
  if (bWhite)
    {
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    }
  glColor3ubv(m_uchColor);
  glBindTexture(GL_TEXTURE_2D, m_asFonts[m_eFont].glCharTexture);

  // parse through the string, looking for line feeds
  char *pch = m_pchString;
  while (*pch != 0)
    {
    char *pchStart = pch;
    int iLength = 0;
    // look for a line-feed or the end of the string
    while (*pch != 0 && *pch != 10)
      {
      pch++;
      iLength++;
      }
    // draw this line if necessary
    if (iLength > 0)
      DrawLine(pchStart, iLength, fX, fY, fZ);
    // ditch out if we're at the end of the string
    if (*pch == 0) break;
    // otherwise, go to the next line
    pch++;
    fX = m_fPositionX;
    fY += fRatio * (float) m_asFonts[m_eFont].uiFontHeight * 1.1f;
    }

  // re-enable lighting
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
  if (m_bAlphaBlend && CGameMain::m_bAlphaSupported)
    {
    glDisable(GL_BLEND);
    }
  if (bWhite)
    {
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }

  return;
}

void CTextGL::DrawLine(const char *pchString, int iLength, float fX, float fY, float fZ) const
{
  float fRatio = m_fHeight / (float) m_asFonts[m_eFont].uiFontHeight;

  // handle center or right justification
  if (m_eJustify != E_LEFT)
    {
    float fLineWidth = GetLineWidth(pchString, iLength);
    if (m_eJustify == E_CENTER)
      fX -= 0.5f * fLineWidth;
    else if (m_eJustify == E_RIGHT)
      fX -= fLineWidth;
    }

  for (int i = 0; i < iLength; i++, pchString++)
    {
    // handle space
    if (*pchString == ' ')
      {
      fX += fRatio * (float) m_asFonts[m_eFont].uiSpaceWidth;
      continue;
      }
    // handle bad chars
    int iGlyph = m_uiAsciiConv[*pchString];
    if (iGlyph == 0) continue;
    iGlyph--;
    // draw a single character
    float fX1 = fX + fRatio * (float) m_asFonts[m_eFont].uiCharWidth[iGlyph];
    float fTx0 = (float) m_asFonts[m_eFont].uiCharPositionX[iGlyph] / m_asFonts[m_eFont].uiTextureWidth;
    float fTx1 = fTx0 + ((float) m_asFonts[m_eFont].uiCharWidth[iGlyph] - 0.5f) / m_asFonts[m_eFont].uiTextureWidth;
    float fTy0 = (float) m_asFonts[m_eFont].uiCharPositionY[iGlyph] / m_asFonts[m_eFont].uiTextureHeight;
    float fTy1 = fTy0 + (float) m_asFonts[m_eFont].uiFontHeight / m_asFonts[m_eFont].uiTextureHeight;
    glBegin(GL_QUADS);
      glTexCoord2f(fTx0, fTy0); glVertex3f(fX,  fY,             fZ);
      glTexCoord2f(fTx0, fTy1); glVertex3f(fX,  fY + m_fHeight, fZ);
      glTexCoord2f(fTx1, fTy1); glVertex3f(fX1, fY + m_fHeight, fZ);
      glTexCoord2f(fTx1, fTy0); glVertex3f(fX1, fY,             fZ);
    glEnd();
    fX = fX1 + fRatio;
    }

}


/////////////////////////////////////////////////////////////////////////////
// modifier functions

void CTextGL::SetColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchColor[0] = uchRed;
  m_uchColor[1] = uchGreen;
  m_uchColor[2] = uchBlue;
}

void CTextGL::SetFont(eFont eType)
{
  if ((int) eType < 0 || eType >= E_FONT_INVALID) return;

  m_eFont = eType;

  // re-calculate the width
  CalculateWidth();
}

bool CTextGL::SetHeight(float fHeight)
{
  if (fHeight <= 0.0f) return false;

  m_fHeight = fHeight;

  // re-calculate the width
  CalculateWidth();
  return true;
}

bool CTextGL::SetJustification(EJustify eJustify)
{
  if ((int) eJustify < 0 || eJustify >= E_JUST_INVALID) return false;

  m_eJustify = eJustify;
  return true;
}

void CTextGL::SetPosition(float fX, float fY, float fZ)
{
  m_fPositionX = fX;
  m_fPositionY = fY;
  m_fPositionZ = fZ;
}

bool CTextGL::SetText(const char *pchText)
{
  unsigned int uiLength = (unsigned int) strlen(pchText);

  if (m_pchString && uiLength >= m_uiBufferLength)
    {
    // free the previous buffer
    free(m_pchString);
    m_pchString = NULL;
    m_uiBufferLength = 0;
    }

  if (!m_pchString)
    {
    // allocate a bigger buffer
    m_pchString = (char *) malloc(uiLength + 1);
    if (!m_pchString) return false;
    // set the new buffer length
    m_uiBufferLength = uiLength + 1;
    }

  // copy string text into our buffer
  memcpy(m_pchString, pchText, uiLength + 1);

  // calculate the width of this block
  CalculateWidth();

  // all done
  return true;
}

/////////////////////////////////////////////////////////////////////////////
// private modifiers

void CTextGL::CalculateWidth(void)
{
  float fWidest = 0.0f;

  // check for null pointer
  if (m_pchString == NULL)
    {
    m_fWidth = 0.0f;
    return;
    }

  // parse through the string, looking for line feeds
  char *pch = m_pchString;
  while (*pch != 0)
    {
    char *pchStart = pch;
    int iLength = 0;
    // look for a line-feed or the end of the string
    while (*pch != 0 && *pch != 10)
      {
      pch++;
      iLength++;
      }
    // get width of this line
    float fThisWidth = GetLineWidth(pchStart, iLength);
    if (fThisWidth > fWidest) fWidest = fThisWidth;
    // ditch out if we're at the end of the string
    if (*pch == 0) break;
    // otherwise, go to the next line
    pch++;
    }

  // set width of widest line in this CTextGL object
  m_fWidth = fWidest;
}

/////////////////////////////////////////////////////////////////////////////
// private static functions

bool CTextGL::CreateFontGL(CPackage *pcMedia, eFont eType, const char *pchBMPName, int iCharSpacing)
{
  unsigned char uchBMPHeader[62];

  // first, load up the bitmap file header
  unsigned int uiFile = pcMedia->OpenFile(pchBMPName);
  if (uiFile == 0)
    {
    printf("CTextGL Error: Couldn't open font file: %s\n", pchBMPName);
    return false;
    }
  if (pcMedia->ReadBytes(uiFile, uchBMPHeader, 62) != 62)
    {
    printf("CTextGL Error: Couldn't read 62 bytes from BMP font file\n");
    return false;
    }
  // check the header
  unsigned int uiDataOffset = (uchBMPHeader[0xd] << 24) | (uchBMPHeader[0xc] << 16) | (uchBMPHeader[0xb] << 8) | uchBMPHeader[0xa];
  unsigned int uiHeaderSize = (uchBMPHeader[0x11] << 24) | (uchBMPHeader[0x10] << 16) | (uchBMPHeader[0xf] << 8) | uchBMPHeader[0xe];
  unsigned int uiWidth  = (uchBMPHeader[0x15] << 24) | (uchBMPHeader[0x14] << 16) | (uchBMPHeader[0x13] << 8) | uchBMPHeader[0x12];
  int iHeight = (uchBMPHeader[0x19] << 24) | (uchBMPHeader[0x18] << 16) | (uchBMPHeader[0x17] << 8) | uchBMPHeader[0x16];
  unsigned int uiPlanes = (uchBMPHeader[0x1b] << 8) | uchBMPHeader[0x1a];
  unsigned int uiBPP    = (uchBMPHeader[0x1d] << 8) | uchBMPHeader[0x1c];
  if (uchBMPHeader[0] != 'B' || uchBMPHeader[1] != 'M' || uiDataOffset != 0x436 || uiHeaderSize != 0x28 || uiPlanes != 1 || uiBPP != 8)
    {
    printf("CTextGL Error: Invalid BMP file header!\n");
    return false;
    }
  int iStride = (uiWidth + 3) & 0xfffffffc;

  // skip past palette data
  pcMedia->SkipBytes(uiFile, 0x3f8);

  // load the bitmap image
  unsigned char *pucPicBuffer, *pucLoad;
  unsigned int uiHeight;
  bool bTopDown = false;
  if (iHeight < 0)
    {
    iHeight = -iHeight;
    bTopDown = true;
    }
  uiHeight = (unsigned int) iHeight;
  pucPicBuffer = (unsigned char *) malloc(iStride * uiHeight);
  if (!pucPicBuffer)
    {
    printf("CTextGL Error: Unable to allocate %i bytes for font file image. Height=%i, Width=%i, Stride=%i\n", iStride * iHeight, iHeight, uiWidth, iStride);
    return false;
    }
  if (bTopDown)
    pucLoad = pucPicBuffer;
  else
    pucLoad = pucPicBuffer + (uiHeight - 1) * iStride;
  for (unsigned int i = 0; i < uiHeight; i++)
    {
    if (pcMedia->ReadBytes(uiFile, pucLoad, iStride) != iStride)
      {
      printf("CTextGL Error: Unable to load line #%i from font file.\n", i);
      free(pucPicBuffer);
      return false;
      }
    if (bTopDown)
      pucLoad += iStride;
    else
      pucLoad -= iStride;
    }
  unsigned char uchBackground = pucPicBuffer[0];
  if (uchBackground != 0 && uchBackground != 0xff)
    {
    printf("CTextGL Error: unable to determine background color\n");
    free(pucPicBuffer);
    return false;
    }

  // find the top and bottom lines
  unsigned char *pucSearch = pucPicBuffer;
  unsigned int uiTop, uiBottom, uiFontHeight;
  for (uiTop = 0; uiTop < uiHeight; uiTop++)
    {
    unsigned int i;
    for (i = 0; i < uiWidth; i++)
      {
      if (*pucSearch++ != 0xff) break;
      }
    if (i < uiWidth) break;
    pucSearch += iStride - uiWidth;
    }
  if (uiTop == uiHeight)
    {
    // image is blank
    printf("CTextGL Error: Couldn't find top line - image is blank\n");
    free(pucPicBuffer);
    return false;
    }
  pucSearch = pucPicBuffer + (uiHeight - 1) * iStride;
  for (uiBottom = uiHeight - 1; uiBottom > 0; uiBottom--)
    {
    unsigned int i;
    for (i = 0; i < uiWidth; i++)
      {
      if (*pucSearch++ != 0xff) break;
      }
    if (i < uiWidth) break;
    pucSearch -= iStride + uiWidth;
    }
  if (uiBottom == 0)
    {
    // image is blank
    printf("CTextGL Error: Couldn't find bottom line - image is blank\n");
    free(pucPicBuffer);
    return false;
    }
  uiFontHeight = uiBottom - uiTop + 1;

  // search through the image data and gather starting coordinates and widths
  unsigned int uiCharLeft[87], uiCharWidth[87];
  unsigned int uiWidest = 0, uiLeftIdx = 0;
  int iChar;
  for (iChar = 0; iChar < 87; iChar++)
    {
    // first, search for the start of the next character
    do
      {
      pucSearch = pucPicBuffer + uiTop * iStride + uiLeftIdx;
      unsigned int uiRow;
      for (uiRow = 0; uiRow < uiFontHeight; uiRow++)
        {
        if (*pucSearch != 0xff) break;
        pucSearch += iStride;
        }
      if (uiRow < uiFontHeight) break;
      uiLeftIdx++;
      } while(uiLeftIdx < uiWidth);
    if (uiLeftIdx == uiWidth) break;
    uiCharLeft[iChar] = uiLeftIdx++;
    // now find the end of the character
    unsigned int uiTransitionsRemaining = 1;
    if (m_uchCharTable[iChar] == '\"')
      uiTransitionsRemaining = 3;
    do
      {
      pucSearch = pucPicBuffer + uiTop * iStride + uiLeftIdx;
      unsigned int uiRow;
      for (uiRow = 0; uiRow < uiFontHeight; uiRow++)
        {
        if (*pucSearch != 0xff) break;
        pucSearch += iStride;
        }
      if (((uiTransitionsRemaining & 1) && (uiRow == uiFontHeight)) ||
          (!(uiTransitionsRemaining & 1) && (uiRow < uiFontHeight))) uiTransitionsRemaining--;
      uiLeftIdx++;
      } while(uiLeftIdx < uiWidth && uiTransitionsRemaining > 0);
    if (uiLeftIdx == uiWidth) break;
    uiCharWidth[iChar] = uiLeftIdx - uiCharLeft[iChar];
    if (uiCharWidth[iChar] > uiWidest) uiWidest = uiCharWidth[iChar];
    }
  if (iChar < 87)
    {
    // not all characters found
    printf("CTextGL Error: only %i out of 87 characters found!\n", iChar);
    free(pucPicBuffer);
    return false;
    }

  // decide how many rows of characters to use in texture
  unsigned int uiCharRows = 1;
  unsigned int uiRowStartChar[8];
  unsigned int uiRowNumChars[8];
  uiRowStartChar[0] = 0;
  uiRowNumChars[0] = 87;
  if (uiWidth > m_uiMaxWidth)
  {
    for (unsigned int uiCurChar = 0; uiCurChar < 87; uiCurChar++)
    {
      // if this char extends beyond the maximum width, then go to the next row
      if (uiCharLeft[uiCurChar] + uiCharWidth[uiCurChar]
                                - uiCharLeft[uiRowStartChar[uiCharRows-1]] > m_uiMaxWidth)
      {
        uiRowStartChar[uiCharRows] = uiCurChar;
        uiRowNumChars[uiCharRows-1] = uiCurChar - uiRowStartChar[uiCharRows-1];
        uiCharRows++;
        continue;
      }
    }
    // fill in the count of characters in the last row
    uiRowNumChars[uiCharRows-1] = 87 - uiRowStartChar[uiCharRows-1];
    // sanity check that this font will fit in a texture
    if (uiCharRows * uiFontHeight > m_uiMaxWidth)
    {
      printf("Error: font '%s' bitmap will not fit in biggest texture.\n", pchBMPName);
      free(pucPicBuffer);
      return false;
    }
  }

  // find nearest power of 2 for width and height of texture
  const unsigned int uiNeededWidth = (uiWidth < m_uiMaxWidth) ? uiWidth : m_uiMaxWidth;
  const unsigned int uiNeededHeight = uiCharRows * uiFontHeight;
  unsigned int uiTextureHeight = 32, uiTextureWidth = 32;
  while (uiTextureHeight < uiNeededHeight) uiTextureHeight <<= 1;
  while (uiTextureWidth < uiNeededWidth) uiTextureWidth <<= 1;

  // allocate a buffer for making the textures
  unsigned int uiTexBufSize = uiTextureWidth * uiTextureHeight;
  unsigned char *pucTexBuf = (unsigned char *) malloc(uiTexBufSize);
  if (!pucTexBuf)
    {
    free(pucPicBuffer);
    return false;
    }

  // set the font height and space width, and texture width and height
  m_asFonts[eType].uiFontHeight    = uiFontHeight;
  m_asFonts[eType].uiSpaceWidth    = iCharSpacing;
  m_asFonts[eType].uiTextureWidth  = uiTextureWidth;
  m_asFonts[eType].uiTextureHeight = uiTextureHeight;

  // save the width and position of each character
  unsigned int uiCurrentRow = 0;
  unsigned int uiRowFontLeftX = uiCharLeft[0];
  for (int iChar = 0; iChar < 87; iChar++)
    {
    m_asFonts[eType].uiCharPositionX[iChar] = uiCharLeft[iChar] - uiRowFontLeftX;
    m_asFonts[eType].uiCharPositionY[iChar] = uiCurrentRow * uiFontHeight;
    m_asFonts[eType].uiCharWidth[iChar] = uiCharWidth[iChar];
    if (uiRowStartChar[uiCurrentRow] + uiRowNumChars[uiCurrentRow] == iChar + 1 && iChar < 86)
      {
      uiCurrentRow += 1;
      uiRowFontLeftX = uiCharLeft[iChar + 1];
      }
    }

  // clear the texture buffer
  memset(pucTexBuf, 0, uiTexBufSize);

  // now copy the font into the texture buffer, inverting each byte
  for (unsigned int uiCharRow = 0; uiCharRow < uiCharRows; uiCharRow++)
    {
    const unsigned int uiCharStart = uiRowStartChar[uiCharRow];
    const unsigned int uiCharEnd = uiCharStart + uiRowNumChars[uiCharRow] - 1;
    const unsigned int uiLeftX = uiCharLeft[uiCharStart];
    const unsigned int uiRightX = uiCharLeft[uiCharEnd] + uiCharWidth[uiCharEnd] - 1;
    const unsigned int uiRowFontTopY = uiCharRow * uiFontHeight;
    for (unsigned int iY = uiTop; iY <= uiBottom; iY++)
      {
      unsigned char *pucDst = pucTexBuf + uiTextureWidth * (uiRowFontTopY + iY - uiTop);
      for (unsigned int iX = uiLeftX; iX <= uiRightX; iX++)
	{
	*pucDst++ = 0xff - pucPicBuffer[iY * iStride + iX];
	}
      }
    }

  // free the pic buffer
  free(pucPicBuffer);

  // now generate the texture
  glGenTextures(1, &(m_asFonts[eType].glCharTexture));
  // create an OpenGL texture from this bitmap
  glBindTexture(GL_TEXTURE_2D, m_asFonts[eType].glCharTexture);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  if (CGameMain::m_bUseLuminance)
    {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY8, uiTextureWidth, uiTextureHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pucTexBuf);
    m_uiNumFonts++;
    free(pucTexBuf);
    return true;
    }

  // now convert to RGBA for OpenGL implementations which can't do GL_LUMINANCE in hardware
  unsigned char *pucRGBA = (unsigned char *) malloc(uiTextureWidth * uiTextureHeight * 4);
  if (!pucRGBA)
    {
    free(pucTexBuf);
    printf("CTextGL Error: Unable to allocate %i bytes for font file image.\n", uiTextureWidth * uiTextureHeight * 4);
    return false;
    }
  unsigned char *pucSrc = pucTexBuf;
  unsigned char *pucDst = pucRGBA;
  for (unsigned int ui = 0; ui < uiTextureWidth * uiTextureHeight; ui++)
    {
    *pucDst++ = *pucSrc;
    *pucDst++ = *pucSrc;
    *pucDst++ = *pucSrc;
    *pucDst++ = *pucSrc++;
    }

  // generate GL_RGBA texture for font
  glTexImage2D(GL_TEXTURE_2D, 0, 4, uiTextureWidth, uiTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pucRGBA);

  m_uiNumFonts++;
  free(pucTexBuf);
  free(pucRGBA);
  return true;
}
