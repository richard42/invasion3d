/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Invader20.h                                              *
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


#if !defined(INVADER20_H)
#define INVADER20_H

#ifdef WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>
#include "Invader.h"

class CInvader20 : public CInvader
{
public:
  CInvader20();
  ~CInvader20();

  // function to initialize the static model
  static bool InitModel(void);
  static void DestroyModel(void);

  // accessor functions
  void Draw(void) const;
  EInvaderType GetType(void) const { return E_INVADER_20; }
  unsigned int CountVertices(void) const  { return m_ciTotalVertices; }
  unsigned int CountQuads(void) const     { return m_ciBodySections * m_ciRadialPieces + 10 + 18 + 26; }
  unsigned int CountTriangles(void) const { return 2 * m_ciRadialPieces; }
  void GetShotPosition(float *pfX, float *pfY) const { *pfX = m_fPositionX; *pfY = m_fPositionY + 2 * m_ciBodyHeight / 3; }
  void GetBodyBounding(float *fL, float *fR, float *fT, float *fB) const;
  void GetParticlePolys(SPolys *psPoly) const;

  // modifier functions
  void SetBodyColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue);
  void SetAntColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue);
  void SetEyeColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue);
  void SetArmColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue);
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
  unsigned char m_uchBodyColor[3];
  unsigned char m_uchAntColor[3];
  unsigned char m_uchEyeColor[3];
  unsigned char m_uchArmColor[3];
  unsigned char m_uchLegColor[3];

  // private functions
  static void BuildLegs(float (*pfVertex)[3], float (*pfNormal)[3], bool bLegPosition);

  // const model parameters
  static const int m_ciBodyWidth;
  static const int m_ciBodyHeight;
  static const int m_ciBodyDepth;
  static const int m_ciRadialPieces;
  static const int m_ciBodySections;
  static const int m_ciEyeSeparation;
  static const int m_ciEyeWidth;
  static const int m_ciEyeHeight;
  static const int m_ciAntSeparation;
  static const int m_ciAntLength;
  static const int m_ciAntThickness;
  static const int m_ciArmThickness;
  static const int m_ciLegThickness;
  // calculated parameters
  static const int m_ciTotalVertices;
  static const int m_ciEyeQuads;
  static const int m_ciBodyPairs;

  // static model data
  static GLfloat (*glfVertex)[3];
  static GLfloat (*glfNormal)[3];
};

#endif // !defined(INVADER20_H)
