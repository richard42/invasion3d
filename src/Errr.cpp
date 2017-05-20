/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Errr.cpp                                                 *
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
#include "Errr.h"
#include "Particles.h"
#include "GameMain.h" // for m_bAlphaSupported

// const model parameters
const int CErrr::m_ciThickness = 7;


/////////////////////////////////////////////////////////////////////////////
// CErrr constructor/destructor

CErrr::CErrr()
{
  // set default colors
  m_uchBodyColor[0] = 0xff;  m_uchBodyColor[1] = 0x21;  m_uchBodyColor[2] = 0xff;
  m_uchArmColor[0]  = 0x10;  m_uchArmColor[1]  = 0x00;  m_uchArmColor[2]  = 0xf7;
  m_iAlpha = 0x7fffffff;

  // allocate model arrays
  m_pfVertex  = (GLfloat (*)[3]) malloc(ERRR_QUADS * 4 * sizeof(GLfloat) * 3);
  m_pfNormal  = (GLfloat (*)[3]) malloc(ERRR_QUADS * 4 * sizeof(GLfloat) * 3);

  // set up pointers
  GLfloat (*pfQuad)[4][3] = (GLfloat (*)[4][3]) m_pfVertex;
  GLfloat (*pfNorm)[4][3] = (GLfloat (*)[4][3]) m_pfNormal;

  float fX0, fX1, fY0, fY1;
  float fZ0 = (float) -m_ciThickness / 2.0f;
  float fZ1 = (float) m_ciThickness / 2.0f;

  // build body quads
  fX0 = -59.0f;
  fX1 = 9.5f;
  fY0 = -63.0f;
  fY1 = 49.5f;
  for (int i = 0; i < ERRR_BODY_QUADS; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 3; k++)
        {
        pfNorm[i][j][k] = 0.0f;
        }
  pfQuad[0][0][0] = pfQuad[0][1][0] = pfQuad[1][0][0] = pfQuad[1][1][0] = pfQuad[2][2][0] = pfQuad[2][3][0] = pfQuad[3][0][0] = pfQuad[3][1][0] = pfQuad[4][0][0] = pfQuad[4][1][0] = pfQuad[4][2][0] = pfQuad[4][3][0] = fX0;
  pfQuad[0][2][0] = pfQuad[0][3][0] = pfQuad[1][2][0] = pfQuad[1][3][0] = pfQuad[2][0][0] = pfQuad[2][1][0] = pfQuad[3][2][0] = pfQuad[3][3][0] = fX1;
  pfQuad[0][0][1] = pfQuad[0][3][1] = pfQuad[1][0][1] = pfQuad[1][1][1] = pfQuad[1][2][1] = pfQuad[1][3][1] = pfQuad[2][0][1] = pfQuad[2][3][1] = pfQuad[4][0][1] = pfQuad[4][3][1] = fY0;
  pfQuad[0][1][1] = pfQuad[0][2][1] = pfQuad[2][1][1] = pfQuad[2][2][1] = pfQuad[3][0][1] = pfQuad[3][1][1] = pfQuad[3][2][1] = pfQuad[3][3][1] = pfQuad[4][1][1] = pfQuad[4][2][1] = fY1;
  pfQuad[0][0][2] = pfQuad[0][1][2] = pfQuad[0][2][2] = pfQuad[0][3][2] = pfQuad[1][1][2] = pfQuad[1][2][2] = pfQuad[3][0][2] = pfQuad[3][3][2] = pfQuad[4][2][2] = pfQuad[4][3][2] = fZ0;
  pfQuad[1][0][2] = pfQuad[1][3][2] = pfQuad[2][0][2] = pfQuad[2][1][2] = pfQuad[2][2][2] = pfQuad[2][3][2] = pfQuad[3][1][2] = pfQuad[3][2][2] = pfQuad[4][0][2] = pfQuad[4][1][2] = fZ1;
  pfNorm[0][0][2] = pfNorm[0][1][2] = pfNorm[0][2][2] = pfNorm[0][3][2] = pfNorm[1][0][1] = pfNorm[1][1][1] = pfNorm[1][2][1] = pfNorm[1][3][1] = pfNorm[4][0][0] = pfNorm[4][1][0] = pfNorm[4][2][0] = pfNorm[4][3][0] = -1.0f;
  pfNorm[2][0][2] = pfNorm[2][1][2] = pfNorm[2][2][2] = pfNorm[2][3][2] = pfNorm[3][0][1] = pfNorm[3][1][1] = pfNorm[3][2][1] = pfNorm[3][3][1] = 1.0f;
  pfQuad += 5;
  pfNorm += 5;
  fX0 = fX1;
  fX1 = 35.0f;
  float fY2 = fY0;
  float fY3 = -30.0f;
  fY0 = -82.0f;
  pfQuad[0][0][0] = pfQuad[0][1][0] = pfQuad[1][0][0] = pfQuad[1][1][0] = pfQuad[2][2][0] = pfQuad[2][3][0] = pfQuad[3][0][0] = pfQuad[3][1][0] = pfQuad[4][0][0] = pfQuad[4][1][0] = pfQuad[4][2][0] = pfQuad[4][3][0] = fX0;
  pfQuad[0][2][0] = pfQuad[0][3][0] = pfQuad[1][2][0] = pfQuad[1][3][0] = pfQuad[2][0][0] = pfQuad[2][1][0] = pfQuad[3][2][0] = pfQuad[3][3][0] = pfQuad[5][0][0] = pfQuad[5][1][0] = pfQuad[5][2][0] = pfQuad[5][3][0] = fX1;
  pfQuad[0][0][1] = pfQuad[0][3][1] = pfQuad[1][0][1] = pfQuad[1][1][1] = pfQuad[1][2][1] = pfQuad[1][3][1] = pfQuad[2][0][1] = pfQuad[2][3][1] = pfQuad[4][0][1] = pfQuad[4][3][1] = pfQuad[5][0][1] = pfQuad[5][3][1] = fY0;
  pfQuad[0][1][1] = pfQuad[0][2][1] = pfQuad[2][1][1] = pfQuad[2][2][1] = pfQuad[3][0][1] = pfQuad[3][1][1] = pfQuad[3][2][1] = pfQuad[3][3][1] = fY1;
  pfQuad[4][1][1] = pfQuad[4][2][1] = fY2;
  pfQuad[5][1][1] = pfQuad[5][2][1] = fY3;
  pfQuad[0][0][2] = pfQuad[0][1][2] = pfQuad[0][2][2] = pfQuad[0][3][2] = pfQuad[1][1][2] = pfQuad[1][2][2] = pfQuad[3][0][2] = pfQuad[3][3][2] = pfQuad[4][2][2] = pfQuad[4][3][2] = pfQuad[5][0][2] = pfQuad[5][1][2] = fZ0;
  pfQuad[1][0][2] = pfQuad[1][3][2] = pfQuad[2][0][2] = pfQuad[2][1][2] = pfQuad[2][2][2] = pfQuad[2][3][2] = pfQuad[3][1][2] = pfQuad[3][2][2] = pfQuad[4][0][2] = pfQuad[4][1][2] = pfQuad[5][2][2] = pfQuad[5][3][2] = fZ1;
  pfNorm[0][0][2] = pfNorm[0][1][2] = pfNorm[0][2][2] = pfNorm[0][3][2] = pfNorm[1][0][1] = pfNorm[1][1][1] = pfNorm[1][2][1] = pfNorm[1][3][1] = pfNorm[4][0][0] = pfNorm[4][1][0] = pfNorm[4][2][0] = pfNorm[4][3][0] = -1.0f;
  pfNorm[2][0][2] = pfNorm[2][1][2] = pfNorm[2][2][2] = pfNorm[2][3][2] = pfNorm[3][0][1] = pfNorm[3][1][1] = pfNorm[3][2][1] = pfNorm[3][3][1] = pfNorm[5][0][0] = pfNorm[5][1][0] = pfNorm[5][2][0] = pfNorm[5][3][0] = 1.0f;
  pfQuad += 6;
  pfNorm += 6;
  fX0 = fX1;
  fX1 = 58.5f;
  fY0 = fY3;
  pfQuad[0][0][0] = pfQuad[0][1][0] = pfQuad[1][0][0] = pfQuad[1][1][0] = pfQuad[2][2][0] = pfQuad[2][3][0] = pfQuad[3][0][0] = pfQuad[3][1][0] = fX0;
  pfQuad[0][2][0] = pfQuad[0][3][0] = pfQuad[1][2][0] = pfQuad[1][3][0] = pfQuad[2][0][0] = pfQuad[2][1][0] = pfQuad[3][2][0] = pfQuad[3][3][0] = pfQuad[4][0][0] = pfQuad[4][1][0] = pfQuad[4][2][0] = pfQuad[4][3][0] = fX1;
  pfQuad[0][0][1] = pfQuad[0][3][1] = pfQuad[1][0][1] = pfQuad[1][1][1] = pfQuad[1][2][1] = pfQuad[1][3][1] = pfQuad[2][0][1] = pfQuad[2][3][1] = pfQuad[4][0][1] = pfQuad[4][3][1] = fY0;
  pfQuad[0][1][1] = pfQuad[0][2][1] = pfQuad[2][1][1] = pfQuad[2][2][1] = pfQuad[3][0][1] = pfQuad[3][1][1] = pfQuad[3][2][1] = pfQuad[3][3][1] = pfQuad[4][1][1] = pfQuad[4][2][1] = fY1;
  pfQuad[0][0][2] = pfQuad[0][1][2] = pfQuad[0][2][2] = pfQuad[0][3][2] = pfQuad[1][1][2] = pfQuad[1][2][2] = pfQuad[3][0][2] = pfQuad[3][3][2] = pfQuad[4][0][2] = pfQuad[4][1][2] = fZ0;
  pfQuad[1][0][2] = pfQuad[1][3][2] = pfQuad[2][0][2] = pfQuad[2][1][2] = pfQuad[2][2][2] = pfQuad[2][3][2] = pfQuad[3][1][2] = pfQuad[3][2][2] = pfQuad[4][2][2] = pfQuad[4][3][2] = fZ1;
  pfNorm[0][0][2] = pfNorm[0][1][2] = pfNorm[0][2][2] = pfNorm[0][3][2] = pfNorm[1][0][1] = pfNorm[1][1][1] = pfNorm[1][2][1] = pfNorm[1][3][1] = -1.0f;
  pfNorm[2][0][2] = pfNorm[2][1][2] = pfNorm[2][2][2] = pfNorm[2][3][2] = pfNorm[3][0][1] = pfNorm[3][1][1] = pfNorm[3][2][1] = pfNorm[3][3][1] = pfNorm[4][0][0] = pfNorm[4][1][0] = pfNorm[4][2][0] = pfNorm[4][3][0] = 1.0f;
  pfQuad += 5;
  pfNorm += 5;

  // build arms and legs
  SetArmState(true);
  SetLegs(false, false);
  pfQuad += 60;
  pfNorm += 60;

  // build face quads
  SetMouthState(false);
  pfQuad += 1;
  pfNorm += 1;
  fZ0 = (float) -m_ciThickness / 2.0f - 1.0f;
  fX0 = -46.5f;
  fX1 = -39.5f;
  fY0 = -40.0f;
  fY1 = -33.0f;
  for (int iEyebrow = 0; iEyebrow < 5; iEyebrow++)
      {
      BuildSquare(pfQuad + iEyebrow, pfNorm + iEyebrow, fX0, fY0, fX1, fY1, fZ0);
      fX0 += 6.0f;
      fY0 += 6.0f;
      fX1 += 6.0f;
      fY1 += 6.0f;
      }
  pfQuad += 5;
  pfNorm += 5;
  fX0 = 22.5f;
  fX1 = 29.5f;
  fY0 = -40.0f;
  fY1 = -33.0f;
  for (int iEyebrow = 0; iEyebrow < 5; iEyebrow++)
      {
      BuildSquare(pfQuad + iEyebrow, pfNorm + iEyebrow, fX0, fY0, fX1, fY1, fZ0);
      fX0 -= 6.0f;
      fY0 += 6.0f;
      fX1 -= 6.0f;
      fY1 += 6.0f;
      }
  pfQuad += 5;
  pfNorm += 5;
  BuildSquare(pfQuad, pfNorm, -37.0f, -7.5f, -29.0f, 6.5f, fZ0);
  BuildSquare(pfQuad+1, pfNorm+1, 11.5f, -7.5f, 19.5f, 6.5f, fZ0);

}

CErrr::~CErrr()
{
  // free model memory
  free(m_pfVertex);
  m_pfVertex  = NULL;
  free(m_pfNormal);
  m_pfNormal  = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CErrr accessor functions

void CErrr::GetParticlePolys(SPolys *psPoly) const
{
  // set up the Polys struct for no triangles and 2 quad colors
  psPoly->uiNumTriColors = 0;
  psPoly->uiNumQuadColors = 2;

  // set up the number of quads for each color and the colors
  psPoly->uiNumQuads[0] = ERRR_BODY_QUADS;
  psPoly->uiNumQuads[1] = ERRR_ARM_QUADS + ERRR_FACE_QUADS;
  psPoly->aucQuadColors[0][0] = m_uchBodyColor[0];
  psPoly->aucQuadColors[0][1] = m_uchBodyColor[1];
  psPoly->aucQuadColors[0][2] = m_uchBodyColor[2];
  psPoly->aucQuadColors[1][0] = m_uchArmColor[0];
  psPoly->aucQuadColors[1][1] = m_uchArmColor[1];
  psPoly->aucQuadColors[1][2] = m_uchArmColor[2];

  // add the vertices and normals
  for (unsigned int uiQ = 0; uiQ < ERRR_QUADS; uiQ++)
    {
    for (unsigned int uiV = 0; uiV < 4; uiV++)
      {
      memcpy(psPoly->pfQuadVertices[uiQ][uiV], m_pfVertex[uiQ*4+uiV], 3 * sizeof(float));
      memcpy(psPoly->pfQuadNormals[uiQ][uiV],  m_pfNormal[uiQ*4+uiV], 3 * sizeof(float));
      }
    }
}

void CErrr::Draw(void) const
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
  glDrawArrays(GL_QUADS, 0, ERRR_BODY_QUADS * 4);

  // draw arms, legs, and face
  piDiffuse[0]   = m_uchArmColor[0] << 23;
  piDiffuse[1]   = m_uchArmColor[1] << 23;
  piDiffuse[2]   = m_uchArmColor[2] << 23;
  piAmbient[0]   = m_uchArmColor[0] << 20;
  piAmbient[1]   = m_uchArmColor[1] << 20;
  piAmbient[2]   = m_uchArmColor[2] << 20;
  glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
  glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
  glDrawArrays(GL_QUADS, ERRR_BODY_QUADS * 4, (ERRR_ARM_QUADS + ERRR_FACE_QUADS) * 4);

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
// CErrr modifier functions

void CErrr::SetBodyColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchBodyColor[0] = uchRed;
  m_uchBodyColor[1] = uchGreen;
  m_uchBodyColor[2] = uchBlue;
}

void CErrr::SetArmColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchArmColor[0] = uchRed;
  m_uchArmColor[1] = uchGreen;
  m_uchArmColor[2] = uchBlue;
}

void CErrr::SetArmState(bool bRaised)
{
  float fX0, fY0;

  // pointers
  GLfloat (*pfQuads)[4][3] = (GLfloat (*)[4][3]) m_pfVertex + ERRR_BODY_QUADS;
  GLfloat (*pfNorms)[4][3] = (GLfloat (*)[4][3]) m_pfNormal + ERRR_BODY_QUADS;
  // left arm
  fX0 = -59.0f;
  fY0 = -14.5f;
  BuildBox(pfQuads, pfNorms, fX0 - 9.0f, fY0, fX0, fY0 + 9.0f, (float) m_ciThickness);
  fX0 -= 8.0f;
  if (bRaised) fY0 -= 8.0f; else fY0 += 8.0f;
  BuildBox(pfQuads+6, pfNorms+6, fX0 - 9.0f, fY0, fX0, fY0 + 9.0f, (float) m_ciThickness);
  fX0 -= 8.0f;
  if (bRaised) fY0 -= 22.0f; else fY0 += 8.0f;
  BuildBox(pfQuads+12, pfNorms+12, fX0 - 9.0f, fY0, fX0, fY0 + 23.0f, (float) m_ciThickness);

  // right arm
  fX0 = 58.5f;
  fY0 = -14.5f;
  BuildBox(pfQuads+18, pfNorms+18, fX0, fY0, fX0 + 9.0f, fY0 + 9.0f, (float) m_ciThickness);
  fX0 += 8.0f;
  if (bRaised) fY0 -= 8.0f; else fY0 += 8.0f;
  BuildBox(pfQuads+24, pfNorms+24, fX0, fY0, fX0 + 9.0f, fY0 + 9.0f, (float) m_ciThickness);
  fX0 += 8.0f;
  if (bRaised) fY0 -= 22.0f; else fY0 += 8.0f;
  BuildBox(pfQuads+30, pfNorms+30, fX0, fY0, fX0 + 9.0f, fY0 + 23.0f, (float) m_ciThickness);
}

void CErrr::SetMouthState(bool bWide)
{
  float fWidth, fCenterX, fZ;

  // pointers
  GLfloat (*pfQuads)[4][3] = (GLfloat (*)[4][3]) m_pfVertex + ERRR_BODY_QUADS + ERRR_ARM_QUADS;
  GLfloat (*pfNorms)[4][3] = (GLfloat (*)[4][3]) m_pfNormal + ERRR_BODY_QUADS + ERRR_ARM_QUADS;

  if (bWide) fWidth = 47.5; else fWidth = 21.0f;
  fCenterX = -6.5f;
  fZ = (float) -m_ciThickness / 2.0f - 1.0f;

  BuildSquare(pfQuads, pfNorms, fCenterX - fWidth / 2.0f, 24.0f, fCenterX + fWidth / 2.0f, 33.5f, fZ);
}

void CErrr::SetLegs(bool bLeftUp, bool bRightUp)
{
  float fLeftY, fRightY;

  // set height of legs
  if (bLeftUp)
    fLeftY = 70.0f;
  else
    fLeftY = 82.0f;
  if (bRightUp)
    fRightY = 70.0f;
  else
    fRightY = 82.0f;

  // pointers
  GLfloat (*pfQuad)[4][3] = (GLfloat (*)[4][3]) m_pfVertex + ERRR_BODY_QUADS + 36;
  GLfloat (*pfNorm)[4][3] = (GLfloat (*)[4][3]) m_pfNormal + ERRR_BODY_QUADS + 36;
  BuildBox(pfQuad,    pfNorm,   -21.5f, 49.5f,         -15.5f, fLeftY, (float) m_ciThickness);
  BuildBox(pfQuad+6,  pfNorm+6, -47.0f, fLeftY - 6.0f, -15.5f, fLeftY, (float) m_ciThickness);
  BuildBox(pfQuad+12, pfNorm+12, 13.5f, 49.5f,          19.5f, fRightY, (float) m_ciThickness);
  BuildBox(pfQuad+18, pfNorm+18, 13.5f, fRightY - 6.0f, 45.0f, fRightY, (float) m_ciThickness);
}

bool CErrr::TestCollision(float fLeftX, float fRightX, float fTopY, float fBotY, int iDamage)
{
  // first do a quick Y-coordinate comparison
  if (fTopY > m_fPositionY + 49 || fBotY < m_fPositionY - 63)
    return false;

  float fAngleRad = m_fAngle * (float) M_PI / 180.0f;

  // figure out the deepest point of front surface of the Mooninite
  float fZ = m_fPositionZ - (float) m_ciThickness / 2.0f + 59.0f * sin(fAngleRad);

  // if it doesn't pass through the Z=0 plane, there can't be any collision
  if (fZ < 0.0f) return false;

  // else calculate the left and right X coordinates of the intersection of the invader with the Z=0 plane
  float fSlope = cos(fAngleRad) / sin(fAngleRad);
  float fZ0LeftX = m_fPositionX - (float) m_ciThickness / 2.0f * sin(fAngleRad);
  float fZ0RightX = m_fPositionX + (float) m_ciThickness / 2.0f * sin(fAngleRad);
  fZ0LeftX -= (m_fPositionZ - (float) m_ciThickness / 2.0f * cos(fAngleRad)) * -fSlope;
  fZ0RightX -= (m_fPositionZ + (float) m_ciThickness / 2.0f * cos(fAngleRad)) * -fSlope;

  // test for the X intersection
  if (fRightX < fZ0LeftX || fLeftX > fZ0RightX) return false;

  // there is a collision
  return true;
}

/////////////////////////////////////////////////////////////////////////////
// CErrr private functions

