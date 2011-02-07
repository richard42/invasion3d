/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Textures.h                                               *
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


#if !defined(TEXTURES_H)
#define TEXTURES_H

#ifdef WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>

class CPackage;

// enumerated type for weapon selection
typedef enum {E_TEX_LASER = 0,
              E_TEX_BULLET,
              E_TEX_CANNONBALL,
              E_TEX_INVADER,
              E_TEX_PARTICLE,
              E_TEX_MAME,
              E_TEX_SHOCKWAVE,
              E_TEX_INVALID} ETexture;

class CTextures
{
public:
  CTextures()  {}
  ~CTextures() {}

  // function to initialize the static data
  static bool InitTextures(CPackage *pcMedia);
  static void DestroyTextures(void);

  // accessor functions
  static GLuint GetTextureID(ETexture eTexture);

private:
  // member data
  static GLuint  m_iTextureID[E_TEX_INVALID];
  static char    m_chFilenames[E_TEX_INVALID][32];
  static bool    m_bInitialized;

  // private functions
  static bool    LoadTexture(CPackage *pcMedia, const char *chFilename, GLuint *puiTexID);

};

#endif // defined(TEXTURES_H)
