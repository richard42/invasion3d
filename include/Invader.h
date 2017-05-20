/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Invader.h                                                *
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


#if !defined(INVADER_H)
#define INVADER_H

#ifdef WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>

#include "Gameplay.h"
#include "Particles.h"

class CWeapons;

// mathematical definitions
#if !defined(M_PI)
  #define M_PI   3.14159265358979
  #define M_SQR2 1.414213562373095
#endif

typedef enum {E_INVADER_10 = 0,
              E_INVADER_20,
              E_INVADER_30,
              E_INVADER_MYSTERY,
              E_INVADER_ERRR,
              E_INVADER_IGNIGNOKT} EInvaderType;

// virtual CInvader class
class CInvader
{
public:
  CInvader();
  ~CInvader();

  // accessor functions
  virtual void Draw(void) const = 0;
  virtual EInvaderType GetType(void) const = 0;
  virtual unsigned int CountVertices(void) const = 0;
  virtual unsigned int CountQuads(void) const = 0;
  virtual unsigned int CountTriangles(void) const = 0;
  virtual void GetParticlePolys(SPolys *psPoly) const = 0;
  virtual void GetShotPosition(float *pfX, float *pfY) const = 0;
  virtual void GetBodyBounding(float *fL, float *fR, float *fT, float *fB) const;
  bool Alive(void) const { return (m_iHealth > 0); }
  const float *GetVertices(void) const { return (float *) m_pfVertex; }
  void GetVelocity(float *pfX, float *pfY) const { *pfX = m_fVelocityX; *pfY = m_fVelocityY; }
  void GetPosition(float *pfX, float *pfY, float *pfZ) const;
  void GetDesiredPosition(float *pfX, float *pfY) const;
  bool CanFireShot(void) const;
  bool GetShockHit(void) const { return m_bShockHit; }
  void ModulateColor(GLint *piDiffuse, GLint *piAmbient, const unsigned char *pucColor) const;

  // modifier functions
  void SetShockHit(bool bHit) { m_bShockHit = bHit; }
  void SetFullHealth(int iWaveNumber);
  void SetPosition(float fX, float fY, float fZ);
  void SetAxis(float fAngleX, float fAngleY, float fAngleZ);
  void SetRotation(float fAngle);
  void SetFlipAngle(float fAngle) { m_fFlipAngle = fAngle; }
  void SetSpinAngle(float fAngle) { m_fSpinAngle = fAngle; }

  void SetDesiredPosition(float fX, float fY);
  void ChangeDesiredPosition(float fX, float fY);
  void ApplyForce(float fX, float fY) { m_fVelocityX += fX; m_fVelocityY += fY; }
  void ActivateMotion(bool bActivate);
  void ProcessState(unsigned int uiFrameTime);
  void FireShot(CWeapons *pcWeapons);
  bool TestCollision(CInvader *pcInvader);
  virtual bool KamikazeCollision(float fLeftX, float fRightX, float fTopY, float fBotY) { return false; }
  virtual bool BunkerCollision(float fLeftX, float fRightX, float fTopY) { return false; }
  virtual bool TestCollision(float fLeftX, float fRightX, float fTopY, float fBotY, int iDamage) = 0;
  virtual void ToggleLegs(bool bLegPosition) { return; }

  static bool SetupCollisionData(void);

protected:
  // helper functions
  static void GetNormal(float& fNx,  float& fNy,  float& fNz,
                        float  fP0x, float  fP0y, float  fP0z,
                        float  fP1x, float  fP1y, float  fP1z,
                        float  fP2x, float  fP2y, float  fP2z);
  static void TubeSetStart(float fPx, float fPy, float fPz, float fDirDegrees, float fWidth);
  static void TubeMove(float (*pfVertex)[3], float (*pfNormal)[3], float fLength, float fEndDirDegrees, bool bEndCap);
  static void ZRibbonStart(float fX, float fY, float fZ, float fThickness);
  static void ZRibbonMove(float (*pfQuads)[4][3], float (*pfNorms)[4][3], float fDx, float fDy); // must go clockwise
  static void BuildBox(float (*pfQuads)[4][3], float (*pfNorms)[4][3], float fX0, float fY0, float fX1, float fY1, float fThickness);
  static void BuildSquare(float (*pfQuads)[4][3], float (*pfNorms)[4][3], float fX0, float fY0, float fX1, float fY1, float fZ);
  static void BuildPanes(float (*pfQuads)[4][3], float (*pfNorms)[4][3], float fX0, float fY0, float fX1, float fY1, float fThickness);
  static bool GenerateCollisionArrays(int iPoints1, float *pfOutlineX1, float *pfOutlineY1, int iPoints2, float *pfOutlineX2, float *pfOutlineY2, float *pfMaxRadius, float *pfNormX, float *pfNormY);


  // protected member data
  float m_fPositionX;
  float m_fPositionY;
  float m_fPositionZ;
  float m_fFlipAngle;
  float m_fSpinAngle;
  float m_fAngle;
  float m_fAngleX;
  float m_fAngleY;
  float m_fAngleZ;
  float m_bFlash;
  int   m_iFullHealth;
  int   m_iLastHealth;
  int   m_iHealth;
  bool  m_bShockHit;

  // motion data
  bool         m_bMotionActive;
  float        m_fVelocityX;
  float        m_fVelocityY;
  float        m_fDesiredX;
  float        m_fDesiredY;
  float        m_fErrorX;
  float        m_fErrorY;
  float        m_fErrorHistoryX[256];
  float        m_fErrorHistoryY[256];
  unsigned int m_uiTimeHistory[256];
  int          m_iHistoryHead;
  int          m_iHistoryTail;
  unsigned int m_uiTimeSum;
  float        m_fErrorSumX;
  float        m_fErrorSumY;
  float        m_fPTerm, m_fITerm, m_fDTerm;
  float        m_fMaxThrust;

  // shot data
  int   m_iShotTimer;

  GLfloat (*m_pfVertex)[3];
  GLfloat (*m_pfNormal)[3];

private:
  // static data
  static float m_fRibbonX, m_fRibbonY, m_fRibbonZ0, m_fRibbonZ1;
  static float m_fTubeCoords[2][3];
  static float m_fTubeCenter[3];
  static float m_fTubeWidth;
  static float m_fTubeDirDegrees;

  // static data for collisions between invaders
  static float m_fCollideRadius[3][3][360];
  static float m_fCollideNormX[3][3][360];  // these normals use screen (world) coordinates, not
  static float m_fCollideNormY[3][3][360];  // trigonometric coordinates
};

#endif // !defined(INVADER_H)
