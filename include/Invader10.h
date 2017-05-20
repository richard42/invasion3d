/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Invader10.h                                              *
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


#if !defined(INVADER10_H)
#define INVADER10_H

#ifdef WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>
#include "Invader.h"

class CInvader10 : public CInvader
{
public:
  CInvader10();
  ~CInvader10();

  // function to initialize the static model
  static bool InitModel(void);
  static void DestroyModel(void);

  // accessor functions
  void Draw(void) const;
  EInvaderType GetType(void) const { return E_INVADER_10; }
  unsigned int CountVertices(void) const  { return m_ciTotalVertices; }
  unsigned int CountQuads(void) const     { return m_ciRadialPieces + 9 + 10 + 30; }
  unsigned int CountTriangles(void) const { return 2 * m_ciRadialPieces; }
  void GetShotPosition(float *pfX, float *pfY) const { *pfX = m_fPositionX; *pfY = m_fPositionY + ((m_ciCapHeight + m_ciHeadHeight) >> 1); }
  void GetBodyBounding(float *fL, float *fR, float *fT, float *fB) const;
  void GetParticlePolys(SPolys *psPoly) const;

  // modifier functions
  void SetCapColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue);
  void SetHeadColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue);
  void SetEyeColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue);
  void SetLegColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue);
  void ToggleLegs(bool bLegPosition);
  bool KamikazeCollision(float fLeftX, float fRightX, float fTopY, float fBotY);
  bool BunkerCollision(float fLeftX, float fRightX, float fTopY);
  bool TestCollision(float fLeftX, float fRightX, float fTopY, float fBotY, int iDamage);

  // static data for collisions between invaders
  static int    m_iOutlinePoints;
  static float *m_pfOutlineX;
  static float *m_pfOutlineY;

private:
  // member data
  unsigned char m_uchCapColor[3];
  unsigned char m_uchHeadColor[3];
  unsigned char m_uchEyeColor[3];
  unsigned char m_uchLegColor[3];

  // private functions
  static void BuildLegs(float (*pfVertex)[3], float (*pfNormal)[3], bool bLegPosition);

  // const model parameters
  static const int m_ciCapWidth;
  static const int m_ciCapHeight;
  static const int m_ciCapDepth;
  static const int m_ciHeadHeight;
  static const int m_ciRadialPieces;
  static const int m_ciEyeSeparation;
  static const int m_ciEyeWidth;
  static const int m_ciEyeHeight;
  static const int m_ciEyeDepth;
  static const int m_ciLegThickness;
  static const int m_ciLegSeparation;
  static const int m_ciLegHeight;
  static const int m_ciFootLength;
  // calculated parameters
  static const int m_ciTotalVertices;

  // static model data
  static GLfloat (*glfVertex)[3];
  static GLfloat (*glfNormal)[3];
};

#endif // !defined(INVADER10_H)
