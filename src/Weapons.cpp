/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Weapons.cpp                                              *
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
**                              04/30/05                                   **
**                                                                         **
** Weapons.cpp - interface for virtual class CWeapons - keeps track of and **
**               draws all of the projectiles on the screen                **
**                                                                         **
****************************************************************************/

#ifdef WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include "GameMain.h" // for m_bAlphaSupported flag
#include "Weapons.h"
#include "Textures.h"

// static tables                       laser   gun   cannon  invader
float CWeapons::m_fProjHalfWidth[]  = {3.75f, 3.75f, 10.5f, 2.1875f};
float CWeapons::m_fProjHalfHeight[] = {3.75f, 6.25f, 10.5f, 9.6f};

/////////////////////////////////////////////////////////////////////////////
// CWeapons constructor and destructor

CWeapons::CWeapons()
{
  Clear();

  // grab the OpenGL TextureIDs for the bullet textures
  m_iTextureID[E_PROJ_LASER] = CTextures::GetTextureID(E_TEX_LASER);
  m_iTextureID[E_PROJ_BULLET] = CTextures::GetTextureID(E_TEX_BULLET);
  m_iTextureID[E_PROJ_CANNONBALL] = CTextures::GetTextureID(E_TEX_CANNONBALL);
  m_iTextureID[E_PROJ_INVADER] = CTextures::GetTextureID(E_TEX_INVADER);
}

CWeapons::~CWeapons()
{
}

void CWeapons::Clear(void)
{
  // initialize the pointers into the circular buffer
  m_uiActive  = 0;
  m_uiCurrent = 0;

  // clear the shot counter
  m_uiTotalShots     = 0;
  m_uiLastMissedShot = 0;
}

void CWeapons::ClearEnemy(void)
{
  unsigned int uiTotal = m_uiActive;

  LookAtFirst();
  for (unsigned int ui = 0; ui < uiTotal; ui++)
    {
    if (m_asProjectiles[m_uiCurrent].eProjType >= E_PROJ_INVADER)
      RemoveCurrent();
    else
      GoToNext();
    }
}

/////////////////////////////////////////////////////////////////////////////
// CWeapons accessor functions

void CWeapons::Draw(void) const
{
  unsigned int uiTotal = m_uiActive;

  // just leave if there's nothing to draw
  if (uiTotal == 0) return;

  // set up OpenGL
  glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  if (CGameMain::m_bAlphaSupported)
    glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  for (unsigned int uiIdx = 0; uiIdx < uiTotal; uiIdx++)
    {
    // draw this projectile
    float fX = m_asProjectiles[uiIdx].fPosX;
    float fY = m_asProjectiles[uiIdx].fPosY;
    float fAngle;
    switch (m_asProjectiles[uiIdx].eProjType)
      {
      case E_PROJ_LASER:
        fAngle = (float) (rand() % 360);
        glPushMatrix();
        glTranslatef(fX, fY, -7.0f - (float) uiIdx / (float) uiTotal);
        glRotatef(fAngle, 0.0f, 0.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D, m_iTextureID[E_PROJ_LASER]);
        glColor3ub(255, 255, 255);
	      glBegin(GL_QUADS);
		    glTexCoord2f(0.0f, 0.0f); glVertex3f(-15, -15, 0.0f);
		    glTexCoord2f(0.0f, 1.0f); glVertex3f(-15, +15, 0.0f);
    		glTexCoord2f(1.0f, 1.0f); glVertex3f(+15, +15, 0.0f);
		    glTexCoord2f(1.0f, 0.0f); glVertex3f(+15, -15, 0.0f);
        glEnd();
        glPopMatrix();
        break;
      case E_PROJ_BULLET:
        glPushMatrix();
        glTranslatef(fX, fY, -7.0f - (float) uiIdx / (float) uiTotal);
        glBindTexture(GL_TEXTURE_2D, m_iTextureID[E_PROJ_BULLET]);
        glColor3ub(255, 255, 255);
	      glBegin(GL_QUADS);
		    glTexCoord2f(0.0f, 0.0f); glVertex3f(-5, -10, 0.0f);
		    glTexCoord2f(0.0f, 1.0f); glVertex3f(-5, +10, 0.0f);
    		glTexCoord2f(1.0f, 1.0f); glVertex3f(+5, +10, 0.0f);
		    glTexCoord2f(1.0f, 0.0f); glVertex3f(+5, -10, 0.0f);
        glEnd();
        glPopMatrix();
        break;
      case E_PROJ_CANNONBALL:
        glPushMatrix();
        glTranslatef(fX, fY, -7.0f - (float) uiIdx / (float) uiTotal);
        glBindTexture(GL_TEXTURE_2D, m_iTextureID[E_PROJ_CANNONBALL]);
        glColor3ub(255, 255, 255);
	      glBegin(GL_QUADS);
		    glTexCoord2f(0.0f, 0.0f); glVertex3f(-15, -15, 0.0f);
		    glTexCoord2f(0.0f, 1.0f); glVertex3f(-15, +15, 0.0f);
    		glTexCoord2f(1.0f, 1.0f); glVertex3f(+15, +15, 0.0f);
		    glTexCoord2f(1.0f, 0.0f); glVertex3f(+15, -15, 0.0f);
        glEnd();
        glPopMatrix();
        break;
      case E_PROJ_INVADER:
        glPushMatrix();
        glTranslatef(fX, fY, -7.0f - (float) uiIdx / (float) uiTotal);
        glBindTexture(GL_TEXTURE_2D, m_iTextureID[E_PROJ_INVADER]);
        glColor3ub(255, 255, 255);
	      glBegin(GL_QUADS);
		    glTexCoord2f(0.0f, 0.0f); glVertex3f(-7, -15, 0.0f);
		    glTexCoord2f(0.0f, 1.0f); glVertex3f(-7, +15, 0.0f);
    		glTexCoord2f(1.0f, 1.0f); glVertex3f(+7, +15, 0.0f);
		    glTexCoord2f(1.0f, 0.0f); glVertex3f(+7, -15, 0.0f);
        glEnd();
        glPopMatrix();
        break;
      case E_PROJ_QUAD_LASER:
        break;
      }
    }

  // reset OpenGL
  glEnable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  if (CGameMain::m_bAlphaSupported)
    glDisable(GL_BLEND);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

}

unsigned int CWeapons::Count(void) const
{
  return m_uiActive;
}

unsigned int CWeapons::EnemyShots(void) const
{
  unsigned int uiEnemyShots = 0;

  // count the number of Invader projectiles on the screen
  for (unsigned int ui = 0; ui < m_uiActive; ui++)
    {
    if (m_asProjectiles[ui].eProjType >= E_PROJ_INVADER)
      uiEnemyShots++;
    }

  return uiEnemyShots;
}

unsigned int CWeapons::LowestOutstanding(void) const
{
  unsigned int uiLowest = m_uiTotalShots + 1;

  // find the LaserBase shot with the lowest ShotNumber
  for (unsigned int ui = 0; ui < m_uiActive; ui++)
    {
    if (m_asProjectiles[ui].eProjType < E_PROJ_INVADER && (unsigned int) m_asProjectiles[ui].iShotNumber < uiLowest)
      uiLowest = m_asProjectiles[ui].iShotNumber;
    }

  return uiLowest;
}


/////////////////////////////////////////////////////////////////////////////
// CWeapons modifier functions

bool CWeapons::Add(EProjectile eType, float fPosX, float fPosY, float fVelY)
{
  unsigned int uiAddIdx;

  // if we're all full then leave
  if (m_uiActive == 256) return false;

  // figure out where to put this
  uiAddIdx = m_uiActive;

  // add one to our counter
  m_uiActive++;

  // set up the structure
  m_asProjectiles[uiAddIdx].eProjType   = eType;
  m_asProjectiles[uiAddIdx].fPosX       = fPosX;
  m_asProjectiles[uiAddIdx].fPosY       = fPosY;
  m_asProjectiles[uiAddIdx].fVelY       = fVelY;
  m_asProjectiles[uiAddIdx].iShotNumber = 0;
  m_asProjectiles[uiAddIdx].iHits       = 0;

  // increment shot counter if this is a LaserBase shot
  if (eType < E_PROJ_INVADER)
    {
    m_uiTotalShots++;
    m_asProjectiles[uiAddIdx].iShotNumber = m_uiTotalShots;
    }

  return true;
}

void CWeapons::LookAtFirst(void)
{
  m_uiCurrent = 0;
}

bool CWeapons::GetCurrent(SProjectile *psProjectile)
{
  // copy current projectile into input buffer
  memcpy(psProjectile, &m_asProjectiles[m_uiCurrent], sizeof(SProjectile));

  return true;
}

bool CWeapons::RemoveCurrent(void)
{
  // if this shot is a LaserBase shot and it didn't hit anything, then update the LastMissedShot variable
  if (m_asProjectiles[m_uiCurrent].eProjType < E_PROJ_INVADER && m_asProjectiles[m_uiCurrent].iHits < 1)
    {
    if (m_uiLastMissedShot < (unsigned int) m_asProjectiles[m_uiCurrent].iShotNumber)
      m_uiLastMissedShot = m_asProjectiles[m_uiCurrent].iShotNumber;
    }

  // copy the projectiles ahead of this one backwards
  memcpy(&m_asProjectiles[m_uiCurrent], &m_asProjectiles[m_uiCurrent+1], (m_uiActive - m_uiCurrent - 1) * sizeof(SProjectile));

  // decrement our counter
  m_uiActive--;
  return true;
}

void CWeapons::GoToNext(void)
{
  m_uiCurrent++;
}

void CWeapons::AdvanceAll(unsigned int uiFrameTime)
{
  unsigned int uiTotal = m_uiActive;

  LookAtFirst();
  for (unsigned int ui = 0; ui < uiTotal; ui++)
    {
    // advance this projectile
    m_asProjectiles[m_uiCurrent].fPosY += m_asProjectiles[m_uiCurrent].fVelY * uiFrameTime;
    // if this projectile is off the screen, get rid of it
    if (m_asProjectiles[m_uiCurrent].fPosY < -400.0f || m_asProjectiles[m_uiCurrent].fPosY > 480.0f)
      RemoveCurrent();
    else
      GoToNext();
    }

  return;
}

void CWeapons::CheckCollisions(void)
{
  unsigned int uiGoodShot, uiBadShot;

  // check for collisions between invaders' shots and Laser Base's shots
  if (m_uiActive < 2) return;

  // find first LaserBase shot
  for (uiGoodShot = 0; uiGoodShot < m_uiActive; uiGoodShot++)
    {
    if (m_asProjectiles[uiGoodShot].eProjType < E_PROJ_INVADER) break;
    }
  if (uiGoodShot == m_uiActive) return;

  do
    {
    float fL1 = m_asProjectiles[uiGoodShot].fPosX - m_fProjHalfWidth[m_asProjectiles[uiGoodShot].eProjType];
    float fR1 = m_asProjectiles[uiGoodShot].fPosX + m_fProjHalfWidth[m_asProjectiles[uiGoodShot].eProjType];
    float fT1 = m_asProjectiles[uiGoodShot].fPosY - m_fProjHalfHeight[m_asProjectiles[uiGoodShot].eProjType];
    float fB1 = m_asProjectiles[uiGoodShot].fPosY + m_fProjHalfHeight[m_asProjectiles[uiGoodShot].eProjType];
    // find first Invader shot
    for (uiBadShot = 0; uiBadShot < m_uiActive; uiBadShot++)
      {
      if (m_asProjectiles[uiBadShot].eProjType >= E_PROJ_INVADER) break;
      }
    if (uiBadShot == m_uiActive) return;
    // now test for collisions
    do
      {
      float fL2 = m_asProjectiles[uiBadShot].fPosX - m_fProjHalfWidth[m_asProjectiles[uiBadShot].eProjType];
      float fR2 = m_asProjectiles[uiBadShot].fPosX + m_fProjHalfWidth[m_asProjectiles[uiBadShot].eProjType];
      float fT2 = m_asProjectiles[uiBadShot].fPosY - m_fProjHalfHeight[m_asProjectiles[uiBadShot].eProjType];
      float fB2 = m_asProjectiles[uiBadShot].fPosY + m_fProjHalfHeight[m_asProjectiles[uiBadShot].eProjType];
      // test this pair of projectiles
      if (fL1 < fR2 && fR1 > fL2 && fB1 > fT2 && fT1 < fB2)
        {
        // they collided - if the LaserBase shot is a cannonball, just remove the Invader shot
        if (m_asProjectiles[uiGoodShot].eProjType == E_PROJ_CANNONBALL)
          {
          memcpy(&m_asProjectiles[uiBadShot], &m_asProjectiles[uiBadShot+1], (m_uiActive - uiBadShot - 1) * sizeof(SProjectile));
          m_uiActive--;
          if (uiGoodShot > uiBadShot) uiGoodShot--;
          break;
          }
        // update the LastMissedShot variable
        if ((unsigned int) m_asProjectiles[uiGoodShot].iShotNumber > m_uiLastMissedShot)
          {
          m_uiLastMissedShot = m_asProjectiles[uiGoodShot].iShotNumber;
          }
        // decide the order of destruction
        unsigned int uiDestroyFirst, uiDestroySecond;
        if (uiGoodShot < uiBadShot)
          { uiDestroyFirst = uiBadShot;  uiDestroySecond = uiGoodShot; }
        else
          { uiDestroyFirst = uiGoodShot; uiDestroySecond = uiBadShot; }
        // destroy them
        memcpy(&m_asProjectiles[uiDestroyFirst], &m_asProjectiles[uiDestroyFirst+1], (m_uiActive - uiDestroyFirst - 1) * sizeof(SProjectile));
        m_uiActive--;
        memcpy(&m_asProjectiles[uiDestroySecond], &m_asProjectiles[uiDestroySecond+1], (m_uiActive - uiDestroySecond - 1) * sizeof(SProjectile));
        m_uiActive--;
        // adjust the uiGoodShot index
        if (uiGoodShot == uiDestroyFirst)
          uiGoodShot -= 2;
        else
          uiGoodShot--;
        // now break out of the inner (invader shot) loop
        break;
        }
      // find next Invader shot
      do uiBadShot++; while(uiBadShot < m_uiActive && m_asProjectiles[uiBadShot].eProjType < E_PROJ_INVADER);
      } while (uiBadShot < m_uiActive);
    // find next LaserBase shot
    do uiGoodShot++; while(uiGoodShot < m_uiActive && m_asProjectiles[uiGoodShot].eProjType >= E_PROJ_INVADER);
    } while(uiGoodShot < m_uiActive);

  // all done
  return;
}

/////////////////////////////////////////////////////////////////////////////
// private helper functions

