/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Invader.cpp                                              *
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

#include <stdlib.h>
#include <math.h>
#include "Invader.h"
#include "Invader10.h"
#include "Invader20.h"
#include "Invader30.h"
#include "Weapons.h"

// declare static data
float CInvader::m_fRibbonX;
float CInvader::m_fRibbonY;
float CInvader::m_fRibbonZ0;
float CInvader::m_fRibbonZ1;
float CInvader::m_fTubeCoords[2][3];
float CInvader::m_fTubeCenter[3];
float CInvader::m_fTubeWidth;
float CInvader::m_fTubeDirDegrees;

float CInvader::m_fCollideRadius[3][3][360];
float CInvader::m_fCollideNormX[3][3][360];  // these normals use screen (world) coordinates, not
float CInvader::m_fCollideNormY[3][3][360];  // trigonometric coordinates

/////////////////////////////////////////////////////////////////////////////
// CInvader constructor and destructor

CInvader::CInvader()
{
  // set position variables
  m_fPositionX = 0.0f;
  m_fPositionY = 0.0f;
  m_fPositionZ = 0.0f;
  m_fFlipAngle = 0.0f;
  m_fSpinAngle = 0.0f;
  m_fAngle     = 0.0f;
  m_fAngleX    = 0.0f;
  m_fAngleY    = 0.0f;
  m_fAngleZ    = 0.0f;

  // no flash, and set shot counter to 0
  m_bFlash     = false;
  m_iShotTimer = 0;

  // set health
  m_iHealth = 10;
  m_iFullHealth = 10;

  // set motion variables
  m_bMotionActive = false;
  m_fVelocityX    = 0.0f;
  m_fVelocityY    = 0.0f;
  m_fDesiredX     = 0.0f;
  m_fDesiredY     = 0.0f;
  m_fErrorX       = 0.0f;
  m_fErrorY       = 0.0f;
  m_fErrorSumX    = 0.0f;
  m_fErrorSumY    = 0.0f;
  for (int i = 0; i < 256; i++)
    {
    m_fErrorHistoryX[i] = 0.0f;
    m_fErrorHistoryY[i] = 0.0f;
    m_uiTimeHistory[i] = 0;
    }
  m_iHistoryHead  = 0;
  m_iHistoryTail  = 0;
  m_uiTimeSum     = 0;

  // PID control system
  m_fPTerm        = 0.0001f;
  m_fITerm        = 0.000002f;
  m_fDTerm        = 0.01f;
  m_fMaxThrust    = 0.016f;

  // set pointers to NULL
  m_pfVertex = NULL;
  m_pfNormal = NULL;
}

CInvader::~CInvader()
{
}

/////////////////////////////////////////////////////////////////////////////
// CInvader public static functions

bool CInvader::SetupCollisionData(void)
{
  // make calls to helper function to generate each part of the collision arrays
  if (!GenerateCollisionArrays(CInvader10::m_iOutlinePoints, CInvader10::m_pfOutlineX, CInvader10::m_pfOutlineY,
                              CInvader10::m_iOutlinePoints, CInvader10::m_pfOutlineX, CInvader10::m_pfOutlineY,
                              m_fCollideRadius[0][0], m_fCollideNormX[0][0], m_fCollideNormY[0][0])) return false;
  if (!GenerateCollisionArrays(CInvader10::m_iOutlinePoints, CInvader10::m_pfOutlineX, CInvader10::m_pfOutlineY,
                              CInvader20::m_iOutlinePoints, CInvader20::m_pfOutlineX, CInvader20::m_pfOutlineY,
                              m_fCollideRadius[0][1], m_fCollideNormX[0][1], m_fCollideNormY[0][1])) return false;
  if (!GenerateCollisionArrays(CInvader10::m_iOutlinePoints, CInvader10::m_pfOutlineX, CInvader10::m_pfOutlineY,
                              CInvader30::m_iOutlinePoints, CInvader30::m_pfOutlineX, CInvader30::m_pfOutlineY,
                              m_fCollideRadius[0][2], m_fCollideNormX[0][2], m_fCollideNormY[0][2])) return false;

  if (!GenerateCollisionArrays(CInvader20::m_iOutlinePoints, CInvader20::m_pfOutlineX, CInvader20::m_pfOutlineY,
                              CInvader10::m_iOutlinePoints, CInvader10::m_pfOutlineX, CInvader10::m_pfOutlineY,
                              m_fCollideRadius[1][0], m_fCollideNormX[1][0], m_fCollideNormY[1][0])) return false;
  if (!GenerateCollisionArrays(CInvader20::m_iOutlinePoints, CInvader20::m_pfOutlineX, CInvader20::m_pfOutlineY,
                              CInvader20::m_iOutlinePoints, CInvader20::m_pfOutlineX, CInvader20::m_pfOutlineY,
                              m_fCollideRadius[1][1], m_fCollideNormX[1][1], m_fCollideNormY[1][1])) return false;
  if (!GenerateCollisionArrays(CInvader20::m_iOutlinePoints, CInvader20::m_pfOutlineX, CInvader20::m_pfOutlineY,
                              CInvader30::m_iOutlinePoints, CInvader30::m_pfOutlineX, CInvader30::m_pfOutlineY,
                              m_fCollideRadius[1][2], m_fCollideNormX[1][2], m_fCollideNormY[1][2])) return false;

  if (!GenerateCollisionArrays(CInvader30::m_iOutlinePoints, CInvader30::m_pfOutlineX, CInvader30::m_pfOutlineY,
                              CInvader10::m_iOutlinePoints, CInvader10::m_pfOutlineX, CInvader10::m_pfOutlineY,
                              m_fCollideRadius[2][0], m_fCollideNormX[2][0], m_fCollideNormY[2][0])) return false;
  if (!GenerateCollisionArrays(CInvader30::m_iOutlinePoints, CInvader30::m_pfOutlineX, CInvader30::m_pfOutlineY,
                              CInvader20::m_iOutlinePoints, CInvader20::m_pfOutlineX, CInvader20::m_pfOutlineY,
                              m_fCollideRadius[2][1], m_fCollideNormX[2][1], m_fCollideNormY[2][1])) return false;
  if (!GenerateCollisionArrays(CInvader30::m_iOutlinePoints, CInvader30::m_pfOutlineX, CInvader30::m_pfOutlineY,
                              CInvader30::m_iOutlinePoints, CInvader30::m_pfOutlineX, CInvader30::m_pfOutlineY,
                              m_fCollideRadius[2][2], m_fCollideNormX[2][2], m_fCollideNormY[2][2])) return false;

  return true;
}

/////////////////////////////////////////////////////////////////////////////
// CInvader accessor functions

void CInvader::GetPosition(float *pfX, float *pfY, float *pfZ) const
{
  *pfX = m_fPositionX;
  *pfY = m_fPositionY;
  *pfZ = m_fPositionZ;
}

void CInvader::GetDesiredPosition(float *pfX, float *pfY) const
{
  *pfX = m_fDesiredX;
  *pfY = m_fDesiredY;
}

bool CInvader::CanFireShot(void) const
{
  if (fabs(m_fPositionY - m_fDesiredY) > 2.0f) return false;
  if (m_iShotTimer > 0) return false;
  return true;
}

// this is a dummy function for invaders other than 10, 20, and 30
void CInvader::GetBodyBounding(float *fL, float *fR, float *fT, float *fB) const
{
  *fL = 0.0f;
  *fR = 0.0f;
  *fT = 0.0f;
  *fB = 0.0f;
}

void CInvader::ModulateColor(GLint *piDiffuse, GLint *piAmbient, const unsigned char *pucColor) const
{
  // set this color according to our health

  float fHealth = (float) m_iHealth / (float) m_iFullHealth;
  float fDamage = 1.0f - fHealth;

  piDiffuse[0] = (int) (pucColor[0] * fHealth + 0xff * fDamage) << 23;
  piDiffuse[1] = (int) (pucColor[1] * fHealth) << 23;
  piDiffuse[2] = (int) (pucColor[2] * fHealth) << 23;
  piDiffuse[3] = 0x7fffffff;

  int iLShift;
  if (GetType() == E_INVADER_20)
    iLShift = 20;
  else
    iLShift = 21;

  piAmbient[0] = (int) (pucColor[0] * fHealth + 0xff * fDamage) << iLShift;
  piAmbient[1] = (int) (pucColor[1] * fHealth) << iLShift;
  piAmbient[2] = (int) (pucColor[2] * fHealth) << iLShift;
  piAmbient[3] = 0x7fffffff;
}

/////////////////////////////////////////////////////////////////////////////
// CInvader modifier functions

void CInvader::SetFullHealth(int iWaveNumber)
{
  // set health for my invader type according to wave number
  m_iFullHealth = CGameplay::m_iInvaderHealth[GetType()][iWaveNumber - 1];
  m_iHealth     = m_iFullHealth;

  // set shot counter to 0 so I can shoot right away
  m_iShotTimer = 0;

  // set our shockwave impact flag to false
  m_bShockHit = false;

  // clear the flip and spin angles (in case we went kamikaze before being resurrected)
  m_fFlipAngle = 0.0f;
  m_fSpinAngle = 0.0f;

  // clear the control system variables
  m_fVelocityX    = 0.0f;
  m_fVelocityY    = 0.0f;
  m_fErrorX       = 0.0f;
  m_fErrorY       = 0.0f;
  m_fErrorSumX    = 0.0f;
  m_fErrorSumY    = 0.0f;
  for (int i = 0; i < 32; i++)
    {
    m_fErrorHistoryX[i] = 0.0f;
    m_fErrorHistoryY[i] = 0.0f;
    m_uiTimeHistory[i] = 0;
    }
  m_iHistoryHead  = 0;
  m_iHistoryTail  = 0;
  m_uiTimeSum     = 0;

}

void CInvader::SetPosition(float fX, float fY, float fZ)
{
  m_fPositionX = fX;
  m_fPositionY = fY;
  m_fPositionZ = fZ;
}

void CInvader::SetAxis(float fAngleX, float fAngleY, float fAngleZ)
{
  m_fAngleX = fAngleX;
  m_fAngleY = fAngleY;
  m_fAngleZ = fAngleZ;
}

void CInvader::SetRotation(float fAngle)
{
  m_fAngle = fAngle;
}

void CInvader::SetDesiredPosition(float fX, float fY)
{
  m_fDesiredX = fX;
  m_fDesiredY = fY;
}

void CInvader::ChangeDesiredPosition(float fX, float fY)
{
  m_fDesiredX += fX;
  m_fDesiredY += fY;
}

void CInvader::ActivateMotion(bool bActivate)
{
  m_bMotionActive = bActivate;
}

void CInvader::FireShot(CWeapons *pcWeapons)
{
  // decide what shot to fire, the velocity of the shot, and reset the shot counter
  EProjectile eProj = E_PROJ_INVADER;
  float       fVelY = CGameplay::m_fProjSpeed[eProj];
  m_iShotTimer      = CGameplay::m_iReloadTime[eProj];

  // find the X and Y coordinates to shoot from
  float fX, fY;
  GetShotPosition(&fX, &fY);

  // create new projectile(s)
  pcWeapons->Add(eProj, fX, fY + CWeapons::m_fProjHalfHeight[eProj], fVelY);
}

void CInvader::ProcessState(unsigned int uiFrameTime)
{
  // un-flash
  m_bFlash = false;

  // decrement shot counter
  m_iShotTimer -= uiFrameTime;
  if (m_iShotTimer < 0) m_iShotTimer = 0;

  // process motion
  if (!m_bMotionActive) return;

  // the idea here is to implement a PID controller for each invader
  float fCurErrorX = m_fDesiredX - m_fPositionX;
  float fCurErrorY = m_fDesiredY - m_fPositionY;

  // start out the thrust using the proportional term
  float fThrustX = fCurErrorX * m_fPTerm;
  float fThrustY = fCurErrorY * m_fPTerm;

  // now add the derivative term
  fThrustX += (fCurErrorX - m_fErrorX) * m_fDTerm / uiFrameTime;
  fThrustY += (fCurErrorY - m_fErrorY) * m_fDTerm / uiFrameTime;

  // update last error values
  m_fErrorX = fCurErrorX;
  m_fErrorY = fCurErrorY;

  // move the tail of our buffer up until we have < 1/2 second of integrated error
  while (m_iHistoryHead != m_iHistoryTail && m_uiTimeSum > 500)
    {
    m_fErrorSumX  -= m_fErrorHistoryX[m_iHistoryTail];
    m_fErrorSumY  -= m_fErrorHistoryY[m_iHistoryTail];
    m_uiTimeSum   -= m_uiTimeHistory[m_iHistoryTail];
    m_iHistoryTail = (m_iHistoryTail + 1) & 255;
    }

  // integrate the current error
  m_fErrorSumX += fCurErrorX;
  m_fErrorSumY += fCurErrorY;
  m_uiTimeSum  += uiFrameTime;

  // update our history circular buffer
  m_fErrorHistoryX[m_iHistoryHead] = fCurErrorX;
  m_fErrorHistoryY[m_iHistoryHead] = fCurErrorY;
  m_uiTimeHistory[m_iHistoryHead]  = uiFrameTime;

  // advance the head of the circular buffer
  m_iHistoryHead = (m_iHistoryHead + 1) & 255;

  // lastly, add in the integral term
  fThrustX += m_fErrorSumX * m_fITerm / m_uiTimeSum;
  fThrustY += m_fErrorSumY * m_fITerm / m_uiTimeSum;

  // clip the thrust according to our maximum value
  if (fThrustX > m_fMaxThrust) fThrustX = m_fMaxThrust;
  else if (fThrustX < -m_fMaxThrust) fThrustX = -m_fMaxThrust;
  if (fThrustY > m_fMaxThrust) fThrustY = m_fMaxThrust;
  else if (fThrustY < -m_fMaxThrust) fThrustY = -m_fMaxThrust;

  // if our frame time is too high, cap it to prevent oscillation
  if (uiFrameTime > 100) uiFrameTime = 100;

  // adjust our velocity and position
  m_fVelocityX += fThrustX * uiFrameTime;
  m_fVelocityY += fThrustY * uiFrameTime;
  m_fPositionX += m_fVelocityX * uiFrameTime;
  m_fPositionY += m_fVelocityY * uiFrameTime;

  // all done
  return;
}

bool CInvader::TestCollision(CInvader *pcInvader)
{
  // get invader types
  EInvaderType eType1 = GetType();
  EInvaderType eType2 = pcInvader->GetType();

  // first make sure neither invader is a Mystery or Errr or Ignignokt
  if (eType1 > E_INVADER_30 || eType2 > E_INVADER_30) return false;

  // compute vector between invaders
  float fX1, fX2, fY1, fY2, fZ1, fZ2;
  GetPosition(&fX1, &fY1, &fZ1);
  pcInvader->GetPosition(&fX2, &fY2, &fZ2);
  float fVecX = fX2 - fX1;
  float fVecY = fY2 - fY1;

  // now compute distance and angle between invaders
  float fDist = sqrt(fVecX * fVecX + fVecY * fVecY);
  float fAngleRad;
  if (fVecY <= 0)
    fAngleRad = acos(fVecX / fDist);
  else
    fAngleRad = 2 * (float) M_PI - acos(fVecX / fDist);
  int iAngleDeg = (int) (180.0f * fAngleRad / (float) M_PI);

  // determine if there was a collision or not
  if (fDist >= m_fCollideRadius[eType1][eType2][iAngleDeg])
    return false;

  // there was a collision, so determine the angle of force
  float fForceX = m_fCollideNormX[eType1][eType2][iAngleDeg];
  float fForceY = m_fCollideNormY[eType1][eType2][iAngleDeg];

  // determine the velocities to impart to the invaders
  float fVelX1, fVelX2, fVelY1, fVelY2;
  GetVelocity(&fVelX1, &fVelY1);
  pcInvader->GetVelocity(&fVelX2, &fVelY2);
  float fDot1 = fForceX * fVelX1 + fForceY * fVelY1;
  float fDot2 = -(fForceX * fVelX2 + fForceY * fVelY2);

  // clip negative dot products to 0
  if (fDot1 < 0.0f) fDot1 = 0.0f;
  if (fDot2 < 0.0f) fDot2 = 0.0f;

  // impart the velocities
  ApplyForce(-fForceX * (fDot1 + fDot2), -fForceY * (fDot1 + fDot2));
  pcInvader->ApplyForce(fForceX * (fDot1 + fDot2), fForceY * (fDot1 + fDot2));

  // now put on the hurt
  int iDamage = (int) ((fDot1 + fDot2) * CGameplay::m_fCollideDamage);
  m_iLastHealth = m_iHealth;
  m_iHealth -= iDamage;
  pcInvader->m_iLastHealth = pcInvader->m_iHealth;
  pcInvader->m_iHealth -= iDamage;

  return true;
}

/////////////////////////////////////////////////////////////////////////////
// protected static helper functions

bool CInvader::GenerateCollisionArrays(int iPoints1, float *pfOutlineX1, float *pfOutlineY1,
                                       int iPoints2, float *pfOutlineX2, float *pfOutlineY2,
                                       float *pfMaxRadius, float *pfNormX, float *pfNormY)
{
  // check input parameters
  if (iPoints1 < 3 || iPoints2 < 3) return false;
  if (!pfOutlineX1 || !pfOutlineY1 || !pfOutlineX2 || !pfOutlineY2) return false;
  if (!pfMaxRadius || !pfNormX || !pfNormY) return false;

  // find the halfway point of the outline of the 2nd invader
  int iOutlineP1 = 0;
  int iOutlineP2;
  for (iOutlineP2 = 1; iOutlineP2 < iPoints2; iOutlineP2++)
    if (pfOutlineY2[iOutlineP2] == pfOutlineY2[0]) break;
  if (iOutlineP2 >= iPoints2) return false;  // no halfway point found

  // the first invader is stationary at (0,0)
  // the second invader moves counter-clockwise around the first
  float fInvaderPosX = pfOutlineX1[0] - pfOutlineX2[iOutlineP2];
  float fInvaderPosY = 0.0f;

  // set the very first distance and normal
  pfMaxRadius[0] = fInvaderPosX;
  pfNormX[0] = 1.0f;
  pfNormY[0] = 0.0f;

  // setup the 'next' points
  int iNextOutlineP1 = 1;
  int iNextOutlineP2 = (iOutlineP2 + 1) % iPoints2;

  // next angle to store, in degrees
  int iNextAngle = 1;

  // the main loop
  do
    {
    // sanity check
    if (iOutlineP1 >= iPoints1 || iNextOutlineP1 >= iPoints1) return false;
    if (iOutlineP2 >= iPoints2 || iNextOutlineP2 >= iPoints2) return false;
    // build vectors (in world space) from the two current surfaces
    float fVecX1 = pfOutlineX1[iNextOutlineP1] - pfOutlineX1[iOutlineP1];
    float fVecY1 = pfOutlineY1[iNextOutlineP1] - pfOutlineY1[iOutlineP1];
    float fVecX2 = pfOutlineX2[iOutlineP2] - pfOutlineX2[iNextOutlineP2];
    float fVecY2 = pfOutlineY2[iOutlineP2] - pfOutlineY2[iNextOutlineP2];
    // calculate trigonometric angles of two surfaces
    float fRad1 = sqrt(fVecX1 * fVecX1 + fVecY1 * fVecY1);
    float fRad2 = sqrt(fVecX2 * fVecX2 + fVecY2 * fVecY2);
    float fAngle1, fAngle2;
    if (fVecY1 <= 0)
      fAngle1 = acos(fVecX1 / fRad1);
    else
      fAngle1 = 2 * (float) M_PI - acos(fVecX1 / fRad1);
    if (fVecY2 <= 0)
      fAngle2 = acos(fVecX2 / fRad2);
    else
      fAngle2 = 2 * (float) M_PI - acos(fVecX2 / fRad2);
    // now decide which surface is being scraped
    int iScraped;
    float fAngleSpan = fAngle1 - fAngle2;
    if (fAngleSpan > M_PI) fAngleSpan -= 2.0f * (float) M_PI;
    if (fAngleSpan > 0.0f || fAngleSpan < (float) -M_PI)
      iScraped = 2;
    else
      iScraped = 1;
    // calculate normal for points during this scraping, and the vector to move the 2nd invader
    float fNormX, fNormY, fMoveX, fMoveY;
    if (iScraped == 1)
      {
      fNormY = fVecX1 / fRad1;
      fNormX = -fVecY1 / fRad1;
      fMoveX = fVecX1;
      fMoveY = fVecY1;
      }
    else
      {
      fNormY = fVecX2 / fRad2;
      fNormX = -fVecY2 / fRad2;
      fMoveX = fVecX2;
      fMoveY = fVecY2;
      }
    // do the scraping loop
    unsigned int uiSteps = (unsigned int) (sqrt(fMoveX * fMoveX + fMoveY * fMoveY) * 10.0f);
    for (unsigned int uiPartial = 1; uiPartial <= uiSteps; uiPartial++)
      {
      // calculate position along scraped line
      float fPartialX = fInvaderPosX + fMoveX * (float) uiPartial / (float) uiSteps;
      float fPartialY = fInvaderPosY + fMoveY * (float) uiPartial / (float) uiSteps;
      // calculate distance and angle
      float fDist = sqrt(fPartialX * fPartialX + fPartialY * fPartialY);
      float fAngle;
      if (fPartialY <= 0)
        fAngle = acos(fPartialX / fDist);
      else
        fAngle = 2 * (float) M_PI - acos(fPartialX / fDist);
      // convert angle to degrees
      fAngle *= 180.0f / (float) M_PI;
      // see if we have skipped an integer angle
      if ((int) fAngle >= iNextAngle + 1) return false; // too few steps
      // sanity check - are we going backwards?
      if (fAngle - (float) iNextAngle + 1.0f < 0.0f || fAngle - (float) iNextAngle + 1.0f > 180.0f)
        return false; // we shouldn't be going clockwise!
      // see if we need to record values for another angle
      if ((int) fAngle >= iNextAngle)
        {
        // sanity check
        if (iNextAngle >= 360) return false;
        // store (approximate) values for this angle
        pfMaxRadius[iNextAngle] = fDist;
        pfNormX[iNextAngle] = fNormX;
        pfNormY[iNextAngle] = fNormY;
        iNextAngle++;
        // if we're at the end, just leave otherwise we'll fail the sanity checks
        if (iNextAngle == 360)
          break;
        }
      }
    // move the invader
    fInvaderPosX += fMoveX;
    fInvaderPosY += fMoveY;
    // move to the next point on the scraped surface
    if (iScraped == 1)
      {
      iOutlineP1 = iNextOutlineP1;
      iNextOutlineP1 = (iNextOutlineP1 + 1) % iPoints1;
      }
    else
      {
      iOutlineP2 = iNextOutlineP2;
      iNextOutlineP2 = (iNextOutlineP2 + 1) % iPoints2;
      }
    } while(iNextAngle < 360);

  return true;
}

void CInvader::GetNormal(float& fNx,  float& fNy,  float& fNz,
                         float  fP0x, float  fP0y, float  fP0z,
                         float  fP1x, float  fP1y, float  fP1z,
                         float  fP2x, float  fP2y, float  fP2z)
{
  // calculate two vectors
  float fAx = fP1x - fP0x;
  float fAy = fP1y - fP0y;
  float fAz = fP1z - fP0z;
  float fBx = fP2x - fP0x;
  float fBy = fP2y - fP0y;
  float fBz = fP2z - fP0z;

  // calculate cross product
  float fX = fAy * fBz - fAz * fBy;
  float fY = fAz * fBx - fAx * fBz;
  float fZ = fAx * fBy - fAy * fBx;

  // normalize
  float fR = sqrt(fX * fX + fY * fY + fZ * fZ);
  fNx = fX / fR;
  fNy = fY / fR;
  fNz = fZ / fR;

  return;
}

void CInvader::TubeSetStart(float fPx, float fPy, float fPz, float fDirDegrees, float fWidth)
{
  // put fDirDegrees into [0,360)
  while (fDirDegrees < 0.0f)    fDirDegrees += 360.0f;
  while (fDirDegrees >= 360.0f) fDirDegrees -= 360.0f;

  // set member variables
  m_fTubeWidth      = fWidth;
  m_fTubeDirDegrees = fDirDegrees;
  m_fTubeCenter[0]  = fPx;
  m_fTubeCenter[1]  = fPy;
  m_fTubeCenter[2]  = fPz;

  // the Z coordinates are easy
  float fZ0 = fPz - fWidth / 2.0f;  // close
  float fZ1 = fPz + fWidth / 2.0f;  // far

  // X and Y are a bit trickier
  float fR = fWidth / 2.0f;
  float fDirRad = (fDirDegrees + 90) * (float) M_PI / 180.0f;
  float fX0 = fPx + fR * cos(fDirRad);
  float fX1 = fPx - fR * cos(fDirRad);
  float fY0 = fPy - fR * sin(fDirRad);
  float fY1 = fPy + fR * sin(fDirRad);

  m_fTubeCoords[0][0] = fX0;
  m_fTubeCoords[0][1] = fY0;
  m_fTubeCoords[0][2] = fZ0;

  m_fTubeCoords[1][0] = fX1;
  m_fTubeCoords[1][1] = fY1;
  m_fTubeCoords[1][2] = fZ1;

  return;
}

void CInvader::TubeMove(float (*pfVertex)[3], float (*pfNormal)[3], float fLength, float fEndDirDegrees, bool bEndCap)
{
  float fC[3];  // new center position

  float (*pfQuads)[4][3] = (float (*)[4][3]) pfVertex;
  float (*pfNorms)[4][3] = (float (*)[4][3]) pfNormal;

  // put fEndDirDegrees into [0,360)
  while (fEndDirDegrees < 0.0f)    fEndDirDegrees += 360.0f;
  while (fEndDirDegrees >= 360.0f) fEndDirDegrees -= 360.0f;

  // calculate the new center
  float fDirRad = m_fTubeDirDegrees * (float) M_PI / 180.0f;
  fC[0] = m_fTubeCenter[0] + fLength * cos(fDirRad);
  fC[1] = m_fTubeCenter[1] - fLength * sin(fDirRad);
  fC[2] = m_fTubeCenter[2];

  // get the previous X and Y coordinates
  float fX0 = m_fTubeCoords[0][0];
  float fY0 = m_fTubeCoords[0][1];
  float fZ0 = m_fTubeCoords[0][2];
  float fX1 = m_fTubeCoords[1][0];
  float fY1 = m_fTubeCoords[1][1];
  float fZ1 = m_fTubeCoords[1][2];

  // the plane of intersection will be on the angle halfway between incoming and outgoing directions
  float fPlaneDeg = (m_fTubeDirDegrees + fEndDirDegrees) / 2.0f + 90.0f;
  float fPlaneRad = fPlaneDeg * (float) M_PI / 180.0f;

  // calculate projected position for (X0,Y0)
  float fL2 = (fC[0] - fX0) * sin(fDirRad) / sin(fPlaneRad - fDirRad) + (fC[1] - fY0) * cos(fDirRad) / sin(fPlaneRad - fDirRad);
  float fX2 = fC[0] + fL2 * cos(fPlaneRad);
  float fY2 = fC[1] - fL2 * sin(fPlaneRad);

  // use symmetry to calculate projected position of (X1,Y1)
  float fX3 = fC[0] - fL2 * cos(fPlaneRad);
  float fY3 = fC[1] + fL2 * sin(fPlaneRad);

  // build four quads for tube walls
  pfQuads[0][3][0] = pfQuads[1][0][0] = pfQuads[1][3][0] = pfQuads[2][0][0] = fX0;
  pfQuads[0][0][0] = pfQuads[2][3][0] = pfQuads[3][0][0] = pfQuads[3][3][0] = fX1;
  pfQuads[0][2][0] = pfQuads[1][1][0] = pfQuads[1][2][0] = pfQuads[2][1][0] = fX2;
  pfQuads[0][1][0] = pfQuads[2][2][0] = pfQuads[3][1][0] = pfQuads[3][2][0] = fX3;
  pfQuads[0][3][1] = pfQuads[1][0][1] = pfQuads[1][3][1] = pfQuads[2][0][1] = fY0;
  pfQuads[0][0][1] = pfQuads[2][3][1] = pfQuads[3][0][1] = pfQuads[3][3][1] = fY1;
  pfQuads[0][2][1] = pfQuads[1][1][1] = pfQuads[1][2][1] = pfQuads[2][1][1] = fY2;
  pfQuads[0][1][1] = pfQuads[2][2][1] = pfQuads[3][1][1] = pfQuads[3][2][1] = fY3;
  pfQuads[0][0][2] = pfQuads[0][1][2] = pfQuads[0][2][2] = pfQuads[0][3][2] = pfQuads[1][0][2] = pfQuads[1][1][2] = pfQuads[3][2][2] = pfQuads[3][3][2] = fZ0;
  pfQuads[1][2][2] = pfQuads[1][3][2] = pfQuads[2][0][2] = pfQuads[2][1][2] = pfQuads[2][2][2] = pfQuads[2][3][2] = pfQuads[3][0][2] = pfQuads[3][1][2] = fZ1;

  // calculate and store normals
  pfNorms[0][0][0] = pfNorms[0][1][0] = pfNorms[0][2][0] = pfNorms[0][3][0] = pfNorms[2][0][0] = pfNorms[2][1][0] = pfNorms[2][2][0] = pfNorms[2][3][0] = 0.0f;
  pfNorms[0][0][1] = pfNorms[0][1][1] = pfNorms[0][2][1] = pfNorms[0][3][1] = pfNorms[2][0][1] = pfNorms[2][1][1] = pfNorms[2][2][1] = pfNorms[2][3][1] = 0.0f;
  pfNorms[0][0][2] = pfNorms[0][1][2] = pfNorms[0][2][2] = pfNorms[0][3][2] = -1.0f;
  pfNorms[2][0][2] = pfNorms[2][1][2] = pfNorms[2][2][2] = pfNorms[2][3][2] = 1.0f;
  float fNx = fY2 - fY0;
  float fNy = fX2 - fX0;
  float fNr = sqrt(fNx * fNx + fNy * fNy);
  fNx /= fNr;
  fNy /= fNr;
  pfNorms[1][0][0] = pfNorms[1][1][0] = pfNorms[1][2][0] = pfNorms[1][3][0] = fNx;
  pfNorms[1][0][1] = pfNorms[1][1][1] = pfNorms[1][2][1] = pfNorms[1][3][1] = -fNy;
  pfNorms[3][0][0] = pfNorms[3][1][0] = pfNorms[3][2][0] = pfNorms[3][3][0] = -fNx;
  pfNorms[3][0][1] = pfNorms[3][1][1] = pfNorms[3][2][1] = pfNorms[3][3][1] = fNy;
  pfNorms[1][0][2] = pfNorms[1][1][2] = pfNorms[1][2][2] = pfNorms[1][3][2] = pfNorms[3][0][2] = pfNorms[3][1][2] = pfNorms[3][2][2] = pfNorms[3][3][2] = 0.0f;

  // build end cap if necessary
  if (bEndCap)
      {
      pfQuads[4][0][0] = pfQuads[4][1][0] = fX3;
      pfQuads[4][2][0] = pfQuads[4][3][0] = fX2;
      pfQuads[4][0][1] = pfQuads[4][1][1] = fY3;
      pfQuads[4][2][1] = pfQuads[4][3][1] = fY2;
      pfQuads[4][0][2] = pfQuads[4][3][2] = fZ0;
      pfQuads[4][1][2] = pfQuads[4][2][2] = fZ1;
      float fPlaneNorm;
      if (fabs(m_fTubeDirDegrees - fEndDirDegrees) > 180.0f)
        fPlaneNorm = fPlaneRad + (float) M_PI / 2.0f;
      else
        fPlaneNorm = fPlaneRad - (float) M_PI / 2.0f;
      pfNorms[4][0][0] = pfNorms[4][1][0] = pfNorms[4][2][0] = pfNorms[4][3][0] = cos(fPlaneNorm);
      pfNorms[4][0][1] = pfNorms[4][1][1] = pfNorms[4][2][1] = pfNorms[4][3][1] = -sin(fPlaneNorm);
      pfNorms[4][0][2] = pfNorms[4][1][2] = pfNorms[4][2][2] = pfNorms[4][3][2] = 0.0f;
      }

  // save new tube state
  m_fTubeDirDegrees = fEndDirDegrees;
  m_fTubeCenter[0]  = fC[0];
  m_fTubeCenter[1]  = fC[1];
  m_fTubeCenter[2]  = fC[2];
  m_fTubeCoords[0][0] = fX2;
  m_fTubeCoords[0][1] = fY2;
  m_fTubeCoords[0][2] = fZ0;
  m_fTubeCoords[1][0] = fX3;
  m_fTubeCoords[1][1] = fY3;
  m_fTubeCoords[1][2] = fZ1;

  return;
}

void CInvader::ZRibbonStart(float fX, float fY, float fZ, float fThickness)
{
  m_fRibbonX = fX;
  m_fRibbonY = fY;
  m_fRibbonZ0 = fZ - fThickness / 2.0f;
  m_fRibbonZ1 = fZ + fThickness / 2.0f;
}

void CInvader::ZRibbonMove(float (*pfQuads)[4][3], float (*pfNorms)[4][3], float fDx, float fDy)
{
  float fX1 = m_fRibbonX + fDx;
  float fY1 = m_fRibbonY + fDy;

  // add quad coordinates
  pfQuads[0][0][0] = pfQuads[0][1][0] = m_fRibbonX;
  pfQuads[0][2][0] = pfQuads[0][3][0] = fX1;
  pfQuads[0][0][1] = pfQuads[0][1][1] = m_fRibbonY;
  pfQuads[0][2][1] = pfQuads[0][3][1] = fY1;
  pfQuads[0][1][2] = pfQuads[0][2][2] = m_fRibbonZ0;
  pfQuads[0][0][2] = pfQuads[0][3][2] = m_fRibbonZ1;

  // calculate normal
  float fR = sqrt(fDx * fDx + fDy * fDy);
  float fNy = fDx / fR;
  float fNx = fDy / fR;

  // store normal
  pfNorms[0][0][0] = pfNorms[0][1][0] = pfNorms[0][2][0] = pfNorms[0][3][0] = fNx;
  pfNorms[0][0][1] = pfNorms[0][1][1] = pfNorms[0][2][1] = pfNorms[0][3][1] = -fNy;
  pfNorms[0][0][2] = pfNorms[0][1][2] = pfNorms[0][2][2] = pfNorms[0][3][2] = 0.0f;

  m_fRibbonX = fX1;
  m_fRibbonY = fY1;
}

void CInvader::BuildBox(float (*pfQuads)[4][3], float (*pfNorms)[4][3], float fX0, float fY0, float fX1, float fY1, float fThickness)
{
  // write quad coordinates
  pfQuads[0][0][0] = pfQuads[0][1][0] = pfQuads[1][0][0] = pfQuads[1][1][0] = pfQuads[2][2][0] = pfQuads[2][3][0] = pfQuads[3][0][0] = pfQuads[3][1][0] = pfQuads[4][0][0] = pfQuads[4][1][0] = pfQuads[4][2][0] = pfQuads[4][3][0] = fX0;
  pfQuads[0][2][0] = pfQuads[0][3][0] = pfQuads[1][2][0] = pfQuads[1][3][0] = pfQuads[2][0][0] = pfQuads[2][1][0] = pfQuads[3][2][0] = pfQuads[3][3][0] = pfQuads[5][0][0] = pfQuads[5][1][0] = pfQuads[5][2][0] = pfQuads[5][3][0] = fX1;
  pfQuads[0][0][1] = pfQuads[0][3][1] = pfQuads[1][0][1] = pfQuads[1][1][1] = pfQuads[1][2][1] = pfQuads[1][3][1] = pfQuads[2][0][1] = pfQuads[2][3][1] = pfQuads[4][0][1] = pfQuads[4][3][1] = pfQuads[5][0][1] = pfQuads[5][3][1] = fY0;
  pfQuads[0][1][1] = pfQuads[0][2][1] = pfQuads[2][1][1] = pfQuads[2][2][1] = pfQuads[3][0][1] = pfQuads[3][1][1] = pfQuads[3][2][1] = pfQuads[3][3][1] = pfQuads[4][1][1] = pfQuads[4][2][1] = pfQuads[5][1][1] = pfQuads[5][2][1] = fY1;
  pfQuads[0][0][2] = pfQuads[0][1][2] = pfQuads[0][2][2] = pfQuads[0][3][2] = pfQuads[1][1][2] = pfQuads[1][2][2] = pfQuads[3][0][2] = pfQuads[3][3][2] = pfQuads[4][2][2] = pfQuads[4][3][2] = pfQuads[5][0][2] = pfQuads[5][1][2] = -fThickness / 2.0f;
  pfQuads[1][0][2] = pfQuads[1][3][2] = pfQuads[2][0][2] = pfQuads[2][1][2] = pfQuads[2][2][2] = pfQuads[2][3][2] = pfQuads[3][1][2] = pfQuads[3][2][2] = pfQuads[4][0][2] = pfQuads[4][1][2] = pfQuads[5][2][2] = pfQuads[5][3][2] = fThickness / 2.0f;

  // write normals
  for (int i = 0; i < 6; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 3; k++)
        pfNorms[i][j][k] = 0.0f;
  pfNorms[0][0][2] = pfNorms[0][1][2] = pfNorms[0][2][2] = pfNorms[0][3][2] = pfNorms[1][0][1] = pfNorms[1][1][1] = pfNorms[1][2][1] = pfNorms[1][3][1] = pfNorms[4][0][0] = pfNorms[4][1][0] = pfNorms[4][2][0] = pfNorms[4][3][0] = -1.0f;
  pfNorms[2][0][2] = pfNorms[2][1][2] = pfNorms[2][2][2] = pfNorms[2][3][2] = pfNorms[3][0][1] = pfNorms[3][1][1] = pfNorms[3][2][1] = pfNorms[3][3][1] = pfNorms[5][0][0] = pfNorms[5][1][0] = pfNorms[5][2][0] = pfNorms[5][3][0] = 1.0f;

  return;
}

void CInvader::BuildSquare(float (*pfQuads)[4][3], float (*pfNorms)[4][3], float fX0, float fY0, float fX1, float fY1, float fZ)
{
  // write square coordinates
  pfQuads[0][0][0] = pfQuads[0][1][0] = fX0;
  pfQuads[0][2][0] = pfQuads[0][3][0] = fX1;
  pfQuads[0][0][1] = pfQuads[0][3][1] = fY0;
  pfQuads[0][1][1] = pfQuads[0][2][1] = fY1;
  pfQuads[0][0][2] = pfQuads[0][1][2] = pfQuads[0][2][2] = pfQuads[0][3][2] = fZ;

  // write normals
  pfNorms[0][0][0] = pfNorms[0][1][0] = pfNorms[0][2][0] = pfNorms[0][3][0] = 0.0f;
  pfNorms[0][0][1] = pfNorms[0][1][1] = pfNorms[0][2][1] = pfNorms[0][3][1] = 0.0f;
  pfNorms[0][0][2] = pfNorms[0][1][2] = pfNorms[0][2][2] = pfNorms[0][3][2] = -1.0f;
}

void CInvader::BuildPanes(float (*pfQuads)[4][3], float (*pfNorms)[4][3], float fX0, float fY0, float fX1, float fY1, float fThickness)
{
  float fZ0 = -fThickness / 2.0f;
  float fZ1 = fThickness / 2.0f;

  // write square coordinates
  pfQuads[0][0][0] = pfQuads[0][1][0] = pfQuads[1][2][0] = pfQuads[1][3][0] = fX0;
  pfQuads[0][2][0] = pfQuads[0][3][0] = pfQuads[1][0][0] = pfQuads[1][1][0] = fX1;
  pfQuads[0][0][1] = pfQuads[0][3][1] = pfQuads[1][0][1] = pfQuads[1][3][1] = fY0;
  pfQuads[0][1][1] = pfQuads[0][2][1] = pfQuads[1][1][1] = pfQuads[1][2][1] = fY1;
  pfQuads[0][0][2] = pfQuads[0][1][2] = pfQuads[0][2][2] = pfQuads[0][3][2] = fZ0;
  pfQuads[1][0][2] = pfQuads[1][1][2] = pfQuads[1][2][2] = pfQuads[1][3][2] = fZ1;

  // write normals
  pfNorms[0][0][0] = pfNorms[0][1][0] = pfNorms[0][2][0] = pfNorms[0][3][0] = pfNorms[1][0][0] = pfNorms[1][1][0] = pfNorms[1][2][0] = pfNorms[1][3][0] = 0.0f;
  pfNorms[0][0][1] = pfNorms[0][1][1] = pfNorms[0][2][1] = pfNorms[0][3][1] = pfNorms[1][0][1] = pfNorms[1][1][1] = pfNorms[1][2][1] = pfNorms[1][3][1] = 0.0f;
  pfNorms[0][0][2] = pfNorms[0][1][2] = pfNorms[0][2][2] = pfNorms[0][3][2] = -1.0f;
  pfNorms[1][0][2] = pfNorms[1][1][2] = pfNorms[1][2][2] = pfNorms[1][3][2] = 1.0f;

}
