/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Ignignokt.cpp                                            *
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
#include "Ignignokt.h"
#include "Particles.h"
#include "GameMain.h" // for m_bAlphaSupported

// const model parameters
const int CIgnignokt::m_ciThickness = 7;


/////////////////////////////////////////////////////////////////////////////
// CIgnignokt constructor/destructor

CIgnignokt::CIgnignokt()
{
  // set default colors
  m_uchBodyColor[0] = 0x38;  m_uchBodyColor[1] = 0xfe;  m_uchBodyColor[2] = 0x30;
  m_uchArmColor[0]  = 0x2c;  m_uchArmColor[1]  = 0x44;  m_uchArmColor[2]  = 0x92;
  m_iAlpha = 0x7fffffff;

  // allocate model arrays
  m_pfVertex  = (GLfloat (*)[3]) malloc(IGN_QUADS * 4 * sizeof(GLfloat) * 3);
  m_pfNormal  = (GLfloat (*)[3]) malloc(IGN_QUADS * 4 * sizeof(GLfloat) * 3);

  // set up pointers
  GLfloat (*pfQuad)[4][3] = (GLfloat (*)[4][3]) m_pfVertex;
  GLfloat (*pfNorm)[4][3] = (GLfloat (*)[4][3]) m_pfNormal;

  float fX0, fY0;
  float fZ0 = (float) -m_ciThickness / 2.0f;
  float fZ1 = (float) m_ciThickness / 2.0f;

  // build ribbon around body
  ZRibbonStart(-75.0,-72.5f,0.0f, (float) m_ciThickness);
  ZRibbonMove(pfQuad + 0,  pfNorm + 0,   23.5f,   0.0f);
  ZRibbonMove(pfQuad + 1,  pfNorm + 1,    0.0f, -34.5f);
  ZRibbonMove(pfQuad + 2,  pfNorm + 2,   30.0f,   0.0f);
  ZRibbonMove(pfQuad + 3,  pfNorm + 3,    0.0f,   8.0f);
  ZRibbonMove(pfQuad + 4,  pfNorm + 4,   11.0f,   0.0f);
  ZRibbonMove(pfQuad + 5,  pfNorm + 5,    0.0f,  28.5f);
  ZRibbonMove(pfQuad + 6,  pfNorm + 6,   11.5f,   0.0f);
  ZRibbonMove(pfQuad + 7,  pfNorm + 7,    0.0f,  -9.0f);
  ZRibbonMove(pfQuad + 8,  pfNorm + 8,   34.0f,   0.0f);
  ZRibbonMove(pfQuad + 9,  pfNorm + 9,    0.0f, -27.5f);
  ZRibbonMove(pfQuad + 10, pfNorm + 10,  34.0f,   0.0f);
  ZRibbonMove(pfQuad + 11, pfNorm + 11,   0.0f, 111.5f);
  ZRibbonMove(pfQuad + 12, pfNorm + 12,  12.5f,   0.0f);
  ZRibbonMove(pfQuad + 13, pfNorm + 13,   0.0f,  79.0f);
  ZRibbonMove(pfQuad + 14, pfNorm + 14,-164.5f,   0.0f);
  ZRibbonMove(pfQuad + 15, pfNorm + 15,   0.0f, -68.0f);
  ZRibbonMove(pfQuad + 16, pfNorm + 16,  13.0f,   0.0f);
  ZRibbonMove(pfQuad + 17, pfNorm + 17,   0.0f, -52.0f);
  ZRibbonMove(pfQuad + 18, pfNorm + 18,  -5.0f,   0.0f);
  ZRibbonMove(pfQuad + 19, pfNorm + 19,   0.0f, -36.0f);
  pfQuad += 20;
  pfNorm += 20;

  // build body quads
  BuildPanes(pfQuad +  0, pfNorm +  0, -75.0f, -72.5f,   -51.5f,-36.5f, (float) m_ciThickness);
  BuildPanes(pfQuad +  2, pfNorm +  2, -70.0f, -36.5f,   -51.5f, 19.0f, (float) m_ciThickness);
  BuildPanes(pfQuad +  4, pfNorm +  4, -83.0f,  15.5f,   -51.5f, 83.5f, (float) m_ciThickness);
  BuildPanes(pfQuad +  6, pfNorm +  6, -51.5f,-107.0f,   -21.5f, 83.5f, (float) m_ciThickness);
  BuildPanes(pfQuad +  8, pfNorm +  8, -21.5f, -99.0f,   -10.5f, 83.5f, (float) m_ciThickness);
  BuildPanes(pfQuad + 10, pfNorm + 10, -10.5f, -70.5f,     1.0f, 83.5f, (float) m_ciThickness);
  BuildPanes(pfQuad + 12, pfNorm + 12,   1.0f, -79.5f,    35.0f, 83.5f, (float) m_ciThickness);
  BuildPanes(pfQuad + 14, pfNorm + 14,  35.0f,-107.0f,    69.0f, 83.5f, (float) m_ciThickness);
  BuildPanes(pfQuad + 16, pfNorm + 16,  69.0f,   4.5f,    81.5f, 83.5f, (float) m_ciThickness);
  pfQuad += 18;
  pfNorm += 18;

  // build arms and legs
  SetArmState(false);
  SetLegs(false, false);
  pfQuad += 60;
  pfNorm += 60;

  // build face quads
  SetMouthState(false);
  pfQuad += 1;
  pfNorm += 1;
  fZ0 = (float) -m_ciThickness / 2.0f - 1.0f;
  fX0 = -59.5f;
  fY0 = -34.5f;
  for (int iEyebrow = 0; iEyebrow < 4; iEyebrow++)
      {
      BuildSquare(pfQuad + iEyebrow, pfNorm + iEyebrow, fX0, fY0, fX0 + 10.5f, fY0 + 10.5f, fZ0);
      fX0 += 9.5f;
      fY0 -= 9.5f;
      }
  pfQuad += 4;
  pfNorm += 4;
  fX0 = 21.5f;
  fY0 = -64.5f;
  for (int iEyebrow = 0; iEyebrow < 4; iEyebrow++)
      {
      BuildSquare(pfQuad + iEyebrow, pfNorm + iEyebrow, fX0, fY0, fX0 + 10.5f, fY0 + 10.5f, fZ0);
      fX0 += 9.5f;
      fY0 += 9.5f;
      }
  pfQuad += 4;
  pfNorm += 4;
  BuildSquare(pfQuad, pfNorm, -36.5f, -31.0f, -14.5f, -20.5f, fZ0);
  BuildSquare(pfQuad+1, pfNorm+1, 15.0f, -31.0f, 37.0f, -20.5f, fZ0);

}

CIgnignokt::~CIgnignokt()
{
  // free model memory
  free(m_pfVertex);
  m_pfVertex  = NULL;
  free(m_pfNormal);
  m_pfNormal  = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CIgnignokt accessor functions

void CIgnignokt::GetParticlePolys(SPolys *psPoly) const
{
  // set up the Polys struct for no triangles and 2 quad colors
  psPoly->uiNumTriColors = 0;
  psPoly->uiNumQuadColors = 2;

  // set up the number of quads for each color and the colors
  psPoly->uiNumQuads[0] = IGN_BODY_QUADS;
  psPoly->uiNumQuads[1] = IGN_ARM_QUADS + IGN_FACE_QUADS;
  psPoly->aucQuadColors[0][0] = m_uchBodyColor[0];
  psPoly->aucQuadColors[0][1] = m_uchBodyColor[1];
  psPoly->aucQuadColors[0][2] = m_uchBodyColor[2];
  psPoly->aucQuadColors[1][0] = m_uchArmColor[0];
  psPoly->aucQuadColors[1][1] = m_uchArmColor[1];
  psPoly->aucQuadColors[1][2] = m_uchArmColor[2];

  // add the vertices and normals
  for (unsigned int uiQ = 0; uiQ < IGN_QUADS; uiQ++)
    {
    for (unsigned int uiV = 0; uiV < 4; uiV++)
      {
      memcpy(psPoly->pfQuadVertices[uiQ][uiV], m_pfVertex[uiQ*4+uiV], 3 * sizeof(float));
      memcpy(psPoly->pfQuadNormals[uiQ][uiV],  m_pfNormal[uiQ*4+uiV], 3 * sizeof(float));
      }
    }
}

void CIgnignokt::Draw(void) const
{
  // Position the model in 3-space
  glTranslatef(m_fPositionX, m_fPositionY, m_fPositionZ);

  // set rotation of mooninite
  glRotatef(m_fAngle, m_fAngleX, m_fAngleY, m_fAngleZ);

  GLfloat pfSpecular[4]  = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat pfShininess[1] = {32.0f};
  GLint   piDiffuse[4]   = {m_uchBodyColor[0] << 23, m_uchBodyColor[1] << 23, m_uchBodyColor[2] << 23, m_iAlpha};
  GLint   piAmbient[4]   = {m_uchBodyColor[0] << 20, m_uchBodyColor[1] << 20, m_uchBodyColor[2] << 20, m_iAlpha};

  if (m_iAlpha < 0x7fffffff && CGameMain::m_bAlphaSupported)
    {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    }

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
  glDrawArrays(GL_QUADS, 0, IGN_BODY_QUADS * 4);

  // draw arms, legs, and face
  piDiffuse[0]   = m_uchArmColor[0] << 23;
  piDiffuse[1]   = m_uchArmColor[1] << 23;
  piDiffuse[2]   = m_uchArmColor[2] << 23;
  piAmbient[0]   = m_uchArmColor[0] << 20;
  piAmbient[1]   = m_uchArmColor[1] << 20;
  piAmbient[2]   = m_uchArmColor[2] << 20;
  glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
  glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
  glDrawArrays(GL_QUADS, IGN_BODY_QUADS * 4, (IGN_ARM_QUADS + IGN_FACE_QUADS) * 4);

  // all done
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  if (m_iAlpha < 0x7fffff && CGameMain::m_bAlphaSupported)
    {
    glDisable(GL_BLEND);
    }
  return;
}

/////////////////////////////////////////////////////////////////////////////
// CIgnignokt modifier functions

void CIgnignokt::SetBodyColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchBodyColor[0] = uchRed;
  m_uchBodyColor[1] = uchGreen;
  m_uchBodyColor[2] = uchBlue;
}

void CIgnignokt::SetArmColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchArmColor[0] = uchRed;
  m_uchArmColor[1] = uchGreen;
  m_uchArmColor[2] = uchBlue;
}

void CIgnignokt::SetArmState(bool bRaised)
{
  float fX0, fY0;

  // pointers
  GLfloat (*pfQuads)[4][3] = (GLfloat (*)[4][3]) m_pfVertex + IGN_BODY_QUADS;
  GLfloat (*pfNorms)[4][3] = (GLfloat (*)[4][3]) m_pfNormal + IGN_BODY_QUADS;

  // left arm
  fX0 = -83.0f;
  fY0 = 15.5f;
  BuildBox(pfQuads, pfNorms, fX0 - 12.5f, fY0, fX0, fY0 + 12.5f, (float) m_ciThickness);
  fX0 -= 11.5f;
  if (bRaised) fY0 -= 11.5f; else fY0 += 11.5f;
  BuildBox(pfQuads+6, pfNorms+6, fX0 - 12.5f, fY0, fX0, fY0 + 12.5f, (float) m_ciThickness);
  fX0 -= 11.5f;
  if (bRaised) fY0 -= 25.0f; else fY0 += 11.5f;
  BuildBox(pfQuads+12, pfNorms+12, fX0 - 12.5f, fY0, fX0, fY0 + 26.0f, (float) m_ciThickness);

  // right arm
  fX0 = 81.5f;
  fY0 = 4.5f;
  BuildBox(pfQuads+18, pfNorms+18, fX0, fY0, fX0 + 12.5f, fY0 + 12.5f, (float) m_ciThickness);
  fX0 += 11.5f;
  if (bRaised) fY0 -= 11.5f; else fY0 += 11.5f;
  BuildBox(pfQuads+24, pfNorms+24, fX0, fY0, fX0 + 12.5f, fY0 + 12.5f, (float) m_ciThickness);
  fX0 += 11.5f;
  if (bRaised) fY0 -= 25.0f; else fY0 += 11.5f;
  BuildBox(pfQuads+30, pfNorms+30, fX0, fY0, fX0 + 12.5f, fY0 + 26.0f, (float) m_ciThickness);

}

void CIgnignokt::SetMouthState(bool bWide)
{
  float fWidth, fCenterX, fZ;

  // pointers
  GLfloat (*pfQuads)[4][3] = (GLfloat (*)[4][3]) m_pfVertex + IGN_BODY_QUADS + IGN_ARM_QUADS;
  GLfloat (*pfNorms)[4][3] = (GLfloat (*)[4][3]) m_pfNormal + IGN_BODY_QUADS + IGN_ARM_QUADS;

  fWidth = 64.5f;
  fCenterX = 0.0f;
  fZ = (float) -m_ciThickness / 2.0f - 1.0f;

  BuildSquare(pfQuads, pfNorms, fCenterX - fWidth / 2.0f, 20.0f, fCenterX + fWidth / 2.0f, 34.5f, fZ);
}

void CIgnignokt::SetLegs(bool bLeftUp, bool bRightUp)
{
  float fLeftY, fRightY;

  // set height of legs
  if (bLeftUp)
    fLeftY = 99.5f;
  else
    fLeftY = 107.5f;
  if (bRightUp)
    fRightY = 99.5f;
  else
    fRightY = 107.5f;

  // pointers
  GLfloat (*pfQuad)[4][3] = (GLfloat (*)[4][3]) m_pfVertex + IGN_BODY_QUADS + 36;
  GLfloat (*pfNorm)[4][3] = (GLfloat (*)[4][3]) m_pfNormal + IGN_BODY_QUADS + 36;

  BuildBox(pfQuad,    pfNorm,   -25.5f, 83.5f,          -15.5f, fLeftY, (float) m_ciThickness);
  BuildBox(pfQuad+6,  pfNorm+6, -55.5f, fLeftY - 10.0f, -15.5f, fLeftY, (float) m_ciThickness);
  BuildBox(pfQuad+12, pfNorm+12, 20.0f, 83.5f,           30.0f, fRightY, (float) m_ciThickness);
  BuildBox(pfQuad+18, pfNorm+18, 20.0f, fRightY - 10.0f, 60.0f, fRightY, (float) m_ciThickness);
}

bool CIgnignokt::TestCollision(float fLeftX, float fRightX, float fTopY, float fBotY, int iDamage)
{
  // first do a quick Y-coordinate comparison
  if (fTopY > m_fPositionY + 83 || fBotY < m_fPositionY - 70)
    return false;

  float fAngleRad = m_fAngle * (float) M_PI / 180.0f;

  // figure out the deepest point of front surface of the Mooninite
  float fZ = m_fPositionZ - (float) m_ciThickness / 2.0f + 120.0f * sin(fAngleRad);

  // if it doesn't pass through the Z=0 plane, there can't be any collision
  if (fZ < 0.0f) return false;

  // else calculate the left and right X coordinates of the intersection of the invader with the Z=0 plane
  float fSlope = cos(fAngleRad) / sin(fAngleRad);
  float fZ0LeftX = m_fPositionX - (float) m_ciThickness / 2.0f * sin(fAngleRad);
  float fZ0RightX = m_fPositionX + (float) m_ciThickness / 2.0f * sin(fAngleRad);
  fZ0LeftX += (m_fPositionZ + (float) m_ciThickness / 2.0f * cos(fAngleRad)) * -fSlope;
  fZ0RightX += (m_fPositionZ - (float) m_ciThickness / 2.0f * cos(fAngleRad)) * -fSlope;

  // test for the X intersection
  if (fRightX < fZ0LeftX || fLeftX > fZ0RightX) return false;

  // there is a collision
  return true;
}

/////////////////////////////////////////////////////////////////////////////
// CIgnignokt private functions

