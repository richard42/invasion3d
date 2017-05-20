/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Package.cpp                                              *
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
**                              05/20/05                                   **
**                                                                         **
** Package.cpp - implementation of class CPackage - loads a package file   **
**               consisting of multiple media files and allows access      **
**                                                                         **
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Package.h"

/////////////////////////////////////////////////////////////////////////////
// CPackage constructor and destructor

CPackage::CPackage()
{
  m_psDirectory = NULL;
  m_uiFiles     = 0;
}

CPackage::~CPackage()
{
  Unload();
}


/////////////////////////////////////////////////////////////////////////////
// CPackage accessor functions

unsigned int CPackage::Length(unsigned int uiFile) const
{
  // check input state
  if (m_psDirectory == NULL || m_uiFiles < 1 || uiFile < 1 || uiFile > m_uiFiles) return 0;

  return m_psDirectory[uiFile - 1].uiFileLength;
}


/////////////////////////////////////////////////////////////////////////////
// CPackage modifier functions

bool CPackage::LoadPackage(char *pccPackagename)
{
  SHeader sHeader;

  // if we already have a package loaded, then unload it
  if (m_psDirectory || m_uiFiles > 0) Unload();

  // open the file and read the header
  FILE *pfIn = fopen(pccPackagename, "rb");
  if (!pfIn)
    { /* fail to open file, so lets try different location if defined */
      if (strcmp(BIN_DIR,MEDIA_DIR)!=0)  /* data is not in same location as binary, change packagename and try to load it */
      {
        strcpy(pccPackagename,MEDIA_DIR);
        strcat(pccPackagename,"/Invaders.dat");
        pfIn = fopen(pccPackagename, "rb");
      }
    }
  if (!pfIn)
    {
    printf("LoadPackage error: couldn't open file \"%s\" for reading.\n", pccPackagename);
    return false;
    }
  if (fread(&sHeader, 1, sizeof(SHeader), pfIn) != sizeof(SHeader))
    {
    printf("LoadPackage error: couldn't read %i bytes from file \"%s\".\n", (int) sizeof(SHeader), pccPackagename);
    fclose(pfIn);
    return false;
    }

  // check the header
  if (strncmp(sHeader.chTag, "PackitUP", 8) != 0 || sHeader.uiVersion != 0x1010)
    {
    printf("LoadPackage error: invalid header!\n");
    fclose(pfIn);
    return false;
    }

  // allocate memory for directory entries and load them
  size_t uiDirSize = sHeader.uiDirEntries * sizeof(SDirEntry);
  m_psDirectory = (SDirEntry *) malloc(uiDirSize);
  if (!m_psDirectory)
    {
    printf("LoadPackage error: couldn't allocate %i bytes for directory.\n", (int) uiDirSize);
    fclose(pfIn);
    return false;
    }

  // read and translate directory entries
  SDirEntry32 sTempDir;
  for (unsigned int ui = 0; ui < sHeader.uiDirEntries; ui++)
    {
    // read single directory entry
    if (fread(&sTempDir, 1, sizeof(SDirEntry32), pfIn) != sizeof(SDirEntry32))
      {
      printf("LoadPackage error: couldn't read %i directory bytes from file \"%s\".\n", (int) sizeof(SDirEntry32), pccPackagename);
      fclose(pfIn);
      free(m_psDirectory);
      m_psDirectory = NULL;
      return false;
      }
    // translate directory entry to allow for different sized pointers (64-bit)
    memcpy(m_psDirectory[ui].chFilename, sTempDir.chFilename, 64);
    m_psDirectory[ui].uiFileLength = sTempDir.uiFileLength;
    m_psDirectory[ui].uiCRC32 = sTempDir.uiCRC32;
    m_psDirectory[ui].pData = NULL;
    m_psDirectory[ui].uiIndex = sTempDir.uiIndex;
    }

  // allocate memory for each data piece
  m_uiFiles = sHeader.uiDirEntries;
  for (unsigned int ui = 0; ui < m_uiFiles; ui++)
    {
    m_psDirectory[ui].uiIndex = 0;
    m_psDirectory[ui].pData = malloc(m_psDirectory[ui].uiFileLength);
    if (!m_psDirectory[ui].pData)
      {
      printf("LoadPackage error: couldn't allocate %i data bytes.\n", m_psDirectory[ui].uiFileLength);
      fclose(pfIn);
      Unload();
      return false;
      }
    }

  // load in each piece of the data file
  for (unsigned int ui = 0; ui < m_uiFiles; ui++)
    {
    if (fread(m_psDirectory[ui].pData, 1, m_psDirectory[ui].uiFileLength, pfIn) != m_psDirectory[ui].uiFileLength)
      {
      printf("LoadPackage error: couldn't read %i data bytes.\n", m_psDirectory[ui].uiFileLength);
      fclose(pfIn);
      Unload();
      return false;
      }
    }

  // close data file
  fclose(pfIn);

  // check all CRCs
  for (unsigned int ui = 0; ui < m_uiFiles; ui++)
    {
    if (m_psDirectory[ui].uiCRC32 != CRC32(m_psDirectory[ui].pData, m_psDirectory[ui].uiFileLength))
      {
      printf("LoadPackage error: CRC32 failed for file \"%s\".\n", m_psDirectory[ui].chFilename);
      return false;
      }
    }

  // everything is great!
  return true;
}

void CPackage::Unload(void)
{
  if (!m_psDirectory) return;

  // free all memory
  for (unsigned int ui = 0; ui < m_uiFiles; ui++)
    {
    if (m_psDirectory[ui].pData != NULL)
      free(m_psDirectory[ui].pData);
    }
  free(m_psDirectory);

  // set member variables to zero
  m_psDirectory = NULL;
  m_uiFiles     = 0;
}

unsigned int CPackage::OpenFile(const char *pccFilename)
{
  // check input state
  if (m_psDirectory == NULL || m_uiFiles < 1) return 0;

  // search for this filename in my directory
  for (unsigned int ui = 0; ui < m_uiFiles; ui++)
    {
    if (strcmp(m_psDirectory[ui].chFilename, pccFilename) == 0)
      {
      // file is found - set index pointer to 0
      m_psDirectory[ui].uiIndex = 0;
      // return with the directory index plus one
      return ui + 1;
      }
    }

  // not found
  return 0;
}

unsigned int CPackage::ReadBytes(unsigned int uiFile, void *pvData, unsigned int uiBytes)
{
  // check input state
  if (!pvData) return 0;
  if (m_psDirectory == NULL || m_uiFiles < 1 || uiFile < 1 || uiFile > m_uiFiles) return 0;

  // change file number to directory index
  uiFile--;

  // make sure we don't read past the end of the data
  if (uiBytes > m_psDirectory[uiFile].uiFileLength - m_psDirectory[uiFile].uiIndex)
    {
    uiBytes = m_psDirectory[uiFile].uiFileLength - m_psDirectory[uiFile].uiIndex;
    }

  // copy the data
  memcpy(pvData, (char *) m_psDirectory[uiFile].pData + m_psDirectory[uiFile].uiIndex, uiBytes);

  // adjust the index pointer
  m_psDirectory[uiFile].uiIndex += uiBytes;

  // return the number of bytes copied
  return uiBytes;
}

void CPackage::SkipBytes(unsigned int uiFile, unsigned int uiBytes)
{
  // check input state
  if (m_psDirectory == NULL || m_uiFiles < 1 || uiFile < 1 || uiFile > m_uiFiles) return;

  // change file number to directory index
  uiFile--;

  // make sure we don't skip past the end of the data
  if (uiBytes > m_psDirectory[uiFile].uiFileLength - m_psDirectory[uiFile].uiIndex)
    {
    uiBytes = m_psDirectory[uiFile].uiFileLength - m_psDirectory[uiFile].uiIndex;
    }

  // adjust the index pointer
  m_psDirectory[uiFile].uiIndex += uiBytes;
}


/////////////////////////////////////////////////////////////////////////////
// private helper functions

unsigned int CPackage::CRC32(const void *pvData, unsigned int uiDataBytes)
{
    unsigned int         uiReg   = 0xffffffff;
    unsigned int         uiPoly  = 0x04c11db7;
    const unsigned char *pchData = (const unsigned char *) pvData;

    for (unsigned int ui = 0; ui < uiDataBytes; ui++)
      {
      unsigned char ucShift = *pchData++;
      for (unsigned int uj = 0; uj < 8; uj++)
        {
        if (((uiReg >> 31) ^ (ucShift >> 7)) & 1)
          {
          uiReg = (uiReg << 1) ^ uiPoly;
          }
        else
          {
          uiReg <<= 1;
          }
        ucShift <<= 1;
        }
      }

    return uiReg;
}
