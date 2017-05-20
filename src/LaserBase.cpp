/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - LaserBase.cpp                                            *
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

#ifdef WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>

#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include "GameMain.h" // for m_bAlphaSupported flag
#include "LaserBase.h"

// const model parameters
const int CLaserBase::m_ciBaseWidth  = 70;
const int CLaserBase::m_ciBaseHeight = 30;
const int CLaserBase::m_ciBaseDepth  = 40;
const int CLaserBase::m_ciTotalVertices = BASE_QUADS * 4 + BASE_QSTRIP * 2 + MAX_WEAPON_VERT;


/////////////////////////////////////////////////////////////////////////////
// CLaserBase constructor/destructor

CLaserBase::CLaserBase()
{
  // set default colors
  m_uchBaseColor[0] = 0x99;  m_uchBaseColor[1] = 0x99;  m_uchBaseColor[2] = 0x99;

  // initial position is 0,0,0
  m_fPositionX = 0.0f;
  m_fPositionY = 0.0f;
  m_fPositionZ = 50.0f;

  // shield is off
  m_bShield      = false;
  m_iShieldPulse = 0;

  // allocate model arrays
  m_pfVertex  = (GLfloat (*)[3]) malloc(m_ciTotalVertices * sizeof(GLfloat) * 3);
  m_pfNormal  = (GLfloat (*)[3]) malloc(m_ciTotalVertices * sizeof(GLfloat) * 3);

  // clear the vertex array
  memset(m_pfVertex, 0, m_ciTotalVertices * sizeof(GLfloat) * 3);

  // set up pointers
  GLfloat (*pfQuad)[4][3] = (GLfloat (*)[4][3]) m_pfVertex;
  GLfloat (*pfNorm)[4][3] = (GLfloat (*)[4][3]) m_pfNormal;

  // build coordinates
  float fX0 = (float) -m_ciBaseWidth / 2.0f;
  float fX1 = fX0 + (float) m_ciBaseHeight / 3.0f;
  float fX3 = (float) m_ciBaseWidth / 2.0f;
  float fX2 = fX3 - (float) m_ciBaseHeight / 3.0f;
  float fY0 = (float) -m_ciBaseHeight / 2.0f;
  float fY1 = fY0 + (float) m_ciBaseHeight / 3.0f;
  float fY2 = (float) m_ciBaseHeight / 2.0f;
  float fZ0 = (float) -m_ciBaseDepth / 2.0f;
  float fZ1 = fZ0 + (float) m_ciBaseHeight / 3.0f;
  float fZ3 = (float) m_ciBaseDepth / 2.0f;
  float fZ2 = fZ3 - (float) m_ciBaseHeight / 3.0f;

  // clear the normals
  for (int i = 0; i < BASE_QUADS; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 3; k++)
        {
        pfNorm[i][j][k] = 0.0f;
        }
  // build front, back, and side quads
  pfQuad[0][0][0] = pfQuad[0][1][0] = pfQuad[2][2][0] = pfQuad[2][3][0] = pfQuad[3][0][0] = pfQuad[3][1][0] = pfQuad[3][2][0] = pfQuad[3][3][0] = fX0;
  pfQuad[0][2][0] = pfQuad[0][3][0] = pfQuad[1][0][0] = pfQuad[1][1][0] = pfQuad[1][2][0] = pfQuad[1][3][0] = pfQuad[2][0][0] = pfQuad[2][1][0] = fX3;
  pfQuad[0][0][1] = pfQuad[0][3][1] = pfQuad[1][0][1] = pfQuad[1][3][1] = pfQuad[2][0][1] = pfQuad[2][3][1] = pfQuad[3][0][1] = pfQuad[3][3][1] = fY1;
  pfQuad[0][1][1] = pfQuad[0][2][1] = pfQuad[1][1][1] = pfQuad[1][2][1] = pfQuad[2][1][1] = pfQuad[2][2][1] = pfQuad[3][1][1] = pfQuad[3][2][1] = fY2;
  pfQuad[0][0][2] = pfQuad[0][1][2] = pfQuad[0][2][2] = pfQuad[0][3][2] = pfQuad[1][0][2] = pfQuad[1][1][2] = pfQuad[3][2][2] = pfQuad[3][3][2] = fZ0;
  pfQuad[1][2][2] = pfQuad[1][3][2] = pfQuad[2][0][2] = pfQuad[2][1][2] = pfQuad[2][2][2] = pfQuad[2][3][2] = pfQuad[3][0][2] = pfQuad[3][1][2] = fZ3;
  pfNorm[3][0][0] = pfNorm[3][1][0] = pfNorm[3][2][0] = pfNorm[3][3][0] = pfNorm[0][0][2] = pfNorm[0][1][2] = pfNorm[0][2][2] = pfNorm[0][3][2] = -1.0f;
  pfNorm[1][0][0] = pfNorm[1][1][0] = pfNorm[1][2][0] = pfNorm[1][3][0] = pfNorm[2][0][2] = pfNorm[2][1][2] = pfNorm[2][2][2] = pfNorm[2][3][2] = 1.0f;
  pfQuad += 4;
  pfNorm += 4;
  // build top and bottom quads
  pfQuad[1][0][0] = pfQuad[1][1][0] = fX0;
  pfQuad[0][0][0] = pfQuad[0][1][0] = fX1;
  pfQuad[0][2][0] = pfQuad[0][3][0] = fX2;
  pfQuad[1][2][0] = pfQuad[1][3][0] = fX3;
  pfQuad[0][0][1] = pfQuad[0][1][1] = pfQuad[0][2][1] = pfQuad[0][3][1] = fY0;
  pfQuad[1][0][1] = pfQuad[1][1][1] = pfQuad[1][2][1] = pfQuad[1][3][1] = fY2;
  pfQuad[1][0][2] = pfQuad[1][3][2] = fZ0;
  pfQuad[0][1][2] = pfQuad[0][2][2] = fZ1;
  pfQuad[0][0][2] = pfQuad[0][3][2] = fZ2;
  pfQuad[1][1][2] = pfQuad[1][2][2] = fZ3;
  pfNorm[0][0][1] = pfNorm[0][1][1] = pfNorm[0][2][1] = pfNorm[0][3][1] = -1.0f;
  pfNorm[1][0][1] = pfNorm[1][1][1] = pfNorm[1][2][1] = pfNorm[1][3][1] = 1.0f;
  pfQuad += 2;
  pfNorm += 2;

  // build quad strips
  GLfloat (*pfQSVert)[3] = (GLfloat (*)[3]) pfQuad;
  GLfloat (*pfQSNorm)[3] = (GLfloat (*)[3]) pfNorm;
  for (int iRad = 0; iRad <= 9; iRad++)
      {
      float fAngle = (float) M_PI * iRad / 18.0f;
      float fS = sin(fAngle) * m_ciBaseHeight / 3.0f;
      float fC = cos(fAngle) * m_ciBaseHeight / 3.0f;
      pfQSVert[00][0] = pfQSVert[41][0] = pfQSVert[60][0] = pfQSVert[61][0] = fX1 - fC;
      pfQSVert[01][0] = pfQSVert[20][0] = pfQSVert[21][0] = pfQSVert[40][0] = fX2 + fC;
      pfQSVert[00][1] = pfQSVert[01][1] = pfQSVert[20][1] = pfQSVert[21][1] = pfQSVert[40][1] = pfQSVert[41][1] = pfQSVert[60][1] = pfQSVert[61][1] = fY1 - fS;
      pfQSVert[00][2] = pfQSVert[01][2] = pfQSVert[20][2] = pfQSVert[61][2] = fZ1 - fC;
      pfQSVert[21][2] = pfQSVert[40][2] = pfQSVert[41][2] = pfQSVert[60][2] = fZ2 + fC;
      pfQSNorm[00][0] = pfQSNorm[01][0] = pfQSNorm[40][0] = pfQSNorm[41][0] = 0.0f;
      pfQSNorm[60][0] = pfQSNorm[61][0] = -cos(fAngle);
      pfQSNorm[20][0] = pfQSNorm[21][0] = cos(fAngle);
      pfQSNorm[00][1] = pfQSNorm[01][1] = pfQSNorm[20][1] = pfQSNorm[21][1] = pfQSNorm[40][1] = pfQSNorm[41][1] = pfQSNorm[60][1] = pfQSNorm[61][1] = -sin(fAngle);
      pfQSNorm[20][2] = pfQSNorm[21][2] = pfQSNorm[60][2] = pfQSNorm[61][2] = 0.0f;
      pfQSNorm[00][2] = pfQSNorm[01][2] = -cos(fAngle);
      pfQSNorm[40][2] = pfQSNorm[41][2] = cos(fAngle);
      pfQSVert += 2;
      pfQSNorm += 2;
      }

  // build gun
  m_iBarrelPercent = 100;
  m_iBarrelDegrees = 0;
  SetWeapon(E_LASER);
}

CLaserBase::~CLaserBase()
{
  // free model memory
  free(m_pfVertex);
  m_pfVertex  = NULL;
  free(m_pfNormal);
  m_pfNormal  = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CLaserBase accessor functions

void CLaserBase::GetBounding(float *pfLeftX, float *pfRightX, float *pfTopY, float *pfBotY) const
{
  *pfLeftX  = m_fPositionX - m_ciBaseWidth / 2.0f;
  *pfRightX = m_fPositionX + m_ciBaseWidth / 2.0f;
  *pfTopY   = m_fPositionY - m_ciBaseHeight / 2.0f;
  *pfBotY   = m_fPositionY + m_ciBaseHeight / 2.0f;
}

void CLaserBase::GetPosition(float *pfX, float *pfY, float *pfZ) const
{
  *pfX = m_fPositionX;
  *pfY = m_fPositionY;
  *pfZ = m_fPositionZ;
}

void CLaserBase::Draw(void) const
{
  // Position the model in 3-space
  glTranslatef(m_fPositionX, m_fPositionY, m_fPositionZ);

  GLfloat pfSpecular[4]  = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat pfShininess[1] = {32.0f};
  GLint   piDiffuse[4]   = {m_uchBaseColor[0] << 23, m_uchBaseColor[1] << 23, m_uchBaseColor[2] << 23, 0x7fffffff};
  GLint   piAmbient[4]   = {m_uchBaseColor[0] << 20, m_uchBaseColor[1] << 20, m_uchBaseColor[2] << 20, 0x7fffffff};

  glMaterialfv(GL_FRONT, GL_SPECULAR, pfSpecular);
  glMaterialfv(GL_FRONT, GL_SHININESS, pfShininess);
  glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
  glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);

  // draw the base
  glShadeModel(GL_SMOOTH);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, m_pfVertex);
  glNormalPointer(GL_FLOAT, 0, m_pfNormal);
  glDrawArrays(GL_QUADS, 0, BASE_QUADS * 4);
  int iIndex = BASE_QUADS * 4;
  for (int iStrip = 0; iStrip < 4; iStrip++)
    {
    glDrawArrays(GL_QUAD_STRIP, iIndex, 20);
    iIndex += 20;
    }
  glShadeModel(GL_FLAT);

  if (m_iBarrelPercent > 0)
    {
    // draw weapon
    piDiffuse[0]   = m_uchWeaponColor[0] << 23;
    piDiffuse[1]   = m_uchWeaponColor[1] << 23;
    piDiffuse[2]   = m_uchWeaponColor[2] << 23;
    piAmbient[0]   = m_uchWeaponColor[0] << 20;
    piAmbient[1]   = m_uchWeaponColor[1] << 20;
    piAmbient[2]   = m_uchWeaponColor[2] << 20;
    glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
    glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
    switch (m_eWeaponType)
      {
      case E_LASER:
        glShadeModel(GL_SMOOTH);
        glDrawArrays(GL_TRIANGLE_FAN, iIndex, 6);
        iIndex += 6;
        glDrawArrays(GL_QUAD_STRIP, iIndex, 10);
        glShadeModel(GL_FLAT);
        break;
      case E_GUN:
        for (int iStrip = 0; iStrip < 4; iStrip++)
          {
          glDrawArrays(GL_QUAD_STRIP, iIndex, 14);
          iIndex += 14;
          }
        piDiffuse[0] = piDiffuse[1] = piDiffuse[2] = 0;
        piAmbient[0] = piAmbient[1] = piAmbient[2] = 0;
        glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
        glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
        glDrawArrays(GL_TRIANGLE_FAN, iIndex, 8);
        break;
      case E_DUAL_GUN:
        int iTriFan1, iTriFan2;
        for (int iStrip = 0; iStrip < 4; iStrip++)
          {
          glDrawArrays(GL_QUAD_STRIP, iIndex, 14);
          iIndex += 14;
          }
        iTriFan1 = iIndex;
        iIndex += 8;
        for (int iStrip = 0; iStrip < 4; iStrip++)
          {
          glDrawArrays(GL_QUAD_STRIP, iIndex, 14);
          iIndex += 14;
          }
        iTriFan2 = iIndex;
        iIndex += 8;
        piDiffuse[0] = piDiffuse[1] = piDiffuse[2] = 0;
        piAmbient[0] = piAmbient[1] = piAmbient[2] = 0;
        glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
        glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
        glDrawArrays(GL_TRIANGLE_FAN, iTriFan1, 8);
        glDrawArrays(GL_TRIANGLE_FAN, iTriFan2, 8);
        break;
      case E_CANNON:
        for (int iStrip = 0; iStrip < 4; iStrip++)
          {
          glDrawArrays(GL_QUAD_STRIP, iIndex, 18);
          iIndex += 18;
          }
        piDiffuse[0] = piDiffuse[1] = piDiffuse[2] = 0;
        piAmbient[0] = piAmbient[1] = piAmbient[2] = 0;
        glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
        glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
        glDrawArrays(GL_TRIANGLE_FAN, iIndex, 10);
        break;
      }
    }

  // disable normals
  glDisableClientState(GL_NORMAL_ARRAY);

  if (m_bShield)
    {
    // set up OpenGL to draw shield
    glDisable(GL_LIGHTING);
    if (CGameMain::m_bAlphaSupported)
      glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_BACK, GL_LINE);
    glLineWidth(5);
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);

    // set the color of the shield
    float fAlpha = 0.5f + 0.15f * sin((float) m_iShieldPulse * (float) M_PI / 16.0f);
    glColor4f(0.0f, 0.0f, 1.0f, fAlpha);

    // re-send LaserBase polys to draw shield
    glDrawArrays(GL_QUADS, 0, BASE_QUADS * 4);
    iIndex = BASE_QUADS * 4;
    for (int iStrip = 0; iStrip < 4; iStrip++)
      {
      glDrawArrays(GL_QUAD_STRIP, iIndex, 20);
      iIndex += 20;
      }

    // put everything back
    glEnable(GL_LIGHTING);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_BACK, GL_FILL);
    if (CGameMain::m_bAlphaSupported)
	    glDisable(GL_BLEND);
    }

  // all done
  glDisableClientState(GL_VERTEX_ARRAY);
  return;
}

void CLaserBase::GetGunPosition1(float *pfX, float *pfY) const
{
  *pfY = m_fPositionY - (float) m_ciBaseHeight * 1.16f;

  if (m_eWeaponType != E_DUAL_GUN)
    *pfX = m_fPositionX;
  else
    *pfX = m_fPositionX - (float) m_ciBaseWidth / 4.0f;
}

void CLaserBase::GetGunPosition2(float *pfX, float *pfY) const
{
  if (m_eWeaponType != E_DUAL_GUN) return;

  *pfY = m_fPositionY - (float) m_ciBaseHeight * 1.16f;
  *pfX = m_fPositionX + (float) m_ciBaseWidth / 4.0f;
}

/////////////////////////////////////////////////////////////////////////////
// CLaserBase modifier functions

void CLaserBase::SetBaseColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchBaseColor[0] = uchRed;
  m_uchBaseColor[1] = uchGreen;
  m_uchBaseColor[2] = uchBlue;
}

void CLaserBase::SetPosition(float fX, float fY, float fZ)
{
  m_fPositionX = fX;
  m_fPositionY = fY;
  m_fPositionZ = fZ;
}

bool CLaserBase::SetWeapon(EWeapon eType)
{
  switch (eType)
    {
    case E_LASER:
      m_uchWeaponColor[0] = 0xaf;
      m_uchWeaponColor[1] = 0xaf;
      m_uchWeaponColor[2] = 0x00;
      break;
    case E_GUN:
      m_uchWeaponColor[0] = 0x5f;
      m_uchWeaponColor[1] = 0x5f;
      m_uchWeaponColor[2] = 0xaf;
      break;
    case E_DUAL_GUN:
      m_uchWeaponColor[0] = 0x5f;
      m_uchWeaponColor[1] = 0x5f;
      m_uchWeaponColor[2] = 0xaf;
      break;
    case E_CANNON:
      m_uchWeaponColor[0] = 0xaf;
      m_uchWeaponColor[1] = 0x5f;
      m_uchWeaponColor[2] = 0x5f;
      break;
    default:
      return false;
    }

  m_eWeaponType = eType;
  BuildWeapon();
  return true;
}

void CLaserBase::SetBarrelExtension(int iPercent)
{
  m_iBarrelPercent = iPercent < 0 ? 0 : (iPercent > 100 ? 100 : iPercent);
  BuildWeapon();
}

void CLaserBase::SetBarrelRotation(int iDegrees)
{
  m_iBarrelDegrees = iDegrees % 360;
  BuildWeapon();
}

void CLaserBase::RotateBarrel(int iDegrees)
{
  m_iBarrelDegrees = (m_iBarrelDegrees + iDegrees) % 360;
  BuildWeapon();
}

bool CLaserBase::TestCollision(float fLeftX, float fRightX, float fTopY, float fBotY)
{
  if (fRightX < m_fPositionX - m_ciBaseWidth / 2.0f ||
      fLeftX  > m_fPositionX + m_ciBaseWidth / 2.0f ||
      fTopY   > m_fPositionY + m_ciBaseHeight / 2.0f ||
      fBotY   < m_fPositionY - m_ciBaseHeight / 2.0f) return false;

  return true;
}

/////////////////////////////////////////////////////////////////////////////
// CLaserBase private functions

void CLaserBase::BuildWeapon()
{
  float fX0, fX1, fY, fR;
  int iIndex = BASE_QUADS * 4 + BASE_QSTRIP * 2;

  fY = (float) -m_ciBaseHeight / 2.0f;

  if (m_eWeaponType == E_LASER)
    {
    float (*pfVert)[3] = m_pfVertex + iIndex;
    float (*pfNorm)[3] = m_pfNormal + iIndex;
    float fHeight = (float) m_ciBaseHeight * 0.0066f * (float) m_iBarrelPercent;
    float fTopR   = (float) m_ciBaseDepth / 9.0f;
    float fBotR   = (float) m_ciBaseDepth / 6.0f;
    // clear the normals
    for (int i = 0; i < 16; i++)
      {
      pfNorm[i][0] = 0.0f;
      pfNorm[i][1] = 0.0f;
      pfNorm[i][2] = 0.0f;
      }
    // first, add 6 vertices for triange fan
    pfVert[0][0] = pfVert[0][2] = pfVert[1][0] = pfVert[2][2] = pfVert[3][0] = pfVert[4][2] = pfVert[5][0] = 0.0f;
    pfVert[0][1] = fY - fHeight;
    pfVert[1][1] = pfVert[2][1] = pfVert[3][1] = pfVert[4][1] = pfVert[5][1] = fY - fHeight + fTopR;
    pfVert[1][2] = pfVert[4][0] = pfVert[5][2] = -fTopR;
    pfVert[2][0] = pfVert[3][2] = fTopR;
    pfNorm[0][1] = pfNorm[1][2] = pfNorm[4][0] = pfNorm[5][2] = -1.0f;
    pfNorm[2][0] = pfNorm[3][2] = 1.0f;
    pfVert += 6;
    pfNorm += 6;
    // then add 10 vertices for quad strip
    if (fHeight < fTopR) fY = 0.0f;
    pfVert[0][0] = pfVert[1][0] = pfVert[2][2] = pfVert[3][2] = pfVert[4][0] = pfVert[5][0] = pfVert[6][2] = pfVert[7][2] = pfVert[8][0] = pfVert[9][0] = 0.0f;
    pfVert[0][2] = pfVert[6][0] = pfVert[8][2] = -fTopR;
    pfVert[2][0] = pfVert[4][2] = fTopR;
    pfVert[1][2] = pfVert[7][0] = pfVert[9][2] = -fBotR;
    pfVert[3][0] = pfVert[5][2] = fBotR;
    pfVert[0][1] = pfVert[2][1] = pfVert[4][1] = pfVert[6][1] = pfVert[8][1] = fY - fHeight + fTopR;
    pfVert[1][1] = pfVert[3][1] = pfVert[5][1] = pfVert[7][1] = pfVert[9][1] = fY;
    pfNorm[0][2] = pfNorm[1][2] = pfNorm[6][0] = pfNorm[7][0] = pfNorm[8][2] = pfNorm[9][2] = -1.0f;
    pfNorm[2][0] = pfNorm[3][0] = pfNorm[4][2] = pfNorm[5][2] = 1.0f;
    }
  else if (m_eWeaponType == E_GUN)
    {
    fR = (float)  m_ciBaseDepth / 7.0f;
    BuildGun(iIndex, 6, 0.0f, fY, 0.0f, fR);
    }
  else if (m_eWeaponType == E_DUAL_GUN)
    {
    fR = (float)  m_ciBaseDepth / 7.0f;
    fX0 = (float) -m_ciBaseWidth / 4.0f;
    fX1 = (float)  m_ciBaseWidth / 4.0f;
    BuildGun(iIndex, 6, fX0, fY, 0.0f, fR);
    iIndex += 64;
    BuildGun(iIndex, 6, fX1, fY, 0.0f, fR);
    }
  else if (m_eWeaponType == E_CANNON)
    {
    fR = (float)  m_ciBaseDepth / 4.0f;
    BuildGun(iIndex, 8, 0.0f, fY, 0.0f, fR);
    }

  return;
}

void CLaserBase::BuildGun(int iIndex, int iSides, float fX, float fY, float fZ, float fRadius)
{
  float fR0 = fRadius * 2.0f / 3.0f;
  float fR1 = fRadius * 5.0f / 6.0f;
  float fHeight = (float) m_ciBaseHeight * 0.0066f * (float) m_iBarrelPercent;
  float fY1;
  float fY2 = -fHeight;
  if (m_iBarrelPercent <= 50)
    {
    fY1 = fY2;
    }
  else
    {
    fY1 = (float) -m_ciBaseHeight * 0.33f;
    }

  float (*pfVert)[3] = m_pfVertex + iIndex;
  float (*pfNorm)[3] = m_pfNormal + iIndex;

  float fRadOff = (float) m_iBarrelDegrees * (float) M_PI / 180.0f;
  int iStripLen = (iSides + 1) * 2;

  // store first 'layer' of the barrel
  for (int iS = 0; iS <= iSides; iS++)
    {
    float fAngle = (float) M_PI * 2.0f * iS / iSides;
    float fX0 = sin(fAngle + fRadOff) * fR1;
    float fX1 = sin(fAngle + fRadOff) * fRadius;
    float fZ0 = -cos(fAngle + fRadOff) * fR1;
    float fZ1 = -cos(fAngle + fRadOff) * fRadius;
    // store vertices
    pfVert[0][0] = pfVert[1][0] = fX + fX1;
    pfVert[0][1] = fY + fY1;
    pfVert[0][2] = pfVert[1][2] = fZ + fZ1;
    pfVert[1][1] = fY;
    pfVert[iStripLen + 0][0] = fX + fX0;
    pfVert[iStripLen + 0][1] = pfVert[iStripLen + 1][1] = fY + fY1;
    pfVert[iStripLen + 0][2] = fZ + fZ0;
    pfVert[iStripLen + 1][0] = fX + fX1;
    pfVert[iStripLen + 1][2] = fZ + fZ1;
    // store normals
    pfNorm[0][0] = pfNorm[1][0] = sin(fAngle + fRadOff);
    pfNorm[0][1] = pfNorm[1][1] = 0.0f;
    pfNorm[0][2] = pfNorm[1][2] = -cos(fAngle + fRadOff);
    pfNorm[iStripLen + 0][0] = pfNorm[iStripLen + 1][0] = 0.0f;
    pfNorm[iStripLen + 0][1] = pfNorm[iStripLen + 1][1] = -1.0f;
    pfNorm[iStripLen + 0][2] = pfNorm[iStripLen + 1][2] = 0.0f;
    pfVert += 2;
    pfNorm += 2;
    }
  pfVert += iStripLen;
  pfNorm += iStripLen;

  // now build second barrel layer
  for (int iS = 0; iS <= iSides; iS++)
    {
    float fAngle = (float) M_PI * 2.0f * iS / iSides;
    float fX0 = sin(fAngle + fRadOff) * fR0;
    float fX1 = sin(fAngle + fRadOff) * fR1;
    float fZ0 = -cos(fAngle + fRadOff) * fR0;
    float fZ1 = -cos(fAngle + fRadOff) * fR1;
    // store vertices
    pfVert[0][0] = pfVert[1][0] = fX + fX1;
    pfVert[0][1] = fY + fY2;
    pfVert[0][2] = pfVert[1][2] = fZ + fZ1;
    pfVert[1][1] = fY + fY1;
    pfVert[iStripLen + 0][0] = fX + fX0;
    pfVert[iStripLen + 0][1] = pfVert[iStripLen + 1][1] = fY + fY2;
    pfVert[iStripLen + 0][2] = fZ + fZ0;
    pfVert[iStripLen + 1][0] = fX + fX1;
    pfVert[iStripLen + 1][2] = fZ + fZ1;
    // store normals
    pfNorm[0][0] = pfNorm[1][0] = sin(fAngle + fRadOff);
    pfNorm[0][1] = pfNorm[1][1] = 0.0f;
    pfNorm[0][2] = pfNorm[1][2] = -cos(fAngle + fRadOff);
    pfNorm[iStripLen + 0][0] = pfNorm[iStripLen + 1][0] = 0.0f;
    pfNorm[iStripLen + 0][1] = pfNorm[iStripLen + 1][1] = -1.0f;
    pfNorm[iStripLen + 0][2] = pfNorm[iStripLen + 1][2] = 0.0f;
    pfVert += 2;
    pfNorm += 2;
    }
  pfVert += iStripLen;
  pfNorm += iStripLen;

  // lastly, do the triangle strip in the center
  pfVert[0][0] = fX;
  pfVert[0][1] = fY + fY2;
  pfVert[0][2] = fZ;
  pfNorm[0][0] = 0.0f;
  pfNorm[0][1] = -1.0f;
  pfNorm[0][2] = 0.0f;
  for (int iS = 0; iS <= iSides; iS++)
    {
    float fAngle = (float) M_PI * 2.0f * iS / iSides;
    float fX0 = sin(fAngle + fRadOff) * fR0;
    float fZ0 = -cos(fAngle + fRadOff) * fR0;
    pfVert[iS + 1][0] = fX + fX0;
    pfVert[iS + 1][1] = fY + fY2;
    pfVert[iS + 1][2] = fZ + fZ0;
    pfNorm[iS + 1][0] = 0.0f;
    pfNorm[iS + 1][1] = -1.0f;
    pfNorm[iS + 1][2] = 0.0f;
    }
}
