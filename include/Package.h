/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Package.h                                                *
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


#if !defined(PACKAGE_H)
#define PACKAGE_H

// structures used in the package file
typedef struct {
  char         chTag[8];
  unsigned int uiVersion;
  unsigned int uiDirEntries;
  } SHeader;

typedef struct {
  char          chFilename[64];
  unsigned int  uiFileLength;
  unsigned int  uiCRC32;
  void         *pData;
  unsigned int  uiIndex;
  } SDirEntry;

typedef struct {
  char          chFilename[64];
  unsigned int  uiFileLength;
  unsigned int  uiCRC32;
  unsigned int  p32Data;
  unsigned int  uiIndex;
  } SDirEntry32;

class CPackage
{
public:
  CPackage();
  ~CPackage();

  // accessor functions
  unsigned int Length(unsigned int uiFile) const;

  // modifier functions
  bool         LoadPackage(char *pccPackagename);
  void         Unload(void);
  unsigned int OpenFile(const char *pccFilename);
  unsigned int ReadBytes(unsigned int uiFile, void *pvData, unsigned int uiBytes);
  void         SkipBytes(unsigned int uiFile, unsigned int uiBytes);

private:
  // private member data
  unsigned int  m_uiFiles;
  SDirEntry    *m_psDirectory;

  // helper functions
  unsigned int CRC32(const void *pvData, unsigned int uiDataBytes);

};

#endif // !defined(PACKAGE_H)
