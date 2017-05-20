/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Weapons.h                                                *
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


#if !defined(WEAPONS_H)
#define WEAPONS_H

#ifdef WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>

// mathematical definitions
#if !defined(M_PI)
  #define M_PI   3.14159265358979
  #define M_SQR2 1.414213562373095
#endif

// enumerated type for projectiles
typedef enum {E_PROJ_LASER = 0,
              E_PROJ_BULLET,
              E_PROJ_CANNONBALL,
              E_PROJ_INVADER,      // dividing line between friendly and hostile fire
              E_PROJ_QUAD_LASER,
              E_PROJ_INVALID
             } EProjectile;

// structures which hold projectile information
typedef struct {EProjectile eProjType;
                float       fPosX;
                float       fPosY;
                float       fVelY;
                int         iShotNumber;
                int         iHits;
               } SProjectile;

class CWeapons
{
public:
  CWeapons();
  ~CWeapons();

  // accessor functions
  void Draw(void) const;
  unsigned int Count(void) const;
  unsigned int TotalShots(void) const { return m_uiTotalShots; }
  unsigned int EnemyShots(void) const;
  unsigned int LowestOutstanding(void) const;
  unsigned int LastMissed(void) const { return m_uiLastMissedShot; }

  // modifier functions
  void Clear(void);
  void ClearEnemy(void);
  bool Add(EProjectile eType, float fPosX, float fPosY, float fVelY);
  void LookAtFirst(void);
  bool GetCurrent(SProjectile *psProjectile);
  void IncrementHits(void) { m_asProjectiles[m_uiCurrent].iHits++; }
  bool RemoveCurrent(void);
  void GoToNext(void);
  void AdvanceAll(unsigned int uiFrameTime);
  void CheckCollisions(void);

  // static width and height tables for active area of projectiles (for collision detection)
  static float m_fProjHalfWidth[E_PROJ_INVALID];
  static float m_fProjHalfHeight[E_PROJ_INVALID];

private:
  // private member data
  SProjectile  m_asProjectiles[256];
  unsigned int m_uiActive;
  unsigned int m_uiCurrent;
  unsigned int m_uiTotalShots;
  unsigned int m_uiLastMissedShot;

  // textureID member data
  GLuint       m_iTextureID[E_PROJ_INVALID];

  // helper functions

};

#endif // !defined(WEAPONS_H)
