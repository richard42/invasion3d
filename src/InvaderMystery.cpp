/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - InvaderMystery.cpp                                       *
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
#include "InvaderMystery.h"
#include "Particles.h"

// const model parameters
const int CInvaderMystery::m_ciBodyWidth = 80;
const int CInvaderMystery::m_ciBodyHeight = 50;
const int CInvaderMystery::m_ciRadialPieces = 27;
const int CInvaderMystery::m_ciBodySlices = 7;
const int CInvaderMystery::m_ciWindows = 9;
const int CInvaderMystery::m_ciWindowHeight = 1;
const int CInvaderMystery::m_ciLandingGears = 4;
const int CInvaderMystery::m_ciGearPieces = 16;
const int CInvaderMystery::m_ciGearHeight = 12;
const int CInvaderMystery::m_ciGearSeparation = 60;
const int CInvaderMystery::m_ciCenterHeight = 3;
const int CInvaderMystery::m_ciCenterWidth = 10;
  // calculated parameters
const int CInvaderMystery::m_ciTotalVertices = (m_ciBodySlices - 1) * (m_ciRadialPieces + 1) * 2 + 4 + 20 +
                                                m_ciRadialPieces * 6 + m_ciLandingGears * (m_ciGearPieces + 2);

// set static pointers to NULL
GLfloat (*CInvaderMystery::glfVertex)[3] = NULL;
GLfloat (*CInvaderMystery::glfNormal)[3] = NULL;


/////////////////////////////////////////////////////////////////////////////
// CInvaderMystery static functions

bool CInvaderMystery::InitModel(void)
{
  // allocate memory
  glfVertex  = (GLfloat (*)[3]) malloc(m_ciTotalVertices * sizeof(GLfloat) * 3);
  if (!glfVertex)
    {
    return false;
    }
  glfNormal  = (GLfloat (*)[3]) malloc(m_ciTotalVertices * sizeof(GLfloat) * 3);
  if (!glfNormal)
    {
    free(glfVertex);
    glfVertex = NULL;
    return false;
    }

  float fX0, fY0, fZ0, fX1, fY1, fZ1;
  float fX, fY, fZ, fR;
  float fNx, fNy, fNz;

  // build body top and bottom triangle fans
  int iTop = 0;
  int iBot = m_ciRadialPieces + 2;
  float fAngle0 = ((float) M_PI / 2.0f) / (m_ciBodySlices + 1);
  fR = sin(fAngle0) * (float) m_ciBodyWidth / 2.0f;
  fY = -cos(fAngle0) * (float) m_ciBodyHeight / 2.0f;
  glfVertex[iTop][0] = 0.0f;
  glfVertex[iTop][1] = fY;
  glfVertex[iTop][2] = 0.0f;
  glfVertex[iBot][0] = 0.0f;
  glfVertex[iBot][1] = 0.0f;
  glfVertex[iBot][2] = 0.0f;
  glfNormal[iTop][0] = 0.0f;
  glfNormal[iTop][1] = -1.0f;
  glfNormal[iTop][2] = 0.0f;
  glfNormal[iBot][0] = 0.0f;
  glfNormal[iBot][1] = 1.0f;
  glfNormal[iBot][2] = 0.0f;
  for (int iRad = 0; iRad <= m_ciRadialPieces; iRad++)
      {
      float fAngle = 2 * (float) M_PI * iRad / m_ciRadialPieces;
      glfVertex[iTop + iRad + 1][0] = -cos(fAngle) * fR;
      glfVertex[iTop + iRad + 1][1] = fY;
      glfVertex[iTop + iRad + 1][2] = -sin(fAngle) * fR;
      glfNormal[iTop + iRad + 1][0] = 0.0f;
      glfNormal[iTop + iRad + 1][1] = -1.0f;
      glfNormal[iTop + iRad + 1][2] = 0.0f;
      glfVertex[iBot + iRad + 1][0] = -cos(fAngle) * (float) m_ciBodyWidth / 2.0f;
      glfVertex[iBot + iRad + 1][1] = 0.0f;
      glfVertex[iBot + iRad + 1][2] = sin(fAngle) * (float) m_ciBodyWidth / 2.0f;
      glfNormal[iBot + iRad + 1][0] = 0.0f;
      glfNormal[iBot + iRad + 1][1] = 1.0f;
      glfNormal[iBot + iRad + 1][2] = 0.0f;
      }
  int iIndex = 2 * (m_ciRadialPieces + 2);

  // build body quad strips without windows
  GLfloat (*pfBodyStrips)[3] = glfVertex + iIndex;
  GLfloat (*pfBodyNormals)[3] = glfNormal + iIndex;
  for (int iSlice = 0; iSlice < m_ciBodySlices; iSlice++)
      {
      if (iSlice == m_ciWindowHeight) continue;
      float fZAngle0 = (iSlice + 0) * ((float) M_PI / 2.0f) / (m_ciBodySlices + 1);
      float fZAngle1 = (iSlice + 1) * ((float) M_PI / 2.0f) / (m_ciBodySlices + 1);
      float fR0 = cos(fZAngle0) * m_ciBodyWidth / 2;
      float fR1 = cos(fZAngle1) * m_ciBodyWidth / 2;
      fX0 = -fR0;
      fX1 = -fR1;
      fY0 = -sin(fZAngle0) * m_ciBodyHeight / 2;
      fY1 = -sin(fZAngle1) * m_ciBodyHeight / 2;
      fZ0 = 0.0f;
      fZ1 = 0.0f;
      for (int iRad = 0; iRad <= m_ciRadialPieces; iRad++)
          {
          float fTAngle = 2 * (float) M_PI * (iRad + 1) / m_ciRadialPieces;
          float fNextX0 = -cos(fTAngle) * fR0;
          float fNextX1 = -cos(fTAngle) * fR1;
          float fNextZ0 = -sin(fTAngle) * fR0;
          float fNextZ1 = -sin(fTAngle) * fR1;
          pfBodyStrips[0][0] = fX1;
          pfBodyStrips[0][1] = fY1;
          pfBodyStrips[0][2] = fZ1;
          pfBodyStrips[1][0] = fX0;
          pfBodyStrips[1][1] = fY0;
          pfBodyStrips[1][2] = fZ0;
          float fNx, fNy, fNz;
          GetNormal(fNx,fNy,fNz, fX1,fY1,fZ1, fNextX0,fY0,fNextZ0, fNextX1,fY1,fNextZ1);
          pfBodyNormals[0][0] = fNx;
          pfBodyNormals[0][1] = fNy;
          pfBodyNormals[0][2] = fNz;
          GetNormal(fNx,fNy,fNz, fX0,fY0,fZ0, fNextX0,fY0,fNextZ0, fNextX1,fY1,fNextZ1);
          pfBodyNormals[1][0] = fNx;
          pfBodyNormals[1][1] = fNy;
          pfBodyNormals[1][2] = fNz;
          pfBodyStrips += 2;
          pfBodyNormals += 2;
          fX0 = fNextX0;
          fX1 = fNextX1;
          fZ0 = fNextZ0;
          fZ1 = fNextZ1;
          }
      }
  iIndex += (m_ciBodySlices - 1) * (m_ciRadialPieces + 1) * 2;

  // build body and eye quads
  GLfloat (*pfBodyQuads)[3] = glfVertex + iIndex;
  GLfloat (*pfBodyQuadNormals)[3] = glfNormal + iIndex;
  GLfloat (*pfWindowQuads)[3] = glfVertex + iIndex + (m_ciRadialPieces - m_ciWindows) * 4;
  GLfloat (*pfWindowNormals)[3] = glfNormal + iIndex + (m_ciRadialPieces - m_ciWindows) * 4;
  float fZAngle0 = (m_ciWindowHeight + 0) * ((float) M_PI / 2.0f) / (m_ciBodySlices + 1);
  float fZAngle1 = (m_ciWindowHeight + 1) * ((float) M_PI / 2.0f) / (m_ciBodySlices + 1);
  float fR0 = cos(fZAngle0) * m_ciBodyWidth / 2;
  float fR1 = cos(fZAngle1) * m_ciBodyWidth / 2;
  fX0 = -fR0;
  fX1 = -fR1;
  fY0 = -sin(fZAngle0) * m_ciBodyHeight / 2;
  fY1 = -sin(fZAngle1) * m_ciBodyHeight / 2;
  fZ0 = 0.0f;
  fZ1 = 0.0f;
  float fWindowCount = 0.0f;
  float fWindowFrac  = (float) m_ciWindows / (float) m_ciRadialPieces;
  for (int iRad = 0; iRad < m_ciRadialPieces; iRad++)
      {
      GLfloat (*pfQuadPtr)[3];
      GLfloat (*pfNormPtr)[3];
      float fTAngle = 2 * (float) M_PI * (iRad + 1) / m_ciRadialPieces;
      float fNextX0 = -cos(fTAngle) * fR0;
      float fNextX1 = -cos(fTAngle) * fR1;
      float fNextZ0 = -sin(fTAngle) * fR0;
      float fNextZ1 = -sin(fTAngle) * fR1;
      fWindowCount += fWindowFrac;
      if (fWindowCount >= 0.99999f)
          {
          fWindowCount -= 1.0f;
          // add a window quad
          pfQuadPtr = pfWindowQuads;
          pfNormPtr = pfWindowNormals;
          pfWindowQuads += 4;
          pfWindowNormals += 4;
          }
      else
          {
          // add a body quad
          pfQuadPtr = pfBodyQuads;
          pfNormPtr = pfBodyQuadNormals;
          pfBodyQuads += 4;
          pfBodyQuadNormals += 4;
          }
      pfQuadPtr[0][0] = fX1;
      pfQuadPtr[1][0] = fX0;
      pfQuadPtr[2][0] = fNextX0;
      pfQuadPtr[3][0] = fNextX1;
      pfQuadPtr[0][1] = pfQuadPtr[3][1] = fY1;
      pfQuadPtr[1][1] = pfQuadPtr[2][1] = fY0;
      pfQuadPtr[0][2] = fZ1;
      pfQuadPtr[1][2] = fZ0;
      pfQuadPtr[2][2] = fNextZ0;
      pfQuadPtr[3][2] = fNextZ1;
      GetNormal(fNx,fNy,fNz, fX1,fY1,fZ1, fNextX0,fY0,fNextZ0, fNextX1,fY1,fNextZ1);
      pfNormPtr[0][0] = pfNormPtr[3][0] = fNx;
      pfNormPtr[0][1] = pfNormPtr[3][1] = fNy;
      pfNormPtr[0][2] = pfNormPtr[3][2] = fNz;
      GetNormal(fNx,fNy,fNz, fX0,fY0,fZ0, fNextX0,fY0,fNextZ0, fNextX1,fY1,fNextZ1);
      pfNormPtr[1][0] = pfNormPtr[2][0] = fNx;
      pfNormPtr[1][1] = pfNormPtr[2][1] = fNy;
      pfNormPtr[1][2] = pfNormPtr[2][2] = fNz;
      fX0 = fNextX0;
      fX1 = fNextX1;
      fZ0 = fNextZ0;
      fZ1 = fNextZ1;
      }
  iIndex += m_ciRadialPieces * 4;

  // draw landing gears
  GLfloat (*pfGearTriFans)[3] = glfVertex + iIndex;
  GLfloat (*pfGearNormals)[3] = glfNormal + iIndex;
  for (int iGear = 0; iGear < m_ciLandingGears; iGear++)
      {
      float fGAngle = 2 * (float) M_PI * iGear / m_ciLandingGears;
      float fGearX = -cos(fGAngle) * (float) m_ciGearSeparation / 2.0f;
      float fGearZ = -sin(fGAngle) * (float) m_ciGearSeparation / 2.0f;
      pfGearTriFans[0][0] = fGearX;
      pfGearTriFans[0][1] = (float) m_ciGearHeight;
      pfGearTriFans[0][2] = fGearZ;
      pfGearNormals[0][0] = 0.0f;
      pfGearNormals[0][1] = 1.0f;
      pfGearNormals[0][2] = 0.0f;
      fX = (float) -m_ciGearHeight / 2.0f;
      fZ = 0.0f;
      for (int iRad = 0; iRad <= m_ciGearPieces; iRad++)
          {
          float fAngle = 2 * (float) M_PI * (iRad + 1) / m_ciGearPieces;
          float fNextX = -cos(fAngle) * m_ciGearHeight / 2.0f;
          float fNextZ = sin(fAngle) * m_ciGearHeight / 2.0f;
          pfGearTriFans[iRad + 1][0] = fGearX + fX;
          pfGearTriFans[iRad + 1][1] = 0.0f;
          pfGearTriFans[iRad + 1][2] = fGearZ + fZ;
          GetNormal(fNx,fNy,fNz, 0.0f,(float) m_ciGearHeight,0.0f, fX,0.0f,fZ, fNextX,0.0f,fNextZ);
          pfGearNormals[iRad + 1][0] = fNx;
          pfGearNormals[iRad + 1][1] = fNy;
          pfGearNormals[iRad + 1][2] = fNz;
          fX = fNextX;
          fZ = fNextZ;
          }
      pfGearTriFans += m_ciGearPieces + 2;
      pfGearNormals += m_ciGearPieces + 2;
      }
  iIndex += m_ciLandingGears * (m_ciGearPieces + 2);

  // build bottom center piece
  TubeSetStart(0.0f,0.0f,0.0f, 270.0f, (float) m_ciCenterWidth);
  TubeMove(glfVertex + iIndex, glfNormal + iIndex, (float) m_ciCenterHeight, 270.0f, true);
  iIndex += 20;

  // done
  return true;
}

void CInvaderMystery::DestroyModel(void)
{
  // free static model memory
  if (glfVertex) free(glfVertex);
  glfVertex = NULL;
  if (glfNormal) free(glfNormal);
  glfNormal = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CInvaderMystery constructor/destructor

CInvaderMystery::CInvaderMystery()
{
  // set default colors
  m_uchBodyColor[0]   = 0x00;  m_uchBodyColor[1]    = 0x00;  m_uchBodyColor[2]    = 0xd0;
  m_uchWindowColor[0] = 0xff;   m_uchWindowColor[1] = 0xff;   m_uchWindowColor[2] = 0x00;
  m_uchGearColor[0]   = 0xc0;   m_uchGearColor[1]   = 0x00;   m_uchGearColor[2]   = 0xc0;
  m_uchCenterColor[0] = 0xff;   m_uchCenterColor[1] = 0xff;   m_uchCenterColor[2] = 0xff;

  // set scale
  m_fScale = 1.0f;

  // allocate model arrays
  m_pfVertex  = (GLfloat (*)[3]) malloc(m_ciTotalVertices * sizeof(GLfloat) * 3);
  m_pfNormal  = (GLfloat (*)[3]) malloc(m_ciTotalVertices * sizeof(GLfloat) * 3);

  // copy static model data into our member storage
  memcpy(m_pfVertex, glfVertex,  m_ciTotalVertices * sizeof(GLfloat) * 3);
  memcpy(m_pfNormal, glfNormal,  m_ciTotalVertices * sizeof(GLfloat) * 3);
}

CInvaderMystery::~CInvaderMystery()
{
  // free model memory
  free(m_pfVertex);
  m_pfVertex  = NULL;
  free(m_pfNormal);
  m_pfNormal  = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CInvaderMystery accessor functions

void CInvaderMystery::GetParticlePolys(SPolys *psPoly) const
{
  // set up the Polys struct for triangles and quad colors
  psPoly->uiNumTriColors = 2;
  psPoly->uiNumQuadColors = 3;
  psPoly->aucTriColors[0][0] = m_uchBodyColor[0];
  psPoly->aucTriColors[0][1] = m_uchBodyColor[1];
  psPoly->aucTriColors[0][2] = m_uchBodyColor[2];
  psPoly->aucTriColors[1][0] = m_uchGearColor[0];
  psPoly->aucTriColors[1][1] = m_uchGearColor[1];
  psPoly->aucTriColors[1][2] = m_uchGearColor[2];
  psPoly->aucQuadColors[0][0] = m_uchBodyColor[0];
  psPoly->aucQuadColors[0][1] = m_uchBodyColor[1];
  psPoly->aucQuadColors[0][2] = m_uchBodyColor[2];
  psPoly->aucQuadColors[1][0] = m_uchWindowColor[0];
  psPoly->aucQuadColors[1][1] = m_uchWindowColor[1];
  psPoly->aucQuadColors[1][2] = m_uchWindowColor[2];
  psPoly->aucQuadColors[2][0] = m_uchCenterColor[0];
  psPoly->aucQuadColors[2][1] = m_uchCenterColor[1];
  psPoly->aucQuadColors[2][2] = m_uchCenterColor[2];

  // set up some indices
  unsigned int uiSrcIdx = 0;
  unsigned int uiQuadIdx = 0;
  unsigned int uiTriIdx = 0;

  // do the top and bottom body triangle fans
  psPoly->uiNumTriangles[0] = 2 * m_ciRadialPieces;
  for (unsigned int uiFan = 0; uiFan < 2; uiFan++)
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

  // do the quad strips for the body
  psPoly->uiNumQuads[0] = m_ciBodySlices * m_ciRadialPieces - m_ciWindows;
  for (unsigned int uiStrip = 0; uiStrip < m_ciBodySlices - 1; uiStrip++)
    {
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
    }

  // now the body quads
  unsigned int uiBodyQuads = m_ciRadialPieces - m_ciWindows;
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

  // do the windows
  psPoly->uiNumQuads[1] = m_ciWindows;
  for (unsigned int uiQ = 0; uiQ < m_ciWindows; uiQ++)
    {
    for (unsigned int uiV = 0; uiV < 4; uiV++)
      {
      memcpy(psPoly->pfQuadVertices[uiQuadIdx][uiV], m_pfVertex[uiSrcIdx], 3 * sizeof(float));
      memcpy(psPoly->pfQuadNormals[uiQuadIdx][uiV], m_pfNormal[uiSrcIdx], 3 * sizeof(float));
      uiSrcIdx++;
      }
    uiQuadIdx++;
    }

  // do the landing gears
  psPoly->uiNumTriangles[1] = m_ciLandingGears * m_ciGearPieces;
  for (unsigned int uiFan = 0; uiFan < m_ciLandingGears; uiFan++)
    {
    for (unsigned int ui = 0; ui < m_ciGearPieces; ui++)
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
    uiSrcIdx += m_ciGearPieces + 2;
    }

  // lastly, the center piece
  psPoly->uiNumQuads[2] = 5;
  for (unsigned int uiQ = 0; uiQ < 5; uiQ++)
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

void CInvaderMystery::Draw(void) const
{
  // Position the model in 3-space
  glTranslatef(m_fPositionX, m_fPositionY, m_fPositionZ);

  // set spin of mystery invader
  glRotatef(10.0f, 1.0f, 0.0f, 0.0f);
  glRotatef(m_fAngle, m_fAngleX, m_fAngleY, m_fAngleZ);

  // set scale
  if (m_fScale != 1.0f)
    {
    glScalef(m_fScale, m_fScale, m_fScale);
    glEnable(GL_NORMALIZE);
    }

  GLfloat pfSpecular[4]  = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat pfShininess[1] = {32.0f};
  GLint   piDiffuse[4]   = {m_uchBodyColor[0] << 23, m_uchBodyColor[1] << 23, m_uchBodyColor[2] << 23, 0x7fffffff};
  GLint   piAmbient[4]   = {m_uchBodyColor[0] << 20, m_uchBodyColor[1] << 20, m_uchBodyColor[2] << 20, 0x7fffffff};

  if (m_bFlash)
    {
    piDiffuse[0] = piDiffuse[1] = piDiffuse[2] = piDiffuse[3] = 0x7fffffff;
    piAmbient[0] = piAmbient[1] = piAmbient[2] = piAmbient[3] = 0x7fffffff;
    }

  glMaterialfv(GL_FRONT, GL_SPECULAR, pfSpecular);
  glMaterialfv(GL_FRONT, GL_SHININESS, pfShininess);
  glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
  glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);

  // draw the body
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, m_pfVertex);
  glNormalPointer(GL_FLOAT, 0, m_pfNormal);
  glDrawArrays(GL_TRIANGLE_FAN, 0, m_ciRadialPieces + 2);
  glDrawArrays(GL_TRIANGLE_FAN, m_ciRadialPieces + 2, m_ciRadialPieces + 2);
  int iIndex = 2 * (m_ciRadialPieces + 2);
  for (int iStrip = 0; iStrip < m_ciBodySlices - 1; iStrip++)
    {
    glDrawArrays(GL_QUAD_STRIP, iIndex, (m_ciRadialPieces + 1) * 2);
    iIndex += (m_ciRadialPieces + 1) * 2;
    }
  glDrawArrays(GL_QUADS, iIndex, (m_ciRadialPieces - m_ciWindows) * 4);
  iIndex += (m_ciRadialPieces - m_ciWindows) * 4;

  // draw windows
  piDiffuse[0]   = m_uchWindowColor[0] << 23;
  piDiffuse[1]   = m_uchWindowColor[1] << 23;
  piDiffuse[2]   = m_uchWindowColor[2] << 23;
  piAmbient[0]   = m_uchWindowColor[0] << 20;
  piAmbient[1]   = m_uchWindowColor[1] << 20;
  piAmbient[2]   = m_uchWindowColor[2] << 20;
  glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
  glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
  glDrawArrays(GL_QUADS, iIndex, m_ciWindows * 4);
  iIndex += m_ciWindows * 4;

  // draw landing gear
  piDiffuse[0]   = m_uchGearColor[0] << 23;
  piDiffuse[1]   = m_uchGearColor[1] << 23;
  piDiffuse[2]   = m_uchGearColor[2] << 23;
  piAmbient[0]   = m_uchGearColor[0] << 20;
  piAmbient[1]   = m_uchGearColor[1] << 20;
  piAmbient[2]   = m_uchGearColor[2] << 20;
  glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
  glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
  for (int iGear = 0; iGear < m_ciLandingGears; iGear++)
    {
    glDrawArrays(GL_TRIANGLE_FAN, iIndex, m_ciGearPieces + 2);
    iIndex += m_ciGearPieces + 2;
    }

  // draw center bottom piece
  piDiffuse[0]   = m_uchCenterColor[0] << 23;
  piDiffuse[1]   = m_uchCenterColor[1] << 23;
  piDiffuse[2]   = m_uchCenterColor[2] << 23;
  piAmbient[0]   = m_uchCenterColor[0] << 20;
  piAmbient[1]   = m_uchCenterColor[1] << 20;
  piAmbient[2]   = m_uchCenterColor[2] << 20;
  glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
  glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
  glDrawArrays(GL_QUADS, iIndex, 20);
  iIndex += 20;

  // all done
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  if (m_fScale != 1.0f)
    {
    glDisable(GL_NORMALIZE);
    }
  return;
}

/////////////////////////////////////////////////////////////////////////////
// CInvaderMystery modifier functions

void CInvaderMystery::SetBodyColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchBodyColor[0] = uchRed;
  m_uchBodyColor[1] = uchGreen;
  m_uchBodyColor[2] = uchBlue;
}

void CInvaderMystery::SetWindowColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchWindowColor[0] = uchRed;
  m_uchWindowColor[1] = uchGreen;
  m_uchWindowColor[2] = uchBlue;
}

void CInvaderMystery::SetGearColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchGearColor[0] = uchRed;
  m_uchGearColor[1] = uchGreen;
  m_uchGearColor[2] = uchBlue;
}

void CInvaderMystery::SetCenterColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchCenterColor[0] = uchRed;
  m_uchCenterColor[1] = uchGreen;
  m_uchCenterColor[2] = uchBlue;
}

bool CInvaderMystery::TestCollision(float fLeftX, float fRightX, float fTopY, float fBotY, int iDamage)
{
  // first do a real fast but rough test
  if (fRightX < m_fPositionX - m_ciBodyWidth / 2.0f ||
      fLeftX  > m_fPositionX + m_ciBodyWidth / 2.0f ||
      fTopY   > m_fPositionY ||
      fBotY   < m_fPositionY - m_ciBodyHeight / 2.0f) return false;

  float fX = (fLeftX + fRightX) / 2.0f - m_fPositionX;
  
  // handle case where center of projectile is to the left or right of the ship
  if (fX <= -m_ciBodyWidth / 2.0f || fX  >= m_ciBodyWidth / 2.0f)
    {
    if (fBotY < m_fPositionY) return false;
    // it got hit
    m_bFlash = true;
    m_iHealth -= iDamage;
    return true;
    }

  // calculate height of mystery ship dome at the longitude of the projectile
  float fY = sqrt(m_ciBodyHeight * m_ciBodyHeight / 4.0f - m_ciBodyHeight * m_ciBodyHeight * fX * fX / (m_ciBodyWidth * m_ciBodyWidth));

  // see if the projectile is within the ship
  if (fBotY > m_fPositionY - fY)
    {
    m_bFlash = true;
    m_iHealth -= iDamage;
    return true;
    }

  return false;
}

/////////////////////////////////////////////////////////////////////////////
// CInvaderMystery private functions

