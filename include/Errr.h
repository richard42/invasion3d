/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Errr.h                                                   *
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

#if !defined(ERRR_H)
#define ERRR_H

#ifdef WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>
#include "Invader.h"

#define ERRR_BODY_QUADS 16
#define ERRR_ARM_QUADS  60
#define ERRR_FACE_QUADS 13
#define ERRR_QUADS (ERRR_BODY_QUADS + ERRR_ARM_QUADS + ERRR_FACE_QUADS)

class CErrr : public CInvader
{
public:
  CErrr();
  ~CErrr();

  // accessor functions
  void Draw(void) const;
  EInvaderType GetType(void) const { return E_INVADER_ERRR; }
  unsigned int CountVertices(void) const  { return ERRR_QUADS * 4; }
  unsigned int CountQuads(void) const     { return ERRR_QUADS; }
  unsigned int CountTriangles(void) const { return 0; }
  void GetShotPosition(float *pfX, float *pfY) const { *pfX = m_fPositionX; *pfY = m_fPositionY + 60; }
  void GetParticlePolys(SPolys *psPoly) const;

  // modifier functions
  void SetBodyColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue);
  void SetArmColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue);
  void SetAlpha(unsigned char uchAlpha) { m_iAlpha = (int) uchAlpha << 23; }
  void SetArmState(bool bRaised);
  void SetLegs(bool bLeftUp, bool bRightUp);
  void SetMouthState(bool bWide);
  bool TestCollision(float fLeftX, float fRightX, float fTopY, float fBotY, int iDamage);

private:
  // member data
  unsigned char m_uchBodyColor[3];
  unsigned char m_uchArmColor[3];
  int           m_iAlpha;

  // const model parameters
  static const int m_ciThickness;
};

#endif // !defined(ERRR_H)
