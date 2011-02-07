/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Bunker.cpp                                               *
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

/****************************************************************************
**                              04/22/05                                   **
**                                                                         **
** Bunker.cpp - contains implementation of CBunker class                   **
****************************************************************************/

#include <math.h>
#include "Bunker.h"
#include "GameMain.h" // for m_bAlphaSupported flag
#include "Gameplay.h"

// const model parameters
const int CBunker::m_ciWidth = 100;
const int CBunker::m_ciHeight = 50;


/////////////////////////////////////////////////////////////////////////////
// CBunker constructor/destructor

CBunker::CBunker()
{
  // set default colors
  SetColor(0, 0, 0xe0);

  // set default position
  m_fPositionX = 0.0f;
  m_fPositionY = 0.0f;
  m_fPositionZ = 0.0f;

  // no flash
  m_bFlash = false;

  // calculate the coordinates
  float fX0 = (float) -m_ciWidth / 2.0f;
  float fX1 = fX0 + (float) m_ciHeight / 4.0f;
  float fX2 = fX1 + (float) m_ciHeight / 8.0f;
  float fX3 = -fX2;
  float fX4 = -fX1;
  float fX5 = -fX0;
  float fY0 = (float) -m_ciHeight / 2.0f;
  float fY1 = (float) -m_ciHeight / 4.0f;
  float fY2 = -fY1;
  float fY3 = -fY0;
  float fZ0 = fX0;
  float fZ1 = fX1;
  float fZ2 = fX4;
  float fZ3 = fX5;

  // make pointers
  GLfloat (*pfQuad)[4][3] = m_afQuads;
  GLfloat (*pfNorm)[4][3] = m_afNormals;

  // clear normals
  for (int i = 0; i < BUNKER_QUADS; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 3; k++)
        pfNorm[i][j][k] = 0.0f;

  // build quads for top part
  pfQuad[0][1][0] = pfQuad[2][2][0] = pfQuad[3][1][0] = pfQuad[3][2][0] = fX0;
  pfQuad[0][0][0] = pfQuad[2][3][0] = pfQuad[3][0][0] = pfQuad[3][3][0] = pfQuad[4][0][0] = pfQuad[4][1][0] = fX1;
  pfQuad[0][3][0] = pfQuad[1][0][0] = pfQuad[1][3][0] = pfQuad[2][0][0] = pfQuad[4][2][0] = pfQuad[4][3][0] = fX4;
  pfQuad[0][2][0] = pfQuad[1][1][0] = pfQuad[1][2][0] = pfQuad[2][1][0] = fX5;
  pfQuad[0][0][1] = pfQuad[0][3][1] = pfQuad[1][0][1] = pfQuad[1][3][1] = pfQuad[2][0][1] = pfQuad[2][3][1] = pfQuad[3][0][1] = pfQuad[3][3][1] = pfQuad[4][0][1] = pfQuad[4][1][1] = pfQuad[4][2][1] = pfQuad[4][3][1] = fY0;
  pfQuad[0][1][1] = pfQuad[0][2][1] = pfQuad[1][1][1] = pfQuad[1][2][1] = pfQuad[2][1][1] = pfQuad[2][2][1] = pfQuad[3][1][1] = pfQuad[3][2][1] = fY1;
  pfQuad[0][1][2] = pfQuad[0][2][2] = pfQuad[1][1][2] = pfQuad[3][2][2] = fZ0;
  pfQuad[0][0][2] = pfQuad[0][3][2] = pfQuad[1][0][2] = pfQuad[3][3][2] = pfQuad[4][1][2] = pfQuad[4][2][2] = fZ1;
  pfQuad[1][3][2] = pfQuad[2][0][2] = pfQuad[2][3][2] = pfQuad[3][0][2] = pfQuad[4][0][2] = pfQuad[4][3][2] = fZ2;
  pfQuad[1][2][2] = pfQuad[2][1][2] = pfQuad[2][2][2] = pfQuad[3][1][2] = fZ3;
  pfQuad += 5;
  // build normals for top part
  pfNorm[0][0][1] = pfNorm[0][1][1] = pfNorm[0][2][1] = pfNorm[0][3][1] = pfNorm[1][0][1] = pfNorm[1][1][1] = pfNorm[1][2][1] = pfNorm[1][3][1] = pfNorm[2][0][1] = pfNorm[2][1][1] = pfNorm[2][2][1] = pfNorm[2][3][1] = pfNorm[3][0][1] = pfNorm[3][1][1] = pfNorm[3][2][1] = pfNorm[3][3][1] = -0.707f;
  pfNorm[3][0][0] = pfNorm[3][1][0] = pfNorm[3][2][0] = pfNorm[3][3][0] = pfNorm[0][0][2] = pfNorm[0][1][2] = pfNorm[0][2][2] = pfNorm[0][3][2] = -0.707f;
  pfNorm[1][0][0] = pfNorm[1][1][0] = pfNorm[1][2][0] = pfNorm[1][3][0] = pfNorm[2][0][2] = pfNorm[2][1][2] = pfNorm[2][2][2] = pfNorm[2][3][2] = 0.707f;
  pfNorm[4][0][1] = pfNorm[4][1][1] = pfNorm[4][2][1] = pfNorm[4][3][1] = -1.0f;
  pfNorm += 5;

  // build quads for left and right sides
  pfQuad[0][0][0] = pfQuad[0][1][0] = pfQuad[0][2][0] = pfQuad[0][3][0] = fX5;
  pfQuad[1][0][0] = pfQuad[1][1][0] = pfQuad[1][2][0] = pfQuad[1][3][0] = fX0;
  pfQuad[0][0][1] = pfQuad[0][3][1] = pfQuad[1][0][1] = pfQuad[1][3][1] = fY1;
  pfQuad[0][1][1] = pfQuad[0][2][1] = pfQuad[1][1][1] = pfQuad[1][2][1] = fY3;
  pfQuad[0][0][2] = pfQuad[0][1][2] = pfQuad[1][2][2] = pfQuad[1][3][2] = fZ0;
  pfQuad[0][2][2] = pfQuad[0][3][2] = pfQuad[1][0][2] = pfQuad[1][1][2] = fZ3;
  pfQuad += 2;
  // build normals for left and right sides
  pfNorm[0][0][0] = pfNorm[0][1][0] = pfNorm[0][2][0] = pfNorm[0][3][0] = 1.0f;
  pfNorm[1][0][0] = pfNorm[1][1][0] = pfNorm[1][2][0] = pfNorm[1][3][0] = -1.0f;
  pfNorm += 2;

  // build quads for front and back sides
  pfQuad[0][0][0] = pfQuad[0][1][0] = pfQuad[1][0][0] = pfQuad[1][1][0] = pfQuad[3][2][0] = pfQuad[3][3][0] = pfQuad[5][2][0] = pfQuad[5][3][0] = fX0;
  pfQuad[1][2][0] = pfQuad[5][1][0] = fX1;
  pfQuad[1][3][0] = pfQuad[5][0][0] = fX2;
  pfQuad[2][0][0] = pfQuad[4][3][0] = fX3;
  pfQuad[2][1][0] = pfQuad[4][2][0] = fX4;
  pfQuad[0][2][0] = pfQuad[0][3][0] = pfQuad[2][2][0] = pfQuad[2][3][0] = pfQuad[3][0][0] = pfQuad[3][1][0] = pfQuad[4][0][0] = pfQuad[4][1][0] = fX5;
  pfQuad[0][0][1] = pfQuad[0][3][1] = pfQuad[3][0][1] = pfQuad[3][3][1] = fY1;
  pfQuad[0][1][1] = pfQuad[0][2][1] = pfQuad[1][0][1] = pfQuad[1][3][1] = pfQuad[2][0][1] = pfQuad[2][3][1] = pfQuad[3][1][1] = pfQuad[3][2][1] = pfQuad[4][0][1] = pfQuad[4][3][1] = pfQuad[5][0][1] = pfQuad[5][3][1] = fY2;
  pfQuad[1][1][1] = pfQuad[1][2][1] = pfQuad[2][1][1] = pfQuad[2][2][1] = pfQuad[4][1][1] = pfQuad[4][2][1] = pfQuad[5][1][1] = pfQuad[5][2][1] = fY3;
  pfQuad[0][0][2] = pfQuad[0][1][2] = pfQuad[0][2][2] = pfQuad[0][3][2] = pfQuad[1][0][2] = pfQuad[1][1][2] = pfQuad[1][2][2] = pfQuad[1][3][2] = pfQuad[2][0][2] = pfQuad[2][1][2] = pfQuad[2][2][2] = pfQuad[2][3][2] = fZ0;
  pfQuad[3][0][2] = pfQuad[3][1][2] = pfQuad[3][2][2] = pfQuad[3][3][2] = pfQuad[4][0][2] = pfQuad[4][1][2] = pfQuad[4][2][2] = pfQuad[4][3][2] = pfQuad[5][0][2] = pfQuad[5][1][2] = pfQuad[5][2][2] = pfQuad[5][3][2] = fZ3;
  pfQuad += 6;
  // build normals for front and back sides
  pfNorm[0][0][2] = pfNorm[0][1][2] = pfNorm[0][2][2] = pfNorm[0][3][2] = pfNorm[1][0][2] = pfNorm[1][1][2] = pfNorm[1][2][2] = pfNorm[1][3][2] = pfNorm[2][0][2] = pfNorm[2][1][2] = pfNorm[2][2][2] = pfNorm[2][3][2] = -1.0f;
  pfNorm[3][0][2] = pfNorm[3][1][2] = pfNorm[3][2][2] = pfNorm[3][3][2] = pfNorm[4][0][2] = pfNorm[4][1][2] = pfNorm[4][2][2] = pfNorm[4][3][2] = pfNorm[5][0][2] = pfNorm[5][1][2] = pfNorm[5][2][2] = pfNorm[5][3][2] = 1.0f;
  pfNorm += 6;

  // build quads for bottom
  pfQuad[0][0][0] = pfQuad[0][1][0] = fX0;
  pfQuad[0][2][0] = pfQuad[0][3][0] = pfQuad[1][0][0] = pfQuad[1][1][0] = fX1;
  pfQuad[1][2][0] = pfQuad[1][3][0] = pfQuad[2][0][0] = pfQuad[2][1][0] = fX2;
  pfQuad[2][2][0] = pfQuad[2][3][0] = pfQuad[3][0][0] = pfQuad[3][1][0] = fX3;
  pfQuad[3][2][0] = pfQuad[3][3][0] = pfQuad[4][0][0] = pfQuad[4][1][0] = fX4;
  pfQuad[4][2][0] = pfQuad[4][3][0] = fX5;
  pfQuad[1][2][1] = pfQuad[1][3][1] = pfQuad[2][0][1] = pfQuad[2][1][1] = pfQuad[2][2][1] = pfQuad[2][3][1] = pfQuad[3][0][1] = pfQuad[3][1][1] = fY2;
  pfQuad[0][0][1] = pfQuad[0][1][1] = pfQuad[0][2][1] = pfQuad[0][3][1] = pfQuad[1][0][1] = pfQuad[1][1][1] = pfQuad[3][2][1] = pfQuad[3][3][1] = pfQuad[4][0][1] = pfQuad[4][1][1] = pfQuad[4][2][1] = pfQuad[4][3][1] = fY3;
  pfQuad[0][0][2] = pfQuad[0][3][2] = pfQuad[1][0][2] = pfQuad[1][3][2] = pfQuad[2][0][2] = pfQuad[2][3][2] = pfQuad[3][0][2] = pfQuad[3][3][2] = pfQuad[4][0][2] = pfQuad[4][3][2] = fZ0;
  pfQuad[0][1][2] = pfQuad[0][2][2] = pfQuad[1][1][2] = pfQuad[1][2][2] = pfQuad[2][1][2] = pfQuad[2][2][2] = pfQuad[3][1][2] = pfQuad[3][2][2] = pfQuad[4][1][2] = pfQuad[4][2][2] = fZ3;
  pfQuad += 5;
  // build normals for bottom
  pfNorm[0][0][1] = pfNorm[0][1][1] = pfNorm[0][2][1] = pfNorm[0][3][1] = pfNorm[2][0][1] = pfNorm[2][1][1] = pfNorm[2][2][1] = pfNorm[2][3][1] = pfNorm[4][0][1] = pfNorm[4][1][1] = pfNorm[4][2][1] = pfNorm[4][3][1] = 1.0f;
  pfNorm[1][0][0] = pfNorm[1][1][0] = pfNorm[1][2][0] = pfNorm[1][3][0] = 2.0f / sqrt(5.0f);
  pfNorm[1][0][1] = pfNorm[1][1][1] = pfNorm[1][2][1] = pfNorm[1][3][1] = pfNorm[3][0][1] = pfNorm[3][1][1] = pfNorm[3][2][1] = pfNorm[3][3][1] = 1.0f / sqrt(5.0f);
  pfNorm[3][0][0] = pfNorm[3][1][0] = pfNorm[3][2][0] = pfNorm[3][3][0] = -2.0f / sqrt(5.0f);
  pfNorm += 5;

}

CBunker::~CBunker()
{

}

/////////////////////////////////////////////////////////////////////////////
// CBunker accessor functions

void CBunker::Draw(void) const
{
  // Position the model in 3-space
  glTranslatef(m_fPositionX, m_fPositionY, m_fPositionZ);

  GLfloat pfSpecular[4]  = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat pfShininess[1] = {32.0f};
  GLint   piDiffuse[4]   = {m_uchDrawColor[0] << 23, m_uchDrawColor[1] << 23, m_uchDrawColor[2] << 23, m_uchDrawColor[3] << 23};
  GLint   piAmbient[4]   = {m_uchDrawColor[0] << 21, m_uchDrawColor[1] << 21, m_uchDrawColor[2] << 21, m_uchDrawColor[3] << 23};

  // make it white if we are flashing from a hit
  if (m_bFlash)
    {
    piDiffuse[0] = piDiffuse[1] = piDiffuse[2] = piDiffuse[3] = 0x7fffffff;
    piAmbient[0] = piAmbient[1] = piAmbient[2] = piAmbient[3] = 0x7fffffff;
    }

  glMaterialfv(GL_FRONT, GL_SPECULAR, pfSpecular);
  glMaterialfv(GL_FRONT, GL_SHININESS, pfShininess);
  glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
  glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);

  if (CGameMain::m_bAlphaSupported && m_uchDrawColor[3] < 0xff)
    {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

  // draw the bunker
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, m_afQuads);
  glNormalPointer(GL_FLOAT, 0, m_afNormals);
  glDrawArrays(GL_QUADS, 0, BUNKER_QUADS * 4);

  if (CGameMain::m_bAlphaSupported && m_uchDrawColor[3] < 0xff)
    {
    glDisable(GL_BLEND);
    }

  // all done
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  return;
}

void CBunker::GetBounding(float *pfLeftX, float *pfRightX, float *pfTopY, float *pfBotY) const
{
  *pfLeftX = m_fPositionX - (float) m_ciWidth / 2.0f;
  *pfRightX = m_fPositionX + (float) m_ciWidth / 2.0f;
  *pfTopY = m_fPositionY - (float) m_ciHeight / 2.0f;
  *pfBotY = m_fPositionY + (float) m_ciHeight / 2.0f;
}

void CBunker::GetTopSurface(float *pfLeftX, float *pfRightX, float *pfTopY) const
{
  *pfLeftX = m_fPositionX - (float) m_ciWidth / 2.0f + (float) m_ciHeight / 4.0f;
  *pfRightX = m_fPositionX + (float) m_ciWidth / 2.0f - (float) m_ciHeight / 4.0f;
  *pfTopY = m_fPositionY - (float) m_ciHeight / 2.0f;
}


/////////////////////////////////////////////////////////////////////////////
// CBunker modifier functions

void CBunker::SetColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue)
{
  m_uchColor[0] = uchRed;
  m_uchColor[1] = uchGreen;
  m_uchColor[2] = uchBlue;
  m_uchDrawColor[0] = uchRed;
  m_uchDrawColor[1] = uchGreen;
  m_uchDrawColor[2] = uchBlue;
  m_uchDrawColor[3] = 0xff;
}

void CBunker::SetPosition(float fX, float fY, float fZ)
{
  m_fPositionX = fX;
  m_fPositionY = fY;
  m_fPositionZ = fZ;
}

void CBunker::SetFullHealth(int iWaveNumber)
{
  m_iFullHealth = CGameplay::m_iBunkerHealth[iWaveNumber - 1];
  m_iHealth     = m_iFullHealth;
  m_iLastHealth = m_iFullHealth;
  m_uchDrawColor[0] = m_uchColor[0];
  m_uchDrawColor[1] = m_uchColor[1];
  m_uchDrawColor[2] = m_uchColor[2];
  m_uchDrawColor[3] = 0xff; 
}

bool CBunker::TestCollision(float fLeftX, float fRightX, float fTopY, float fBotY, int iDamage)
{
  // generate my coordinates
  float fX0 = m_fPositionX - (float) m_ciWidth / 2.0f;
  float fX1 = fX0 + (float) m_ciHeight / 4.0f;
  float fX3 = m_fPositionX + (float) m_ciWidth / 2.0f;
  float fX2 = fX3 - (float) m_ciHeight / 4.0f;

  float fY0 = m_fPositionY - (float) m_ciHeight / 2.0f;
  float fY1 = fY0 + (float) m_ciHeight / 4.0f;
  float fY3 = m_fPositionY + (float) m_ciHeight / 2.0f;
  float fY2 = fY3 - (float) m_ciHeight / 4.0f;

  // test for a hit on the bottom side
  if (((fRightX > fX0 && fLeftX < fX1) || (fRightX > fX2 && fLeftX < fX3)) && fTopY < fY3 && fBotY > fY1)
    {
    TakeDamage(iDamage);
    return true;
    }
  if (fRightX > fX1 && fLeftX < fX2 && fTopY < fY2 && fBotY > fY1)
    {
    TakeDamage(iDamage);
    return true;
    }

  // test for hit on the top side
  float fX = (fRightX + fLeftX) / 2.0f;
  if (fRightX > fX1 && fLeftX < fX2 && fBotY > fY0 && fTopY < fY2)
    {
    TakeDamage(iDamage);
    return true;
    }
  if (fRightX > fX0 && fLeftX < fX1 && fBotY > fY1 + (fX1 - fX) && fTopY < fY2)
    {
    TakeDamage(iDamage);
    return true;
    }
  if (fRightX > fX2 && fLeftX < fX3 && fBotY > fY1 - (fX3 - fX) && fTopY < fY2)
    {
    TakeDamage(iDamage);
    return true;
    }

  // no collision
  return false;
}

void CBunker::HandleState(void)
{

  // if we previously made a flash, calculate the new (damaged) color
  if (m_bFlash)
    {
    m_bFlash = false;
    // calculate new color
    int iDamage = m_iFullHealth - m_iHealth;
    m_uchDrawColor[0] = (m_uchColor[0] * m_iHealth + 0xd0 * iDamage) / m_iFullHealth;
    m_uchDrawColor[1] = (m_uchColor[1] * m_iHealth + 0x10 * iDamage) / m_iFullHealth;
    m_uchDrawColor[2] = (m_uchColor[2] * m_iHealth + 0x10 * iDamage) / m_iFullHealth;
    m_uchDrawColor[3] = 0xff;
    }

  // make a flash if we just got hit
  if (m_iHealth < m_iLastHealth)
    {
    m_bFlash = 1;
    m_iLastHealth = m_iHealth;
    }

  // if we're fading out, then do it
  if (m_iHealth == 0 && m_iFadeOut > 0)
    {
    m_uchDrawColor[3] = (unsigned char) (m_iFadeOut << 1);
    m_iFadeOut--;
    }

}

/////////////////////////////////////////////////////////////////////////////
// CBunker private functions

void CBunker::TakeDamage(int iDamage)
{
  m_iHealth -= iDamage;
  if (m_iHealth <= 0)
    {
    // we're dead Jim
    m_iHealth = 0;
    m_iFadeOut = 127;
    }
}

