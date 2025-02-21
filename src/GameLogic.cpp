/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - GameLogic.cpp                                            *
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
**                              04/29/05                                   **
**                                                                         **
** GameLogic.cpp - contains interface for CGameLogic class, used to handle **
**                 everything while the game is running.                   **
**                                                                         **
****************************************************************************/

#ifdef WIN32
  #include <windows.h>
#endif
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "GameMain.h"
#include "GameLogic.h"
#include "Gameplay.h"
#include "GLProfile.h"
#include "Settings.h"
#include "Invader.h"
#include "Invader10.h"
#include "Invader20.h"
#include "Invader30.h"
#include "InvaderMystery.h"
#include "Ignignokt.h"
#include "Errr.h"
#include "Bunker.h"
#include "LaserBase.h"
#include "TextGL.h"
#include "Textures.h"
#include "Ticker.h"
#include "Particles.h"
#include "Weapons.h"
#include "Sound.h"

// static data
int CGameLogic::m_iMoonLingerTime[7] = {6700, 8300, 5000, 4200, 5000, 4200, 11000};
int CGameLogic::m_iMoonQuoteTime[7]  = {1700, 1700, 800,  700,  700,  500,  500};
float CGameLogic::m_fShockWidth = 70.0f;

// const static data
const unsigned int CGameLogic::m_cuiMoonEnterTime = 1000;
const unsigned int CGameLogic::m_cuiMoonExitTime  = 2000;
const unsigned int CGameLogic::m_cuiMoonSpinTime  = 850;

/////////////////////////////////////////////////////////////////////////////
// CGameLogic class constructor and destructor

CGameLogic::CGameLogic()
{
  // create the invaders
  for (int i = 0; i < 48; i++)
    {
    if (i < 16) m_pcInvader[i] = new CInvader30;
    else if (i < 32) m_pcInvader[i] = new CInvader20;
    else m_pcInvader[i] = new CInvader10;
    }
  m_pcMystery = new CInvaderMystery;
  m_pcKamikaze = NULL;

  // allocate memory for stars' positions and speeds
  m_pfStars     = (float (*)[3]) malloc(CGameplay::m_iNumStars * sizeof(float) * 3);
  m_pfStarSpeed = (float *)      malloc(CGameplay::m_iNumStars * sizeof(float));

  // set up the "Dead Invader" table indices
  m_iFirstDeadInvader = 0;
  m_iNumDeadInvaders  = 0;

}

CGameLogic::~CGameLogic()
{
  // delete the invaders
  for (int i = 0; i < 48; i++)
    {
    delete m_pcInvader[i];
    }
  delete m_pcMystery;

  // free the stars' coordinates memory
  free(m_pfStars);
  free(m_pfStarSpeed);
}

/////////////////////////////////////////////////////////////////////////////
// CGameLogic Initialize

void CGameLogic::Initialize(CGameMain *pGameMain, CSettings *pSettings, CSound *pSound)
{
  // set pointer to main class for switching modes
  m_pGameMain = pGameMain;
  m_pSettings = pSettings;
  m_pSound    = pSound;

  // initialize the game state date
  m_iWaveNumber       = 1;
  m_iLives            = 3;
  m_iLastWaveKilled   = 0;
  m_iScore            = 0;
  m_iShieldRemaining  = 1700;        // 1.7 seconds
  m_bShieldActive     = false;
  m_eGameState        = E_WAVE_BEGIN;
  m_uiStateTime       = 0;
  m_uiLastTime        = 0;
  m_bImDead           = false;
  m_bMooninitesActive = false;

  // setup laser base
  m_fLaserPositionX = 0.0f;
  m_fLaserPositionY = 415.0f;
  m_LaserBase.SetPosition(m_fLaserPositionX, m_fLaserPositionY, 0.0f);
  m_LaserBase.SetBarrelExtension(100);
  m_LaserBase.SetBarrelRotation(0);
  m_LaserBase.SetWeapon(E_LASER);

  // setup weapon
  m_iAmmo[0]         = 50;        // machine gun ammo
  m_iAmmo[1]         = 5;         // cannon ammo
  m_eCurrentWeapon   = E_LASER;

  // set the positions of the bunkers
  for (int iX = 0; iX < 4; iX++)
    {
    float fX = (float) iX * 250.0f - 375.0f;
    m_Bunker[iX].SetPosition(fX, 290.0f, 0.0f);
    }

  // set the health of the bunkers so they will appear
  for (int i = 0; i < 4; i++)
    {
    m_Bunker[i].SetFullHealth(1);
    }

  // set random coordinates for the stars
  for (int i = 0; i < CGameplay::m_iNumStars; i++)
    {
    m_pfStars[i][0] = (1200.0f * (float) rand() / RAND_MAX) - 600.0f;
    m_pfStars[i][1] = (960.0f * (float) rand() / RAND_MAX) - 430.0f;
    m_pfStars[i][2] = 150.0f;
    }
  // set random velocities for the stars
  for (int i = 0; i < CGameplay::m_iNumStars; i++)
    {
    m_pfStarSpeed[i] = CGameplay::m_fStarSpeed * (((float) rand() / RAND_MAX) * 1.5f + 0.5f);
    }

  // get other crap off the screen
  m_Weapons.Clear();
  m_Particles.Clear();
  m_Ticker.Clear();
  m_bShockwave = false;
}

void CGameLogic::RearrangeInvaders(void)
{
  // if there is a Kamikaze invader, throw him on the dead pile
  if (m_pcKamikaze)
    {
    m_pcDeadInvaders[(m_iFirstDeadInvader + m_iNumDeadInvaders) % 48] = m_pcKamikaze;
    m_iNumDeadInvaders++;
    m_pcKamikaze = NULL;
    }

  // Put everybody else into the DeadInvaders table
  int iInsertIdx = (m_iFirstDeadInvader + m_iNumDeadInvaders) % 48;
  for (int iInvader = 0; iInvader < 48; iInvader++)
    {
    if (m_pcInvader[iInvader])
      {
      m_pcDeadInvaders[iInsertIdx] = m_pcInvader[iInvader];
      m_pcInvader[iInvader] = NULL;
      iInsertIdx = (iInsertIdx + 1) % 48;
      }
    }

  // now put them all back into the 'alive' matrix in the right order
  unsigned int uiIdx[3];
  uiIdx[E_INVADER_30] = 0;
  uiIdx[E_INVADER_20] = 16;
  uiIdx[E_INVADER_10] = 32;

  for (int iInvader = 0; iInvader < 48; iInvader++)
    {
    // get a pointer to an invader
    CInvader *pInvader1 = m_pcDeadInvaders[iInvader];
    // find the place for this invader
    EInvaderType eType = pInvader1->GetType();
    // put it in it's place and go loop back
    m_pcInvader[uiIdx[eType]] = pInvader1;
    uiIdx[eType]++;
    m_pcDeadInvaders[iInvader] = NULL;
    }

  // set the array information for the dead invader array
  m_iFirstDeadInvader = 0;
  m_iNumDeadInvaders = 0;
}

void CGameLogic::SetupWave(void)
{
  // put the Invaders' pointers in the right places
  RearrangeInvaders();

  // set the positions of the invaders
  for (int iY = 0; iY < 6; iY++)
    {
    for (int iX = 0; iX < 8; iX++)
      {
      float fX = (float) iX * 110.0f - 385.0f;
      float fY = (float) iY * 88.0f  - 260.0f;
      int iInvader = iY * 8 + iX;
      m_pcInvader[iInvader]->SetFullHealth(m_iWaveNumber);
      m_pcInvader[iInvader]->ActivateMotion(false);
      m_pcInvader[iInvader]->SetPosition((float) iX * 300.0f - 1050.0f, fY - 600.0f, 0.0f);
      m_pcInvader[iInvader]->SetDesiredPosition(fX, fY);
      }
    }
  m_iLegCount = 5000;
  m_bLegPosition = false;

  // set the swarm offset to 0
  m_fSwarmOffsetX      = 0.0f;
  m_fSwarmOffsetY      = 0.0f;
  m_fSwarmSpeed        = 1.6f;  // world units per 'step'
  m_bSwarmMovePositive = true;
  m_iLeftColumnsGone   = 0;
  m_iRightColumnsGone  = 0;

  // setup laser base
  m_fLaserMoveX        = 0.0f;
  m_fLaserMoveY        = 0.0f;
  m_LaserBase.SetBarrelRotation(0);
  m_iShieldRemaining   = 1700;
  m_bShieldRequested   = false;
  m_bShieldActive      = false;
  m_iShotTimer         = 0;  // number of milliseconds until you can shoot again
  m_bShotRequested     = false;
  m_iDeathsThisWave    = 0;
  m_iLastInvaderKilled = -1;
  m_iConsecutiveKills  = 0;
  m_iAwardedHitBonus   = 0;
  m_iAmmo[0]           += CGameplay::m_iExtraGunAmmo[m_iWaveNumber - 1];  // machine gun ammo
  m_iAmmo[1]           += CGameplay::m_iExtraCannonAmmo[m_iWaveNumber-1]; // cannon ammo

  // reset weapon to extended position
  m_bWeaponSwitching = false;
  m_LaserBase.SetWeapon(m_eCurrentWeapon);
  m_LaserBase.SetBarrelExtension(100);

  // clear the MoveKey flags
  for (int i = 0; i < 4; i++) m_bMoveKeys[i] = false;

  // set the states of the mystery invader
  m_bMysteryActive  = false;
  m_uiMysteryTime   = 0;

  // get rid of any projectile which might be lying around
  m_Weapons.Clear();

  // get rid of everything in the particle engine
  m_Particles.Clear();

  // disable the shockwave
  m_bShockwave = false;
  m_pGameMain->SetBulletTime(false);

  // reset the trigger flags for the mooninites
  for (int i = 0; i < 7; i++)
    m_bMoonTrigger[i] = false;

  // reset the hit counter
  m_iHitCounter = 0;

  // set the health of the bunkers
  for (int i = 0; i < 4; i++)
    {
    m_Bunker[i].SetFullHealth(m_iWaveNumber);
    }

  // set the number of enemies left in wave
  m_iEnemiesLeft = CGameplay::m_iExtraInvaders[m_iWaveNumber - 1];
}


/////////////////////////////////////////////////////////////////////////////
// CGameLogic accessors

void CGameLogic::Draw(GLProfile &profile) const
{
  // draw the star-field background
  glDisable(GL_LIGHTING);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, m_pfStars);
  glColor3ub(255, 255, 255);
  glPointSize(1.0f);
  glDrawArrays(GL_POINTS, 0, CGameplay::m_iNumStars);
  glDisableClientState(GL_VERTEX_ARRAY);
  glEnable(GL_LIGHTING);
#if defined(GL_PROFILE)
  if (profile.bGLFinishAfterEach)
    glFinish();
  profile.uiTimeStars = SDL_GetTicks();
#endif

  // draw the shockwave
  if (m_bShockwave)
    {
    // set up OpenGL
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    if (CGameMain::m_bAlphaSupported)
      {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      }
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, CTextures::GetTextureID(E_TEX_SHOCKWAVE));
    // set up some values
    float fR1 = m_fShockwaveRadius;
    float fR2 = fR1 * 1.09f + m_fShockWidth;
    unsigned int uiPoints = (32 + (int) (fR1 / 8)) & 0xfffe;  // must have an even number of points
    // draw the shockwave
    glBegin(GL_TRIANGLE_STRIP);
    for (unsigned int ui = 0; ui <= uiPoints + 1; ui++)
      {
      float fTexX = (float) ui / (float) uiPoints;
      float fTexY = (float) (ui & 1);
      glTexCoord2d(fTexX, fTexY);
      float fAngle = 2.0f * (float) M_PI * (float) ui / (float) uiPoints;
      float fRadius;
      if ((ui & 1) == 0)
        fRadius = fR2;
      else
        fRadius = fR1;
      glVertex3f(m_fShockwaveX + fRadius * cos(fAngle), m_fShockwaveY + fRadius * sin(fAngle), 20.0f);
      }
    glEnd();
    // reset the OpenGL state
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    if (CGameMain::m_bAlphaSupported) glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    }
#if defined(GL_PROFILE)
  if (profile.bGLFinishAfterEach)
    glFinish();
  profile.uiTimeShockwave = SDL_GetTicks();
#endif

  // draw all of the invaders
  if (m_eGameState != E_WAVE_BEGIN && m_eGameState != E_WAVE_END && m_eGameState != E_WON_GAME)
    {
    for (int i = 0; i < 48; i++)
      {
      if (m_pcInvader[i])
        {
        glPushMatrix();
        m_pcInvader[i]->Draw();
        glPopMatrix();
        }
      }
    if (m_bMysteryActive)
      {
      glPushMatrix();
      m_pcMystery->Draw();
      glPopMatrix();
      }
    if (m_pcKamikaze)
      {
      glPushMatrix();
      m_pcKamikaze->Draw();
      glPopMatrix();
      }
    if (m_bMooninitesActive)
      {
      glPushMatrix();
      m_Errr.Draw();
      glPopMatrix();
      glPushMatrix();
      m_Ignignokt.Draw();
      glPopMatrix();
      }
    }
#if defined(GL_PROFILE)
  if (profile.bGLFinishAfterEach)
    glFinish();
  profile.uiTimeInvaders = SDL_GetTicks();
#endif

  // draw the bunkers
  for (int i = 0; i < 4; i++)
    {
    if (m_Bunker[i].MustDraw())
      {
      glPushMatrix();
      m_Bunker[i].Draw();
      glPopMatrix();
      }
    }

  // draw the laser base
  if (!m_bImDead)
    {
    glPushMatrix();
    m_LaserBase.Draw();
    glPopMatrix();
    }
#if defined(GL_PROFILE)
  if (profile.bGLFinishAfterEach)
    glFinish();
  profile.uiTimeBases = SDL_GetTicks();
#endif

  // draw On-Screen Display
  DrawOSD();
#if defined(GL_PROFILE)
  if (profile.bGLFinishAfterEach)
    glFinish();
  profile.uiTimeOSD = SDL_GetTicks();
#endif
  // draw the particles
  m_Particles.Draw();
#if defined(GL_PROFILE)
  if (profile.bGLFinishAfterEach)
    glFinish();
  profile.uiTimeParticles = SDL_GetTicks();
#endif
  // draw the bullets
  m_Weapons.Draw();
#if defined(GL_PROFILE)
  if (profile.bGLFinishAfterEach)
    glFinish();
  profile.uiTimeWeapons = SDL_GetTicks();
#endif

  // draw special screens
  switch (m_eGameState)
    {
    case E_WAVE_BEGIN:
      DrawWaveBegin();
      break;
    case E_WAVE_END:
      DrawWaveEnd();
      break;
    case E_PAUSED:
      DrawPauseScreen();
      break;
    case E_GAME_OVER:
      DrawGameOver();
      break;
    case E_HIGH_SCORE:
      DrawHighScore();
      break;
    case E_WON_GAME:
      DrawEndingSequence();
      break;
    default:
      break;
    }
#if defined(GL_PROFILE)
  if (profile.bGLFinishAfterEach)
    glFinish();
  profile.uiTimeSpecial = SDL_GetTicks();
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CGameLogic Event Handling

void CGameLogic::ProcessEvents(void)
{
  SDL_Event event;

  // Grab all the events off the queue.
  while (SDL_PollEvent(&event))
    {
    // generate some values for joystick input
    int iIndex, iJoystick;
    bool bPositive;
    float fValue;
    if (event.type == SDL_JOYBUTTONDOWN || event.type == SDL_JOYBUTTONUP)
      {
      iJoystick = event.jbutton.which;
      iIndex    = event.jbutton.button;
      if (event.type == SDL_JOYBUTTONDOWN)
        fValue = 1.0f;
      else
        fValue = 0.0f;
      }
    else if (event.type == SDL_JOYAXISMOTION)
      {
      iJoystick = event.jaxis.which;
      iIndex    = event.jaxis.axis;
      if (event.jaxis.value > 0)
        {
        bPositive = true;
        if (event.jaxis.value >= 8192)
          fValue = (float) (event.jaxis.value - 8192) / 24575.0f;
        else
          fValue = 0.0f;
        }
      else
        {
        bPositive = false;
        if (event.jaxis.value <= -8192)
          fValue = (float) (event.jaxis.value + 8192) / -24576.0f;
        else
        fValue = 0.0f;
        }
      }
    // handle the events depending on the state of the game
    if (m_eGameState == E_PAUSED)
      {
      if (m_bQuitConfirmation && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN)
        {
        // go back to demo mode
        ExitGameMode();
        }
      else if ((event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) ||
          (event.type == SDL_KEYDOWN && m_pSettings->GetCommand(event.key.keysym.sym) == E_INPUT_PAUSE) ||
          (event.type == SDL_JOYBUTTONDOWN && m_pSettings->GetCommand(iJoystick, false, iIndex, false) == E_INPUT_PAUSE) ||
          (event.type == SDL_JOYAXISMOTION && m_pSettings->GetCommand(iJoystick, true,  iIndex, bPositive) == E_INPUT_PAUSE))
        {
        // switch back to game play mode
        m_eGameState = E_GAME_PLAY;
        m_pSound->Unpause();
        // calculate the amount of time spent in Pause mode and add it to the wave start time
        // so that when we calculate the wave time at the end of the wave, it won't include pause time
        int iPauseTime = SDL_GetTicks() - m_iPauseStart;
        m_iWaveStart += iPauseTime;
        }
      }
    else if (m_eGameState == E_GAME_PLAY)
      {
      // first look for an escape key press
      if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
        {
        // display quit confirmation screen
        m_pSound->Pause();
        m_iPauseStart = SDL_GetTicks();
        m_eGameState = E_PAUSED;
        m_bQuitConfirmation = true;
        }
      // get the command
      ECommand eCmd = E_INPUT_NONE;
      if (event.type == SDL_KEYDOWN)
        {
        eCmd = m_pSettings->GetCommand(event.key.keysym.sym);
        fValue = 1.0f;
        // set the MoveKey flag for this direction and clear the flag for the other direction
        if (eCmd >= E_INPUT_LEFT && eCmd <= E_INPUT_DOWN)
          {
          m_bMoveKeys[(int) eCmd] = true;
          m_bMoveKeys[(int) eCmd ^ 1] = false;
          }
        }
      else if (event.type == SDL_KEYUP)
        {
        eCmd = m_pSettings->GetCommand(event.key.keysym.sym);
        fValue = 0.0f;
        // reset the MoveKey flag for this direction
        if (eCmd >= E_INPUT_LEFT && eCmd <= E_INPUT_DOWN)
          {
           m_bMoveKeys[(int) eCmd] = false;
          // if the MoveKey flag is set for the opposite direction, then don't send a command message
          if (m_bMoveKeys[(int) eCmd ^ 1]) eCmd = E_INPUT_NONE;
          }
        }
      else if (event.type == SDL_JOYBUTTONDOWN || event.type == SDL_JOYBUTTONUP)
        {
        eCmd = m_pSettings->GetCommand(iJoystick, false, iIndex, false);
        }
      else if (event.type == SDL_JOYAXISMOTION)
        {
        eCmd = m_pSettings->GetCommand(iJoystick, true, iIndex, bPositive);
        }
      // handle the command
      if (eCmd != E_INPUT_NONE) HandleGameInput(eCmd, fValue);
      }
    else if (m_eGameState == E_HIGH_SCORE && event.type == SDL_KEYDOWN)
      {
      // check status of shift and caps lock
      bool bShift = (event.key.keysym.mod & KMOD_SHIFT) != 0;
      bool bCaps  = (event.key.keysym.mod & KMOD_CAPS) != 0;
      bool bUpper = (bShift && !bCaps) || (bCaps && !bShift);
      // add character if necessary
      if (strlen(m_chHighScoreName) < 9)
        {
        char chAdd = 0;
        if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z)
          {
          if (bUpper)
            chAdd = 'A' + event.key.keysym.sym - SDLK_a;
          else
            chAdd = 'a' + event.key.keysym.sym - SDLK_a;
          }
        else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9 && !bShift)
          chAdd = '0' + event.key.keysym.sym - SDLK_0;
        else if (event.key.keysym.sym == SDLK_PERIOD && !bShift)
          chAdd = '.';
        // now add the letter
        if (chAdd != 0)
          {
          int iLength = (int) strlen(m_chHighScoreName);
          m_chHighScoreName[iLength] = chAdd;
          m_chHighScoreName[iLength+1] = 0;
          }
        }
      // test for backspace
      if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(m_chHighScoreName) > 0)
        {
        // lop off the last character
        m_chHighScoreName[strlen(m_chHighScoreName) - 1] = 0;
        }
      // test for enter
      if (event.key.keysym.sym == SDLK_RETURN)
        {
        // put in a name if there is none
        if (strlen(m_chHighScoreName) == 0) strcpy(m_chHighScoreName, "Anonymous");
        // add the high score
        m_pSettings->InsertHighScore(m_iScore, m_chHighScoreName);
        m_pSettings->SaveToFile();
        // go back to demo mode
        ExitGameMode();
        }
      }
    } // while (SDL_PollEvent(&event))
}

void CGameLogic::HandleGameInput(ECommand eCmd, float fValue)
{
  // handle input commands
  switch (eCmd)
    {
    case E_INPUT_LEFT:
      m_fLaserMoveX = -fValue * CGameplay::m_fBaseMoveSpeedLR;
      break;
    case E_INPUT_RIGHT:
      m_fLaserMoveX = fValue * CGameplay::m_fBaseMoveSpeedLR;
      break;
    case E_INPUT_UP:
      m_fLaserMoveY = -fValue * CGameplay::m_fBaseMoveSpeedUD;
      break;
    case E_INPUT_DOWN:
      m_fLaserMoveY = fValue * CGameplay::m_fBaseMoveSpeedUD;
      break;
    case E_INPUT_FIRE:
      if (fValue > 0.5f)
        m_bShotRequested = true;
      else
        m_bShotRequested = false;
      break;
    case E_INPUT_SHIELD:
      if (fValue > 0.5f)
        m_bShieldRequested = true;
      else
        m_bShieldRequested = false;
      break;
    case E_INPUT_PAUSE:
      if (fValue == 1.0f)
        {
        m_pSound->Pause();
        m_iPauseStart = SDL_GetTicks();
        m_eGameState = E_PAUSED;
        m_bQuitConfirmation = false;
        }
      break;
    case E_INPUT_WEAPON_UP:
      if (fValue <= 0.5f)
        break;
      else
        {
        EWeapon ePrevWeapon;
        if (m_bWeaponSwitching)
          {
          ePrevWeapon = m_eNextWeapon;
          }
        else 
          {
          ePrevWeapon = m_eCurrentWeapon;
          m_iWeaponExtension = 100;
          }
        // calculate next weapon
        m_eNextWeapon = (EWeapon) (((int) ePrevWeapon + 1) & 3);
        // setup other variables
        m_bWeaponSwitching = true;
        m_bWeaponRetracting = true;
        }
      break;
    case E_INPUT_WEAPON_DOWN:
      if (fValue <= 0.5f)
        break;
      else
        {
        EWeapon ePrevWeapon;
        if (m_bWeaponSwitching)
          {
          ePrevWeapon = m_eNextWeapon;
          }
        else 
          {
          ePrevWeapon = m_eCurrentWeapon;
          m_iWeaponExtension = 100;
          }
        // calculate next weapon
        m_eNextWeapon = (EWeapon) (((int) ePrevWeapon - 1) & 3);
        // setup other variables
        m_bWeaponSwitching = true;
        m_bWeaponRetracting = true;
        }
      break;
    case E_INPUT_WEAPON0:
      if (fValue <= 0.5f || m_eCurrentWeapon == E_LASER) break;
      if (!m_bWeaponSwitching) m_iWeaponExtension = 100;
      m_eNextWeapon = E_LASER;
      m_bWeaponSwitching = true;
      m_bWeaponRetracting = true;
      break;
    case E_INPUT_WEAPON1:
      if (fValue <= 0.5f || m_eCurrentWeapon == E_GUN) break;
      if (!m_bWeaponSwitching) m_iWeaponExtension = 100;
      m_eNextWeapon = E_GUN;
      m_bWeaponSwitching = true;
      m_bWeaponRetracting = true;
      break;
    case E_INPUT_WEAPON2:
      if (fValue <= 0.5f || m_eCurrentWeapon == E_DUAL_GUN) break;
      if (!m_bWeaponSwitching) m_iWeaponExtension = 100;
      m_eNextWeapon = E_DUAL_GUN;
      m_bWeaponSwitching = true;
      m_bWeaponRetracting = true;
      break;
    case E_INPUT_WEAPON3:
      if (fValue <= 0.5f || m_eCurrentWeapon == E_CANNON) break;
      if (!m_bWeaponSwitching) m_iWeaponExtension = 100;
      m_eNextWeapon = E_CANNON;
      m_bWeaponSwitching = true;
      m_bWeaponRetracting = true;
      break;
    default:
      break;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CGameLogic Main State Processing Loop

void CGameLogic::ProcessState(unsigned int uiFrameTime)
{
  // advance the state counter
  if (m_eGameState != E_PAUSED)
    {
    m_uiLastTime = m_uiStateTime;
    m_uiStateTime += uiFrameTime;
    m_Ticker.AdvanceState(uiFrameTime);
    }

  // move the starfield
  for (int i = 0; i < CGameplay::m_iNumStars; i++)
    {
    m_pfStars[i][1] -= m_pfStarSpeed[i] * (float) uiFrameTime;
    if (m_pfStars[i][1] < -400.0f)
      {
      m_pfStars[i][0] = (1200.0f * (float) rand() / RAND_MAX) - 600.0f;
      m_pfStars[i][1] += 930.0f;
      m_pfStarSpeed[i] = CGameplay::m_fStarSpeed * (((float) rand() / RAND_MAX) * 1.5f + 0.5f);
      }
    }

  // handle each state
  switch (m_eGameState)
    {
    case E_WAVE_BEGIN:
      if (m_uiStateTime >= 3000)
        {
        // do all the wave initialization
        SetupWave();
        // set up my state variables
        m_uiLastTime  = 0;
        m_uiStateTime = 1;
        m_eGameState  = E_GAME_PLAY;
        // activate motion for the invaders
        for (int i = 0; i < 48; i++)
          m_pcInvader[i]->ActivateMotion(true);
        // print a message
        m_Ticker.AddMessage("Wave %i Start!", m_iWaveNumber);
        // start the music
        m_pSound->PlaySoundClip(E_SOUND_MUSIC_1);
        // start wave timer
        m_iWaveStart = SDL_GetTicks();
        }
      break;
    case E_WAVE_END:
      // let the weapons and particles continue
      m_Particles.ProcessState(uiFrameTime);
      m_Weapons.AdvanceAll(uiFrameTime);
      // play the extra life sound at the right time
      if (m_uiLastTime < 3000 && m_uiStateTime >= 3000 && m_iHitCounter * 100 / m_Weapons.TotalShots() >= 90)
        {
        m_pSound->PlaySoundClip(E_SOUND_EXTRALIFE);
        }
      // now test for the end of the state
      if (m_uiStateTime >= 10000)
        {
        if (m_iWaveNumber == 20)
          {
          m_eGameState = E_WON_GAME;
          m_uiLastTime = 0;
          m_uiStateTime = 1;
          // start the music
          m_pSound->PlaySoundClip(E_SOUND_MUSIC_2);
          }
        else
          {
          m_eGameState = E_WAVE_BEGIN;
          m_uiLastTime = 0;
          m_uiStateTime = 1;
          m_iWaveNumber++;
          }
        }
      break;
    case E_PAUSED:
      break;
    case E_GAME_PLAY:
      ProcessInvaders(uiFrameTime);
      ProcessLaserBase(uiFrameTime);
      m_Particles.ProcessState(uiFrameTime);
      ProcessCollisions();
      m_Weapons.AdvanceAll(uiFrameTime);
      // call state machine for bunkers
      for (int i = 0; i < 4; i++) m_Bunker[i].HandleState();
      // test for end of state
      if (!m_bImDead && !m_bMooninitesActive && !m_bMysteryActive && !m_bShockwave && m_iEnemiesLeft == 0 && m_iNumDeadInvaders == 48)
        {
        m_iWaveTime   = SDL_GetTicks() - m_iWaveStart;
        m_eGameState  = E_WAVE_END;
        m_uiStateTime = 1;
        m_uiLastTime  = 0;
        // stop the music
        m_pSound->StopSoundClip(E_SOUND_MUSIC_1);
        // disable sounds
        if (m_bMysteryActive)
          m_pSound->StopSoundClip(E_SOUND_MYSTERY);
        if (m_bShieldActive)
          m_pSound->StopSoundClip(E_SOUND_SHIELD);
        // calculate bonus from time left over and hit percentage
        int iExtraTime = CGameplay::m_iBonusTime[m_iWaveNumber - 1] * 1000 - m_iWaveTime;
        if (iExtraTime < 0)
          m_iBonus = 0;
        else
          m_iBonus = iExtraTime * m_iHitCounter / (m_Weapons.TotalShots() * 10);
        m_iScore += m_iBonus;
        if (m_iHitCounter * 100 / m_Weapons.TotalShots() >= 90)
          {
          m_iLives++;
          }
        m_Weapons.ClearEnemy();
        }
      // handle state changes when I'm dead
      if (m_bImDead && m_uiStateTime > m_uiDyingTime + 5000 && m_Weapons.Count() == 0)
        {
        // time to move on
        if (m_iLives == 0)
          {
          m_eGameState = E_GAME_OVER;
          m_uiStateTime = 0;
          m_uiLastTime  = 0;
          // stop the music
          m_pSound->StopSoundClip(E_SOUND_MUSIC_1);
          // turn off mystery sound
          if (m_bMysteryActive)
            m_pSound->StopSoundClip(E_SOUND_MYSTERY);
          // play taps
          m_pSound->PlaySoundClip(E_SOUND_GAMEOVER);
          }
        else
          {
          // resurrect
          m_iLives--;
          m_iShieldRemaining = 1700;
          m_bShieldActive = false;
          m_bImDead = false;
          }
        }
      break;
    case E_GAME_OVER:
      // test for end of state
      if (m_uiStateTime >= 7000)
        {
        // see if we have made a new high score
        if (m_iScore > m_pSettings->GetHighScore(10, NULL))
          {
          m_eGameState  = E_HIGH_SCORE;
          m_uiStateTime = 0;
          m_uiLastTime  = 0;
          m_chHighScoreName[0] = 0;
          }
        else
          {
          // that's the end - go back to demo mode
          ExitGameMode();
          }
        }
      break;
    case E_HIGH_SCORE:
      break;
    case E_WON_GAME:
      ProcessEndingSequence();
      if (m_uiStateTime >= 70 * 1000)
        {
        // stop the music
        m_pSound->StopSoundClip(E_SOUND_MUSIC_2);
        m_pSound->StopSoundClip(E_SOUND_MYSTERY);
        // see if we have made a new high score
        if (m_iScore > m_pSettings->GetHighScore(10, NULL))
          {
          m_eGameState  = E_HIGH_SCORE;
          m_uiStateTime = 0;
          m_uiLastTime  = 0;
          m_chHighScoreName[0] = 0;
          }
        else
          {
          // that's the end - go back to demo mode
          ExitGameMode();
          }
        }
      break;
    }

}

void CGameLogic::ExitGameMode(void)
{
  // if there is a Kamikaze invader, throw him on the dead pile
  if (m_pcKamikaze)
    {
    m_pcDeadInvaders[(m_iFirstDeadInvader + m_iNumDeadInvaders) % 48] = m_pcKamikaze;
    m_iNumDeadInvaders++;
    m_pcKamikaze = NULL;
    }

  // if the Mystery ship is active, disable the sound
  if (m_bMysteryActive)
    m_pSound->StopSoundClip(E_SOUND_MYSTERY);

  // stop the music
  m_pSound->StopSoundClip(E_SOUND_MUSIC_1);

  // un-pause the sound
  m_pSound->Unpause();

  // set mode of game main loop to DEMO
  m_pGameMain->SetMode(E_DEMO);
}

/////////////////////////////////////////////////////////////////////////////
// CGameLogic Invaders State Processing Functions

void CGameLogic::ProcessInvaders(unsigned int uiFrameTime)
{
  // toggle the legs
  m_iLegCount -= uiFrameTime;
  if (m_iLegCount <= 0)
    {
    m_bLegPosition = !m_bLegPosition;
    for (int i = 0; i < 48; i++)
      if (m_pcInvader[i]) m_pcInvader[i]->ToggleLegs(m_bLegPosition);
    m_iLegCount = (rand() & 511) + 800;
    }

  // handle motion and other state changes for each invader
  for (int i = 0; i < 48; i++)
    if (m_pcInvader[i]) m_pcInvader[i]->ProcessState(uiFrameTime);
  // move the swarm
  unsigned int uiSteps = (m_uiStateTime >> 6) - (m_uiLastTime >> 6);
  if (m_uiStateTime >= 2000 && uiSteps > 0)
    {
    float fLeftExtent = -102.4f - 110.0f * m_iLeftColumnsGone;
    float fRightExtent = 102.4f + 110.0f * m_iRightColumnsGone;
    float fY = 0.0f;
    float fX = (m_bSwarmMovePositive ? m_fSwarmSpeed : -m_fSwarmSpeed) * (float) uiSteps;
    m_fSwarmOffsetX += fX;
    if (m_fSwarmOffsetX >= fRightExtent)
      {
      m_bSwarmMovePositive = false;
      fY = 2.0f;
      }
    if (m_fSwarmOffsetX <= fLeftExtent)
      {
      m_bSwarmMovePositive = true;
      fY = 2.0f;
      }
    m_fSwarmOffsetY += fY;
    for (int i = 0; i < 48; i++)
      if (m_pcInvader[i]) m_pcInvader[i]->ChangeDesiredPosition(fX, fY);
    }

  // handle the shockwave
  if (m_bShockwave)
    {
    HandleShockwave(uiFrameTime);
    }

  // handle the 'mystery' invader
  HandleMystery(uiFrameTime);

  // handle the Kamikaze invader
  HandleKamikaze(uiFrameTime);

  // handle the mooninites
  if (m_iWaveNumber >= CGameplay::m_iFirstMoonWave)
    HandleMooninites(uiFrameTime);

  // handle the matrix fill-in
  if (m_iNumDeadInvaders > 0 && (m_uiStateTime / CGameplay::m_iReplenishTime) > (m_uiLastTime / CGameplay::m_iReplenishTime))
    {
    ReplenishInvaders();
    }

  // handle the invaders shooting
  if (!m_bImDead && m_uiStateTime > 2500)
    {
    for (int iColumn = 0; iColumn < 8; iColumn++)
      {
      int iRow;
      for (iRow = 5; iRow >= 0; iRow--)
        {
        if (m_pcInvader[iRow * 8 + iColumn] != NULL) break;
        }
      if (iRow < 0) continue;
      if (!m_pcInvader[iRow * 8 + iColumn]->CanFireShot()) continue;
      // calculate the chance of the invader shooting
      float fX, fY, fZ;
      int iChance;
      m_pcInvader[iRow * 8 + iColumn]->GetPosition(&fX, &fY, &fZ);
      if (m_LaserBase.LinedUp(fX))
        iChance = (CGameplay::m_iCloseShotChance[m_iWaveNumber - 1] << 4) / uiFrameTime;
      else
        iChance = (CGameplay::m_iFarShotChance[m_iWaveNumber - 1] << 4) / uiFrameTime;
      // roll the dice and throw a shot
      if (rand() % iChance == 0)
        {
        // play invader shooting sound clip
        m_pSound->PlaySoundClip(E_SOUND_SHOOT_0);
        // tell invader to shoot
        m_pcInvader[iRow * 8 + iColumn]->FireShot(&m_Weapons);
        // possibly activate mooninites
        if (m_Weapons.EnemyShots() > 6 && (rand() % CGameplay::m_iMoonTrigger4Chance) == 0)
          m_bMoonTrigger[4] = true;
        }
      }
    }

}

void CGameLogic::HandleShockwave(unsigned int uiFrameTime)
{
  // calculate new shockwave radius
  float fNewRadius = m_fShockwaveRadius + CGameplay::m_fShockwaveSpeed * uiFrameTime;

  // first apply force to the invaders
  for (int i = 0; i < 48; i++)
    {
    if (m_pcInvader[i])
      {
      float fX, fY, fZ, fRad;
      m_pcInvader[i]->GetPosition(&fX, &fY, &fZ);
      fX -= m_fShockwaveX;
      fY -= m_fShockwaveY;
      fRad = sqrt(fX * fX + fY * fY);
      if (fRad > m_fShockwaveRadius && fRad <= fNewRadius && !m_pcInvader[i]->GetShockHit())
        {
        float fForce = CGameplay::m_fShockwaveForce * 100000.0f / (fRad * fRad);
        if (fForce > 3.0f) fForce = 3.0f;
        fX *= fForce / fRad;
        fY *= fForce / fRad;
        m_pcInvader[i]->ApplyForce(fX, fY);
        m_pcInvader[i]->SetShockHit(true);
        }
      }
    }
  // next, expand the shockwave
  m_fShockwaveRadius = fNewRadius;
  // finally, disable the shockwave if it's too big
  if (m_fShockwaveRadius > 1500.0f)
    {
    m_bShockwave = false;
    for (int i = 0; i < 48; i++)
      if (m_pcInvader[i]) m_pcInvader[i]->SetShockHit(false);
    // if the shockwave caused a lot of kills, add a bonus
    if (m_iShockwaveKills > 7)
      {
      m_iAmmo[1] += m_iShockwaveKills;
      m_Ticker.AddMessage("Massive carnage +%i cannonballs", m_iShockwaveKills);
      }
    // disable bullet time
    m_pGameMain->SetBulletTime(false);
    }
}

void CGameLogic::HandleMystery(unsigned int uiFrameTime)
{
  m_uiMysteryTime += uiFrameTime;
  if (m_bMysteryActive)
    {
    // rotate mystery invader
    m_fMysteryAngle += 0.06f * (float) uiFrameTime;
    m_pcMystery->SetRotation(m_fMysteryAngle);
    // move to the left
    m_pcMystery->ProcessState(uiFrameTime);
    m_pcMystery->ChangeDesiredPosition(CGameplay::m_fMysterySpeed * (float) uiFrameTime, 0.0f);
    // check to see if we're off the screen
    float fX, fY, fZ;
    m_pcMystery->GetPosition(&fX, &fY, &fZ);
    if (fX < -640.0f)
      {
      // the mystery ship is gone
      m_bMysteryActive = false;
      m_uiMysteryTime  = 0;
      m_pSound->StopSoundClip(E_SOUND_MYSTERY);
      }
    }
  else if (!m_bMysteryActive && m_uiMysteryTime > (unsigned int) CGameplay::m_iMysteryInterval)
    {
    // activate the mystery ship
    m_bMysteryActive = true;
    m_pcMystery->SetFullHealth(m_iWaveNumber);
    m_pcMystery->SetPosition(600.0f, -330.0f, 0.0f);
    m_pcMystery->SetDesiredPosition(600.0f, -330.0f);
    m_pcMystery->SetRotation(0.0f);
    m_pcMystery->SetAxis(0.0f, -1.0f, 0.0f);
    m_pcMystery->ActivateMotion(true);
    m_fMysteryAngle = 0.0f;
    m_pSound->PlaySoundClip(E_SOUND_MYSTERY);
    }
}

void CGameLogic::HandleKamikaze(unsigned int uiFrameTime)
{
  if (m_pcKamikaze)
    {
    // advance the state of this kamikaze invader
    m_uiKamikazeTime += uiFrameTime;
    if (m_uiKamikazeTime < (unsigned int) (180.0f / CGameplay::m_fKamikazeFlipSpeed))
      {
      m_pcKamikaze->SetFlipAngle(CGameplay::m_fKamikazeFlipSpeed * m_uiKamikazeTime);
      }
    else
      {
      // set front flip to 180 degrees
      m_pcKamikaze->SetFlipAngle(180.0f);
      // handle spin
      m_fKamikazeSpin += CGameplay::m_fKamikazeRotSpeed * (float) uiFrameTime;
      if (m_fKamikazeSpin >= 360.0f) m_fKamikazeSpin -= 360.0f;
      m_pcKamikaze->SetSpinAngle(m_fKamikazeSpin);
      // handle down dash
      m_fKamikazeY += CGameplay::m_fKamikazeDashSpeed * (float) uiFrameTime;
      m_pcKamikaze->SetPosition(m_fKamikazeX, m_fKamikazeY, 0.0f);
      // see if we hit the bottom
      if (m_fKamikazeY > 420.0f)
        {
        // we hit the bottom, so blow us up
        m_Particles.AddInvaderPoly(m_pcKamikaze);
        // play explosion sound
        m_pSound->PlaySoundClip(E_SOUND_EXPLODE_0);
        // add this invader to the dead pile and set his pointer to NULL
        m_pcDeadInvaders[(m_iFirstDeadInvader + m_iNumDeadInvaders) % 48] = m_pcKamikaze;
        m_iNumDeadInvaders++;
        m_pcKamikaze = NULL;
        // keep track of columns that have been shot away
        if (m_iEnemiesLeft == 0)
          HandleColumns();
        }
      }
    }
  else
    {
    // decide if an invader will go kamikaze
    if (m_uiStateTime > (unsigned int) CGameplay::m_iKamikazeCounter[m_iWaveNumber - 1] * 1000 && !m_bImDead)
      {
      // loop over all the invaders on the bottom row
      for (unsigned int uiInvader = 40; uiInvader < 48; uiInvader++)
        {
        // skip this invader if he's dead
        if (!m_pcInvader[uiInvader]) continue;
        // skip this invader if he's moving down
        float fX, fY, fZ, fDesiredX, fDesiredY;
        m_pcInvader[uiInvader]->GetPosition(&fX, &fY, &fZ);
        m_pcInvader[uiInvader]->GetDesiredPosition(&fDesiredX, &fDesiredY);
        if (fabs(fDesiredY - fY) > 2.0f) continue;
        // skip this invader if he would hit a bunker
        float fInvT, fInvB, fInvL, fInvR;
        m_pcInvader[uiInvader]->GetBodyBounding(&fInvL, &fInvR, &fInvT, &fInvB);
        unsigned int uiBunker;
        for (uiBunker = 0; uiBunker < 4; uiBunker++)
          {
          if (!m_Bunker[uiBunker].Alive()) continue;
          float fBunkL, fBunkR, fBunkT, fBunkB;
          m_Bunker[uiBunker].GetBounding(&fBunkL, &fBunkR, &fBunkT, &fBunkB);
          if (fInvR > fBunkL && fInvL < fBunkR) break;
          }
        if (uiBunker < 4) continue;
        // skip this invader if he would not hit the laser base
        float fBaseL, fBaseR, fBaseT, fBaseB;
        m_LaserBase.GetBounding(&fBaseL, &fBaseR, &fBaseT, &fBaseB);
        float fDistance = fBaseT - fY - (fY - fInvT);
        int iTime = (int) (180.0f / CGameplay::m_fKamikazeFlipSpeed) + (int) (fDistance / CGameplay::m_fKamikazeDashSpeed);
        fBaseL += m_fLaserMoveX * iTime;
        fBaseR += m_fLaserMoveX * iTime;
        float fTenPct = (fBaseR - fBaseL) / 10.0f;
        if (fInvR < fBaseL + fTenPct || fInvL > fBaseR - fTenPct) continue;
        // toss the dice and see if this invader will go kamikaze or not
        if ((rand() % ((CGameplay::m_iKamikazeChance << 4) / uiFrameTime)) == 0)
          {
          // this invader just went kamikaze
          m_pcKamikaze = m_pcInvader[uiInvader];
          m_pcInvader[uiInvader] = NULL;
          m_pcKamikaze->ActivateMotion(false);    // switch off the control system and go to direct position control
          m_iKamikazeColumn = uiInvader - 40;
          m_uiKamikazeTime  = 0;
          m_fKamikazeSpin   = 0.0f;
          m_fKamikazeX      = fX;
          m_fKamikazeY      = fY;
          // play kamikaze sound
          m_pSound->PlaySoundClip(E_SOUND_KAMIKAZE);
          break;
          }
        } // for (unsigned int uiInvader = 40; uiInvader < 48; uiInvader++)
      }
    } // if (m_pcKamikaze)
}

void CGameLogic::HandleMooninites(unsigned int uiFrameTime)
{
  // first, see if we need to activate the mooninites
  if (!m_bMooninitesActive)
    {
    int iQuote;
    for (iQuote = 0; iQuote < 7; iQuote++)
      if (m_bMoonTrigger[iQuote]) break;
    if (iQuote == 7) return;
    // we need to activate the mooninites
    m_bMooninitesActive = true;
    m_iMoonQuoteNum     = iQuote;
    m_uiMoonTime        = 0;
    m_eMoonMove         = E_MOON_ENTER;
    m_bMoonHit          = false;
    m_Ignignokt.SetAxis(0.0f, -1.0f, 0.0f);
    m_Ignignokt.SetRotation(0.0f);
    m_Errr.SetAxis(0.0f, 1.0f, 0.0f);
    m_Errr.SetRotation(0.0f);
    }

  // start the Moon quote sound clip if it's the right time
  if (m_uiMoonTime               <  (unsigned int) m_iMoonQuoteTime[m_iMoonQuoteNum] &&
      m_uiMoonTime + uiFrameTime >= (unsigned int) m_iMoonQuoteTime[m_iMoonQuoteNum])
    {
    ESound eSound;
    if (m_iMoonQuoteNum == 0) eSound = E_SOUND_MOON_0;
    else if (m_iMoonQuoteNum == 1) eSound = E_SOUND_MOON_1;
    else if (m_iMoonQuoteNum == 3) eSound = E_SOUND_MOON_3;
    else if (m_iMoonQuoteNum == 4) eSound = E_SOUND_MOON_4;
    else if (m_iMoonQuoteNum == 5) eSound = E_SOUND_MOON_5;
    else if (m_iMoonQuoteNum == 6) eSound = E_SOUND_MOON_6;
    m_pSound->PlaySoundClip(eSound);
    }

  // the mooninites are active, so handle the states
  switch (m_eMoonMove)
    {
    case E_MOON_ENTER:
      {
      // set positions
      float fIgnignoktX = -770.0f + 350.0f * m_uiMoonTime / (float) m_cuiMoonEnterTime;
      float fErrrX      =  750.0f - 300.0f * m_uiMoonTime / (float) m_cuiMoonEnterTime;
      m_Ignignokt.SetPosition(fIgnignoktX, -200.0f, -70.0f);
      m_Errr.SetPosition(fErrrX, -174.5f, -50.0f);
      // set legs
      bool bLeft = (m_uiMoonTime & 128) != 0;
      m_Ignignokt.SetLegs(bLeft, !bLeft);
      m_Errr.SetLegs(!bLeft, bLeft);
      // handle state switch
      if (m_uiMoonTime < m_cuiMoonEnterTime && m_uiMoonTime + uiFrameTime >= m_cuiMoonEnterTime)
        {
        m_Ignignokt.SetLegs(false, false);
        m_Errr.SetLegs(false, false);
        m_Ignignokt.SetPosition(-420.0f, -200.0f, -70.0f);
        m_Errr.SetPosition(450.0f, -174.5f, -50.0f);
        m_eMoonMove = E_MOON_WAIT;
        }
      break;
      }
    case E_MOON_WAIT:
      {
      // handle state switch
      unsigned int uiTime = m_cuiMoonEnterTime + m_iMoonLingerTime[m_iMoonQuoteNum];
      if (m_uiMoonTime < uiTime && m_uiMoonTime + uiFrameTime >= uiTime)
        m_eMoonMove = E_MOON_EXIT;
      break;
      }
    case E_MOON_EXIT:
      {
      // set rotation and position
      int iTime = m_uiMoonTime - (m_cuiMoonEnterTime + m_iMoonLingerTime[m_iMoonQuoteNum]);
      if (iTime <= (int) m_cuiMoonSpinTime)
        {
        float fDegrees = 90.0f * iTime / (float) m_cuiMoonSpinTime;
        m_Ignignokt.SetRotation(fDegrees);
        m_Errr.SetRotation(fDegrees);
        }
      else
        {
        int iTimeLeft = m_cuiMoonEnterTime + m_iMoonLingerTime[m_iMoonQuoteNum] + m_cuiMoonExitTime - m_uiMoonTime;
        float fIgnignoktX = -670.0f + 250.0f * iTimeLeft / (float) (m_cuiMoonExitTime - m_cuiMoonSpinTime);
        float fErrrX      =  670.0f - 220.0f * iTimeLeft / (float) (m_cuiMoonExitTime - m_cuiMoonSpinTime);
        m_Ignignokt.SetPosition(fIgnignoktX, -200.0f, -70.0f);
        m_Errr.SetPosition(fErrrX, -174.5f, -50.0f);
        m_Ignignokt.SetRotation(90.0f);
        m_Errr.SetRotation(90.0f);
        }
      // set legs
      bool bLeft = (m_uiMoonTime & 128) != 0;
      m_Ignignokt.SetLegs(bLeft, !bLeft);
      m_Errr.SetLegs(!bLeft, bLeft);
      // handle state switch
      unsigned int uiTime = m_cuiMoonEnterTime + m_iMoonLingerTime[m_iMoonQuoteNum] + m_cuiMoonExitTime;
      if (m_uiMoonTime < uiTime && m_uiMoonTime + uiFrameTime >= uiTime)
        {
        m_bMooninitesActive = false;
        // set all triggers to false
        for (int iQuote = 0; iQuote < 7; iQuote++)
          m_bMoonTrigger[iQuote] = false;
        }
      break;
      }
    }

  // increment time counter
  m_uiMoonTime += uiFrameTime;

}

void CGameLogic::ReplenishInvaders()
{
  // first look for any spaces in the top row to fill
  unsigned int uiInvadersAdded = 0;
  for (unsigned int uiInvader = 0; uiInvader < 8; uiInvader++)
    {
    if (m_iEnemiesLeft > 0 && m_pcInvader[uiInvader] == NULL && (rand() % CGameplay::m_iChanceRefillTop[m_iWaveNumber - 1]) == 0)
      {
      // get the top invader from the 'dead' list
      m_pcInvader[uiInvader] = m_pcDeadInvaders[m_iFirstDeadInvader];
      m_pcDeadInvaders[m_iFirstDeadInvader] = NULL;
      m_iFirstDeadInvader = (m_iFirstDeadInvader + 1) % 48;
      m_iNumDeadInvaders--;
      // set him up with his desired and actual positions
      float fX = (float) uiInvader * 110.0f - 385.0f + m_fSwarmOffsetX;
      float fY = -260.0f + m_fSwarmOffsetY;
      m_pcInvader[uiInvader]->SetDesiredPosition(fX, fY);
      m_pcInvader[uiInvader]->SetPosition(fX, fY - 200.0f, 0.0f);
      m_pcInvader[uiInvader]->ActivateMotion(true);
      m_pcInvader[uiInvader]->SetFullHealth(m_iWaveNumber);
      m_iEnemiesLeft--;
      uiInvadersAdded++;
      }
    }

  // possibly trigger the mooninites
  if (uiInvadersAdded >= 5)
    m_bMoonTrigger[6] = true;

  // then look for empty spots in the lower spaces of the array
  for (int iColumn = 0; iColumn < 8; iColumn++)
    {
    int iDestRow, iSrcRow;
    for (iDestRow = 5; iDestRow > 0; iDestRow--)
      {
      // don't allow an invader to move into a Kamikaze invader's spot until he's dead
      if (m_pcKamikaze && iColumn == m_iKamikazeColumn && iDestRow == 5)
        continue;
      // otherwise if this spot is empty, it shall be the destination
      if (m_pcInvader[iDestRow * 8 + iColumn] == NULL) break;
      }
    if (iDestRow == 0) continue;
    for (iSrcRow = iDestRow - 1; iSrcRow >= 0; iSrcRow--)
      {
      if (m_pcInvader[iSrcRow * 8 + iColumn] != NULL) break;
      }
    if (iSrcRow < 0) continue;
    // limit movement to 2 rows at once
    if (iSrcRow < iDestRow - 2) iDestRow = iSrcRow + 2;
    if ((rand() % CGameplay::m_iChanceRefillInner[m_iWaveNumber - 1]) == 0)
      {
      // move invader forward in its row
      m_pcInvader[iDestRow * 8 + iColumn] = m_pcInvader[iSrcRow * 8 + iColumn];
      m_pcInvader[iSrcRow * 8 + iColumn]  = NULL;
      m_pcInvader[iDestRow * 8 + iColumn]->ChangeDesiredPosition(0.0f, 88.0f * (float) (iDestRow - iSrcRow));
      }
    }

}


/////////////////////////////////////////////////////////////////////////////
// CGameLogic Laser Base State Processing Functions

void CGameLogic::ProcessLaserBase(unsigned int uiFrameTime)
{
  // move the laser base
  if (!m_bImDead)
    {
    m_fLaserPositionX += m_fLaserMoveX * (float) uiFrameTime;
    m_fLaserPositionY += m_fLaserMoveY * (float) uiFrameTime;
    if (m_fLaserPositionX > 480.0f) m_fLaserPositionX = 480.0f;
    else if (m_fLaserPositionX < -480.0f) m_fLaserPositionX = -480.0f;
    if (m_fLaserPositionY > 415.0f) m_fLaserPositionY = 415.0f;
    else if (m_fLaserPositionY < 360.0f) m_fLaserPositionY = 360.0f;
    m_LaserBase.SetPosition(m_fLaserPositionX, m_fLaserPositionY, 0.0f);
    // handle shield (de)activation
    if (!m_bShieldActive && m_bShieldRequested && m_iShieldRemaining > 0)
      {
      m_bShieldActive = true;
      m_LaserBase.SetShield(true);
      m_pSound->PlaySoundClip(E_SOUND_SHIELD);
      }
    else if (m_bShieldActive && (!m_bShieldRequested || m_iShieldRemaining < 1))
      {
      m_bShieldActive = false;
      m_LaserBase.SetShield(false);
      m_pSound->StopSoundClip(E_SOUND_SHIELD);
      }
    // handle shield power
    if (m_bShieldActive)
      {
      unsigned int uiPulse = (m_uiStateTime >> 4) - (m_uiLastTime >> 4);
      while (uiPulse--) m_LaserBase.PulseShield();
      m_iShieldRemaining -= uiFrameTime;
      }
    if (m_iShieldRemaining < 1700)
      {
      // re-charge shield
      m_iShieldRemaining += (m_uiStateTime / 10) - (m_uiLastTime / 10);
      if (m_iShieldRemaining > 1700) m_iShieldRemaining = 1700;
      }
    }

  // handle switching weapons
  if (m_bWeaponSwitching)
    {
    if (m_bWeaponRetracting)
      {
      m_iWeaponExtension -= (m_uiStateTime >> 1) - (m_uiLastTime >> 1);
      if (m_iWeaponExtension <= 0)
        {
        m_iWeaponExtension = 0;
        m_bWeaponRetracting = false;
        m_LaserBase.SetWeapon(m_eNextWeapon);
        m_eCurrentWeapon = m_eNextWeapon;
        m_LaserBase.SetBarrelRotation(0);
        }
      m_LaserBase.SetBarrelExtension(m_iWeaponExtension);
      }
    else
      {
      m_iWeaponExtension += (m_uiStateTime >> 1) - (m_uiLastTime >> 1);
      if (m_iWeaponExtension >= 100)
        {
        m_iWeaponExtension = 100;
        m_bWeaponSwitching = false;
        }
      m_LaserBase.SetBarrelExtension(m_iWeaponExtension);
      }
    }
  // handle shooting
  if (m_iShotTimer > 0) m_iShotTimer -= uiFrameTime;
  if (!m_bImDead && m_bShotRequested && !m_bWeaponSwitching && m_iShotTimer <= 0)
    {
    // check for weapon and ammo
    bool bGotAmmo = true;
    if (m_eCurrentWeapon == E_GUN && m_iAmmo[0] < 1) bGotAmmo = false;
    if (m_eCurrentWeapon == E_DUAL_GUN && m_iAmmo[0] < 2) bGotAmmo = false;
    if (m_eCurrentWeapon == E_CANNON && m_iAmmo[1] < 1) bGotAmmo = false;
    if (bGotAmmo) FireWeapon();
    }

}

void CGameLogic::FireWeapon(void)
{
  // first, take away from our ammo
  if (m_eCurrentWeapon == E_GUN) m_iAmmo[0]--;
  if (m_eCurrentWeapon == E_DUAL_GUN) m_iAmmo[0] -= 2;
  if (m_eCurrentWeapon == E_CANNON) m_iAmmo[1]--;

  // find out where to put projectile
  float fX, fY;
  m_LaserBase.GetGunPosition1(&fX, &fY);

  // decide what kind of projectile to fire and reset the shot counter
  EProjectile eProj;
  ESound      eSoundEffect;
  float       fVelY;
  if (m_eCurrentWeapon == E_LASER)
    {
    eProj        = E_PROJ_LASER;
    eSoundEffect = E_SOUND_SHOOT_1;
    }
  else if (m_eCurrentWeapon == E_GUN)
    {
    eProj        = E_PROJ_BULLET;
    eSoundEffect = E_SOUND_SHOOT_2;
    }
  else if (m_eCurrentWeapon == E_DUAL_GUN)
    {
    eProj        = E_PROJ_BULLET;
    eSoundEffect = E_SOUND_SHOOT_3;
    }
  else if (m_eCurrentWeapon == E_CANNON)
    {
    eProj        = E_PROJ_CANNONBALL;
    eSoundEffect = E_SOUND_SHOOT_4;
    }
  fVelY          = CGameplay::m_fProjSpeed[eProj];
  m_iShotTimer   = CGameplay::m_iReloadTime[eProj];

  // create new projectile(s)
  m_Weapons.Add(eProj, fX, fY, fVelY);
  if (m_eCurrentWeapon == E_DUAL_GUN)
    {
    m_LaserBase.GetGunPosition2(&fX, &fY);
    m_Weapons.Add(eProj, fX, fY, fVelY);
    }

  // rotate the barrel
  if (m_eCurrentWeapon != E_LASER)
    m_LaserBase.RotateBarrel(17);

  // Play sound effect
  m_pSound->PlaySoundClip(eSoundEffect);
}

/////////////////////////////////////////////////////////////////////////////
// CGameLogic Collision Detection Loop

void CGameLogic::ProcessCollisions(void)
{
  bool bInvaderHit = false;

  // loop through all of the projectiles and see if each one hit anything
  m_Weapons.LookAtFirst();
  for (unsigned int ui = 0; ui < m_Weapons.Count(); ui++)
    {
    // get some information about this projectile
    SProjectile sProj;
    m_Weapons.GetCurrent(&sProj);
    float fWidth  = CWeapons::m_fProjHalfWidth[sProj.eProjType];
    float fHeight = CWeapons::m_fProjHalfHeight[sProj.eProjType];
    float fLeftX  = sProj.fPosX - fWidth;
    float fRightX = sProj.fPosX + fWidth;
    float fTopY   = sProj.fPosY - fHeight;
    float fBotY   = sProj.fPosY + fHeight;
    int   iDamage = CGameplay::m_iProjDamage[sProj.eProjType];
    // see if it hit any of the bunkers
    int i;
    for (i = 0; i < 4; i++)
      {
      if (m_Bunker[i].Alive())
        {
        if (m_Bunker[i].TestCollision(fLeftX, fRightX, fTopY, fBotY, iDamage))
          {
          // break out of this loop - there's no point testing the other bunkers because it hit this one
          break;
          }
        }
      }
    if (i < 4)
      {
      // remove this projectile (it hit a bunker)
      m_Weapons.RemoveCurrent();
      // play sound effect
      if (sProj.eProjType == E_PROJ_LASER || sProj.eProjType == E_PROJ_INVADER)
        m_pSound->PlaySoundClip(E_SOUND_HIT_2);
      else
        m_pSound->PlaySoundClip(E_SOUND_HIT_3);
      // go to the next projectile
      continue;
      }
    // see if it hit any of the invaders
    for (i = 0; i < 48; i++)
      {
      if (m_pcInvader[i])
        {
        if (m_pcInvader[i]->TestCollision(fLeftX, fRightX, fTopY, fBotY, iDamage))
          {
          // set flag to show that an invader was hit during this frame
          bInvaderHit = true;
          // increment number of hits for this projectile
          m_Weapons.IncrementHits();
          // increment the hit counter
          m_iHitCounter++;
          // test to see if he's dead
          if (m_pcInvader[i]->Alive())
            {
            // not dead, so hit him with the recoil
            if (sProj.eProjType == E_PROJ_BULLET)
              {
              // apply a force to the invader
              m_pcInvader[i]->ApplyForce(0.0f, -CGameplay::m_fRecoilForce);
              // play bullet impact sound
              m_pSound->PlaySoundClip(E_SOUND_HIT_1);
              }
            else
              {
              // play laser impact sound
              m_pSound->PlaySoundClip(E_SOUND_HIT_0);
              }
            }
          else
            {
            // add score for this kill
            int iScore = ((int) m_pcInvader[i]->GetType() + 1) * 10;
            // extra points for a kill with the laser
            if (sProj.eProjType == E_PROJ_LASER) iScore += (iScore >> 1);
            m_iScore += iScore;
            // decide what kind of explosion to make and give his polygons to the particle engine
            if (sProj.eProjType == E_PROJ_LASER || sProj.eProjType == E_PROJ_INVADER)
              {
              m_Particles.AddInvaderSparkle(m_pcInvader[i]);
              // play sparkle explosion sound
              m_pSound->PlaySoundClip(E_SOUND_EXPLODE_1);
              }
            else
              {
              m_Particles.AddInvaderPoly(m_pcInvader[i]);
              // play poly explosion sound
              m_pSound->PlaySoundClip(E_SOUND_EXPLODE_0);
              }
            // add this invader to the dead pile and set his pointer to NULL
            m_pcDeadInvaders[(m_iFirstDeadInvader + m_iNumDeadInvaders) % 48] = m_pcInvader[i];
            m_iNumDeadInvaders++;
            m_pcInvader[i] = NULL;
            // keep track of consecutive kills and possibly activate mooninites
            if (sProj.eProjType < E_PROJ_INVADER)
              {
              if (m_iLastInvaderKilled == i + 8)
                m_iConsecutiveKills++;
              else
                m_iConsecutiveKills = 1;
              m_iLastInvaderKilled = i;
              if (m_iConsecutiveKills == 6 && (rand() % CGameplay::m_iMoonTrigger5Chance) == 0)
                m_bMoonTrigger[5] = true;
              }
            // keep track of columns that have been shot away
            if (m_iEnemiesLeft == 0)
              HandleColumns();
            // make fun if the invader was hit by own laser
            if (sProj.eProjType == E_PROJ_INVADER)
              m_Ticker.AddMessage("Stupid alien!");
            }
          // break out of this loop - there's no point testing the other invaders because it hit this one
          break;
          }
        }
      }
    if (i < 48)
      {
      // remove this projectile (it hit an invader) unless it's a cannonball
      if (sProj.eProjType == E_PROJ_CANNONBALL)
        {
        if (sProj.iHits == 5)
          {
          // all invaders in a one row shot with this cannonball!
          m_Ticker.AddMessage("Strike! +5 bullets");
          m_iAmmo[0] += 5;
          }
        }
      else
        m_Weapons.RemoveCurrent();
      // go to the next projectile
      continue;
      }
    // see if it hit the mystery ship
    if (m_bMysteryActive && m_pcMystery->TestCollision(fLeftX, fRightX, fTopY, fBotY, iDamage))
      {
      // set flag to show that an invader was hit during this frame
      bInvaderHit = true;
      // increment number of hits for this projectile
      m_Weapons.IncrementHits();
      // increment the hit counter
      m_iHitCounter++;
      // test to see if he's dead
      if (!m_pcMystery->Alive())
        {
        // decide what kind of explosion to make and give his polygons to the particle engine
        if (sProj.eProjType == E_PROJ_LASER)
          m_Particles.AddInvaderSparkle(m_pcMystery);
        else
          m_Particles.AddInvaderPoly(m_pcMystery);
        // stop Mystery sound
        m_pSound->StopSoundClip(E_SOUND_MYSTERY);
        // play mystery explosion sound
        m_pSound->PlaySoundClip(E_SOUND_EXPLODE_3);
        // turn off the Mystery ship
        m_bMysteryActive = false;
        m_uiMysteryTime  = 0;
        // calculate the score
        if (sProj.iShotNumber == 23 || (sProj.iShotNumber > 23 && (sProj.iShotNumber - 23) % 15 == 0))
          m_iScore += 300;
        else
          m_iScore += 100;
        // add ticker message and ammo bonus
        m_iAmmo[0] += 25;
        m_Ticker.AddMessage("UFO bonus +25 bullets!");
        // possibly trigger the mooninites
        if (sProj.eProjType < E_PROJ_INVADER && (rand() & CGameplay::m_iMoonTrigger3Chance) == 0)
          m_bMoonTrigger[3] = true;
        // activate the shockwave
        float fZ;
        m_pcMystery->GetPosition(&m_fShockwaveX, &m_fShockwaveY, &fZ);
        m_bShockwave = true;
        m_fShockwaveRadius = 0.0f;
        m_iShockwaveKills = 0;
        // activate bullet time
        m_pGameMain->SetBulletTime(true);
        }
      // remove this projectile (it hit the mystery ship)
      m_Weapons.RemoveCurrent();
      continue;
      }
    // see if it hit me
    if (!m_bImDead && sProj.eProjType >= E_PROJ_INVADER && m_LaserBase.TestCollision(fLeftX, fRightX, fTopY, fBotY))
      {
      // I got hit
      m_Weapons.RemoveCurrent();
      if (m_bShieldActive)
        {
        // but my shield is on! :)
        continue;
        }
      // I'm dead
      LaserBaseHit();
      continue;
      }
    // see if it hit the Mooninites
    if (m_bMooninitesActive && !m_bMoonHit)
      {
      bool bErrHit = m_Errr.TestCollision(fLeftX, fRightX, fTopY, fBotY, 0);
      bool bIgnHit = m_Ignignokt.TestCollision(fLeftX, fRightX, fTopY, fBotY, 0);
      if (bErrHit || bIgnHit)
        {
        // set flag to show that an invader was hit during this frame
        bInvaderHit = true;
        // increment number of hits for this projectile
        m_Weapons.IncrementHits();
        // The Mooninite got hit
        m_Weapons.RemoveCurrent();
        // play mooninite quote
        if (bIgnHit)
          m_pSound->PlaySoundClip(E_SOUND_MOON_2A);
        else
          m_pSound->PlaySoundClip(E_SOUND_MOON_2B);
        // add ticker message and ammo bonus
        m_iAmmo[1] += 10;
        m_Ticker.AddMessage("Mooninite +10 cannonballs");
        // set MoonHit flag so they can't get hit more than once
        m_bMoonHit = true;
        continue;
        }
      }
    // it didn't hit anything so advance to the next projectile
    m_Weapons.GoToNext();
    }  // for (unsigned int ui = 0; ui < m_Weapons.Count(); ui++)

  // test for collision between kamikaze invader and laser base
  if (m_pcKamikaze && !m_bImDead)
    {
    float fL1, fR1, fT1, fB1;
    m_LaserBase.GetBounding(&fL1, &fR1, &fT1, &fB1);
    if (m_pcKamikaze->KamikazeCollision(fL1, fR1, fT1, fB1))
      {
      // add score for this kill
      m_iScore += ((int) m_pcKamikaze->GetType() + 1) * 10;
      // give kamikaze polygons to the particle engine
      m_Particles.AddInvaderPoly(m_pcKamikaze);
      // add this invader to the dead pile and set his pointer to NULL
      m_pcDeadInvaders[(m_iFirstDeadInvader + m_iNumDeadInvaders) % 48] = m_pcKamikaze;
      m_iNumDeadInvaders++;
      m_pcKamikaze = NULL;
      // I'm dead
      LaserBaseHit();
      }
    }

  // check for collisions between bullets
  // (this has been disabled to increase difficulty)
  //m_Weapons.CheckCollisions();

  // check for collisions between invaders
  if (m_uiStateTime > 2000)
    {
    for (unsigned int uiInvader1 = 0; uiInvader1 < 47; uiInvader1++)
      {
      float fL1, fR1, fT1, fB1;
      // skip this invader if he's invalid
      if (!m_pcInvader[uiInvader1]) continue;
      // get bounding box
      m_pcInvader[uiInvader1]->GetBodyBounding(&fL1, &fR1, &fT1, &fB1);
      // an inner loop over the remaining invaders
      for (unsigned int uiInvader2 = uiInvader1 + 1; uiInvader2 < 48; uiInvader2++)
        {
        float fL2, fR2, fT2, fB2;
        // skip this invader if he's invalid
        if (!m_pcInvader[uiInvader2]) continue;
        // get bounding box
        m_pcInvader[uiInvader2]->GetBodyBounding(&fL2, &fR2, &fT2, &fB2);
        // do a quick test for overlapping bounding boxes
        if (fR2 < fL1 || fL2 > fR1 || fB2 < fT1 || fT2 > fB1)
          continue;
        // test for collision
        if (m_pcInvader[uiInvader1]->TestCollision(m_pcInvader[uiInvader2]))
          {
          // play invader impact sound
          m_pSound->PlaySoundClip(E_SOUND_HIT_4);
          // they hit each other - test for death of first invader
          if (!m_pcInvader[uiInvader1]->Alive())
            {
            // add to player's score
            m_iScore += ((int) m_pcInvader[uiInvader1]->GetType() + 1) * 10;
            // give his polygons to the particle engine
            m_Particles.AddInvaderPoly(m_pcInvader[uiInvader1]);
            // add this invader to the dead pile and set his pointer to NULL
            m_pcDeadInvaders[(m_iFirstDeadInvader + m_iNumDeadInvaders) % 48] = m_pcInvader[uiInvader1];
            m_iNumDeadInvaders++;
            m_pcInvader[uiInvader1] = NULL;
            // increment number of shockwave kills
            m_iShockwaveKills++;
            // keep track of columns that have been shot away
            if (m_iEnemiesLeft == 0)
              HandleColumns();
            }
          // test for death of second invader
          if (!m_pcInvader[uiInvader2]->Alive())
            {
            // add to player's score
            m_iScore += ((int) m_pcInvader[uiInvader2]->GetType() + 1) * 10;
            // give his polygons to the particle engine
            m_Particles.AddInvaderPoly(m_pcInvader[uiInvader2]);
            // add this invader to the dead pile and set his pointer to NULL
            m_pcDeadInvaders[(m_iFirstDeadInvader + m_iNumDeadInvaders) % 48] = m_pcInvader[uiInvader2];
            m_iNumDeadInvaders++;
            m_pcInvader[uiInvader2] = NULL;
            // increment number of shockwave kills
            m_iShockwaveKills++;
            // keep track of columns that have been shot away
            if (m_iEnemiesLeft == 0)
              HandleColumns();
            }
          // break out of inner loop
          break;
          }
        } // for (unsigned int uiInvader2 = uiInvader1 + 1; uiInvader2 < 48; uiInvader2++)
      }   // for (unsigned int uiInvader1 = 0; uiInvader1 < 47; uiInvader1++)
    }     // if (m_iStateCounter > 200)

  // test for collisions between invader and bunker
  if (m_uiStateTime > 2000)
    {
    for (int iBunk = 0; iBunk < 4; iBunk++)
      {
      if (!m_Bunker[iBunk].Alive())
        continue;
      // get bounding box
      float fL1, fR1, fT1;
      m_Bunker[iBunk].GetTopSurface(&fL1, &fR1, &fT1);
      for (unsigned int uiInvader = 0; uiInvader < 48; uiInvader++)
        {
        // skip this invader if he's invalid
        if (!m_pcInvader[uiInvader]) continue;
        // test for collision
        if (m_pcInvader[uiInvader]->BunkerCollision(fL1, fR1, fT1))
          {
          // play invader impact sound
          m_pSound->PlaySoundClip(E_SOUND_HIT_4);
          // play bunker impact sound
          m_pSound->PlaySoundClip(E_SOUND_HIT_3);
          // test for death of invader
          if (!m_pcInvader[uiInvader]->Alive())
            {
            // add to player's score
            m_iScore += ((int) m_pcInvader[uiInvader]->GetType() + 1) * 10;
            // give his polygons to the particle engine
            m_Particles.AddInvaderPoly(m_pcInvader[uiInvader]);
            // add this invader to the dead pile and set his pointer to NULL
            m_pcDeadInvaders[(m_iFirstDeadInvader + m_iNumDeadInvaders) % 48] = m_pcInvader[uiInvader];
            m_iNumDeadInvaders++;
            m_pcInvader[uiInvader] = NULL;
            // increment number of shockwave kills
            m_iShockwaveKills++;
            // keep track of columns that have been shot away
            if (m_iEnemiesLeft == 0)
              HandleColumns();
            }
          }
        }
      }
    }

  // finally, check for a bonus from consecutive hits
  if (bInvaderHit)
    {
    int iConsecutiveHits = m_Weapons.LowestOutstanding() - m_Weapons.LastMissed();
    if (iConsecutiveHits < m_iAwardedHitBonus)
      {
      m_iAwardedHitBonus = 0;
      }
    else if (iConsecutiveHits > m_iAwardedHitBonus + 10)
      {
      m_iAwardedHitBonus += 10;
      m_iAmmo[0] += m_iAwardedHitBonus;
      m_Ticker.AddMessage("Bonus +%i Bullets", m_iAwardedHitBonus);
      }
    }

}

void CGameLogic::HandleColumns(void)
{
  // find out how many columns are missing on the left
  unsigned int uiColumn;
  for (uiColumn = 0; uiColumn < 8; uiColumn++)
    {
    if (m_pcInvader[uiColumn] != NULL || 
        m_pcInvader[uiColumn+8] != NULL || 
        m_pcInvader[uiColumn+16] != NULL || 
        m_pcInvader[uiColumn+24] != NULL || 
        m_pcInvader[uiColumn+32] != NULL || 
        m_pcInvader[uiColumn+40] != NULL)
      break;
    }
  m_iLeftColumnsGone = uiColumn;

  // find out how many columns are missing on the right
  for (uiColumn = 7; uiColumn > 0; uiColumn--)
    {
    if (m_pcInvader[uiColumn] != NULL || 
        m_pcInvader[uiColumn+8] != NULL || 
        m_pcInvader[uiColumn+16] != NULL || 
        m_pcInvader[uiColumn+24] != NULL || 
        m_pcInvader[uiColumn+32] != NULL || 
        m_pcInvader[uiColumn+40] != NULL)
      break;
    }
  m_iRightColumnsGone = 7 - uiColumn;

  // set the speed of the invaders according to missing columns
  if (m_iLeftColumnsGone + m_iRightColumnsGone < 8)
    {
    m_fSwarmSpeed = 1.6f + CGameplay::m_fSpeedBoost[m_iLeftColumnsGone + m_iRightColumnsGone];
    }
}

void CGameLogic::LaserBaseHit(void)
{
  // add laser base polygons/coordinates to particle engine
  m_Particles.AddLaserBase(&m_LaserBase);
  // set 'dead' flag to true
  m_bImDead = true;
  // set time of death, so I know when to resurrect
  m_uiDyingTime = m_uiStateTime;
  // disable shield
  if (m_bShieldActive)
    {
    m_LaserBase.SetShield(false);
    m_bShieldActive = false;
    m_pSound->StopSoundClip(E_SOUND_SHIELD);
    }
  // play invader explosion sound
  m_pSound->PlaySoundClip(E_SOUND_EXPLODE_2);

  // handle the state variables and event triggers for the mooninites
  m_iDeathsThisWave++;
  if (m_iDeathsThisWave == 3)
    m_bMoonTrigger[1] = true;
  if (m_iWaveNumber - m_iLastWaveKilled >= 3)
    m_bMoonTrigger[0] = true;
  m_iLastWaveKilled = m_iWaveNumber;
}

/////////////////////////////////////////////////////////////////////////////
// CGameLogic private drawing functions

void CGameLogic::DrawPauseScreen(void) const
{
  CTextGL cText;

  // turn off lighting and draw a simple dark grey square
  glDisable(GL_LIGHTING);
  if (CGameMain::m_bAlphaSupported)
    glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glBegin(GL_QUADS);
  glColor4f(0.1f, 0.1f, 0.1f, 0.7f);
  glVertex3f(-450.0f, -300.0f, -55.0f);
  glVertex3f(-450.0f,  400.0f, -55.0f);
  glVertex3f( 450.0f,  400.0f, -55.0f);
  glVertex3f( 450.0f, -300.0f, -55.0f);
  glEnd();
  if (CGameMain::m_bAlphaSupported)
    glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);

  // format and draw the help text
  cText.SetAlpha(true);
  cText.SetColor(255, 255, 255);
  cText.SetFont(E_FONT_CREDIT_RIVER);
  cText.SetHeight(62.0f);
  cText.SetJustification(E_CENTER);
  cText.SetPosition(0.0f, -100.0f, -60.0f);
  if (m_bQuitConfirmation)
    cText.SetText("Quit game?\n\nPress (Enter) to Quit\n\nPress (Escape) to resume");
  else
    cText.SetText("--Paused--\n\n\n\nPress Escape to resume");
  cText.Draw();
}

void CGameLogic::DrawWaveBegin(void) const
{
  CTextGL cText;

  cText.SetAlpha(true);
  cText.SetColor(255, 255, 255);
  cText.SetFont(E_FONT_EUPHORIGENIC);
  cText.SetHeight(100.0f);
  cText.SetJustification(E_CENTER);
  cText.SetPosition(0.0f, 0.0f, 0.0f);
  char chText[32];
  sprintf(chText, "Wave %i", m_iWaveNumber);
  cText.SetText(chText);
  float fAngle;
  if (m_uiStateTime <= 1000) fAngle = -90.0f + (float) m_uiStateTime * 0.09f;
  else if (m_uiStateTime > 2000) fAngle = 0.0f - (float) (m_uiStateTime - 2000) * 0.09f;
  else fAngle = 0.0f;
  glRotatef(fAngle, 1.0f, 0.0f, 0.0f);
  glTranslatef(0.0f, -50.0f, 0.0f);
  cText.Draw();
}

void CGameLogic::DrawWaveEnd(void) const
{
  CTextGL cText;

  cText.SetAlpha(true);
  cText.SetColor(255, 255, 255);
  cText.SetFont(E_FONT_EUPHORIGENIC);
  cText.SetJustification(E_CENTER);
  if (m_uiStateTime < 1000)
    cText.SetHeight(m_uiStateTime * 0.1f);
  else if (m_uiStateTime > 9000)
    cText.SetHeight((10000 - m_uiStateTime) * 0.1f);
  else
    cText.SetHeight(100.0f);
  cText.SetPosition(0.0f, -160.0f, -10.0f);
  char chText[64];
  sprintf(chText, "Wave  %i  Completed!", m_iWaveNumber);
  cText.SetText(chText);
  cText.Draw();

  // draw stats
  cText.SetFont(E_FONT_CREDIT_RIVER);
  cText.SetHeight(60.0f);
  float fOffsetX;
  if (m_uiStateTime > 1000)
    {
    if (m_uiStateTime < 3000)
      fOffsetX = 960.0f - (m_uiStateTime - 1000) * 0.48f;
    else if (m_uiStateTime > 8000)
      fOffsetX = -960.0f + (10000 - m_uiStateTime) * 0.48f;
    else
      fOffsetX = 0.0f;
    int iMinutes = m_iWaveTime / 60000;
    float fSeconds = (float) (m_iWaveTime - iMinutes * 60000) / 1000.0f;
    int iSeconds = (int) fSeconds;
    int iTenths  = (int) ((fSeconds - (float) iSeconds) * 10.0f);
    sprintf(chText, "Time: %i:%02i.%i\n\nBonus: %i", iMinutes, iSeconds, iTenths, m_iBonus);
    cText.SetText(chText);
    cText.SetPosition(fOffsetX, -30.0f, -10.0f);
    cText.Draw();
    int iHitRate = m_iHitCounter * 100 / m_Weapons.TotalShots();
    if (iHitRate >= 90)
      sprintf(chText, "Hit Rate: %02i%%\n\nExtra Life!", iHitRate);
    else
      sprintf(chText, "Hit Rate: %02i%%", iHitRate);
    cText.SetText(chText);
    cText.SetPosition(-fOffsetX, 36.0f, -10.0f);
    cText.Draw();
    }

}

void CGameLogic::DrawOSD(void) const
{
  char chText[64];
  CTextGL cText;

  // setup text
  cText.SetColor(255, 255, 255);
  cText.SetFont(E_FONT_6809);
  cText.SetHeight(24.0f);

  // draw score
  strcpy(chText, "Score: ");
  char *pch = chText + 7;
  char chNumber[32];
  sprintf(chNumber, "%i", m_iScore);
  int iNumLength = (int) strlen(chNumber);
  for (int i = 0; i < iNumLength; i++) // put commas into score
    {
    *pch++ = chNumber[i];
    if (i != iNumLength - 1 && (i % 3) == ((iNumLength - 1) % 3)) *pch++ = ',';
    }
  *pch = 0;
  cText.SetJustification(E_RIGHT);
  cText.SetPosition(570.0f, -380.0f, 0.0f);
  cText.SetText(chText);
  cText.Draw();

  // draw remaining lives
  sprintf(chText, "Lives: %i", m_iLives);
  cText.SetJustification(E_LEFT);
  cText.SetPosition(-570.0f, -380.0f, 0.0f);
  cText.SetText(chText);
  cText.Draw();

  // draw ammo remaining
  switch (m_eCurrentWeapon)
    {
    case E_LASER:
      strcpy(chText, "Ammo: ---");
      break;
    case E_GUN:
    case E_DUAL_GUN:
      sprintf(chText, "Ammo: %i", m_iAmmo[0]);
      break;
    case E_CANNON:
      sprintf(chText, "Ammo: %i", m_iAmmo[1]);
      break;
    default:
      strcpy(chText, "Ammo: NONE");
      break;
    }
  cText.SetPosition(-400.0f, -380.0f, 0.0f);
  cText.SetText(chText);
  cText.Draw();

  // for debug mode, display FPS
#if defined(_DEBUG)
  sprintf(chText, "%i", (int) m_pGameMain->GetFPS());
  cText.SetPosition(160.0f, -380.0f, 0.0f);
  cText.SetText(chText);
  cText.Draw();
#else
  sprintf(chText, "Wave %i", m_iWaveNumber);
  cText.SetPosition(160.0f, -380.0f, 0.0f);
  cText.SetText(chText);
  cText.Draw();
#endif

  // draw shield remaining
  strcpy(chText, "Shield");
  cText.SetPosition(-230.0f, -380.0f, 0.0f);
  cText.SetText(chText);
  cText.Draw();
  float fX = -127.0f + 2.74f * (float) m_iShieldRemaining / 17.0f;
  glDisable(GL_LIGHTING);
  glBegin(GL_QUADS);
  glColor4f(0.8f, 0.8f, 0.8f, 1.0f);
  glVertex3f( -130.0f, -380.0f, 1.0f);
  glVertex3f( -130.0f, -359.0f, 1.0f);
  glVertex3f(  150.0f, -359.0f, 1.0f);
  glVertex3f(  150.0f, -380.0f, 1.0f);
  glColor4f(0.2f, 0.2f, 0.8f, 1.0f);
  glVertex3f( -127.0f, -377.0f, -1.0f);
  glVertex3f( -127.0f, -362.0f, -1.0f);
  glVertex3f(     fX, -362.0f, -1.0f);
  glVertex3f(     fX, -377.0f, -1.0f);
  glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
  glVertex3f(     fX, -377.0f, -1.0f);
  glVertex3f(     fX, -362.0f, -1.0f);
  glVertex3f( 147.0f, -362.0f, -1.0f);
  glVertex3f( 147.0f, -377.0f, -1.0f);
  glEnd();
  glEnable(GL_LIGHTING);

  // draw ticker
  m_Ticker.Draw();
}

void CGameLogic::DrawGameOver(void) const
{
  CTextGL cText;

  cText.SetAlpha(true);
  cText.SetColor(255, 255, 255);
  cText.SetFont(E_FONT_EUPHORIGENIC);
  cText.SetHeight(100.0f);

  // this needs to be in front
  glDisable(GL_DEPTH_TEST);

  // handle translation and rotation
  if (m_uiStateTime > 6000)
    {
    glRotatef((m_uiStateTime - 6000) * 0.09f, -1.0f, 0.0f, 0.0f);
    }
  glTranslatef(0.0f, -50.0f, 0.0f);

  // draw "Game"
  if (m_uiStateTime < 2000)
    cText.SetPosition((2000 - m_uiStateTime) * -0.3f, 0.0f, 0.0f);
  else
    cText.SetPosition(-10.0f, 0.0f, 0.0f);
  cText.SetJustification(E_RIGHT);
  cText.SetText("Game");
  cText.Draw();

  // draw "Over"
  if (m_uiStateTime < 2000)
    cText.SetPosition((2000 - m_uiStateTime) * 0.3f, 0.0f, 0.0f);
  else
    cText.SetPosition(10.0f, 0.0f, 0.0f);
  cText.SetJustification(E_LEFT);
  cText.SetText("Over");
  cText.Draw();

  // reset OpenGL state
  glEnable(GL_DEPTH_TEST);
}

void CGameLogic::DrawHighScore(void) const
{
  CTextGL cText;

  // handle sliding
  float fOffsetY = 0.0f;
  if (m_uiStateTime < 2000)
    {
    fOffsetY = (2000 - m_uiStateTime) * -0.24f;
    }

  // turn off lighting and draw a simple dark grey square
  glDisable(GL_LIGHTING);
  if (CGameMain::m_bAlphaSupported)
    glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glBegin(GL_QUADS);
  glColor4f(0.1f, 0.1f, 0.1f, 0.7f);
  glVertex3f(-280.0f, -125.0f + fOffsetY, -150.0f);
  glVertex3f(-280.0f,  125.0f + fOffsetY, -150.0f);
  glVertex3f( 280.0f,  125.0f + fOffsetY, -150.0f);
  glVertex3f( 280.0f, -125.0f + fOffsetY, -150.0f);
  glEnd();
  if (CGameMain::m_bAlphaSupported)
   glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);

  // format and draw header
  cText.SetAlpha(true);
  cText.SetColor(255, 255, 255);
  cText.SetFont(E_FONT_6809);
  cText.SetHeight(50.0f);
  cText.SetJustification(E_CENTER);
  cText.SetPosition(0.0f, -100.0f + fOffsetY, -160.0f);
  cText.SetText("New High Score!\nEnter Name:");
  cText.Draw();

  // draw the entered name
  cText.SetFont(E_FONT_CREDIT_RIVER);
  cText.SetJustification(E_CENTER);
  cText.SetPosition(0.0f, 50.0f + fOffsetY, -160.0f);
  cText.SetText(m_chHighScoreName);
  cText.Draw();

  // draw the cursor
  if (m_uiStateTime & 512)
    {
    float fCursorX = cText.GetLineWidth(m_chHighScoreName, (int) strlen(m_chHighScoreName)) / 2.0f + 5.0f;
    glDisable(GL_LIGHTING);
    if (CGameMain::m_bAlphaSupported)
      glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glColor4f(0.8f, 0.8f, 0.1f, 0.7f);
    glVertex3f(fCursorX +  0.0f,  50.0f + fOffsetY, -160.0f);
    glVertex3f(fCursorX +  0.0f, 100.0f + fOffsetY, -160.0f);
    glVertex3f(fCursorX + 20.0f, 100.0f + fOffsetY, -160.0f);
    glVertex3f(fCursorX + 20.0f,  50.0f + fOffsetY, -160.0f);
    glEnd();
    if (CGameMain::m_bAlphaSupported)
      glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    }

}

void CGameLogic::DrawEndingSequence() const
{
  CTextGL      cText;
  char         chText[256];
  unsigned int uiTime = m_uiStateTime;

  cText.SetAlpha(true);
  cText.SetJustification(E_CENTER);

  // do congratulations message
  if (uiTime < 10000)
    {
    float fOffsetY = 0.0f;
    if (uiTime < 1500)
      fOffsetY = 900.0f - (float) uiTime * 0.6f;
    else if (uiTime > 8500)
      fOffsetY = 0.0f - (uiTime - 8500) * 0.6f;
    // print message
    cText.SetColor(255, 255, 255);
    cText.SetFont(E_FONT_6809);
    cText.SetHeight(40.0f);
    cText.SetPosition(0.0f, fOffsetY - 140.0f, -10.0f);
    sprintf(chText, "CONGRATULATIONS!\n\nYou have defeated the entire\nInvader army!  Your fearlessness\nand quick reactions have saved\nthe whole world!  You will be\nrewarded handsomely!");
    cText.SetText(chText);
    cText.Draw();
    return;
    }
  uiTime -= 10000;

  // do credits
  if (uiTime < 42000)
    {
    unsigned int uiCredit = (uiTime / 7000);
    uiTime = uiTime % 7000;
    float fOffsetY = 0.0f;
    if (uiTime < 1000)
      fOffsetY = 650.0f - (float) uiTime * 0.65f;
    else if (uiTime > 6000)
      fOffsetY = (6000 - (int) uiTime) * 0.65f;
    // setup Title
    CTextGL      cTitle;
    cTitle.SetAlpha(true);
    cTitle.SetColor(255, 255, 208);
    cTitle.SetJustification(E_CENTER);
    cTitle.SetFont(E_FONT_VECTROID);
    cTitle.SetHeight(40.0f);
    cTitle.SetPosition(0.0f, fOffsetY - 70.0f, -10.0f);
    // Setup name
    cText.SetFont(E_FONT_BERYLLIUM);
    cText.SetColor(192, 192, 255);
    cText.SetHeight(60.0f);
    cText.SetPosition(0.0f, fOffsetY + 10.0f, -10.0f);
    // set strings and invaders
    CInvader *pcInvLeft, *pcInvRight;
    switch (uiCredit)
      {
      case 0:
        cText.SetText("Richard Goedeken");
        cTitle.SetText("Game Design / 3D Models");
        pcInvLeft = new CInvader20;
        pcInvRight = new CInvader20;
        break;
      case 1:
        cText.SetText("Richard Goedeken");
        cTitle.SetText("Programming");
        pcInvLeft = new CInvader10;
        pcInvRight = new CInvader10;
        break;
      case 2:
        cText.SetText("Ray Larabie\nwww.larabiefonts.com");
        cTitle.SetText("Fonts");
        pcInvLeft = new CInvader30;
        pcInvRight = new CInvader30;
        break;
      case 3:
        cText.SetText("Taito Space Invaders\n(Toshihiro Nishikado)");
        cTitle.SetText("Inspired by");
        pcInvLeft = new CInvaderMystery;
        pcInvRight = new CInvaderMystery;
        break;
      case 4:
        cText.SetText("www.mame.net");
        cTitle.SetText("Dedicated to:");
        cText.SetPosition(0.0f, fOffsetY + 125.0f, -10.0f);
        // add mame logo
        glDisable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        if (CGameMain::m_bAlphaSupported)
          {
          glEnable(GL_BLEND);
          glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
          }
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glColor3ub(255, 255, 255);
        glBindTexture(GL_TEXTURE_2D, CTextures::GetTextureID(E_TEX_MAME));
	      glBegin(GL_QUADS);
		    glTexCoord2f(0.0f, 0.0f); glVertex3f(-200, fOffsetY + 0.0f, -10.0f);
		    glTexCoord2f(0.0f, 1.0f); glVertex3f(-200, fOffsetY + 100.0f, -10.0f);
    		glTexCoord2f(1.0f, 1.0f); glVertex3f(+200, fOffsetY + 100.0f, -10.0f);
		    glTexCoord2f(1.0f, 0.0f); glVertex3f(+200, fOffsetY + 0.0f, -10.0f);
        glEnd();
        glEnable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        if (CGameMain::m_bAlphaSupported) glDisable(GL_BLEND);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        break;
      case 5:
        cText.SetText("www.fascinationsoftware.com");
        cTitle.SetText("Visit");
        break;
      }
    // position and draw invaders, then delete
    if (uiCredit < 4)
      {
      float fWidth = cTitle.Width();
      pcInvLeft->SetPosition(-fWidth / 2.0f - 55.0f, fOffsetY - 55.0f, -10.0f);
      pcInvRight->SetPosition(fWidth / 2.0f + 55.0f, fOffsetY - 55.0f, -10.0f);
      pcInvLeft->SetAxis(0.0f, -1.0f, 0.0f);
      pcInvRight->SetAxis(0.0f, 1.0f, 0.0f);
      float fAngle = (float) uiTime * 0.12f;
      pcInvLeft->SetRotation(fAngle);
      pcInvRight->SetRotation(fAngle);
      glPushMatrix();
      pcInvLeft->Draw();
      glPopMatrix();
      glPushMatrix();
      pcInvRight->Draw();
      glPopMatrix();
      delete pcInvLeft;
      delete pcInvRight;
      }
    // draw text
    cText.Draw();
    cTitle.Draw();
    return;
    }
  uiTime -= 42000;

  // draw mooninites
  if (uiTime < 13000)
    {
    glPushMatrix();
    m_Ignignokt.Draw();
    glPopMatrix();
    glPushMatrix();
    m_Errr.Draw();
    glPopMatrix();
    }

  // draw mystery ship
  if (uiTime > 7000)
    {
    glPushMatrix();
    m_pcMystery->Draw();
    glPopMatrix();
    }

  if (uiTime < 9000) return;
  uiTime -= 9000;

  // draw flashing white plane
  if ((uiTime & 32) != 0 && uiTime < 6000)
    {
    float fX, fY, fZ;
    if (uiTime < 2000)
      {
      fX = 20.0f + 180.0f * uiTime * 0.0005f;
      fY = -250.0f + 500.0f * uiTime * 0.0005f;
      fZ = 40.0f - 70.0f * uiTime * 0.0005f;
      }
    else if (uiTime > 4000)
      {
      fX = 20.0f + 180.0f * (6000 - uiTime) * 0.0005f;
      fY = -250.0f + 500.0f * (6000 - uiTime) * 0.0005f;
      fZ = 40.0f - 70.0f * (6000 - uiTime) * 0.0005f;
      }
    else
      {
      fX = 200.0f;
      fY = 250.0f;
      fZ = -30.0f;
      }
    glDisable(GL_LIGHTING);
    if (CGameMain::m_bAlphaSupported)
      {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      }
    glColor4ub(255, 255, 255, 192);
  	glBegin(GL_QUADS);
	  glVertex3f(-20.0f, -250.0f, 120.0f);
  	glVertex3f(-fX,    fY,      fZ);
    glVertex3f(fX,     fY,      fZ);
  	glVertex3f(20.0f,  -250.0f, 120.0f);
    glEnd();
    glEnable(GL_LIGHTING);
    if (CGameMain::m_bAlphaSupported) glDisable(GL_BLEND);
    }

}

/////////////////////////////////////////////////////////////////////////////
// CGameLogic private Ending Sequencer

void CGameLogic::ProcessEndingSequence(void)
{
  unsigned int uiTime = m_uiStateTime;

  if (uiTime < 52000) return;
  uiTime -= 52000;

  // deal with mooninites entering the scene
  if (uiTime < 3000)
    {
    // mooninites walking
    float fIgnignoktX = -770.0f + 670.0f * (int) uiTime / 3000.0f;
    float fErrrX      =  750.0f - 650.0f * (int) uiTime / 3000.0f;
    m_Ignignokt.SetPosition(fIgnignoktX, 150.0f, 0.0f);
    m_Errr.SetPosition(fErrrX, 175.5f, 0.0f);
    m_Ignignokt.SetRotation(0.0f);
    m_Errr.SetRotation(0.0f);
    // set legs
    bool bLeft = (uiTime & 128) != 0;
    m_Ignignokt.SetLegs(bLeft, !bLeft);
    m_Errr.SetLegs(!bLeft, bLeft);
    return;
    }
  else if (m_uiLastTime < 55000 && m_uiStateTime >= 55000)
    {
    m_Ignignokt.SetPosition(-100.0f, 150.0f, 0.0f);
    m_Errr.SetPosition(100.0f, 175.5f, 0.0f);
    m_Ignignokt.SetLegs(false, false);
    m_Errr.SetLegs(false, false);
    m_Ignignokt.SetAxis(1.0f, 0.0f, 0.0f);
    m_Errr.SetAxis(1.0f, 0.0f, 0.0f);
    m_pSound->PlaySoundClip(E_SOUND_MOON_7);
    return;
    }
  uiTime -= 3000;

  // wait while Ignignokt and Err say something
  if (uiTime < 4000) return;
  uiTime -= 4000;

  // handle bigass mystery ship
  if (m_uiLastTime < 59000 && m_uiStateTime >= 59000)
    {
    m_pSound->PlaySoundClip(E_SOUND_MYSTERY);
    m_pcMystery->ActivateMotion(false);
    m_pcMystery->SetAxis(0.0f, -1.0f, 0.0f);
    m_pcMystery->SetScale(3.5f);
    // call processstate to turn off the flash
    m_pcMystery->ProcessState(0);
    }
  m_fMysteryAngle = 0.12f * (float) uiTime;
  m_pcMystery->SetRotation(m_fMysteryAngle);
  if (uiTime < 2000)
    {
    // move to the left
    m_pcMystery->SetPosition(800.0f - 800.0f * (int) uiTime * 0.0005f, -250.0f, 40.0f);
    return;
    }
  uiTime -= 2000;

  // wait for 2 seconds while light beam comes out
  if (uiTime < 2000) return;
  uiTime -= 2000;

  // fading mooninites
  if (uiTime < 2000)
    {
    unsigned char uchFade = (unsigned char) (255 - uiTime * 0.1275f);
    m_Ignignokt.SetAlpha(uchFade);
    m_Errr.SetAlpha(uchFade);
    if (uiTime > 1000)
      {
      m_Ignignokt.SetRotation((uiTime - 1000) * 0.09f);
      m_Errr.SetRotation((uiTime - 1000) * 0.09f);
      }
    return;
    }
  uiTime -= 2000;

  // wait for 2 seconds while light beam goes in
  if (uiTime < 2000) return;
  uiTime -= 2000;

  // make spaceship fly away
  if (uiTime < 2000)
    {
    float fY = -250.0f - uiTime * 0.08f;
    float fZ = 40 + uiTime * 0.05f;
    m_pcMystery->SetPosition(0.0f, fY, fZ);
    m_pcMystery->SetScale(3.5f - uiTime * 0.0015f);
    }

}
