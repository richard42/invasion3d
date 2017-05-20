/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Invader20.cpp                                            *
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
#include <memory.h>
#include <math.h>
#include "Invader.h"
#include "Invader20.h"
#include "Particles.h"

// const model parameters
const int CInvader20::m_ciBodyWidth = 80;
const int CInvader20::m_ciBodyHeight = 50;
const int CInvader20::m_ciBodyDepth = 70;
const int CInvader20::m_ciRadialPieces = 15;
const int CInvader20::m_ciBodySections = 8;
const int CInvader20::m_ciEyeSeparation = 2;
const int CInvader20::m_ciEyeWidth = 1;
const int CInvader20::m_ciEyeHeight = 1;
const int CInvader20::m_ciAntSeparation = 20;
const int CInvader20::m_ciAntLength = 15;
const int CInvader20::m_ciAntThickness = 3;
const int CInvader20::m_ciArmThickness = 7;
const int CInvader20::m_ciLegThickness = 8;

const int CInvader20::m_ciEyeQuads = CInvader20::m_ciEyeWidth * CInvader20::m_ciEyeHeight * 2;
const int CInvader20::m_ciBodyPairs = (CInvader20::m_ciBodySections - (CInvader20::m_ciEyeWidth << 1)) * (CInvader20::m_ciRadialPieces + 1) +
                                      (CInvader20::m_ciEyeWidth << 1) * (CInvader20::m_ciRadialPieces - CInvader20::m_ciEyeHeight + 1);
const int CInvader20::m_ciTotalVertices = m_ciBodyPairs * 2 + (m_ciRadialPieces + 2) * 2 + m_ciEyeQuads * 4 + (10 + 18 + 26) * 4;

// set static pointers to NULL
GLfloat (*CInvader20::glfVertex)[3] = NULL;
GLfloat (*CInvader20::glfNormal)[3] = NULL;

// static data for collisions between invaders
int    CInvader20::m_iOutlinePoints = m_ciBodySections * 2 + 4;
float *CInvader20::m_pfOutlineX = NULL;
float *CInvader20::m_pfOutlineY = NULL;


/////////////////////////////////////////////////////////////////////////////
// CInvader20 static functions

bool CInvader20::InitModel(void)
{
  // allocate memory
  glfVertex  = (GLfloat (*)[3]) malloc(m_ciTotalVertices * sizeof(GLfloat) * 3);
  glfNormal  = (GLfloat (*)[3]) malloc(m_ciTotalVertices * sizeof(GLfloat) * 3);

  m_pfOutlineX = (float *) malloc(m_iOutlinePoints * sizeof(float));
  m_pfOutlineY = (float *) malloc(m_iOutlinePoints * sizeof(float));

  if (!glfVertex || !glfNormal || !m_pfOutlineX || !m_pfOutlineY) return false;

  // set up the (X,Y) Invader Outline for doing collisions
  m_pfOutlineX[0] = (float) m_ciBodyWidth / 2.0f;
  m_pfOutlineY[0] = 0.0f;
  m_pfOutlineX[m_ciBodySections + 2] = (float) -m_ciBodyWidth / 2.0f;
  m_pfOutlineY[m_ciBodySections + 2] = 0.0f;
  for (int iSection = 0; iSection <= m_ciBodySections; iSection++)
    {
    float fAngle = (iSection + 1) * (float) M_PI / (m_ciBodySections + 2);
    m_pfOutlineX[iSection + 1] = cos(fAngle) * m_ciBodyWidth / 2;
    m_pfOutlineY[iSection + 1] = -sin(fAngle) * m_ciBodyHeight / 2;
    m_pfOutlineX[m_ciBodySections + iSection + 3] = -cos(fAngle) * m_ciBodyWidth / 2;
    m_pfOutlineY[m_ciBodySections + iSection + 3] = sin(fAngle) * m_ciBodyHeight / 2;
    }

  // build left and right triangle fans
  float fAngle0 = (float) M_PI / (m_ciBodySections + 2);
  float fX = cos(fAngle0) * m_ciBodyWidth / 2;
  int iLeftFan = 0;
  int iRightFan = m_ciRadialPieces + 2;
  glfVertex[iLeftFan][0] = (float) -m_ciBodyWidth / 2.0f;
  glfVertex[iLeftFan][1] = 0.0f;
  glfVertex[iLeftFan][2] = 0.0f;
  glfNormal[iLeftFan][0] = -1.0f;
  glfNormal[iLeftFan][1] = 0.0f;
  glfNormal[iLeftFan][2] = 0.0f;
  glfVertex[iRightFan][0] = (float) m_ciBodyWidth / 2.0f;
  glfVertex[iRightFan][1] = 0.0f;
  glfVertex[iRightFan][2] = 0.0f;
  glfNormal[iRightFan][0] = 1.0f;
  glfNormal[iRightFan][1] = 0.0f;
  glfNormal[iRightFan][2] = 0.0f;
  float fY0 = 0.0f;
  float fZ0 = -sin(fAngle0) * m_ciBodyDepth / 2;
  for (int iRad = 0; iRad <= m_ciRadialPieces; iRad++)
      {
      float fY1 = -sin(2 * (float) M_PI * (iRad + 1) / m_ciRadialPieces) * sin(fAngle0) * m_ciBodyHeight / 2;
      float fZ1 = -cos(2 * (float) M_PI * (iRad + 1) / m_ciRadialPieces) * sin(fAngle0) * m_ciBodyDepth / 2;
      float fNx, fNy, fNz;
      glfVertex[iLeftFan + iRad + 1][0] = -fX;
      glfVertex[iLeftFan + iRad + 1][1] = fY0;
      glfVertex[iLeftFan + iRad + 1][2] = fZ0;
      GetNormal(fNx,fNy,fNz, (float) -m_ciBodyWidth / 2.0f,0.0f,0.0f, -fX,fY0,fZ0, -fX,fY1,fZ1);
      glfNormal[iLeftFan + iRad + 1][0] = fNx;
      glfNormal[iLeftFan + iRad + 1][1] = fNy;
      glfNormal[iLeftFan + iRad + 1][2] = fNz;
      glfVertex[iRightFan + iRad + 1][0] = fX;
      glfVertex[iRightFan + iRad + 1][1] = -fY0;
      glfVertex[iRightFan + iRad + 1][2] = fZ0;
      GetNormal(fNx,fNy,fNz, (float) m_ciBodyWidth / 2.0f,0.0f,0.0f, fX,-fY0,fZ0, fX,-fY1,fZ1);
      glfNormal[iRightFan + iRad + 1][0] = fNx;
      glfNormal[iRightFan + iRad + 1][1] = fNy;
      glfNormal[iRightFan + iRad + 1][2] = fNz;
      fY0 = fY1;
      fZ0 = fZ1;
      }
  // build body quad strips without eyes
  int iIndex = (m_ciRadialPieces + 2) * 2;
  int iLeftEyeSection = ((m_ciBodySections - m_ciEyeSeparation) >> 1) - m_ciEyeWidth;
  int iRightEyeSection = iLeftEyeSection + m_ciEyeWidth + m_ciEyeSeparation;
  GLfloat (*pfBodyStripPtr)[3] = glfVertex + iIndex;
  GLfloat (*pfBodyNormals)[3] = glfNormal + iIndex;
  for (int iSection = 0; iSection < m_ciBodySections; iSection++)
      {
      if (iSection >= iLeftEyeSection  && iSection < iLeftEyeSection  + m_ciEyeWidth) continue;
      if (iSection >= iRightEyeSection && iSection < iRightEyeSection + m_ciEyeWidth) continue;
      float fXAngle0 = (iSection + 1) * (float) M_PI / (m_ciBodySections + 2);
      float fXAngle1 = (iSection + 2) * (float) M_PI / (m_ciBodySections + 2);
      float fX0 = -cos(fXAngle0) * m_ciBodyWidth / 2;
      float fX1 = -cos(fXAngle1) * m_ciBodyWidth / 2;
      float fY0 = 0.0f;
      float fY1 = 0.0f;
      float fZ0 = -sin(fXAngle0) * m_ciBodyDepth / 2;
      float fZ1 = -sin(fXAngle1) * m_ciBodyDepth / 2;
      for (int iRad = 0; iRad <= m_ciRadialPieces; iRad++)
          {
          float fTAngle1 = 2 * (float) M_PI * (iRad + 1) / m_ciRadialPieces;
          float fNextY0 = -sin(fTAngle1) * sin(fXAngle0) * m_ciBodyHeight / 2;
          float fNextY1 = -sin(fTAngle1) * sin(fXAngle1) * m_ciBodyHeight / 2;
          float fNextZ0 = -cos(fTAngle1) * sin(fXAngle0) * m_ciBodyDepth / 2;
          float fNextZ1 = -cos(fTAngle1) * sin(fXAngle1) * m_ciBodyDepth / 2;
          pfBodyStripPtr[0][0] = fX0;
          pfBodyStripPtr[0][1] = fY0;
          pfBodyStripPtr[0][2] = fZ0;
          pfBodyStripPtr[1][0] = fX1;
          pfBodyStripPtr[1][1] = fY1;
          pfBodyStripPtr[1][2] = fZ1;
          float fNx, fNy, fNz;
          GetNormal(fNx,fNy,fNz, fX0,fY0,fZ0, fX1,fNextY1,fNextZ1, fX0,fNextY0,fNextZ0);
          pfBodyNormals[0][0] = fNx;
          pfBodyNormals[0][1] = fNy;
          pfBodyNormals[0][2] = fNz;
          GetNormal(fNx,fNy,fNz, fX1,fY1,fZ1, fX1,fNextY1,fNextZ1, fX0,fNextY0,fNextZ0);
          pfBodyNormals[1][0] = fNx;
          pfBodyNormals[1][1] = fNy;
          pfBodyNormals[1][2] = fNz;
          pfBodyStripPtr += 2;
          pfBodyNormals += 2;
          fY0 = fNextY0;
          fY1 = fNextY1;
          fZ0 = fNextZ0;
          fZ1 = fNextZ1;
          }
      }

  // build body quad strips around eyes
  int iRadOffset = (m_ciRadialPieces + 4) / 8;
  for (int iSectionIdx = 0; iSectionIdx < (m_ciEyeWidth << 1); iSectionIdx++)
      {
      int iSection;
      if (iSectionIdx < m_ciEyeWidth)
        iSection = iSectionIdx + iLeftEyeSection;
      else
        iSection = iSectionIdx - m_ciEyeWidth + iRightEyeSection;
      float fXAngle0 = (iSection + 1) * (float) M_PI / (m_ciBodySections + 2);
      float fXAngle1 = (iSection + 2) * (float) M_PI / (m_ciBodySections + 2);
      float fX0 = -cos(fXAngle0) * m_ciBodyWidth / 2;
      float fX1 = -cos(fXAngle1) * m_ciBodyWidth / 2;
      float fTAngle0 = 2 * (float) M_PI * (iRadOffset) / m_ciRadialPieces;
      float fY0 = -sin(fTAngle0) * sin(fXAngle0) * m_ciBodyHeight / 2;
      float fY1 = -sin(fTAngle0) * sin(fXAngle1) * m_ciBodyHeight / 2;
      float fZ0 = -cos(fTAngle0) * sin(fXAngle0) * m_ciBodyDepth / 2;
      float fZ1 = -cos(fTAngle0) * sin(fXAngle1) * m_ciBodyDepth / 2;
      for (int iRad = 0; iRad <= m_ciRadialPieces - m_ciEyeHeight; iRad++)
          {
          float fTAngle1 = 2 * (float) M_PI * (iRad + iRadOffset + 1) / m_ciRadialPieces;
          float fNextY0 = -sin(fTAngle1) * sin(fXAngle0) * m_ciBodyHeight / 2;
          float fNextY1 = -sin(fTAngle1) * sin(fXAngle1) * m_ciBodyHeight / 2;
          float fNextZ0 = -cos(fTAngle1) * sin(fXAngle0) * m_ciBodyDepth / 2;
          float fNextZ1 = -cos(fTAngle1) * sin(fXAngle1) * m_ciBodyDepth / 2;
          pfBodyStripPtr[0][0] = fX0;
          pfBodyStripPtr[0][1] = fY0;
          pfBodyStripPtr[0][2] = fZ0;
          pfBodyStripPtr[1][0] = fX1;
          pfBodyStripPtr[1][1] = fY1;
          pfBodyStripPtr[1][2] = fZ1;
          float fNx, fNy, fNz;
          GetNormal(fNx,fNy,fNz, fX0,fY0,fZ0, fX1,fNextY1,fNextZ1, fX0,fNextY0,fNextZ0);
          pfBodyNormals[0][0] = fNx;
          pfBodyNormals[0][1] = fNy;
          pfBodyNormals[0][2] = fNz;
          GetNormal(fNx,fNy,fNz, fX1,fY1,fZ1, fX1,fNextY1,fNextZ1, fX0,fNextY0,fNextZ0);
          pfBodyNormals[1][0] = fNx;
          pfBodyNormals[1][1] = fNy;
          pfBodyNormals[1][2] = fNz;
          pfBodyStripPtr += 2;
          pfBodyNormals += 2;
          fY0 = fNextY0;
          fY1 = fNextY1;
          fZ0 = fNextZ0;
          fZ1 = fNextZ1;
          }
      }
  iIndex += m_ciBodyPairs * 2;

  // build eye quads
  GLfloat (*pfEyeQuads)[3]   = glfVertex + iIndex;
  GLfloat (*pfEyeNormals)[3] = glfNormal + iIndex;
  for (int iSectionIdx = 0; iSectionIdx < (m_ciEyeWidth << 1); iSectionIdx++)
      {
      int iSection;
      if (iSectionIdx < m_ciEyeWidth)
        iSection = iSectionIdx + iLeftEyeSection;
      else
        iSection = iSectionIdx - m_ciEyeWidth + iRightEyeSection;
      float fXAngle0 = (iSection + 1) * (float) M_PI / (m_ciBodySections + 2);
      float fXAngle1 = (iSection + 2) * (float) M_PI / (m_ciBodySections + 2);
      float fX0 = -cos(fXAngle0) * m_ciBodyWidth / 2;
      float fX1 = -cos(fXAngle1) * m_ciBodyWidth / 2;
      float fTAngle0 = 2 * (float) M_PI * (iRadOffset) / m_ciRadialPieces;
      float fY0 = -sin(fTAngle0) * sin(fXAngle0) * m_ciBodyHeight / 2;
      float fY1 = -sin(fTAngle0) * sin(fXAngle1) * m_ciBodyHeight / 2;
      float fZ0 = -cos(fTAngle0) * sin(fXAngle0) * m_ciBodyDepth / 2;
      float fZ1 = -cos(fTAngle0) * sin(fXAngle1) * m_ciBodyDepth / 2;
      for (int iRad = 0; iRad < m_ciEyeHeight; iRad++)
          {
          float fTAngle1 = 2 * (float) M_PI * (iRadOffset - iRad - 1) / m_ciRadialPieces;
          float fNextY0 = -sin(fTAngle1) * sin(fXAngle0) * m_ciBodyHeight / 2;
          float fNextY1 = -sin(fTAngle1) * sin(fXAngle1) * m_ciBodyHeight / 2;
          float fNextZ0 = -cos(fTAngle1) * sin(fXAngle0) * m_ciBodyDepth / 2;
          float fNextZ1 = -cos(fTAngle1) * sin(fXAngle1) * m_ciBodyDepth / 2;
          pfEyeQuads[0][0] = pfEyeQuads[1][0] = fX0;
          pfEyeQuads[2][0] = pfEyeQuads[3][0] = fX1;
          pfEyeQuads[0][1] = fY0;
          pfEyeQuads[1][1] = fNextY0;
          pfEyeQuads[2][1] = fNextY1;
          pfEyeQuads[3][1] = fY1;
          pfEyeQuads[0][2] = fZ0;
          pfEyeQuads[1][2] = fNextZ0;
          pfEyeQuads[2][2] = fNextZ1;
          pfEyeQuads[3][2] = fZ1;
          float fNx, fNy, fNz;
          GetNormal(fNx,fNy,fNz, fX0,fY0,fZ0, fX0,fNextY0,fNextZ0, fX1,fNextY1,fNextZ1);
          pfEyeNormals[0][0] = pfEyeNormals[1][0] = pfEyeNormals[2][0] = pfEyeNormals[3][0] = fNx;
          pfEyeNormals[0][1] = pfEyeNormals[1][1] = pfEyeNormals[2][1] = pfEyeNormals[3][1] = fNy;
          pfEyeNormals[0][2] = pfEyeNormals[1][2] = pfEyeNormals[2][2] = pfEyeNormals[3][2] = fNz;
          pfEyeQuads += 4;
          pfEyeNormals += 4;
          fY0 = fNextY0;
          fY1 = fNextY1;
          fZ0 = fNextZ0;
          fZ1 = fNextZ1;
          }
      }
  iIndex += m_ciEyeQuads * 4;

  // build antenna
  float fAntX = (float) m_ciAntSeparation / 2.0f;
  float fAntY = (float) -0.45f * m_ciBodyHeight * sqrt(1.0f - fAntX * fAntX / (float) (m_ciBodyWidth * m_ciBodyWidth / 4.0f));
  TubeSetStart(fAntX, fAntY, 0.0f, 60.0f, (float) m_ciAntThickness);
  TubeMove(glfVertex + iIndex, glfNormal + iIndex, (float) m_ciAntLength, 120.0, true);
  TubeSetStart(-fAntX, fAntY, 0.0f, 120.0f, (float) m_ciAntThickness);
  TubeMove(glfVertex + iIndex + 20, glfNormal + iIndex + 20, (float) m_ciAntLength, 60.0, true);
  iIndex += 40;

  // build arms
  float fArmX =  0.45f * m_ciBodyWidth * cos((float) M_PI / 6.0f);
  float fArmY = -0.45f * m_ciBodyHeight * sin((float) M_PI / 6.0f);
  TubeSetStart(fArmX, fArmY, 0.0f, 330.0f, (float) m_ciArmThickness);
  TubeMove(glfVertex + iIndex + 0, glfNormal + iIndex + 0, (float) (m_ciBodyWidth / 5), 270.0, false);
  TubeMove(glfVertex + iIndex + 16, glfNormal + iIndex + 16, (float) (m_ciBodyHeight / 2), 180.0, true);
  TubeSetStart(-fArmX, fArmY, 0.0f, 210.0f, (float) m_ciArmThickness);
  TubeMove(glfVertex + iIndex + 36, glfNormal + iIndex + 36, (float) (m_ciBodyWidth / 5), 270.0, false);
  TubeMove(glfVertex + iIndex + 52, glfNormal + iIndex + 52, (float) (m_ciBodyHeight / 2), 0.0, true);
  iIndex += 72;

  // build legs
  BuildLegs(glfVertex + iIndex, glfNormal + iIndex, false);
  iIndex += 104;

  // done
  return true;
}

void CInvader20::DestroyModel(void)
{
  if (glfVertex) free(glfVertex);
  glfVertex = NULL;
  if (glfNormal) free(glfNormal);
  glfNormal = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CInvader20 constructor/destructor

CInvader20::CInvader20()
{
  // set default colors
  m_uchBodyColor[0] = 0x00;  m_uchBodyColor[1] = 0xff;  m_uchBodyColor[2] = 0x00;
  m_uchAntColor[0] = 0xff;   m_uchAntColor[1] = 0xff;   m_uchAntColor[2] = 0x00;
  m_uchEyeColor[0] = 0x9f;   m_uchEyeColor[1] = 0x00;   m_uchEyeColor[2] = 0x00;
  m_uchArmColor[0] = 0x00;   m_uchArmColor[1] = 0x00;   m_uchArmColor[2] = 0xff;
  m_uchLegColor[0] = 0x00;   m_uchLegColor[1] = 0x00;   m_uchLegColor[2] = 0xff;

  // allocate model arrays
  m_pfVertex  = (GLfloat (*)[3]) malloc(m_ciTotalVertices * sizeof(GLfloat) * 3);
  m_pfNormal  = (GLfloat (*)[3]) malloc(m_ciTotalVertices * sizeof(GLfloat) * 3);

  // copy static model data into our member storage
  memcpy(m_pfVertex, glfVertex,  m_ciTotalVertices * sizeof(GLfloat) * 3);
  memcpy(m_pfNormal, glfNormal,  m_ciTotalVertices * sizeof(GLfloat) * 3);

  // set my maximum thrust
  m_fMaxThrust    = 0.02f;
}

CInvader20::~CInvader20()
{
  // free model memory
  free(m_pfVertex);
  m_pfVertex  = NULL;
  free(m_pfNormal);
  m_pfNormal  = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CInvader20 accessor functions

void CInvader20::GetBodyBounding(float *fL, float *fR, float *fT, float *fB) const
{
  *fL = m_fPositionX - (float) m_ciBodyWidth / 2.0f;
  *fR = *fL + (float) m_ciBodyWidth;
  *fT = m_fPositionY - (float) m_ciBodyHeight / 2.0f;
  *fB = *fT + (float) m_ciBodyHeight;
}

void CInvader20::GetParticlePolys(SPolys *psPoly) const
{
  float fHealth = (float) m_iLastHealth / (float) m_iFullHealth;
  float fDamage = 1.0f - fHealth;

  // set up the Polys struct for triangles and quad colors
  psPoly->uiNumTriColors = 1;
  psPoly->uiNumQuadColors = 5;
  psPoly->aucTriColors[0][0] = (unsigned char) (m_uchBodyColor[0] * fHealth + 0xff * fDamage);
  psPoly->aucTriColors[0][1] = (unsigned char) (m_uchBodyColor[1] * fHealth);
  psPoly->aucTriColors[0][2] = (unsigned char) (m_uchBodyColor[2] * fHealth);
  psPoly->aucQuadColors[0][0] = (unsigned char) (m_uchBodyColor[0] * fHealth + 0xff * fDamage);
  psPoly->aucQuadColors[0][1] = (unsigned char) (m_uchBodyColor[1] * fHealth);
  psPoly->aucQuadColors[0][2] = (unsigned char) (m_uchBodyColor[2] * fHealth);
  psPoly->aucQuadColors[1][0] = m_uchEyeColor[0];
  psPoly->aucQuadColors[1][1] = m_uchEyeColor[1];
  psPoly->aucQuadColors[1][2] = m_uchEyeColor[2];
  psPoly->aucQuadColors[2][0] = m_uchAntColor[0];
  psPoly->aucQuadColors[2][1] = m_uchAntColor[1];
  psPoly->aucQuadColors[2][2] = m_uchAntColor[2];
  psPoly->aucQuadColors[3][0] = m_uchArmColor[0];
  psPoly->aucQuadColors[3][1] = m_uchArmColor[1];
  psPoly->aucQuadColors[3][2] = m_uchArmColor[2];
  psPoly->aucQuadColors[4][0] = m_uchLegColor[0];
  psPoly->aucQuadColors[4][1] = m_uchLegColor[1];
  psPoly->aucQuadColors[4][2] = m_uchLegColor[2];

  // set up some indices
  unsigned int uiSrcIdx = 0;
  unsigned int uiQuadIdx = 0;
  unsigned int uiTriIdx = 0;

  // do the triangle fans at the body ends
  psPoly->uiNumTriangles[0] = 2 * m_ciRadialPieces;
  for (unsigned int uiFan = 0; uiFan < 2; uiFan++) // two triangle fans
    {
    for (unsigned int ui = 0; ui < m_ciRadialPieces; ui++)
      {
      // copy the vertices for this triangle
      memcpy(psPoly->pfTriVertices[uiTriIdx][0], m_pfVertex[uiSrcIdx + 0], 3 * sizeof(float));
      memcpy(psPoly->pfTriVertices[uiTriIdx][1], m_pfVertex[uiSrcIdx + ui + 1], 3 * sizeof(float));
      memcpy(psPoly->pfTriVertices[uiTriIdx][2], m_pfVertex[uiSrcIdx + ui + 2], 3 * sizeof(float));
      // copy the normals for this triangle
      memcpy(psPoly->pfTriNormals[uiTriIdx][0], m_pfNormal[uiSrcIdx + 0], 3 * sizeof(float));
      memcpy(psPoly->pfTriNormals[uiTriIdx][1], m_pfNormal[uiSrcIdx + ui + 1], 3 * sizeof(float));
      memcpy(psPoly->pfTriNormals[uiTriIdx][2], m_pfNormal[uiSrcIdx + ui + 2], 3 * sizeof(float));
      // go to next destination triangle
      uiTriIdx++;
      }
    // skip the triangle fan's vertices
    uiSrcIdx += m_ciRadialPieces + 2;
    }

  // do the body
  psPoly->uiNumQuads[0] = m_ciRadialPieces * m_ciBodySections - m_ciEyeQuads;
  for (unsigned int uiStrip = 0; uiStrip < m_ciBodySections; uiStrip++)
    {
    unsigned int uiStripLength;
    if (uiStrip < m_ciBodySections - (m_ciEyeWidth << 1))
      uiStripLength = m_ciRadialPieces;
    else
      uiStripLength = m_ciRadialPieces - m_ciEyeHeight;
    for (unsigned int ui = 0; ui < uiStripLength; ui++)
      {
      // copy the vertices for this quad
      memcpy(psPoly->pfQuadVertices[uiQuadIdx][0], m_pfVertex[uiSrcIdx + 0], 3 * sizeof(float));
      memcpy(psPoly->pfQuadVertices[uiQuadIdx][1], m_pfVertex[uiSrcIdx + 1], 3 * sizeof(float));
      memcpy(psPoly->pfQuadVertices[uiQuadIdx][2], m_pfVertex[uiSrcIdx + 3], 3 * sizeof(float));
      memcpy(psPoly->pfQuadVertices[uiQuadIdx][3], m_pfVertex[uiSrcIdx + 2], 3 * sizeof(float));
      // copy the normals for this quad
      memcpy(psPoly->pfQuadNormals[uiQuadIdx][0], m_pfNormal[uiSrcIdx + 0], 3 * sizeof(float));
      memcpy(psPoly->pfQuadNormals[uiQuadIdx][1], m_pfNormal[uiSrcIdx + 1], 3 * sizeof(float));
      memcpy(psPoly->pfQuadNormals[uiQuadIdx][2], m_pfNormal[uiSrcIdx + 3], 3 * sizeof(float));
      memcpy(psPoly->pfQuadNormals[uiQuadIdx][3], m_pfNormal[uiSrcIdx + 2], 3 * sizeof(float));
      // go to next pair in source quad strip and next dest quad
      uiQuadIdx++;
      uiSrcIdx += 2;
      }
    // skip last 2 vertices
    uiSrcIdx += 2;
    }

  // do the eyes
  psPoly->uiNumQuads[1] = m_ciEyeQuads;
  for (unsigned int uiQ = 0; uiQ < m_ciEyeQuads; uiQ++)
    {
    for (unsigned int uiV = 0; uiV < 4; uiV++)
      {
      memcpy(psPoly->pfQuadVertices[uiQuadIdx][uiV], m_pfVertex[uiSrcIdx], 3 * sizeof(float));
      memcpy(psPoly->pfQuadNormals[uiQuadIdx][uiV], m_pfNormal[uiSrcIdx], 3 * sizeof(float));
      uiSrcIdx++;
      }
    uiQuadIdx++;
    }

  // do the antennae
  psPoly->uiNumQuads[2] = 10;
  for (unsigned int uiQ = 0; uiQ < 10; uiQ++)
    {
    for (unsigned int uiV = 0; uiV < 4; uiV++)
      {
      memcpy(psPoly->pfQuadVertices[uiQuadIdx][uiV], m_pfVertex[uiSrcIdx], 3 * sizeof(float));
      memcpy(psPoly->pfQuadNormals[uiQuadIdx][uiV], m_pfNormal[uiSrcIdx], 3 * sizeof(float));
      uiSrcIdx++;
      }
    uiQuadIdx++;
    }

  // do the arms
  psPoly->uiNumQuads[3] = 18;
  for (unsigned int uiQ = 0; uiQ < 18; uiQ++)
    {
    for (unsigned int uiV = 0; uiV < 4; uiV++)
      {
      memcpy(psPoly->pfQuadVertices[uiQuadIdx][uiV], m_pfVertex[uiSrcIdx], 3 * sizeof(float));
      memcpy(psPoly->pfQuadNormals[uiQuadIdx][uiV], m_pfNormal[uiSrcIdx], 3 * sizeof(float));
      uiSrcIdx++;
      }
    uiQuadIdx++;
    }

  // do the legs
  psPoly->uiNumQuads[4] = 26;
  for (unsigned int uiQ = 0; uiQ < 26; uiQ++)
    {
    for (unsigned int uiV = 0; uiV < 4; uiV++)
      {
      memcpy(psPoly->pfQuadVertices[uiQuadIdx][uiV], m_pfVertex[uiSrcIdx], 3 * sizeof(float));
      memcpy(psPoly->pfQuadNormals[uiQuadIdx][uiV], m_pfNormal[uiSrcIdx], 3 * sizeof(float));
      uiSrcIdx++;
      }
    uiQuadIdx++;
    }

}

void CInvader20::Draw(void) const
{
  // Position the model in 3-space
  glTranslatef(m_fPositionX, m_fPositionY, m_fPositionZ);

  // set rotation of invader
  glRotatef(m_fAngle, m_fAngleX, m_fAngleY, m_fAngleZ);

  // do flip and spin for Kamikaze dive
  if (m_fFlipAngle > 0.0f || m_fSpinAngle > 0.0f)
    {
    glRotatef(m_fSpinAngle, 0.0f, 1.0f, 0.0f);
    glRotatef(m_fFlipAngle, 1.0f, 0.0f, 0.0f);
    }

  GLfloat pfSpecular[4]  = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat pfShininess[1] = {32.0f};
  GLint   piDiffuse[4];
  GLint   piAmbient[4];
  ModulateColor(piDiffuse, piAmbient, m_uchBodyColor);

  if (m_bFlash)
    {
    piDiffuse[0] = piDiffuse[1] = piDiffuse[2] = piDiffuse[3] = 0x7fffffff;
    piAmbient[0] = piAmbient[1] = piAmbient[2] = piAmbient[3] = 0x7fffffff;
    }

  glMaterialfv(GL_FRONT, GL_SPECULAR, pfSpecular);
  glMaterialfv(GL_FRONT, GL_SHININESS, pfShininess);
  glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
  glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);

  int iIndex = 0;

  // draw the body
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, m_pfVertex);
  glNormalPointer(GL_FLOAT, 0, m_pfNormal);
  glDrawArrays(GL_TRIANGLE_FAN, 0, m_ciRadialPieces + 2);
  iIndex += m_ciRadialPieces + 2;
  glDrawArrays(GL_TRIANGLE_FAN, iIndex, m_ciRadialPieces + 2);
  iIndex += m_ciRadialPieces + 2;
  for (int iStrip = 0; iStrip < m_ciBodySections - (m_ciEyeWidth << 1); iStrip++)
    {
    int iStripLength = (m_ciRadialPieces + 1) << 1;
    glDrawArrays(GL_QUAD_STRIP, iIndex, iStripLength);
    iIndex += iStripLength;
    }
  for (int iStrip = 0; iStrip < (m_ciEyeWidth << 1); iStrip++)
    {
    int iStripLength = (m_ciRadialPieces - m_ciEyeHeight + 1) << 1;
    glDrawArrays(GL_QUAD_STRIP, iIndex, iStripLength);
    iIndex += iStripLength;
    }
  
  // draw eyes
  piDiffuse[0]   = m_uchEyeColor[0] << 23;
  piDiffuse[1]   = m_uchEyeColor[1] << 23;
  piDiffuse[2]   = m_uchEyeColor[2] << 23;
  piAmbient[0]   = m_uchEyeColor[0] << 20;
  piAmbient[1]   = m_uchEyeColor[1] << 20;
  piAmbient[2]   = m_uchEyeColor[2] << 20;
  glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
  glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
  glDrawArrays(GL_QUADS, iIndex, m_ciEyeQuads << 2);
  iIndex += m_ciEyeQuads << 2;

  // draw antennae
  piDiffuse[0]   = m_uchAntColor[0] << 23;
  piDiffuse[1]   = m_uchAntColor[1] << 23;
  piDiffuse[2]   = m_uchAntColor[2] << 23;
  piAmbient[0]   = m_uchAntColor[0] << 20;
  piAmbient[1]   = m_uchAntColor[1] << 20;
  piAmbient[2]   = m_uchAntColor[2] << 20;
  glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
  glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
  glDrawArrays(GL_QUADS, iIndex, 40);
  iIndex += 40;

  // draw arms
  piDiffuse[0]   = m_uchArmColor[0] << 23;
  piDiffuse[1]   = m_uchArmColor[1] << 23;
  piDiffuse[2]   = m_uchArmColor[2] << 23;
  piAmbient[0]   = m_uchArmColor[0] << 20;
  piAmbient[1]   = m_uchArmColor[1] << 20;
  piAmbient[2]   = m_uchArmColor[2] << 20;
  glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
  glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
  glDrawArrays(GL_QUADS, iIndex, 72);
  iIndex += 72;

  // draw legs
  piDiffuse[0]   = m_uchLegColor[0] << 23;
  piDiffuse[1]   = m_uchLegColor[1] << 23;
  piDiffuse[2]   = m_uchLegColor[2] << 23;
  piAmbient[0]   = m_uchLegColor[0] << 20;
  piAmbient[1]   = m_uchLegColor[1] << 20;
  piAmbient[2]   = m_uchLegColor[2] << 20;
  glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
  glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
  glDrawArrays(GL_QUADS, iIndex, 104);
  iIndex += 104;

  // all done
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  return;
}

/////////////////////////////////////////////////////////////////////////////
// CInvader20 modifier functions

void CInvader20::SetBodyColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchBodyColor[0] = uchRed;
  m_uchBodyColor[1] = uchGreen;
  m_uchBodyColor[2] = uchBlue;
}

void CInvader20::SetAntColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchAntColor[0] = uchRed;
  m_uchAntColor[1] = uchGreen;
  m_uchAntColor[2] = uchBlue;
}

void CInvader20::SetEyeColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchEyeColor[0] = uchRed;
  m_uchEyeColor[1] = uchGreen;
  m_uchEyeColor[2] = uchBlue;
}

void CInvader20::SetArmColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchArmColor[0] = uchRed;
  m_uchArmColor[1] = uchGreen;
  m_uchArmColor[2] = uchBlue;
}

void CInvader20::SetLegColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchLegColor[0] = uchRed;
  m_uchLegColor[1] = uchGreen;
  m_uchLegColor[2] = uchBlue;
}

void CInvader20::ToggleLegs(bool bLegPosition)
{
  BuildLegs(m_pfVertex + m_ciTotalVertices - 104, m_pfNormal + m_ciTotalVertices - 104, bLegPosition);
  return;
}

bool CInvader20::TestCollision(float fLeftX, float fRightX, float fTopY, float fBotY, int iDamage)
{
  // first do a real fast but rough test
  if (fRightX < m_fPositionX - m_ciBodyWidth / 2.0f ||
      fLeftX  > m_fPositionX + m_ciBodyWidth / 2.0f ||
      fTopY   > m_fPositionY + m_ciBodyHeight / 2.0f ||
      fBotY   < m_fPositionY - m_ciBodyHeight / 2.0f) return false;

  float fX = (fLeftX + fRightX) / 2.0f - m_fPositionX;
  
  // handle case where center of projectile is to the left or right of the ship
  if (fX <= -m_ciBodyWidth / 2.0f || fX  >= m_ciBodyWidth / 2.0f)
    {
    if (fBotY < m_fPositionY || fTopY > m_fPositionY) return false;
    // it got hit
    m_bFlash = true;
    m_iLastHealth = m_iHealth;
    m_iHealth -= iDamage;
    return true;
    }

  // calculate height of invader body at the longitude of the projectile
  float fY = sqrt(m_ciBodyHeight * m_ciBodyHeight / 4.0f - m_ciBodyHeight * m_ciBodyHeight * fX * fX / (m_ciBodyWidth * m_ciBodyWidth));

  // see if the projectile is within the ship
  if (fBotY > m_fPositionY - fY && fTopY < m_fPositionY + fY)
    {
    m_bFlash = true;
    m_iLastHealth = m_iHealth;
    m_iHealth -= iDamage;
    return true;
    }

  return false;
}

// this function tests for a collision between a rotating kamikaze invader and the LaserBase
bool CInvader20::KamikazeCollision(float fLeftX, float fRightX, float fTopY, float fBotY)
{
  // first do a real fast but rough test
  if (fRightX < m_fPositionX - m_ciBodyWidth / 2.0f ||
      fLeftX  > m_fPositionX + m_ciBodyWidth / 2.0f ||
      fTopY   > m_fPositionY + m_ciBodyHeight / 2.0f) return false;

  // if the center of this invader is directly over the LaserBase, then there is a collision
  if (m_fPositionX > fLeftX && m_fPositionX < fRightX) return true;

  // determine the greatest radius of the invader body which may intersect with the LaserBase
  float fRad;
  if (m_fPositionY > fTopY)
    fRad = m_ciBodyWidth / 2.0f;
  else
    fRad = sqrt(m_ciBodyWidth * m_ciBodyWidth * (0.25f - (fTopY - m_fPositionY) * (fTopY - m_fPositionY) / (m_ciBodyHeight * m_ciBodyHeight)));

  // generate X coordinates for left and right edges of invader,
  // rotated around Y axis and taking depth into account
  // (note this is imperfect because it assumes a straight extruded oval body, while it's actually rounded)
  float fAngle = m_fSpinAngle * (float) M_PI / 180.0f;
  float fDepth = (float) m_ciBodyDepth / 2.0f * fabs(sin(fAngle));
  float fX0 = m_fPositionX - fRad * fabs(cos(fAngle)) - fDepth;
  float fX1 = m_fPositionX + fRad * fabs(cos(fAngle)) + fDepth;

  // now do the test
  if (fX1 > fLeftX && fX0 < fRightX) return true;

  return false;
}

bool CInvader20::BunkerCollision(float fLeftX, float fRightX, float fTopY)
{
  // first do a real fast but rough test
  if (fRightX < m_fPositionX - m_ciBodyWidth / 2.0f ||
      fLeftX  > m_fPositionX + m_ciBodyWidth / 2.0f ||
      fTopY   > m_fPositionY + m_ciBodyHeight / 2.0f) return false;

  float fForceX = 0.0f;
  float fForceY = 0.0f;

  // handle case where the very bottom of invader body hits top of bunker
  if (m_fPositionX >= fLeftX && m_fPositionX <= fRightX)
    {
    fForceY = 1.0f;
    }
  else if (m_fPositionX < fLeftX)
    {
    // calculate height of invader body at the longitude of the edge of the bunker
    float fX = fLeftX - m_fPositionX;
    float fY = sqrt(m_ciBodyHeight * m_ciBodyHeight / 4.0f - m_ciBodyHeight * m_ciBodyHeight * fX * fX / (m_ciBodyWidth * m_ciBodyWidth));
    // test for collision
    if (fTopY <= m_fPositionY + fY)
      {
      float fR = sqrt(fX * fX + fY * fY);
      fForceY = fY / fR;
      fForceX = fX / fR;
      }
    }
  else if (m_fPositionX > fRightX)
    {
    // calculate height of invader body at the longitude of the edge of the bunker
    float fX = fRightX - m_fPositionX;
    float fY = sqrt(m_ciBodyHeight * m_ciBodyHeight / 4.0f - m_ciBodyHeight * m_ciBodyHeight * fX * fX / (m_ciBodyWidth * m_ciBodyWidth));
    // test for collision
    if (fTopY <= m_fPositionY + fY)
      {
      float fR = sqrt(fX * fX + fY * fY);
      fForceY = fY / fR;
      fForceX = fX / fR;
      }
    }

  // final collision test
  if (fForceX == 0.0f && fForceY == 0.0f) return false;

  // there was a collision!
  float fDot = 2.0f * (fForceX * m_fVelocityX + fForceY * m_fVelocityY);   // this is the force of the collision

  // clip negative dot products to 0
  if (fDot < 0.0f) fDot = 0.0f;

  // impart the force
  ApplyForce(-fForceX * fDot, -fForceY * fDot);

  // now put on the hurt
  int iDamage = (int) (fDot * CGameplay::m_fCollideDamage);
  m_iLastHealth = m_iHealth;
  m_iHealth -= iDamage;

  return true;
}


/////////////////////////////////////////////////////////////////////////////
// CInvader20 private functions

void CInvader20::BuildLegs(float (*pfVertex)[3], float (*pfNormal)[3], bool bLegPosition)
{
  float fAngle;

  if (bLegPosition)
    fAngle = 30.0f;
  else
    fAngle = 45.0f;

  float fLegX = 0.45f * m_ciBodyWidth * cos((float) M_PI / 4.0f);
  float fLegY = 0.45f * m_ciBodyHeight * sin((float) M_PI / 4.0f);
  TubeSetStart(fLegX, fLegY, 0.0f, 270.0f, (float) m_ciLegThickness);
  TubeMove(pfVertex + 0,  pfNormal + 0, (float) (m_ciBodyHeight / 6), 270.0f - fAngle, false);
  TubeMove(pfVertex + 16, pfNormal + 16, (float) (m_ciBodyHeight / 6), 270.0f - fAngle * 2.0f, false);
  TubeMove(pfVertex + 32, pfNormal + 32, (float) (m_ciBodyWidth / 6), 180.0f - fAngle * 2.0f, true);
  TubeSetStart(-fLegX, fLegY, 0.0f, 270.0f, (float) m_ciLegThickness);
  TubeMove(pfVertex + 52, pfNormal + 52, (float) (m_ciBodyHeight / 6), 270.0f + fAngle, false);
  TubeMove(pfVertex + 68, pfNormal + 68, (float) (m_ciBodyHeight / 6), 270.0f + fAngle * 2.0f, false);
  TubeMove(pfVertex + 84, pfNormal + 84, (float) (m_ciBodyWidth / 6), fAngle * 2.0f, true);
}
