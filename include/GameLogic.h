/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - GameLogic.h                                              *
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

#if !defined(GAMELOGIC_H)
#define GAMELOGIC_H

#ifdef WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>

#include "Bunker.h"
#include "LaserBase.h"
#include "Settings.h"
#include "Weapons.h"
#include "Ticker.h"
#include "Particles.h"
#include "Errr.h"
#include "Ignignokt.h"

class CGameMain;
class CInvader;
class CInvaderMystery;
class CSound;

typedef enum {E_WAVE_BEGIN = 0,
              E_GAME_PLAY,
              E_PAUSED,
              E_WAVE_END,
              E_GAME_OVER,
              E_HIGH_SCORE,
              E_WON_GAME} EGameState;

class CGameLogic
{
public:
  CGameLogic();
  ~CGameLogic();

  // accessor functions
  void Draw(void) const;

  // modifier functions
  void Initialize(CGameMain *pGameMain, CSettings *pSettings, CSound *pSound);
  void ProcessState(unsigned int uiFrameTime);
  void ProcessEvents(void);

private:
  // member objects
  float          (*m_pfStars)[3];
  float           *m_pfStarSpeed;
  CGameMain       *m_pGameMain;
  CSettings       *m_pSettings;
  CSound          *m_pSound;
  CInvader        *m_pcInvader[48];
  CInvaderMystery *m_pcMystery;
  CInvader        *m_pcKamikaze;
  CErrr            m_Errr;
  CIgnignokt       m_Ignignokt;
  CBunker          m_Bunker[4];
  CLaserBase       m_LaserBase;
  CParticles       m_Particles;
  CWeapons         m_Weapons;
  CTicker          m_Ticker;

  CInvader  *m_pcDeadInvaders[48];
  int        m_iFirstDeadInvader;
  int        m_iNumDeadInvaders;

  // Wave state data
  int        m_iWaveNumber;
  int        m_iWaveStart;
  int        m_iWaveTime;
  int        m_iPauseStart;
  int        m_iEnemiesLeft;

  // Invader state data
  int           m_iLegCount;
  bool          m_bLegPosition;
  float         m_fSwarmOffsetX;
  float         m_fSwarmOffsetY;
  float         m_fSwarmSpeed;
  bool          m_bSwarmMovePositive;
  int           m_iLeftColumnsGone;
  int           m_iRightColumnsGone;
  bool          m_bMysteryActive;
  unsigned int  m_uiMysteryTime;
  float         m_fMysteryAngle;

  // Kamikaze invader state data
  int           m_iKamikazeColumn;
  unsigned int  m_uiKamikazeTime;
  float         m_fKamikazeSpin;
  float         m_fKamikazeX;
  float         m_fKamikazeY;

  // Mooninite state data
  const static unsigned int m_cuiMoonEnterTime;
  const static unsigned int m_cuiMoonExitTime;
  const static unsigned int m_cuiMoonSpinTime;
  static int   m_iMoonLingerTime[7];
  static int   m_iMoonQuoteTime[7];
  bool         m_bMooninitesActive;
  bool         m_bMoonTrigger[7];
  int          m_iMoonQuoteNum;
  unsigned int m_uiMoonTime;
  enum         {E_MOON_ENTER, E_MOON_WAIT, E_MOON_EXIT} m_eMoonMove;
  bool         m_bMoonHit;

  // Player state data
  int          m_iLives;
  int          m_iBonus;
  int          m_iScore;
  int          m_iShieldRemaining;    // amount of shield energy remaining (1-100)
  bool         m_bShieldActive;
  int          m_iHitCounter;         // count of invaders which have been hit by player shots during this wave
  bool         m_bImDead;
  unsigned int m_uiDyingTime;         // value of m_uiStateTime when player was killed
  int          m_iLastWaveKilled;     // the last wave number in which the player was killed
  int          m_iDeathsThisWave;     // the number of times that the player has been killed during this wave
  int          m_iLastInvaderKilled;  // the index (0-47) of the last invader shot by the player
  int          m_iConsecutiveKills;   // number of invaders which have been shot consecutively in one column
  int          m_iAwardedHitBonus;    // last awarded bonus for number of consecutive hits (0, 10, 20, etc)

  // Shockwave state data
  bool       m_bShockwave;
  float      m_fShockwaveX;
  float      m_fShockwaveY;
  float      m_fShockwaveRadius;
  int        m_iShockwaveKills;

  // laser base and input controls
  bool       m_bMoveKeys[4];
  float      m_fLaserPositionX;
  float      m_fLaserPositionY;
  float      m_fLaserMoveX;
  float      m_fLaserMoveY;
  bool       m_bShieldRequested;
  int        m_iAmmo[2];
  EWeapon    m_eCurrentWeapon;
  bool       m_bWeaponSwitching;
  EWeapon    m_eNextWeapon;
  bool       m_bWeaponRetracting;
  int        m_iWeaponExtension;
  int        m_iShotTimer;
  bool       m_bShotRequested;

  // Game Mode State Data
  EGameState    m_eGameState;
  unsigned int  m_uiStateTime;
  unsigned int  m_uiLastTime;
  char          m_chHighScoreName[10];
  bool          m_bQuitConfirmation;

  // private setup functions
  void RearrangeInvaders(void);
  void SetupWave(void);

  // private event handling
  void HandleGameInput(ECommand eCmd, float fValue);

  // private state processing
  void ProcessInvaders(unsigned int uiFrameTime);
  void HandleMystery(unsigned int uiFrameTime);
  void HandleShockwave(unsigned int uiFrameTime);
  void HandleKamikaze(unsigned int uiFrameTime);
  void HandleMooninites(unsigned int uiFrameTime);
  void ReplenishInvaders();

  void ProcessLaserBase(unsigned int uiFrameTime);
  void FireWeapon(void);

  void ProcessCollisions(void);
  void HandleColumns(void);
  void LaserBaseHit(void);
  void ExitGameMode(void);

  void ProcessEndingSequence(void);

  // private drawing functions
  void DrawOSD(void) const;
  void DrawPauseScreen(void) const;
  void DrawWaveBegin(void) const;
  void DrawWaveEnd(void) const;
  void DrawGameOver(void) const;
  void DrawHighScore(void) const;
  void DrawEndingSequence() const;

  // private static data
  static float m_fShockWidth;
};

#endif // defined(GAMELOGIC_H)
