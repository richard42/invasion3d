/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Invader10.cpp                                            *
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
#include "Invader10.h"
#include "Particles.h"

// const model parameters
const int CInvader10::m_ciCapWidth = 80;
const int CInvader10::m_ciCapHeight = 25;
const int CInvader10::m_ciCapDepth = 40;
const int CInvader10::m_ciHeadHeight = 22;
const int CInvader10::m_ciRadialPieces = 10;
const int CInvader10::m_ciEyeSeparation = 10;
const int CInvader10::m_ciEyeWidth = 16;
const int CInvader10::m_ciEyeHeight = 7;
const int CInvader10::m_ciEyeDepth = 2;
const int CInvader10::m_ciLegThickness = 6;
const int CInvader10::m_ciLegSeparation = 20;
const int CInvader10::m_ciLegHeight = 18;
const int CInvader10::m_ciFootLength = 5;
// calculated parameters
const int CInvader10::m_ciTotalVertices = (m_ciRadialPieces + 1) * 2 + (m_ciRadialPieces + 2) * 2 + (9 + 10 + 30) * 4;

// set static pointers to NULL
GLfloat (*CInvader10::glfVertex)[3] = NULL;
GLfloat (*CInvader10::glfNormal)[3] = NULL;

// static data for collisions between invaders
int    CInvader10::m_iOutlinePoints = 5 + m_ciRadialPieces;
float *CInvader10::m_pfOutlineX = NULL;
float *CInvader10::m_pfOutlineY = NULL;


/////////////////////////////////////////////////////////////////////////////
// CInvader10 static functions

bool CInvader10::InitModel(void)
{
  // allocate memory
  glfVertex   = (GLfloat (*)[3]) malloc(m_ciTotalVertices * sizeof(GLfloat) * 3);
  glfNormal   = (GLfloat (*)[3]) malloc(m_ciTotalVertices * sizeof(GLfloat) * 3);

  m_pfOutlineX = (float *) malloc(m_iOutlinePoints * sizeof(float));
  m_pfOutlineY = (float *) malloc(m_iOutlinePoints * sizeof(float));

  if (!glfVertex || !glfNormal || !m_pfOutlineX || !m_pfOutlineY) return false;

  // set some constants
  int iTotalHeight = m_ciCapHeight + m_ciHeadHeight + m_ciLegHeight;
  int iCapOffset = -(iTotalHeight >> 1) + m_ciCapHeight;

  // set up the (X,Y) Invader Outline for doing collisions
  int iRad;
  m_pfOutlineX[0] = (float) m_ciCapWidth / 2.0f;
  m_pfOutlineY[0] = 0.0f;
  for (iRad = 0; iRad <= m_ciRadialPieces; iRad++)
    {
    m_pfOutlineX[iRad + 1] = cos((float) M_PI * iRad / m_ciRadialPieces) * m_ciCapWidth / 2.0f;
    m_pfOutlineY[iRad + 1] = (float) iCapOffset - sin((float) M_PI * iRad / m_ciRadialPieces) * m_ciCapHeight / 2.0f;
    }
  m_pfOutlineX[iRad + 1] = (float) -m_ciCapWidth / 2.0f;
  m_pfOutlineY[iRad + 1] = 0.0f;
  m_pfOutlineX[iRad + 2] = (float) -m_ciCapWidth / 2.0f;
  m_pfOutlineY[iRad + 2] = iCapOffset + (float) m_ciHeadHeight;
  m_pfOutlineX[iRad + 3] = (float) m_ciCapWidth / 2.0f;
  m_pfOutlineY[iRad + 3] = iCapOffset + (float) m_ciHeadHeight;

  // generate pointers
  GLfloat (*pfVertex)[3] = glfVertex;
  GLfloat (*pfNormal)[3] = glfNormal;

  // make indices
  int iCapQuadStrip   = 0;
  int iCapTriFanFront = (m_ciRadialPieces + 1) * 2;
  int iCapTriFanBack  = iCapTriFanFront + m_ciRadialPieces + 2;

  // build cap
  pfVertex[iCapTriFanFront][0] = 0.0f;
  pfVertex[iCapTriFanFront][1] = (GLfloat) iCapOffset;
  pfVertex[iCapTriFanFront][2] = (GLfloat) -(m_ciCapDepth >> 1);
  pfVertex[iCapTriFanBack][0] = 0.0f;
  pfVertex[iCapTriFanBack][1] = (GLfloat) iCapOffset;
  pfVertex[iCapTriFanBack][2] = (GLfloat) (m_ciCapDepth >> 1);
  pfNormal[iCapTriFanFront][0] = pfNormal[iCapTriFanBack][0] = 0.0f;
  pfNormal[iCapTriFanFront][1] = pfNormal[iCapTriFanBack][1] = 0.0f;
  pfNormal[iCapTriFanFront][2] = -1.0f;
  pfNormal[iCapTriFanBack][2]  = 1.0f;
  for (iRad = 0; iRad <= m_ciRadialPieces; iRad++)
      {
      float fX = -cos((float) M_PI * iRad / m_ciRadialPieces) * m_ciCapWidth / 2.0f;
      float fY = sin((float) M_PI * iRad / m_ciRadialPieces) * m_ciCapHeight / 2.0f;
      // add coordinates for quad strip on top
      pfVertex[iCapQuadStrip + iRad * 2][0] = (GLfloat) fX;
      pfVertex[iCapQuadStrip + iRad * 2][1] = (GLfloat) iCapOffset - fY;
      pfVertex[iCapQuadStrip + iRad * 2][2] = (GLfloat) (m_ciCapDepth >> 1);
      pfVertex[iCapQuadStrip + iRad * 2 + 1][0] = (GLfloat) fX;
      pfVertex[iCapQuadStrip + iRad * 2 + 1][1] = (GLfloat) iCapOffset - fY;
      pfVertex[iCapQuadStrip + iRad * 2 + 1][2] = (GLfloat) -(m_ciCapDepth >> 1);
      // add normals for this pair of vertices
      float fR = sqrt(fX * fX + fY * fY);
      pfNormal[iCapQuadStrip + iRad * 2][0] = pfNormal[iCapQuadStrip + iRad * 2 + 1][0] = (GLfloat) fX / fR;
      pfNormal[iCapQuadStrip + iRad * 2][1] = pfNormal[iCapQuadStrip + iRad * 2 + 1][1] = (GLfloat) -fY / fR;
      pfNormal[iCapQuadStrip + iRad * 2][2] = pfNormal[iCapQuadStrip + iRad * 2 + 1][2] = 0.0f;
      // add coordinates for triangle fan on front
      pfVertex[iCapTriFanFront + iRad + 1][0] = (GLfloat) -fX;
      pfVertex[iCapTriFanFront + iRad + 1][1] = (GLfloat) iCapOffset - fY;
      pfVertex[iCapTriFanFront + iRad + 1][2] = (GLfloat) -(m_ciCapDepth >> 1);
      // add coordinates for triangle fan on back
      pfVertex[iCapTriFanBack + iRad + 1][0] = (GLfloat) fX;
      pfVertex[iCapTriFanBack + iRad + 1][1] = (GLfloat) iCapOffset - fY;
      pfVertex[iCapTriFanBack + iRad + 1][2] = (GLfloat) (m_ciCapDepth >> 1);
      // add normals
      pfNormal[iCapTriFanFront + iRad + 1][0] = pfNormal[iCapTriFanBack + iRad + 1][0] = 0.0f;
      pfNormal[iCapTriFanFront + iRad + 1][1] = pfNormal[iCapTriFanBack + iRad + 1][1] = 0.0f;
      pfNormal[iCapTriFanFront + iRad + 1][2] = -1.0f;
      pfNormal[iCapTriFanBack + iRad + 1][2]  = 1.0f;
      }

  pfVertex += iCapTriFanBack + m_ciRadialPieces + 2;
  pfNormal += iCapTriFanBack + m_ciRadialPieces + 2;

  // build head
  GLfloat (*pfHeadQuads)[4][3]   = (GLfloat (*)[4][3]) pfVertex;
  GLfloat (*pfHeadNormals)[4][3] = (GLfloat (*)[4][3]) pfNormal;
  int iEyeTopY = ((m_ciHeadHeight - m_ciEyeHeight) >> 1) + iCapOffset;
  int iEyeBottomY = iEyeTopY + m_ciEyeHeight;
  int iEye0X0 = -(m_ciEyeSeparation >> 1) - m_ciEyeWidth;
  int iEye0X1 = iEye0X0 + m_ciEyeWidth;
  int iEye1X0 = m_ciEyeSeparation >> 1;
  int iEye1X1 = iEye1X0 + m_ciEyeWidth;
  // front of face
  pfHeadQuads[0][0][0] = pfHeadQuads[0][1][0] = pfHeadQuads[1][0][0] = pfHeadQuads[1][1][0] = pfHeadQuads[4][0][0] = pfHeadQuads[4][1][0] = (float) -(m_ciCapWidth >> 1);
  pfHeadQuads[0][2][0] = pfHeadQuads[0][3][0] = pfHeadQuads[3][2][0] = pfHeadQuads[3][3][0] = pfHeadQuads[4][2][0] = pfHeadQuads[4][3][0] = (float) (m_ciCapWidth >> 1);
  pfHeadQuads[1][2][0] = pfHeadQuads[1][3][0] = (float) iEye0X0;
  pfHeadQuads[2][0][0] = pfHeadQuads[2][1][0] = (float) iEye0X1;
  pfHeadQuads[2][2][0] = pfHeadQuads[2][3][0] = (float) iEye1X0;
  pfHeadQuads[3][0][0] = pfHeadQuads[3][1][0] = (float) iEye1X1;
  pfHeadQuads[0][1][1] = pfHeadQuads[0][2][1] = (float) (iCapOffset + m_ciHeadHeight);
  pfHeadQuads[0][0][1] = pfHeadQuads[0][3][1] = pfHeadQuads[1][1][1] = pfHeadQuads[1][2][1] = pfHeadQuads[2][1][1] = pfHeadQuads[2][2][1] = pfHeadQuads[3][1][1] = pfHeadQuads[3][2][1] = (float) iEyeBottomY;
  pfHeadQuads[4][1][1] = pfHeadQuads[4][2][1] = pfHeadQuads[1][0][1] = pfHeadQuads[1][3][1] = pfHeadQuads[2][0][1] = pfHeadQuads[2][3][1] = pfHeadQuads[3][0][1] = pfHeadQuads[3][3][1] = (float) iEyeTopY;
  pfHeadQuads[4][0][1] = pfHeadQuads[4][3][1] = (float) iCapOffset;
  for (int i = 0; i < 5; i++)
    for (int j = 0; j < 4; j++)
      pfHeadQuads[i][j][2] = (float) -(m_ciCapDepth >> 1);
  // back and sides
  pfHeadQuads[5][0][0] = pfHeadQuads[5][1][0] = pfHeadQuads[5][2][0] = pfHeadQuads[5][3][0] = pfHeadQuads[6][0][0] = pfHeadQuads[6][1][0] = (float) -(m_ciCapWidth >> 1);
  pfHeadQuads[7][0][0] = pfHeadQuads[7][1][0] = pfHeadQuads[7][2][0] = pfHeadQuads[7][3][0] = pfHeadQuads[6][2][0] = pfHeadQuads[6][3][0] = (float) (m_ciCapWidth >> 1);
  pfHeadQuads[5][1][1] = pfHeadQuads[5][2][1] = pfHeadQuads[6][1][1] = pfHeadQuads[6][2][1] = pfHeadQuads[7][1][1] = pfHeadQuads[7][2][1] = (float) iCapOffset;
  pfHeadQuads[5][0][1] = pfHeadQuads[5][3][1] = pfHeadQuads[6][0][1] = pfHeadQuads[6][3][1] = pfHeadQuads[7][0][1] = pfHeadQuads[7][3][1] = (float) (iCapOffset + m_ciHeadHeight);
  pfHeadQuads[5][0][2] = pfHeadQuads[5][1][2] = pfHeadQuads[7][2][2] = pfHeadQuads[7][3][2] = (float) -(m_ciCapDepth >> 1);
  pfHeadQuads[6][0][2] = pfHeadQuads[6][1][2] = pfHeadQuads[6][2][2] = pfHeadQuads[6][3][2] = pfHeadQuads[5][2][2] = pfHeadQuads[5][3][2] = pfHeadQuads[7][0][2] = pfHeadQuads[7][1][2] = (float) (m_ciCapDepth >> 1);
  // bottom plate
  pfHeadQuads[8][0][0] = pfHeadQuads[8][1][0] = (float) -(m_ciCapWidth >> 1);
  pfHeadQuads[8][2][0] = pfHeadQuads[8][3][0] = (float) (m_ciCapWidth >> 1);
  pfHeadQuads[8][0][1] = pfHeadQuads[8][1][1] = pfHeadQuads[8][2][1] = pfHeadQuads[8][3][1] = (float) (iCapOffset + m_ciHeadHeight);
  pfHeadQuads[8][1][2] = pfHeadQuads[8][2][2] = (float) (m_ciCapDepth >> 1);
  pfHeadQuads[8][0][2] = pfHeadQuads[8][3][2] = (float) -(m_ciCapDepth >> 1);
  // normals
  for (int i = 0; i < 9; i++)
      {
      pfHeadNormals[i][1][0] = pfHeadNormals[i][1][0] = pfHeadNormals[i][2][0] = pfHeadNormals[i][3][0] = 0.0f;
      pfHeadNormals[i][1][1] = pfHeadNormals[i][1][1] = pfHeadNormals[i][2][1] = pfHeadNormals[i][3][1] = 0.0f;
      if (i < 5)
        {
        pfHeadNormals[i][0][2] = pfHeadNormals[i][1][2] = pfHeadNormals[i][2][2] = pfHeadNormals[i][3][2] = -1.0f;
        }
      else
        {
        pfHeadNormals[i][0][2] = pfHeadNormals[i][1][2] = pfHeadNormals[i][2][2] = pfHeadNormals[i][3][2] = 0.0f;
        }
      }
  pfHeadNormals[5][0][0] = pfHeadNormals[5][1][0] = pfHeadNormals[5][2][0] = pfHeadNormals[5][3][0] = -1.0f;
  pfHeadNormals[6][0][2] = pfHeadNormals[6][1][2] = pfHeadNormals[6][2][2] = pfHeadNormals[6][3][2] = 1.0f;
  pfHeadNormals[7][0][0] = pfHeadNormals[7][1][0] = pfHeadNormals[7][2][0] = pfHeadNormals[7][3][0] = 1.0f;
  pfHeadNormals[8][0][1] = pfHeadNormals[8][1][1] = pfHeadNormals[8][2][1] = pfHeadNormals[8][3][1] = 1.0f;
  pfVertex += 9 * 4;
  pfNormal += 9 * 4;

  // build eyes
  GLfloat (*pfEyeQuads)[4][3]   = (GLfloat (*)[4][3]) pfVertex;
  GLfloat (*pfEyeNormals)[4][3] = (GLfloat (*)[4][3]) pfNormal;
  float fDepth = m_ciEyeDepth;
  if (fDepth > (float) m_ciEyeWidth / 2) fDepth = (float) m_ciEyeWidth / 2;
  if (fDepth > (float) m_ciEyeHeight / 2) fDepth = (float) m_ciEyeHeight / 2;
  // X for left eye
  pfEyeQuads[0][1][0] = pfEyeQuads[2][0][0] = pfEyeQuads[3][0][0] = pfEyeQuads[3][1][0] = (float) iEye0X0;
  pfEyeQuads[0][0][0] = pfEyeQuads[2][1][0] = pfEyeQuads[3][2][0] = pfEyeQuads[3][3][0] = pfEyeQuads[4][0][0] = pfEyeQuads[4][1][0] = (float) iEye0X0 + fDepth;
  pfEyeQuads[0][3][0] = pfEyeQuads[2][2][0] = pfEyeQuads[1][0][0] = pfEyeQuads[1][1][0] = pfEyeQuads[4][2][0] = pfEyeQuads[4][3][0] = (float) iEye0X1 - fDepth;
  pfEyeQuads[0][2][0] = pfEyeQuads[2][3][0] = pfEyeQuads[1][2][0] = pfEyeQuads[1][3][0] = (float) iEye0X1;
  // X for right eye
  pfEyeQuads[5][1][0] = pfEyeQuads[7][0][0] = pfEyeQuads[8][0][0] = pfEyeQuads[8][1][0] = (float) iEye1X0;
  pfEyeQuads[5][0][0] = pfEyeQuads[7][1][0] = pfEyeQuads[8][2][0] = pfEyeQuads[8][3][0] = pfEyeQuads[9][0][0] = pfEyeQuads[9][1][0] = (float) iEye1X0 + fDepth;
  pfEyeQuads[5][3][0] = pfEyeQuads[7][2][0] = pfEyeQuads[6][0][0] = pfEyeQuads[6][1][0] = pfEyeQuads[9][2][0] = pfEyeQuads[9][3][0] = (float) iEye1X1 - fDepth;
  pfEyeQuads[5][2][0] = pfEyeQuads[7][3][0] = pfEyeQuads[6][2][0] = pfEyeQuads[6][3][0] = (float) iEye1X1;
  // Y for both eyes
  pfEyeQuads[0][1][1] = pfEyeQuads[0][2][1] = pfEyeQuads[1][2][1] = pfEyeQuads[3][1][1] = pfEyeQuads[5][1][1] = pfEyeQuads[5][2][1] = pfEyeQuads[6][2][1] = pfEyeQuads[8][1][1] = (float) iEyeBottomY;
  pfEyeQuads[0][0][1] = pfEyeQuads[0][3][1] = pfEyeQuads[1][1][1] = pfEyeQuads[3][2][1] = pfEyeQuads[4][1][1] = pfEyeQuads[4][2][1] = pfEyeQuads[5][0][1] = pfEyeQuads[5][3][1] = pfEyeQuads[6][1][1] = pfEyeQuads[8][2][1] = pfEyeQuads[9][1][1] = pfEyeQuads[9][2][1] = (float) iEyeBottomY - fDepth;
  pfEyeQuads[2][1][1] = pfEyeQuads[2][2][1] = pfEyeQuads[1][0][1] = pfEyeQuads[3][3][1] = pfEyeQuads[4][0][1] = pfEyeQuads[4][3][1] = pfEyeQuads[7][1][1] = pfEyeQuads[7][2][1] = pfEyeQuads[6][0][1] = pfEyeQuads[8][3][1] = pfEyeQuads[9][0][1] = pfEyeQuads[9][3][1] = (float) iEyeTopY + fDepth;
  pfEyeQuads[2][0][1] = pfEyeQuads[2][3][1] = pfEyeQuads[1][3][1] = pfEyeQuads[3][0][1] = pfEyeQuads[7][0][1] = pfEyeQuads[7][3][1] = pfEyeQuads[6][3][1] = pfEyeQuads[8][0][1] = (float) iEyeTopY;
  // Z for both eyes
  pfEyeQuads[0][1][2] = pfEyeQuads[0][2][2] = pfEyeQuads[1][2][2] = pfEyeQuads[1][3][2] = pfEyeQuads[2][0][2] = pfEyeQuads[2][3][2] = pfEyeQuads[3][0][2] = pfEyeQuads[3][1][2] = pfEyeQuads[5][1][2] = pfEyeQuads[5][2][2] = pfEyeQuads[6][2][2] = pfEyeQuads[6][3][2] = pfEyeQuads[7][0][2] = pfEyeQuads[7][3][2] = pfEyeQuads[8][0][2] = pfEyeQuads[8][1][2] = (float) -(m_ciCapDepth >> 1);
  pfEyeQuads[0][0][2] = pfEyeQuads[0][3][2] = pfEyeQuads[1][0][2] = pfEyeQuads[1][1][2] = pfEyeQuads[2][1][2] = pfEyeQuads[2][2][2] = pfEyeQuads[3][2][2] = pfEyeQuads[3][3][2] = pfEyeQuads[4][0][2] = pfEyeQuads[4][1][2] = pfEyeQuads[4][2][2] = pfEyeQuads[4][3][2] = pfEyeQuads[5][0][2] = pfEyeQuads[5][3][2] = pfEyeQuads[6][0][2] = pfEyeQuads[6][1][2] = pfEyeQuads[7][1][2] = pfEyeQuads[7][2][2] = pfEyeQuads[8][2][2] = pfEyeQuads[8][3][2] = pfEyeQuads[9][0][2] = pfEyeQuads[9][1][2] = pfEyeQuads[9][2][2] = pfEyeQuads[9][3][2] = (float) -(m_ciCapDepth >> 1) + fDepth;
  // normals for eyes
  for (int i = 0; i < 10; i++)
      {
      if (i == 3 || i == 8)
        pfEyeNormals[i][0][0] = pfEyeNormals[i][1][0] = pfEyeNormals[i][2][0] = pfEyeNormals[i][3][0] = 0.707f;
      else if (i == 1 || i == 6)
        pfEyeNormals[i][0][0] = pfEyeNormals[i][1][0] = pfEyeNormals[i][2][0] = pfEyeNormals[i][3][0] = -0.707f;
      else
        pfEyeNormals[i][0][0] = pfEyeNormals[i][1][0] = pfEyeNormals[i][2][0] = pfEyeNormals[i][3][0] = 0.0f;
      if (i == 0 || i == 5)
        pfEyeNormals[i][0][1] = pfEyeNormals[i][1][1] = pfEyeNormals[i][2][1] = pfEyeNormals[i][3][1] = -0.707f;
      else if (i == 2 || i == 7)
        pfEyeNormals[i][0][1] = pfEyeNormals[i][1][1] = pfEyeNormals[i][2][1] = pfEyeNormals[i][3][1] = 0.707f;
      else
        pfEyeNormals[i][0][1] = pfEyeNormals[i][1][1] = pfEyeNormals[i][2][1] = pfEyeNormals[i][3][1] = 0.0f;
      if (i == 4 || i == 9)
        pfEyeNormals[i][0][2] = pfEyeNormals[i][1][2] = pfEyeNormals[i][2][2] = pfEyeNormals[i][3][2] = -1.0f;
      else
        pfEyeNormals[i][0][2] = pfEyeNormals[i][1][2] = pfEyeNormals[i][2][2] = pfEyeNormals[i][3][2] = -0.707f;
      }
  pfVertex += 10 * 4;
  pfNormal += 10 * 4;

  // build legs with private function
  BuildLegs(pfVertex, pfNormal, false);

  // done
  return true;
}

void CInvader10::DestroyModel(void)
{
  if (glfVertex) free(glfVertex);
  glfVertex = NULL;
  if (glfNormal) free(glfNormal);
  glfNormal = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CInvader10 constructor/destructor

CInvader10::CInvader10()
{
  // set default colors
  m_uchCapColor[0] = 0x00;   m_uchCapColor[1] = 0xdf;   m_uchCapColor[2] = 0x00;
  m_uchHeadColor[0] = 0x00;  m_uchHeadColor[1] = 0xdf;  m_uchHeadColor[2] = 0x00;
  m_uchEyeColor[0] = 0xdf;   m_uchEyeColor[1] = 0xdf;   m_uchEyeColor[2] = 0x00;
  m_uchLegColor[0] = 0x00;   m_uchLegColor[1] = 0x00;   m_uchLegColor[2] = 0xdf;

  // allocate model arrays
  m_pfVertex   = (GLfloat (*)[3]) malloc(m_ciTotalVertices * sizeof(GLfloat) * 3);
  m_pfNormal   = (GLfloat (*)[3]) malloc(m_ciTotalVertices * sizeof(GLfloat) * 3);

  // copy static model data into our member storage
  memcpy(m_pfVertex, glfVertex, m_ciTotalVertices * sizeof(GLfloat) * 3);
  memcpy(m_pfNormal, glfNormal, m_ciTotalVertices * sizeof(GLfloat) * 3);

  // set my maximum thrust
  m_fMaxThrust    = 0.027f;
}

CInvader10::~CInvader10()
{
  // free member models data arrays
  free(m_pfVertex);
  m_pfVertex   = NULL;
  free(m_pfNormal);
  m_pfNormal   = NULL;

}

/////////////////////////////////////////////////////////////////////////////
// CInvader10 accessor functions

void CInvader10::GetBodyBounding(float *fL, float *fR, float *fT, float *fB) const
{
  *fL = m_fPositionX - (float) (m_ciCapWidth >> 1);
  *fR = m_fPositionX + (float) (m_ciCapWidth >> 1);
  *fT = m_fPositionY - ((m_ciCapHeight + m_ciHeadHeight + m_ciLegHeight) >> 1);
  *fB = *fT + (float) (m_ciCapHeight + m_ciHeadHeight);
}

void CInvader10::GetParticlePolys(SPolys *psPoly) const
{
  float fHealth = (float) m_iLastHealth / (float) m_iFullHealth;
  float fDamage = 1.0f - fHealth;

  // set up the Polys struct for triangles and quad colors
  psPoly->uiNumTriColors = 1;
  psPoly->uiNumQuadColors = 4;
  psPoly->aucTriColors[0][0] = (unsigned char) (m_uchCapColor[0] * fHealth + 0xff * fDamage);
  psPoly->aucTriColors[0][1] = (unsigned char) (m_uchCapColor[1] * fHealth);
  psPoly->aucTriColors[0][2] = (unsigned char) (m_uchCapColor[2] * fHealth);
  psPoly->aucQuadColors[0][0] = (unsigned char) (m_uchCapColor[0] * fHealth + 0xff * fDamage);
  psPoly->aucQuadColors[0][1] = (unsigned char) (m_uchCapColor[1] * fHealth);
  psPoly->aucQuadColors[0][2] = (unsigned char) (m_uchCapColor[2] * fHealth);
  psPoly->aucQuadColors[1][0] = (unsigned char) (m_uchHeadColor[0] * fHealth + 0xff * fDamage);
  psPoly->aucQuadColors[1][1] = (unsigned char) (m_uchHeadColor[1] * fHealth);
  psPoly->aucQuadColors[1][2] = (unsigned char) (m_uchHeadColor[2] * fHealth);
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

  // do the cap
  psPoly->uiNumQuads[0] = m_ciRadialPieces;
  psPoly->uiNumTriangles[0] = 2 * m_ciRadialPieces;
  for (unsigned int ui = 0; ui < m_ciRadialPieces; ui++)  // quad strip on top of the head
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

  // do the head
  psPoly->uiNumQuads[1] = 9;
  for (unsigned int uiQ = 0; uiQ < 9; uiQ++)
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

  // do the legs
  psPoly->uiNumQuads[3] = 30;
  for (unsigned int uiQ = 0; uiQ < 30; uiQ++)
    {
    for (unsigned int uiV = 0; uiV < 4; uiV++)
      {
      memcpy(psPoly->pfQuadVertices[uiQuadIdx], m_pfVertex[uiSrcIdx], 3 * sizeof(float));
      memcpy(psPoly->pfQuadNormals[uiQuadIdx], m_pfNormal[uiSrcIdx], 3 * sizeof(float));
      uiSrcIdx++;
      }
    uiQuadIdx++;
    }

}

void CInvader10::Draw(void) const
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
  ModulateColor(piDiffuse, piAmbient, m_uchCapColor);

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
  glDrawArrays(GL_QUAD_STRIP, 0, (m_ciRadialPieces + 1) * 2);
  int iIndex = (m_ciRadialPieces + 1) * 2;
  glDrawArrays(GL_TRIANGLE_FAN, iIndex, m_ciRadialPieces + 2);
  iIndex += m_ciRadialPieces + 2;
  glDrawArrays(GL_TRIANGLE_FAN, iIndex, m_ciRadialPieces + 2);
  iIndex += m_ciRadialPieces + 2;

  // draw the head
  if (!m_bFlash)
    {
    ModulateColor(piDiffuse, piAmbient, m_uchHeadColor);
    glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
    glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
    }
  glDrawArrays(GL_QUADS, iIndex, 9 * 4);
  iIndex += 9 * 4;

  // draw the eyes
  piDiffuse[0]   = m_uchEyeColor[0] << 23;
  piDiffuse[1]   = m_uchEyeColor[1] << 23;
  piDiffuse[2]   = m_uchEyeColor[2] << 23;
  piAmbient[0]   = m_uchEyeColor[0] << 21;
  piAmbient[1]   = m_uchEyeColor[1] << 21;
  piAmbient[2]   = m_uchEyeColor[2] << 21;
  glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
  glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
  glDrawArrays(GL_QUADS, iIndex, 10 * 4);
  iIndex += 10 * 4;

  // draw the legs
  piDiffuse[0]   = m_uchLegColor[0] << 23;
  piDiffuse[1]   = m_uchLegColor[1] << 23;
  piDiffuse[2]   = m_uchLegColor[2] << 23;
  piAmbient[0]   = m_uchLegColor[0] << 21;
  piAmbient[1]   = m_uchLegColor[1] << 21;
  piAmbient[2]   = m_uchLegColor[2] << 21;
  glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
  glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
  glDrawArrays(GL_QUADS, iIndex, 30 * 4);
  iIndex += 30 * 4;

  // all done
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  return;
}

/////////////////////////////////////////////////////////////////////////////
// CInvader10 modifier functions

void CInvader10::SetCapColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchCapColor[0] = uchRed;
  m_uchCapColor[1] = uchGreen;
  m_uchCapColor[2] = uchBlue;
}

void CInvader10::SetHeadColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchHeadColor[0] = uchRed;
  m_uchHeadColor[1] = uchGreen;
  m_uchHeadColor[2] = uchBlue;
}

void CInvader10::SetEyeColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchEyeColor[0] = uchRed;
  m_uchEyeColor[1] = uchGreen;
  m_uchEyeColor[2] = uchBlue;
}

void CInvader10::SetLegColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchLegColor[0] = uchRed;
  m_uchLegColor[1] = uchGreen;
  m_uchLegColor[2] = uchBlue;
}

void CInvader10::ToggleLegs(bool bLegPosition)
{
  BuildLegs(m_pfVertex + m_ciTotalVertices - 120, m_pfNormal + m_ciTotalVertices - 120, bLegPosition);
  return;
}

bool CInvader10::TestCollision(float fLeftX, float fRightX, float fTopY, float fBotY, int iDamage)
{
  int iTotalHeight = m_ciCapHeight + m_ciHeadHeight + m_ciLegHeight;
  float fY0 = m_fPositionY - (float) (iTotalHeight >> 1);
  float fY1 = fY0 + (float) (m_ciCapHeight + m_ciHeadHeight);

  // first do a real fast but rough test
  if (fRightX < m_fPositionX - m_ciCapWidth / 2.0f ||
      fLeftX  > m_fPositionX + m_ciCapWidth / 2.0f ||
      fTopY   > fY1 ||
      fBotY   < fY0) return false;

  // close enough for me
  m_bFlash = true;
  m_iLastHealth = m_iHealth;
  m_iHealth -= iDamage;
  return true;
}

// this function tests for a collision between a rotating kamikaze invader and the LaserBase
bool CInvader10::KamikazeCollision(float fLeftX, float fRightX, float fTopY, float fBotY)
{
  int iTotalHeight = m_ciCapHeight + m_ciHeadHeight + m_ciLegHeight;
  float fY1 = m_fPositionY + (float) (iTotalHeight >> 1);

  // generate X coordinates for left and right corners,
  // rotated around Y axis and taking depth into account
  float fAngle = m_fSpinAngle * (float) M_PI / 180.0f;
  float fDepth = (float) m_ciCapDepth / 2.0f * fabs(sin(fAngle));
  float fX0 = m_fPositionX - m_ciCapWidth / 2.0f * fabs(cos(fAngle)) - fDepth;
  float fX1 = m_fPositionX + m_ciCapWidth / 2.0f * fabs(cos(fAngle)) + fDepth;

  // first do a real fast but rough test
  if (fRightX < fX0 ||
      fLeftX  > fX1 ||
      fTopY   > fY1) return false;

  // close enough for me
  return true;
}

bool CInvader10::BunkerCollision(float fLeftX, float fRightX, float fTopY)
{
  float fY1 = m_fPositionY - (float) ((m_ciCapHeight + m_ciHeadHeight + m_ciLegHeight) >> 1) + (float) (m_ciCapHeight + m_ciHeadHeight);

  // this collision test is very simple
  if (fRightX < m_fPositionX - m_ciCapWidth / 2.0f ||
      fLeftX  > m_fPositionX + m_ciCapWidth / 2.0f ||
      fTopY   > fY1) return false;

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
// CInvader10 private functions

void CInvader10::BuildLegs(float (*pfVertex)[3], float (*pfNormal)[3], bool bLegPosition)
{

  int iTotalHeight = m_ciCapHeight + m_ciHeadHeight + m_ciLegHeight;
  float fLegX = m_ciLegSeparation / 2.0f;
  float fLegY = (float) iTotalHeight / 2.0f - (float) (m_ciLegHeight + m_ciLegThickness);
  TubeSetStart(fLegX, fLegY, 0.0f, 315.0f, (float) m_ciLegThickness);
  if (bLegPosition)
    {
    TubeMove(pfVertex + 0, pfNormal + 0, (float) (m_ciCapWidth - m_ciLegSeparation) / 2.0f, 225.0f, false);
    TubeMove(pfVertex + 16, pfNormal + 16, (float) (m_ciCapWidth - m_ciLegSeparation) / 8.0f, 315.0f, true);
    }
  else
    {
    TubeMove(pfVertex + 0, pfNormal + 0, (float) (m_ciCapWidth - m_ciLegSeparation) / 2.0f, 0.0f, false);
    TubeMove(pfVertex + 16, pfNormal + 16, (float) (m_ciCapWidth - m_ciLegSeparation) / 8.0f, 0.0f, true);
    }
  TubeSetStart(-fLegX, fLegY, 0.0f, 225.0f, (float) m_ciLegThickness);
  if (bLegPosition)
    {
    TubeMove(pfVertex + 36, pfNormal + 36, (float) (m_ciCapWidth - m_ciLegSeparation) / 2.0f, 315.0f, false);
    TubeMove(pfVertex + 52, pfNormal + 52, (float) (m_ciCapWidth - m_ciLegSeparation) / 8.0f, 225.0f, true);
    }
  else
    {
    TubeMove(pfVertex + 36, pfNormal + 36, (float) (m_ciCapWidth - m_ciLegSeparation) / 2.0f, 180.0f, false);
    TubeMove(pfVertex + 52, pfNormal + 52, (float) (m_ciCapWidth - m_ciLegSeparation) / 8.0f, 180.0f, true);
    }
  fLegX += (m_ciCapWidth - m_ciLegSeparation) / 6.0f;
  fLegY += (m_ciCapWidth - m_ciLegSeparation) / 6.0f;
  TubeSetStart(fLegX, fLegY, 0.0f, 225.0f, (float) m_ciLegThickness);
  TubeMove(pfVertex + 72, pfNormal + 72, (float) m_ciLegSeparation / 2.0f, 180.0f, false);
  TubeMove(pfVertex + 88, pfNormal + 88, (float) m_ciLegSeparation * 1.5f, 135.0f, false);
  TubeMove(pfVertex + 104, pfNormal + 104, (float) m_ciLegSeparation / 2.0f, 135.0f, false);
}
