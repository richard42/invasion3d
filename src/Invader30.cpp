/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Invader30.cpp                                            *
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
#include "Invader30.h"
#include "Particles.h"

// const model parameters
const int CInvader30::m_ciCapWidth = 15;
const int CInvader30::m_ciCapHeight = 2;
const int CInvader30::m_ciConeHeight = 30;
const int CInvader30::m_ciBodyWidth = 70;
const int CInvader30::m_ciBodyHeight = 25;
const int CInvader30::m_ciRadialPieces = 30;
const int CInvader30::m_ciEyeSeparation = 3;
const int CInvader30::m_ciEyeWidth = 2;
const int CInvader30::m_ciEyeHeight = 8;
const int CInvader30::m_ciLegLength = 30;
const int CInvader30::m_ciLegThickness = 5;
  // calculated parameters
const int CInvader30::m_ciTotalVertices = m_ciRadialPieces * 8 + m_ciEyeWidth * 16 + 6 + 36 * 4;

// set static pointers to NULL
GLfloat (*CInvader30::glfVertex)[3] = NULL;
GLfloat (*CInvader30::glfNormal)[3] = NULL;

// static data for collisions between invaders
int    CInvader30::m_iOutlinePoints = 9;
float *CInvader30::m_pfOutlineX = NULL;
float *CInvader30::m_pfOutlineY = NULL;

/////////////////////////////////////////////////////////////////////////////
// CInvader30 static functions

bool CInvader30::InitModel(void)
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
  m_pfOutlineX[1] = (float) m_ciBodyWidth / 2.0f;
  m_pfOutlineY[1] = (float) -m_ciBodyHeight / 2.0f;
  m_pfOutlineX[2] = (float) m_ciCapWidth / 2.0f;
  m_pfOutlineY[2] = (float) -m_ciBodyHeight / 2.0f - m_ciConeHeight;
  m_pfOutlineX[3] = 0.0f;
  m_pfOutlineY[3] = (float) -m_ciBodyHeight / 2.0f - m_ciConeHeight - m_ciCapHeight;
  m_pfOutlineX[4] = (float) -m_ciCapWidth / 2.0f;
  m_pfOutlineY[4] = (float) -m_ciBodyHeight / 2.0f - m_ciConeHeight;
  m_pfOutlineX[5] = (float) -m_ciBodyWidth / 2.0f;
  m_pfOutlineY[5] = (float) -m_ciBodyHeight / 2.0f;
  m_pfOutlineX[6] = (float) -m_ciBodyWidth / 2.0f;
  m_pfOutlineY[6] = 0.0f;
  m_pfOutlineX[7] = (float) -m_ciBodyWidth / 2.0f;
  m_pfOutlineY[7] = (float) m_ciBodyHeight / 2.0f;
  m_pfOutlineX[8] = (float) m_ciBodyWidth / 2.0f;
  m_pfOutlineY[8] = (float) m_ciBodyHeight / 2.0f;

  float fX0, fY0, fZ0, fX1, fY1, fZ1;
  float fY, fX, fZ;

  // build cap triangle fan
  fY = (float) -m_ciBodyHeight / 2.0f - m_ciConeHeight;
  glfVertex[0][0] = 0.0f;
  glfVertex[0][1] = fY - m_ciCapHeight;
  glfVertex[0][2] = 0.0f;
  glfNormal[0][0] = 0.0f;
  glfNormal[0][1] = -1.0f;
  glfNormal[0][2] = 0.0f;
  fX0 = (float) -m_ciCapWidth / 2.0f;
  fZ0 = 0.0f;
  for (int iRad = 0; iRad <= m_ciRadialPieces; iRad++)
      {
      float fX1 = -cos(2.0f * (float) M_PI * (iRad + 1) / m_ciRadialPieces) * m_ciCapWidth / 2.0f;
      float fZ1 = -sin(2.0f * (float) M_PI * (iRad + 1) / m_ciRadialPieces) * m_ciCapWidth / 2.0f;
      float fNx, fNy, fNz;
      glfVertex[iRad + 1][0] = fX0;
      glfVertex[iRad + 1][1] = fY;
      glfVertex[iRad + 1][2] = fZ0;
      GetNormal(fNx,fNy,fNz, 0.0f,fY-m_ciCapHeight,0.0f, fX0,fY,fZ0, fX1,fY,fZ1);
      glfNormal[iRad + 1][0] = fNx;
      glfNormal[iRad + 1][1] = fNy;
      glfNormal[iRad + 1][2] = fNz;
      fX0 = fX1;
      fZ0 = fZ1;
      }
  int iIndex = m_ciRadialPieces + 2;

  // build cone quad strip
  fX0 = (float) -m_ciCapWidth / 2.0f;
  fX1 = (float) -m_ciBodyWidth / 2.0f;
  fY0 = (float) -m_ciBodyHeight / 2.0f - m_ciConeHeight;
  fY1 = (float) -m_ciBodyHeight / 2.0f;
  fZ0 = 0.0f;
  fZ1 = 0.0f;
  for (int iRad = 0; iRad <= m_ciRadialPieces; iRad++)
      {
      float fAngle1 = 2 * (float) M_PI * (iRad + 1) / m_ciRadialPieces;
      float fNextX0 = -cos(fAngle1) * m_ciCapWidth / 2;
      float fNextX1 = -cos(fAngle1) * m_ciBodyWidth / 2;
      float fNextZ0 = -sin(fAngle1) * m_ciCapWidth / 2;
      float fNextZ1 = -sin(fAngle1) * m_ciBodyWidth / 2;
      glfVertex[iIndex + (iRad << 1)][0] = fX0;
      glfVertex[iIndex + (iRad << 1)][1] = fY0;
      glfVertex[iIndex + (iRad << 1)][2] = fZ0;
      glfVertex[iIndex + (iRad << 1) + 1][0] = fX1;
      glfVertex[iIndex + (iRad << 1) + 1][1] = fY1;
      glfVertex[iIndex + (iRad << 1) + 1][2] = fZ1;
      float fNx, fNy, fNz;
      GetNormal(fNx,fNy,fNz, fX0,fY0,fZ0, fX1,fY1,fZ1, fNextX1,fY1,fNextZ1);
      glfNormal[iIndex + (iRad << 1)][0] = fNx;
      glfNormal[iIndex + (iRad << 1)][1] = fNy;
      glfNormal[iIndex + (iRad << 1)][2] = fNz;
      glfNormal[iIndex + (iRad << 1) + 1][0] = fNx;
      glfNormal[iIndex + (iRad << 1) + 1][1] = fNy;
      glfNormal[iIndex + (iRad << 1) + 1][2] = fNz;
      fX0 = fNextX0;
      fX1 = fNextX1;
      fZ0 = fNextZ0;
      fZ1 = fNextZ1;
      }
  iIndex += (m_ciRadialPieces + 1) * 2;

  // build body quads and eye quads
  int iLeftEyeRad = (((m_ciRadialPieces >> 1) - m_ciEyeSeparation) >> 1) - m_ciEyeWidth;
  int iRightEyeRad = iLeftEyeRad + m_ciEyeWidth + m_ciEyeSeparation;
  GLfloat (*pfBodyQuadPtr)[3] = glfVertex + iIndex;
  GLfloat (*pfBodyNormals)[3] = glfNormal + iIndex;
  GLfloat (*pfEyeQuadPtr)[3] = glfVertex + iIndex + (m_ciRadialPieces + 2 * m_ciEyeWidth) * 4;
  GLfloat (*pfEyeNormals)[3] = glfNormal + iIndex + (m_ciRadialPieces + 2 * m_ciEyeWidth) * 4;
  fX = (float) -m_ciBodyWidth / 2.0f;
  fY0 = (float) -m_ciBodyHeight / 2.0f;
  fY1 = (float) m_ciBodyHeight / 2.0f;
  fZ = 0.0f;
  float fEyeTop = (float) (m_ciBodyHeight - m_ciEyeHeight) / 4.0f - (float) m_ciBodyHeight / 2.0f;
  float fEyeBottom = fEyeTop + (float) m_ciEyeHeight;
  for (int iRad = 0; iRad < m_ciRadialPieces; iRad++)
      {
      float fAngle1 = 2 * (float) M_PI * (iRad + 1) / m_ciRadialPieces;
      float fNextX = -cos(fAngle1) * m_ciBodyWidth / 2;
      float fNextZ = -sin(fAngle1) * m_ciBodyWidth / 2;
      float fNx = -cos(fAngle1);
      float fNy = 0.0f;
      float fNz = -sin(fAngle1);
      if ((iRad >= iLeftEyeRad  && iRad < iLeftEyeRad  + m_ciEyeWidth) ||
          (iRad >= iRightEyeRad && iRad < iRightEyeRad + m_ciEyeWidth))
          {
          // two quads for the body
          pfBodyQuadPtr[0][0] = pfBodyQuadPtr[1][0] = pfBodyQuadPtr[4][0] = pfBodyQuadPtr[5][0] = fX;
          pfBodyQuadPtr[2][0] = pfBodyQuadPtr[3][0] = pfBodyQuadPtr[6][0] = pfBodyQuadPtr[7][0] = fNextX;
          pfBodyQuadPtr[0][1] = pfBodyQuadPtr[3][1] = fY0;
          pfBodyQuadPtr[1][1] = pfBodyQuadPtr[2][1] = fEyeTop;
          pfBodyQuadPtr[4][1] = pfBodyQuadPtr[7][1] = fEyeBottom;
          pfBodyQuadPtr[5][1] = pfBodyQuadPtr[6][1] = fY1;
          pfBodyQuadPtr[0][2] = pfBodyQuadPtr[1][2] = pfBodyQuadPtr[4][2] = pfBodyQuadPtr[5][2] = fZ;
          pfBodyQuadPtr[2][2] = pfBodyQuadPtr[3][2] = pfBodyQuadPtr[6][2] = pfBodyQuadPtr[7][2] = fNextZ;
          // one quad for the eye
          pfEyeQuadPtr[0][0] = pfEyeQuadPtr[1][0] = fX;
          pfEyeQuadPtr[2][0] = pfEyeQuadPtr[3][0] = fNextX;
          pfEyeQuadPtr[0][1] = pfEyeQuadPtr[3][1] = fEyeTop;
          pfEyeQuadPtr[1][1] = pfEyeQuadPtr[2][1] = fEyeBottom;
          pfEyeQuadPtr[0][2] = pfEyeQuadPtr[1][2] = fZ;
          pfEyeQuadPtr[2][2] = pfEyeQuadPtr[3][2] = fNextZ;
          // all normals are the same
          pfBodyNormals[0][0] = pfBodyNormals[1][0] = pfBodyNormals[2][0] = pfBodyNormals[3][0] = pfBodyNormals[4][0] = pfBodyNormals[5][0] = pfBodyNormals[6][0] = pfBodyNormals[7][0] = pfEyeNormals[0][0] = pfEyeNormals[1][0] = pfEyeNormals[2][0] = pfEyeNormals[3][0] = fNx;
          pfBodyNormals[0][1] = pfBodyNormals[1][1] = pfBodyNormals[2][1] = pfBodyNormals[3][1] = pfBodyNormals[4][1] = pfBodyNormals[5][1] = pfBodyNormals[6][1] = pfBodyNormals[7][1] = pfEyeNormals[0][1] = pfEyeNormals[1][1] = pfEyeNormals[2][1] = pfEyeNormals[3][1] = fNy;
          pfBodyNormals[0][2] = pfBodyNormals[1][2] = pfBodyNormals[2][2] = pfBodyNormals[3][2] = pfBodyNormals[4][2] = pfBodyNormals[5][2] = pfBodyNormals[6][2] = pfBodyNormals[7][2] = pfEyeNormals[0][2] = pfEyeNormals[1][2] = pfEyeNormals[2][2] = pfEyeNormals[3][2] = fNz;
          pfBodyQuadPtr += 8;
          pfBodyNormals += 8;
          pfEyeQuadPtr += 4;
          pfEyeNormals += 4;
          }
      else
          {
          pfBodyQuadPtr[0][0] = pfBodyQuadPtr[1][0] = fX;
          pfBodyQuadPtr[2][0] = pfBodyQuadPtr[3][0] = fNextX;
          pfBodyQuadPtr[0][1] = pfBodyQuadPtr[3][1] = fY0;
          pfBodyQuadPtr[1][1] = pfBodyQuadPtr[2][1] = fY1;
          pfBodyQuadPtr[0][2] = pfBodyQuadPtr[1][2] = fZ;
          pfBodyQuadPtr[2][2] = pfBodyQuadPtr[3][2] = fNextZ;
          pfBodyNormals[0][0] = pfBodyNormals[1][0] = pfBodyNormals[2][0] = pfBodyNormals[3][0] = fNx;
          pfBodyNormals[0][1] = pfBodyNormals[1][1] = pfBodyNormals[2][1] = pfBodyNormals[3][1] = fNy;
          pfBodyNormals[0][2] = pfBodyNormals[1][2] = pfBodyNormals[2][2] = pfBodyNormals[3][2] = fNz;
          pfBodyQuadPtr += 4;
          pfBodyNormals += 4;
          }
      fX = fNextX;
      fZ = fNextZ;
      }
  iIndex += (m_ciRadialPieces + m_ciEyeWidth * 4) << 2;

  // build body triangle fan (bottom plate)
  fY = (float) m_ciBodyHeight / 2.0f;
  glfVertex[iIndex][0] = 0.0f;
  glfVertex[iIndex][1] = fY;
  glfVertex[iIndex][2] = 0.0f;
  glfNormal[iIndex][0] = 0.0f;
  glfNormal[iIndex][1] = 1.0f;
  glfNormal[iIndex][2] = 0.0f;
  for (int iRad = 0; iRad <= m_ciRadialPieces; iRad++)
      {
      float fAngle = 2.0f * (float) M_PI * iRad / m_ciRadialPieces;
      glfVertex[iIndex + iRad + 1][0] = -cos(fAngle) * m_ciBodyWidth / 2.0f;
      glfVertex[iIndex + iRad + 1][1] = fY;
      glfVertex[iIndex + iRad + 1][2] = sin(fAngle) * m_ciBodyWidth / 2.0f;
      glfNormal[iIndex + iRad + 1][0] = 0.0f;
      glfNormal[iIndex + iRad + 1][1] = 1.0f;
      glfNormal[iIndex + iRad + 1][2] = 0.0f;
      }
  iIndex += m_ciRadialPieces + 2;

  // build legs
  BuildLegs(glfVertex + iIndex, glfNormal + iIndex, false);
  iIndex += 144;

  // done
  return true;
}

void CInvader30::DestroyModel(void)
{
  // free static model memory
  if (glfVertex) free(glfVertex);
  glfVertex = NULL;
  if (glfNormal) free(glfNormal);
  glfNormal = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CInvader30 constructor/destructor

CInvader30::CInvader30()
{
  // set default colors
  m_uchConeColor[0] = 0x00;  m_uchConeColor[1] = 0xff;  m_uchConeColor[2] = 0x00;
  m_uchBodyColor[0] = 0x00;  m_uchBodyColor[1] = 0xff;  m_uchBodyColor[2] = 0x00;
  m_uchEyeColor[0] = 0x9f;   m_uchEyeColor[1] = 0x00;   m_uchEyeColor[2] = 0x00;
  m_uchLegColor[0] = 0x00;   m_uchLegColor[1] = 0x00;   m_uchLegColor[2] = 0xff;

  // allocate model arrays
  m_pfVertex  = (GLfloat (*)[3]) malloc(m_ciTotalVertices * sizeof(GLfloat) * 3);
  m_pfNormal  = (GLfloat (*)[3]) malloc(m_ciTotalVertices * sizeof(GLfloat) * 3);

  // copy static model data into our member storage
  memcpy(m_pfVertex, glfVertex,  m_ciTotalVertices * sizeof(GLfloat) * 3);
  memcpy(m_pfNormal, glfNormal,  m_ciTotalVertices * sizeof(GLfloat) * 3);

  // set my maximum thrust
  m_fMaxThrust    = 0.016f;
}

CInvader30::~CInvader30()
{
  // free model memory
  free(m_pfVertex);
  m_pfVertex  = NULL;
  free(m_pfNormal);
  m_pfNormal  = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CInvader30 accessor functions

void CInvader30::GetBodyBounding(float *fL, float *fR, float *fT, float *fB) const
{
  *fL = m_fPositionX - (float) m_ciBodyWidth / 2.0f;
  *fR = *fL + (float) m_ciBodyWidth;
  *fT = m_fPositionY - (float) m_ciBodyHeight / 2.0f - m_ciConeHeight - m_ciCapHeight;
  *fB = m_fPositionY + (float) m_ciBodyHeight / 2.0f;
}

void CInvader30::GetParticlePolys(SPolys *psPoly) const
{
  float fHealth = (float) m_iLastHealth / (float) m_iFullHealth;
  float fDamage = 1.0f - fHealth;

  // set up the Polys struct for triangles and quad colors
  psPoly->uiNumTriColors = 2;
  psPoly->uiNumQuadColors = 4;
  psPoly->aucTriColors[0][0] = (unsigned char) (m_uchConeColor[0] * fHealth + 0xff * fDamage);
  psPoly->aucTriColors[0][1] = (unsigned char) (m_uchConeColor[1] * fHealth);
  psPoly->aucTriColors[0][2] = (unsigned char) (m_uchConeColor[2] * fHealth);
  psPoly->aucTriColors[1][0] = (unsigned char) (m_uchBodyColor[0] * fHealth + 0xff * fDamage);
  psPoly->aucTriColors[1][1] = (unsigned char) (m_uchBodyColor[1] * fHealth);
  psPoly->aucTriColors[1][2] = (unsigned char) (m_uchBodyColor[2] * fHealth);
  psPoly->aucQuadColors[0][0] = (unsigned char) (m_uchConeColor[0] * fHealth + 0xff * fDamage);
  psPoly->aucQuadColors[0][1] = (unsigned char) (m_uchConeColor[1] * fHealth);
  psPoly->aucQuadColors[0][2] = (unsigned char) (m_uchConeColor[2] * fHealth);
  psPoly->aucQuadColors[1][0] = (unsigned char) (m_uchBodyColor[0] * fHealth + 0xff * fDamage);
  psPoly->aucQuadColors[1][1] = (unsigned char) (m_uchBodyColor[1] * fHealth);
  psPoly->aucQuadColors[1][2] = (unsigned char) (m_uchBodyColor[2] * fHealth);
  psPoly->aucQuadColors[2][0] = m_uchEyeColor[0];
  psPoly->aucQuadColors[2][1] = m_uchEyeColor[1];
  psPoly->aucQuadColors[2][2] = m_uchEyeColor[2];
  psPoly->aucQuadColors[3][0] = m_uchLegColor[0];
  psPoly->aucQuadColors[3][1] = m_uchLegColor[1];
  psPoly->aucQuadColors[3][2] = m_uchLegColor[2];

  // set up some indices
  unsigned int uiSrcIdx = 0;
  unsigned int uiQuadIdx = 0;
  unsigned int uiTriIdx = 0;

  // do the 'cap' triangle fan
  psPoly->uiNumTriangles[0] = m_ciRadialPieces;
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

  // do the cone
  psPoly->uiNumQuads[0] = m_ciRadialPieces;
  for (unsigned int ui = 0; ui < m_ciRadialPieces; ui++)
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

  // do the body
  unsigned int uiBodyQuads = (m_ciRadialPieces + (m_ciEyeWidth << 1));
  psPoly->uiNumQuads[1] = uiBodyQuads;
  for (unsigned int uiQ = 0; uiQ < uiBodyQuads; uiQ++)
    {
    for (unsigned int uiV = 0; uiV < 4; uiV++)
      {
      memcpy(psPoly->pfQuadVertices[uiQuadIdx][uiV], m_pfVertex[uiSrcIdx], 3 * sizeof(float));
      memcpy(psPoly->pfQuadNormals[uiQuadIdx][uiV], m_pfNormal[uiSrcIdx], 3 * sizeof(float));
      uiSrcIdx++;
      }
    uiQuadIdx++;
    }


  // do the eyes
  psPoly->uiNumQuads[2] = m_ciEyeWidth * 2;
  for (unsigned int uiQ = 0; uiQ < m_ciEyeWidth * 2; uiQ++)
    {
    for (unsigned int uiV = 0; uiV < 4; uiV++)
      {
      memcpy(psPoly->pfQuadVertices[uiQuadIdx][uiV], m_pfVertex[uiSrcIdx], 3 * sizeof(float));
      memcpy(psPoly->pfQuadNormals[uiQuadIdx][uiV], m_pfNormal[uiSrcIdx], 3 * sizeof(float));
      uiSrcIdx++;
      }
    uiQuadIdx++;
    }

  // do the lower body triangle fan
  psPoly->uiNumTriangles[1] = m_ciRadialPieces;
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

  // do the legs
  psPoly->uiNumQuads[3] = 36;
  for (unsigned int uiQ = 0; uiQ < 36; uiQ++)
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

void CInvader30::Draw(void) const
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
  ModulateColor(piDiffuse, piAmbient, m_uchConeColor);

  if (m_bFlash)
    {
    piDiffuse[0] = piDiffuse[1] = piDiffuse[2] = piDiffuse[3] = 0x7fffffff;
    piAmbient[0] = piAmbient[1] = piAmbient[2] = piAmbient[3] = 0x7fffffff;
    }

  glMaterialfv(GL_FRONT, GL_SPECULAR, pfSpecular);
  glMaterialfv(GL_FRONT, GL_SHININESS, pfShininess);
  glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
  glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);

  // draw the cap
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, m_pfVertex);
  glNormalPointer(GL_FLOAT, 0, m_pfNormal);
  glDrawArrays(GL_TRIANGLE_FAN, 0, m_ciRadialPieces + 2);
  int iIndex = m_ciRadialPieces + 2;

  // draw the cone
  glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
  glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
  glDrawArrays(GL_QUAD_STRIP, iIndex, (m_ciRadialPieces + 1) * 2);
  iIndex += (m_ciRadialPieces + 1) * 2;

  // draw body
  if (!m_bFlash)
    {
    ModulateColor(piDiffuse, piAmbient, m_uchBodyColor);
    glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
    glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
    }
  glDrawArrays(GL_QUADS, iIndex, (m_ciRadialPieces + (m_ciEyeWidth << 1)) << 2);
  glDrawArrays(GL_TRIANGLE_FAN, iIndex + m_ciRadialPieces*4 + m_ciEyeWidth*16, m_ciRadialPieces + 2);

  // draw eyes
  piDiffuse[0]   = m_uchEyeColor[0] << 23;
  piDiffuse[1]   = m_uchEyeColor[1] << 23;
  piDiffuse[2]   = m_uchEyeColor[2] << 23;
  piAmbient[0]   = m_uchEyeColor[0] << 21;
  piAmbient[1]   = m_uchEyeColor[1] << 21;
  piAmbient[2]   = m_uchEyeColor[2] << 21;
  glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
  glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
  glDrawArrays(GL_QUADS, iIndex + m_ciRadialPieces*4 + m_ciEyeWidth*8, m_ciEyeWidth << 3);
  iIndex += m_ciRadialPieces*4 + m_ciEyeWidth*16 + m_ciRadialPieces + 2;

  // draw legs
  piDiffuse[0]   = m_uchLegColor[0] << 23;
  piDiffuse[1]   = m_uchLegColor[1] << 23;
  piDiffuse[2]   = m_uchLegColor[2] << 23;
  piAmbient[0]   = m_uchLegColor[0] << 21;
  piAmbient[1]   = m_uchLegColor[1] << 21;
  piAmbient[2]   = m_uchLegColor[2] << 21;
  glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
  glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
  glDrawArrays(GL_QUADS, iIndex, 144);
  iIndex += 144;

  // all done
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  return;
}

/////////////////////////////////////////////////////////////////////////////
// CInvader30 modifier functions

void CInvader30::SetConeColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchConeColor[0] = uchRed;
  m_uchConeColor[1] = uchGreen;
  m_uchConeColor[2] = uchBlue;
}

void CInvader30::SetBodyColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchBodyColor[0] = uchRed;
  m_uchBodyColor[1] = uchGreen;
  m_uchBodyColor[2] = uchBlue;
}

void CInvader30::SetEyeColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchEyeColor[0] = uchRed;
  m_uchEyeColor[1] = uchGreen;
  m_uchEyeColor[2] = uchBlue;
}

void CInvader30::SetLegColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchLegColor[0] = uchRed;
  m_uchLegColor[1] = uchGreen;
  m_uchLegColor[2] = uchBlue;
}

void CInvader30::ToggleLegs(bool bLegPosition)
{
  BuildLegs(m_pfVertex + m_ciTotalVertices - 144, m_pfNormal + m_ciTotalVertices - 144, bLegPosition);
  return;
}

bool CInvader30::TestCollision(float fLeftX, float fRightX, float fTopY, float fBotY, int iDamage)
{
  // first do a real fast but rough test
  if (fRightX < m_fPositionX - m_ciBodyWidth / 2.0f ||
      fLeftX  > m_fPositionX + m_ciBodyWidth / 2.0f ||
      fTopY   > m_fPositionY + m_ciBodyHeight / 2.0f ||
      fBotY   < m_fPositionY - m_ciBodyHeight / 2.0f - m_ciConeHeight) return false;

  // now a finer test
  float fX = (fLeftX + fRightX) / 2.0f;
  float fY = m_ciBodyHeight / 2.0f + m_ciConeHeight - (fabs(m_fPositionX - fX) * 2.0f * m_ciConeHeight / m_ciBodyWidth);
  if (fBotY > m_fPositionY - fY)
    {
    m_bFlash = true;
    m_iLastHealth = m_iHealth;
    m_iHealth -= iDamage;
    return true;
    }

  return false;
}

// this function tests for a collision between a rotating kamikaze invader and the LaserBase
bool CInvader30::KamikazeCollision(float fLeftX, float fRightX, float fTopY, float fBotY)
{
  // first do a real fast but rough test
  if (fRightX < m_fPositionX - m_ciBodyWidth / 2.0f ||
      fLeftX  > m_fPositionX + m_ciBodyWidth / 2.0f ||
      fTopY   > m_fPositionY + m_ciBodyHeight / 2.0f + m_ciConeHeight) return false;

  // if the center of this Invader body is inside the LaserBase then there is a collision
  if (m_fPositionX > fLeftX && m_fPositionX < fRightX) return true;

  // otherwise calculate distance between center of Invader and edge of LaserBase
  float fDistance;
  if (m_fPositionX < fLeftX)
    fDistance = fLeftX - m_fPositionX;
  else
    fDistance = m_fPositionX - fRightX;

  // now a finer test
  float fY = m_ciBodyHeight / 2.0f + m_ciConeHeight - (fDistance * 2.0f * m_ciConeHeight / m_ciBodyWidth);
  if (fTopY > m_fPositionY + fY)
    return false;

  // there is a collision between the edge of the LaserBase and the invader cone
  return true;
}

bool CInvader30::BunkerCollision(float fLeftX, float fRightX, float fTopY)
{
  // this collision test is very simple
  if (fRightX < m_fPositionX - m_ciBodyWidth / 2.0f ||
      fLeftX  > m_fPositionX + m_ciBodyWidth / 2.0f ||
      fTopY   > m_fPositionY + m_ciBodyHeight / 2.0f) return false;

  // there was a collision!
  float fDot = 2.0f * m_fVelocityY;   // this is the force of the collision

  // clip negative dot products to 0
  if (fDot < 0.0f) fDot = 0.0f;

  // impart the force
  ApplyForce(0.0f, -fDot);

  // now put on the hurt
  int iDamage = (int) (fDot * CGameplay::m_fCollideDamage);
  m_iLastHealth = m_iHealth;
  m_iHealth -= iDamage;

  return true;
}


/////////////////////////////////////////////////////////////////////////////
// CInvader30 private functions

void CInvader30::BuildLegs(float (*pfVertex)[3], float (*pfNormal)[3], bool bLegPosition)
{
  float fAngle;

  if (bLegPosition)
    fAngle = -30.0f;
  else
    fAngle = 30.0f;

  float fLegX = m_ciBodyWidth / 2.0f - m_ciLegLength / 2.0f - m_ciLegThickness;
  float fLegY = 0.45f * m_ciBodyHeight;
  TubeSetStart(fLegX, fLegY, 0.0f, 300.0f, (float) m_ciLegThickness);
  TubeMove(pfVertex + 0, pfNormal + 0, (float) m_ciLegLength, 240.0f, true);
  TubeSetStart(-fLegX, fLegY, 0.0f, 240.0f, (float) m_ciLegThickness);
  TubeMove(pfVertex + 20, pfNormal + 20, (float) m_ciLegLength, 300.0f, true);
  TubeSetStart(fLegX, fLegY, 0.0f, 240.0f, (float) (m_ciLegThickness - 1));
  TubeMove(pfVertex + 40, pfNormal + 40, (float) m_ciLegLength / 2.0f, 180.0f, false);
  TubeMove(pfVertex + 56, pfNormal + 56, (float) m_ciBodyWidth / 4.0f, 270.0f - fAngle, false);
  TubeMove(pfVertex + 72, pfNormal + 72, (float) m_ciLegLength / 2.0f, 270.0f + fAngle, true);
  TubeSetStart(-fLegX, fLegY, 0.0f, 300.0f, (float) (m_ciLegThickness - 1));
  TubeMove(pfVertex + 92, pfNormal + 92, (float) m_ciLegLength / 2.0f, 0.0f, false);
  TubeMove(pfVertex + 108, pfNormal + 108, (float) m_ciBodyWidth / 4.0f, 270.0f + fAngle, false);
  TubeMove(pfVertex + 124, pfNormal + 124, (float) m_ciLegLength / 2.0f, 270.0f - fAngle, true);
}
