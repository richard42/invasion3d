/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - InvaderMystery.h                                         *
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


#if !defined(INVADERMYSTERY_H)
#define INVADERMYSTERY_H

#ifdef WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>
#include "Invader.h"

class CInvaderMystery : public CInvader
{
public:
  CInvaderMystery();
  ~CInvaderMystery();

  // function to initialize the static model
  static bool InitModel(void);
  static void DestroyModel(void);

  // accessor functions
  void Draw(void) const;
  EInvaderType GetType(void) const { return E_INVADER_MYSTERY; }
  unsigned int CountVertices(void) const  { return m_ciTotalVertices; }
  unsigned int CountQuads(void) const     { return m_ciBodySlices * m_ciRadialPieces + 5; }
  unsigned int CountTriangles(void) const { return m_ciLandingGears * m_ciGearPieces + 2 * m_ciRadialPieces; }
  void GetShotPosition(float *pfX, float *pfY) const { *pfX = m_fPositionX; *pfY = m_fPositionY + m_ciGearHeight / 2; }
  void GetParticlePolys(SPolys *psPoly) const;

  // modifier functions
  void SetBodyColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue);
  void SetWindowColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue);
  void SetGearColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue);
  void SetCenterColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue);
  bool TestCollision(float fLeftX, float fRightX, float fTopY, float fBotY, int iDamage);
  void SetScale(float fScale) { m_fScale = fScale; }

private:
  // member data
  unsigned char m_uchBodyColor[3];
  unsigned char m_uchWindowColor[3];
  unsigned char m_uchGearColor[3];
  unsigned char m_uchCenterColor[3];
  float         m_fScale;

  // const model parameters
  static const int m_ciBodyWidth;
  static const int m_ciBodyHeight;
  static const int m_ciRadialPieces;
  static const int m_ciBodySlices;
  static const int m_ciWindows;
  static const int m_ciWindowHeight;
  static const int m_ciLandingGears;
  static const int m_ciGearPieces;
  static const int m_ciGearHeight;
  static const int m_ciGearSeparation;
  static const int m_ciCenterHeight;
  static const int m_ciCenterWidth;
  // calculated parameters
  static const int m_ciTotalVertices;

  // static model data
  static GLfloat (*glfVertex)[3];
  static GLfloat (*glfNormal)[3];
};

#endif // !defined(INVADERMYSTERY_H)
