/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - LaserBase.h                                              *
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


#if !defined(LASERBASE_H)
#define LASERBASE_H

#ifdef WIN32
  #include <windows.h>
#endif
#if defined(__APPLE__)
  #include <OpenGL/gl.h>
#else
  #include <GL/gl.h>
#endif

// mathematical definitions
#if !defined(M_PI)
  #define M_PI   3.14159265358979
  #define M_SQR2 1.414213562373095
#endif

#define BASE_QUADS       6
#define BASE_QSTRIP      40
#define MAX_WEAPON_VERT  128

// enumerated type for weapon selection
typedef enum {E_LASER = 0,
              E_GUN,
              E_DUAL_GUN,
              E_CANNON} EWeapon;

class CLaserBase
{
public:
  CLaserBase();
  ~CLaserBase();

  // accessor functions
  unsigned int CountVertices(void) const { return m_ciTotalVertices; }
  const float *GetVertices(void) const { return (float *) m_pfVertex; }
  void GetBounding(float *pfLeftX, float *pfRightX, float *pfTopY, float *pfBotY) const;
  void GetPosition(float *pfX, float *pfY, float *pfZ) const;
  void GetGunPosition1(float *pfX, float *pfY) const;
  void GetGunPosition2(float *pfX, float *pfY) const;
  bool ShieldActive(void) const { return m_bShield; }
  bool LinedUp(float fX) const { if (fX > m_fPositionX - m_ciBaseWidth / 2 && fX < m_fPositionX + m_ciBaseWidth / 2) return true; else return false; }
  void Draw(void) const;

  // modifier functions
  void SetPosition(float fX, float fY, float fZ);
  void SetBaseColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue);
  bool SetWeapon(EWeapon eType);
  void SetBarrelExtension(int iPercent);
  void SetBarrelRotation(int iDegrees);
  void SetShield(bool bActive) { m_bShield = bActive; }
  void RotateBarrel(int iDegrees);
  void PulseShield(void) { m_iShieldPulse = (m_iShieldPulse + 1) & 31; }
  bool TestCollision(float fLeftX, float fRightX, float fTopY, float fBotY);

private:
  // member data
  float         m_fPositionX;
  float         m_fPositionY;
  float         m_fPositionZ;
  unsigned char m_uchBaseColor[3];
  unsigned char m_uchWeaponColor[3];
  EWeapon       m_eWeaponType;
  int           m_iBarrelPercent;
  int           m_iBarrelDegrees;
  bool          m_bShield;
  int           m_iShieldPulse;

  float (*m_pfVertex)[3];
  float (*m_pfNormal)[3];

  // private functions
  void BuildWeapon(void);
  void BuildGun(int iIndex, int iSides, float fX, float fY, float fZ, float fRadius);

  // const model parameters
  static const int m_ciBaseWidth;
  static const int m_ciBaseHeight;
  static const int m_ciBaseDepth;
  // calculated parameters
  static const int m_ciTotalVertices;

};

#endif // !defined(LASERBASE_H)
