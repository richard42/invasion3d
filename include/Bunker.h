/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Bunker.h                                                 *
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

#if !defined(BUNKER_H)
#define BUNKER_H

#ifdef WIN32
  #include <windows.h>
#endif
#if defined(__APPLE__)
  #include <OpenGL/gl.h>
#else
  #include <GL/gl.h>
#endif

#include "Invader.h"

#define BUNKER_QUADS 18

class CBunker
{
public:
  CBunker();
  ~CBunker();

  // accessor functions
  void Draw(void) const;
  void GetBounding(float *pfLeftX, float *pfRightX, float *pfTopY, float *pfBotY) const;
  void GetTopSurface(float *pfLeftX, float *pfRightX, float *pfTopY) const;
  bool MustDraw(void) const { return (m_iHealth > 0 || m_iFadeOut > 0); }
  bool Alive(void) const { return (m_iHealth > 0); }

  // modifier functions
  void SetColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue);
  void SetPosition(float fX, float fY, float fZ);
  void SetFullHealth(int iWaveNumber);
  bool TestCollision(float fLeftX, float fRightX, float fTopY, float fBotY, int iDamage);
  void HandleState(void);

private:
  // member data
  unsigned char m_uchColor[4];
  unsigned char m_uchDrawColor[4];
  float         m_fPositionX;
  float         m_fPositionY;
  float         m_fPositionZ;
  bool          m_bFlash;
  int           m_iHealth;
  int           m_iLastHealth;
  int           m_iFullHealth;
  int           m_iFadeOut;

  // private functions
  void TakeDamage(int iDamage);

  // member model data
  GLfloat m_afQuads[BUNKER_QUADS][4][3];
  GLfloat m_afNormals[BUNKER_QUADS][4][3];

  // const model parameters
  static const int m_ciWidth;
  static const int m_ciHeight;

};

#endif // !defined(BUNKER_H)
