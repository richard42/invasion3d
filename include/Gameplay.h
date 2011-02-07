/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Gameplay.h                                               *
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


#if !defined(GAMEPLAY_H)
#define GAMEPLAY_H

class CGameplay
{
public:
  CGameplay();
  ~CGameplay();

  // static data tables
  static int   m_iNumStars;
  static float m_fStarSpeed;
  static int   m_iMysteryInterval;
  static float m_fMysterySpeed;
  static float m_fSpeedBoost[8];
  static float m_fShockwaveSpeed;
  static float m_fShockwaveForce;
  static float m_fRecoilForce;
  static float m_fCollideDamage;
  static int   m_iProjDamage[4];
  static float m_fProjSpeed[5];
  static int   m_iReloadTime[5];
  static int   m_iExtraGunAmmo[20];
  static int   m_iExtraCannonAmmo[20];
  static int   m_iInvaderHealth[6][20];
  static int   m_iBunkerHealth[20];
  static int   m_iReplenishTime;
  static int   m_iChanceRefillTop[20];
  static int   m_iChanceRefillInner[20];
  static float m_fBaseMoveSpeedLR;
  static float m_fBaseMoveSpeedUD;
  static int   m_iExtraInvaders[20];
  static int   m_iBonusTime[20];
  static int   m_iCloseShotChance[20];
  static int   m_iFarShotChance[20];
  static int   m_iKamikazeCounter[20];
  static int   m_iKamikazeChance;
  static float m_fKamikazeFlipSpeed;
  static float m_fKamikazeRotSpeed;
  static float m_fKamikazeDashSpeed;
  static int   m_iFirstMoonWave;
  static int   m_iMoonTrigger3Chance;
  static int   m_iMoonTrigger4Chance;
  static int   m_iMoonTrigger5Chance;

};

#endif // !defined(GAMEPLAY_H)
