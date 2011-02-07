/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Textures.cpp                                             *
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
**                              04/30/05                                   **
**                                                                         **
** Textures.cpp - contains interface for CTextures class, used to load and **
**                create textures from BMP files                           **
**                                                                         **
****************************************************************************/

#ifdef WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "Textures.h"
#include "Package.h"
#include "GameMain.h"  // for m_bUseLuminance flag

// static data declaration
char    CTextures::m_chFilenames[E_TEX_INVALID][32] = {"laser.bmp",
                                                       "bullet.bmp",
                                                       "cannon.bmp",
                                                       "energy.bmp",
                                                       "particle.bmp",
                                                       "mame.bmp",
                                                       "shockwave.bmp"
                                                      };
GLuint  CTextures::m_iTextureID[E_TEX_INVALID];
bool    CTextures::m_bInitialized = false;

/////////////////////////////////////////////////////////////////////////////
// CTextures static accessor functions

GLuint CTextures::GetTextureID(ETexture eTexture)
{
  if (!m_bInitialized || (int) eTexture < 0 || (int) eTexture >= E_TEX_INVALID) return 0;

  return m_iTextureID[eTexture];
}

/////////////////////////////////////////////////////////////////////////////
// CTextures static modifier functions

bool CTextures::InitTextures(CPackage *pcMedia)
{
  if (m_bInitialized) return true;

  // load each BMP and make a texture out of it
  for (int iTex = 0; iTex < E_TEX_INVALID; iTex++)
    {
    GLuint uiTexID;
    if (!LoadTexture(pcMedia, m_chFilenames[iTex], &uiTexID)) return false;
    m_iTextureID[iTex] = uiTexID;
    }

  m_bInitialized = true;
  return true;
}

void CTextures::DestroyTextures(void)
{
  if (m_bInitialized)
    {
    glDeleteTextures(E_TEX_INVALID, m_iTextureID);
    m_bInitialized = false;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CTextures private static functions

bool CTextures::LoadTexture(CPackage *pcMedia, const char *chFilename, GLuint *puiTexID)
{
 unsigned char uchBMPHeader[54];
 unsigned char uchIntensity[256];
 bool          bTrueColor;

  // first, load up the bitmap file header
  unsigned int uiFile = pcMedia->OpenFile(chFilename);
  if (uiFile == 0)
    {
    printf("CTextures Error: Couldn't open font file: %s\n", chFilename);
    return false;
    }
  if (pcMedia->ReadBytes(uiFile, uchBMPHeader, 54) != 54)
    {
    printf("CTextures Error: Couldn't read 54 byte header from BMP texture file\n");
    return false;
    }
  // check the header
  unsigned int uiDataOffset = (uchBMPHeader[0xd] << 24) | (uchBMPHeader[0xc] << 16) | (uchBMPHeader[0xb] << 8) | uchBMPHeader[0xa];
  unsigned int uiHeaderSize = (uchBMPHeader[0x11] << 24) | (uchBMPHeader[0x10] << 16) | (uchBMPHeader[0xf] << 8) | uchBMPHeader[0xe];
  unsigned int uiWidth  = (uchBMPHeader[0x15] << 24) | (uchBMPHeader[0x14] << 16) | (uchBMPHeader[0x13] << 8) | uchBMPHeader[0x12];
  int iHeight = (uchBMPHeader[0x19] << 24) | (uchBMPHeader[0x18] << 16) | (uchBMPHeader[0x17] << 8) | uchBMPHeader[0x16];
  unsigned int uiPlanes = (uchBMPHeader[0x1b] << 8) | uchBMPHeader[0x1a];
  unsigned int uiBPP    = (uchBMPHeader[0x1d] << 8) | uchBMPHeader[0x1c];
  if (uchBMPHeader[0] != 'B' || uchBMPHeader[1] != 'M' || uiHeaderSize != 0x28 || uiPlanes != 1)
    {
    printf("CTextures Error: Invalid BMP file header!\n");
    return false;
    }
  // check file type (8-bit or 24-bit)
  int iStride;
  if (uiDataOffset == 0x436 && uiBPP == 8)
    {
    bTrueColor = false;
    iStride = (uiWidth + 3) & 0xfffffffc;
    }
  else if (uiDataOffset == 0x36 && uiBPP == 24)
    {
    bTrueColor = true;
    iStride = (uiWidth * 3 + 3) & 0xfffffffc;
    }
  else
    {
    printf("CTextures Error: Invalid BMP type: BitsPerPixel = %i and DataOffset = %i!\n", uiBPP, uiDataOffset);
    return false;
    }

  if (!bTrueColor)
    {
    // read palette data
    unsigned char uchPalette[1024];
    if (pcMedia->ReadBytes(uiFile, uchPalette, 1024) != 1024)
      {
      printf("CTextures Error: Couldn't read 1024 byte palette from BMP texture file\n");
      return false;
      }
    // calculate intensity for each palette index
    for (int iColor = 0; iColor < 256; iColor++)
      {
      int iIntensity = (int) uchPalette[iColor << 2];
      iIntensity += (int) uchPalette[(iColor << 2) + 1];
      iIntensity += (int) uchPalette[(iColor << 2) + 2];
      uchIntensity[iColor] = (unsigned char) (iIntensity / 3);
      }
    }
  
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
    printf("CTextures Error: Unable to allocate %i bytes for texture file image. Height=%i, Width=%i, Stride=%i\n", iStride * uiHeight, uiHeight, uiWidth, iStride);
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
      printf("CTextures Error: Unable to load line #%i from font file.\n", i);
      free(pucPicBuffer);
      return false;
      }
    if (bTopDown)
      pucLoad += iStride;
    else
      pucLoad -= iStride;
    }

  // convert the BMP data
  if (!bTrueColor)
    {
    // for architectures which don't support GL_LUMINANCE, we need to make this into an RGBA buffer
    unsigned int uiExpansion = 2;
    if (!CGameMain::m_bUseLuminance)
      {
      bTrueColor = true;
      uiExpansion = 4;
      }
    // we will make our own alpha channel
    unsigned char *pucLABuffer = (unsigned char *) malloc(uiWidth * uiHeight * uiExpansion);
    if (!pucLABuffer)
      {
      printf("CTextures Error: Unable to allocate %i bytes for texture Lum-Alpha image. Height=%i, Width=%i\n", uiWidth * uiHeight * 2, uiHeight, uiWidth);
      free(pucPicBuffer);
      return false;
      }
    memset(pucLABuffer, 0, uiWidth * uiHeight * uiExpansion);
    // convert the data from palette indices to intensity values and duplicate for alpha
    unsigned char *pucSrc = pucPicBuffer;
    unsigned char *pucDst = pucLABuffer;
    for (unsigned int uy = 0; uy < uiHeight; uy++)
      {
      for (unsigned int ux = 0; ux < uiWidth; ux++)
        {
        unsigned char uch = uchIntensity[*pucSrc++];
        *pucDst++ = uch;
        *pucDst++ = uch;
        if (bTrueColor)
          {
          *pucDst++ = uch;
          *pucDst++ = uch;
          }
        }
      pucSrc += iStride - uiWidth;
      }
    // get rid of the old buffer
    free(pucPicBuffer);
    pucPicBuffer = pucLABuffer;
    }
  else
    {
    // we will make our own alpha channel
    unsigned char *pucRGBABuffer = (unsigned char *) malloc(uiWidth * uiHeight * 4);
    if (!pucRGBABuffer)
      {
      printf("CTextures Error: Unable to allocate %i bytes for texture RGBA image. Height=%i, Width=%i\n", uiWidth * uiHeight * 4, uiHeight, uiWidth);
      free(pucRGBABuffer);
      return false;
      }
    // red, green, and blue are reversed
    unsigned char *pucOut = pucRGBABuffer;
    for (unsigned int iY = 0; iY < uiHeight ; iY++)
      {
      unsigned char *pucIn = pucPicBuffer + iY * iStride;
      for (unsigned int iX = 0; iX < uiWidth; iX++)
        {
        pucOut[2] = pucIn[0];
        pucOut[1] = pucIn[1];
        pucOut[0] = pucIn[2];
        if (pucIn[0] > 5 || pucIn[1] > 5 || pucIn[2] > 5)
          pucOut[3] = 255;
        else
          pucOut[3] = 0;
        pucIn += 3;
        pucOut += 4;
        }
      }
    // get rid of the old buffer
    free(pucPicBuffer);
    pucPicBuffer = pucRGBABuffer;
    }

  // create the texture
  glGenTextures(1, puiTexID);
  if (bTrueColor)
    {
    // create an OpenGL texture from this RGB bitmap
    glBindTexture(GL_TEXTURE_2D, *puiTexID);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, uiWidth, uiHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pucPicBuffer);
    }
  else
    {
    // create an OpenGL texture from this Intensity bitmap
    glBindTexture(GL_TEXTURE_2D, *puiTexID);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, uiWidth, uiHeight, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, pucPicBuffer);
    }

  // free our memory and leave
  free(pucPicBuffer);
  return true;
}
