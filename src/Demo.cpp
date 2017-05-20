/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Demo.cpp                                                 *
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
**                              04/27/05                                   **
**                                                                         **
** Demo.cpp - contains interface for CDemo class, used to display the      **
**            attraction demo.                                             **
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
#include "Settings.h"
#include "Demo.h"
#include "Invader.h"
#include "Invader10.h"
#include "Invader20.h"
#include "Invader30.h"
#include "InvaderMystery.h"
#include "Bunker.h"
#include "LaserBase.h"
#include "TextGL.h"

/////////////////////////////////////////////////////////////////////////////
// CDemo class constructor and destructor

CDemo::CDemo()
{
  // initalize the mode flags
  m_bSetupMode  = false;
  m_bHelpScreen = false;

  // create the invaders
  for (int i = 0; i < 48; i++)
    {
    if (i < 16) m_pcInvader[i] = new CInvader30;
    else if (i < 32) m_pcInvader[i] = new CInvader20;
    else m_pcInvader[i] = new CInvader10;
    }
  m_pcMystery = new CInvaderMystery;
}

CDemo::~CDemo()
{
  // delete the invaders
  for (int i = 0; i < 48; i++)
    {
    delete m_pcInvader[i];
    }
  delete m_pcMystery;

}

/////////////////////////////////////////////////////////////////////////////
// CDemo accessors

void CDemo::Draw(void) const
{
  // draw all of the invaders
  for (int i = 0; i < 48; i++)
    {
    glPushMatrix();
    m_pcInvader[i]->Draw();
    glPopMatrix();
    }
  glPushMatrix();
  m_pcMystery->Draw();
  glPopMatrix();

  // draw the bunkers
  for (int i = 0; i < 4; i++)
    {
    glPushMatrix();
    m_Bunker[i].Draw();
    glPopMatrix();
    }

  // draw the laser base
  glPushMatrix();
  m_LaserBase.Draw();
  glPopMatrix();

  // draw the text
  for (int i = 0; i < 3; i++)
    {
    m_Text[i].Draw();
    }

  // draw special screens
  if (m_bHelpScreen)
    DrawHelpScreen();
  else if (m_bHighScores)
    DrawHighScores();
  else if (m_bSetupMode)
    DrawSetupScreen();

}

/////////////////////////////////////////////////////////////////////////////
// CDemo Initialize

void CDemo::Initialize(CGameMain *pGameMain, CSettings *pSettings)
{
  // set pointer to main class for switching modes
  m_pGameMain = pGameMain;
  m_pSettings = pSettings;

  // set the positions of the invaders
  for (int iY = 0; iY < 6; iY++)
    {
    for (int iX = 0; iX < 8; iX++)
      {
      float fX = (float) iX * 110.0f - 385.0f;
      float fY = (float) iY * 88.0f  - 260.0f;
      m_pcInvader[iY * 8 + iX]->SetPosition(fX, fY, 0.0f);
      m_pcInvader[iY * 8 + iX]->SetRotation(0.0f);
      }
    }
  m_pcMystery->SetPosition(420.0f, -340.0f, 0.0f);
  m_pcMystery->SetRotation(0.0f);
  m_pcMystery->SetAxis(0.0f, -1.0f, 0.0f);
  m_fMysteryAngle = 0.0f;

  // set the high score screen state
  m_bHighScores = false;
  m_iHSCountdown = SCORE_OFF_TIME * 1000;

  // set the states of the invaders
  for (int i = 0; i < 48; i++)
    {
    m_sState[i].bMoving       = false;
    m_sState[i].iMillisecLeft = 1000;
    }

  // set the positions of the bunkers
  for (int iX = 0; iX < 4; iX++)
    {
    float fX = (float) iX * 250.0f - 375.0f;
    m_Bunker[iX].SetPosition(fX, 290.0f, 0.0f);
    }

  // set the position of the laser base
  m_LaserBase.SetPosition(0.0f, 415.0f, 0.0f);

  // set up the text strings
  for (int i = 0; i < 3; i++)
    {
    m_Text[i].SetAlpha(true);
    m_Text[i].SetJustification(E_CENTER);
    }

  m_Text[0].SetColor(32, 32, 200);
  m_Text[0].SetFont(E_FONT_EUPHORIGENIC);
  m_Text[0].SetHeight(42.0f);
  m_Text[0].SetPosition(0.0f, -355.0f, -50.0f);
  m_Text[0].SetText("Copyright (C) 2005,2011 by Richard Goedeken");

  m_Text[1].SetFont(E_FONT_VECTROID);
  m_Text[1].SetHeight(70.0f);
  m_Text[1].SetPosition(0.0f, 210.0f, -50.0f);
  m_Text[1].SetText("Invasion 3-D");

  m_Text[2].SetColor(200, 32, 32);
  m_Text[2].SetFont(E_FONT_6809);
  m_Text[2].SetHeight(24.0f);
  m_Text[2].SetPosition(0.0f, 435.0f, -50.0f);
  m_Text[2].SetText("Press F1 for help, F2 to start, or F3 for options.");

}

/////////////////////////////////////////////////////////////////////////////
// CDemo Event Handling

void CDemo::ProcessEvents(void)
{
  SDL_Event event;

  // Grab all the events off the queue.
  while (SDL_PollEvent(&event))
    {
    if (m_bSetupMode) SetupProcessEvent(&event);
    else
      switch (event.type)
        {
        case SDL_KEYDOWN:
          if (event.key.keysym.sym == SDLK_ESCAPE)
            {
            if (m_bHelpScreen)
              {
              m_bHelpScreen = false;
              // reset the high score timer
              m_iHSCountdown = SCORE_OFF_TIME * 1000;
              }
            else
              m_pGameMain->SetMode(E_QUIT);
            }
          else if (event.key.keysym.sym == SDLK_F1)
            {
            // help screen
            if (m_bHighScores)
              m_bHighScores = false;
            m_bHelpScreen = true;
            }
          else if (event.key.keysym.sym == SDLK_F2 && !m_bHelpScreen)
            {
            // play game
            m_pGameMain->SetMode(E_GAMERUN);
            }
          else if (event.key.keysym.sym == SDLK_F3)
            {
            // setup controls
            if (m_bHelpScreen)
              m_bHelpScreen = false;
            if (m_bHighScores)
              m_bHighScores = false;
            m_bSetupMode         = true;
            m_iHiControl         = 0;
            m_iColumn            = 0;
            m_bWaitingForCommand = false;
            // save existing screen parameters
            m_bPreviousFullscreen = m_pSettings->GetFullscreen();
            m_iPreviousScreenWidth = m_pSettings->GetScreenWidth();
            }
          break;
        case SDL_QUIT:
          m_pGameMain->SetMode(E_QUIT);
          break;
        }
    }
}

void CDemo::SetupProcessEvent(SDL_Event *pEvent)
{

  if (m_bWaitingForCommand && m_iColumn == 0 && pEvent->type == SDL_KEYDOWN)
    {
    if (pEvent->key.keysym.sym != SDLK_ESCAPE)
      {
      // search for the key in our key list
      int i;
      for (i = 0; i < 118; i++)
        if (g_asKeys[i].symbol == pEvent->key.keysym.sym) break;
      if (i < 118)
        m_pSettings->Change((ECommand) m_iHiControl, pEvent->key.keysym.sym);
      }
    // go back to not-waiting mode
    m_bWaitingForCommand = false;
    }
  else if (m_bWaitingForCommand && m_iColumn == 1)
    {
    if (pEvent->type == SDL_KEYDOWN && pEvent->key.keysym.sym == SDLK_ESCAPE)
      {
      // if escape is pressed, remove this joystick command
      m_pSettings->Change((ECommand) m_iHiControl, -1, false, 0, false);
      // go back to not-waiting mode
      m_bWaitingForCommand = false;
      }
    else if (pEvent->type == SDL_JOYBUTTONDOWN)
      {
      m_pSettings->Change((ECommand) m_iHiControl, pEvent->jbutton.which, false, pEvent->jbutton.button, false);
      // go back to not-waiting mode
      m_bWaitingForCommand = false;
      }
    else if (pEvent->type == SDL_JOYAXISMOTION && pEvent->jaxis.value < -12000)
      {
      m_pSettings->Change((ECommand) m_iHiControl, pEvent->jaxis.which, true, pEvent->jaxis.axis, false);
      // go back to not-waiting mode
      m_bWaitingForCommand = false;
      }
    else if (pEvent->type == SDL_JOYAXISMOTION && pEvent->jaxis.value > 12000)
      {
      m_pSettings->Change((ECommand) m_iHiControl, pEvent->jaxis.which, true, pEvent->jaxis.axis, true);
      // go back to not-waiting mode
      m_bWaitingForCommand = false;
      }
    }
  else if (!m_bWaitingForCommand && pEvent->type == SDL_KEYDOWN)
    {
    // handle keys: escape, enter, arrows
    switch (pEvent->key.keysym.sym)
      {
      case SDLK_ESCAPE:
        // turn off the setup screen
        m_bSetupMode = false;
        // reset the high score timer
        m_iHSCountdown = SCORE_OFF_TIME * 1000;
        // save the settings
        m_pSettings->SaveToFile();
        // if screen settings have changed, alert the GameMain
        if (m_pSettings->GetFullscreen() != m_bPreviousFullscreen || m_pSettings->GetScreenWidth() != m_iPreviousScreenWidth)
          {
          if (!m_pGameMain->ChangeScreen()) m_pGameMain->SetMode(E_QUIT);
          }
        break;
      case SDLK_RETURN:
        // if we are over a key command, then go into the waiting for input mode
        if (m_iHiControl <= E_INPUT_LAST_COMMAND) m_bWaitingForCommand = true;
        break;
      case SDLK_UP:
        if (m_iHiControl > 0) m_iHiControl--;
        break;
      case SDLK_DOWN:
        if (m_iHiControl < E_INPUT_LAST_COMMAND + 2) m_iHiControl++;
        if (m_iHiControl > E_INPUT_LAST_COMMAND) m_iColumn = 1;
        break;
      case SDLK_LEFT:
        if (m_iHiControl == E_INPUT_LAST_COMMAND + 1)
          {
          bool bFullScreen = m_pSettings->GetFullscreen();
          int iWidth = m_pSettings->GetScreenWidth();
          if (iWidth == 1600) m_pSettings->SetScreen(1400, 1050, bFullScreen);
          else if (iWidth == 1400) m_pSettings->SetScreen(1280, 1024, bFullScreen);
          else if (iWidth == 1280) m_pSettings->SetScreen(1152, 864, bFullScreen);
          else if (iWidth == 1152) m_pSettings->SetScreen(1024, 768, bFullScreen);
          else if (iWidth == 1024) m_pSettings->SetScreen(800, 600, bFullScreen);
          else if (iWidth == 800) m_pSettings->SetScreen(640, 480, bFullScreen);
          else m_pSettings->SetScreen(512, 384, bFullScreen);
          break;
          }
        else if (m_iHiControl == E_INPUT_LAST_COMMAND + 2)
          {
          m_pSettings->SetScreen(m_pSettings->GetScreenWidth(), m_pSettings->GetScreenHeight(), !m_pSettings->GetFullscreen());
          break;
          }
      case SDLK_RIGHT:
        if (m_iHiControl <= E_INPUT_LAST_COMMAND)
          {
          m_iColumn = 1 - m_iColumn;
          }
        else if (m_iHiControl == E_INPUT_LAST_COMMAND + 1)
          {
          bool bFullScreen = m_pSettings->GetFullscreen();
          int iWidth = m_pSettings->GetScreenWidth();
          if (iWidth == 512) m_pSettings->SetScreen(640, 480, bFullScreen);
          else if (iWidth == 640) m_pSettings->SetScreen(800, 600, bFullScreen);
          else if (iWidth == 800) m_pSettings->SetScreen(1024, 768, bFullScreen);
          else if (iWidth == 1024) m_pSettings->SetScreen(1152, 864, bFullScreen);
          else if (iWidth == 1152) m_pSettings->SetScreen(1280, 1024, bFullScreen);
          else if (iWidth == 1280) m_pSettings->SetScreen(1400, 1050, bFullScreen);
          else m_pSettings->SetScreen(1600, 1200, bFullScreen);
          }
        else if (m_iHiControl == E_INPUT_LAST_COMMAND + 2)
          {
          m_pSettings->SetScreen(m_pSettings->GetScreenWidth(), m_pSettings->GetScreenHeight(), !m_pSettings->GetFullscreen());
          }
        break;
      default:
        break;
      }
    }

}

/////////////////////////////////////////////////////////////////////////////
// CDemo State Processing

void CDemo::ProcessState(unsigned int uiMillisec)
{
  // rotate mystery invader
  m_fMysteryAngle += (float) uiMillisec / 16.6f;
  m_pcMystery->SetRotation(m_fMysteryAngle);

  // process the state of each invader
  for (int i = 0; i < 48; i++)
    {
    if (!m_sState[i].bMoving)
      {
      m_sState[i].iMillisecLeft -= uiMillisec;
      if (m_sState[i].iMillisecLeft <= 0)
        SetRandomState(i);
      }
    else
      {
      // we are moving
      float fOldAngle = m_sState[i].fPos;
      // update the angle
      float fNewAngle = fOldAngle + m_sState[i].fDelta * (float) uiMillisec;
      m_sState[i].fPos = fNewAngle;
      m_pcInvader[i]->SetRotation(fNewAngle);
      // bounce off the extent
      if (fabs(fNewAngle) > m_sState[i].fExtent)
        {
        fNewAngle = m_sState[i].fExtent;
        if (m_sState[i].fDelta < 0) fNewAngle = -fNewAngle;
        m_sState[i].fPos = fNewAngle;
        m_pcInvader[i]->SetRotation(fNewAngle);
        m_sState[i].fDelta = -m_sState[i].fDelta;
        m_sState[i].iChangesLeft--;
        }
      // stop if necessary
      if (m_sState[i].iChangesLeft <= 0 && fOldAngle * fNewAngle <= 0)
        {
        m_pcInvader[i]->SetRotation(0.0f);
        m_sState[i].fPos = 0.0f;
        SetRandomState(i);
        }
      }
    }

  // handle state processing for high score display
  if (!m_bHelpScreen && !m_bSetupMode)
    {
    m_iHSCountdown -= uiMillisec;
    if (m_iHSCountdown <= 0)
      {
      // switch the state of the high score list (off/on)
      if (m_bHighScores)
        {
        m_bHighScores = false;
        m_iHSCountdown = SCORE_OFF_TIME * 1000;
        }
      else
        {
        m_bHighScores = true;
        m_iHSCountdown = SCORE_ON_TIME * 1000;
        m_iHSDirection = (rand() & 3);
        m_fHSOffsetX = 0.0f;
        m_fHSOffsetY = 0.0f;
        }
      }
    // now handle the sliding in and out
    if (m_bHighScores)
      {
      switch (m_iHSDirection)
        {
        case 0:
          if (SCORE_ON_TIME * 1000 - m_iHSCountdown <= 2667)
            m_fHSOffsetX = 960.f - (float) ((SCORE_ON_TIME * 1000 - m_iHSCountdown) * 0.36f);
          else if (m_iHSCountdown < 2667)
            m_fHSOffsetX = -960.0f + (float) (m_iHSCountdown * 0.36f);
          else
            m_fHSOffsetX = 0.0f;
          break;
        case 1:
          if (SCORE_ON_TIME * 1000  - m_iHSCountdown <= 2667)
            m_fHSOffsetX = -960.f + (float) ((SCORE_ON_TIME * 1000 - m_iHSCountdown) * 0.36f);
          else if (m_iHSCountdown < 2667)
            m_fHSOffsetX = 960.0f - (float) (m_iHSCountdown * 0.36f);
          else
            m_fHSOffsetX = 0.0f;
          break;
        case 2:
          if (SCORE_ON_TIME * 1000  - m_iHSCountdown <= 2667)
            m_fHSOffsetY = 800.f - (float) ((SCORE_ON_TIME * 1000 - m_iHSCountdown) * 0.3f);
          else if (m_iHSCountdown < 2667)
            m_fHSOffsetY = -800.0f + (float) (m_iHSCountdown * 0.3f);
          else
            m_fHSOffsetY = 0.0f;
          break;
        case 3:
          if (SCORE_ON_TIME * 1000  - m_iHSCountdown <= 2667)
            m_fHSOffsetY = -800.f + (float) ((SCORE_ON_TIME * 1000 - m_iHSCountdown) * 0.3f);
          else if (m_iHSCountdown < 2667)
            m_fHSOffsetY = 800.0f - (float) (m_iHSCountdown * 0.3f);
          else
            m_fHSOffsetY = 0.0f;
          break;
        }
      }
    }

}

/////////////////////////////////////////////////////////////////////////////
// CDemo private modifiers

void CDemo::SetRandomState(int iInvader)
{
  if (rand() & 0x3f)
    {
    m_sState[iInvader].bMoving = false;
    m_sState[iInvader].iMillisecLeft = (rand() & 0xfff) + 320;
    return;
    }

  m_sState[iInvader].bMoving      = true;
  m_sState[iInvader].fExtent      = ((float) rand() / (float) RAND_MAX) * ((float) rand() / (float) RAND_MAX) * 30.0f + 10.0f;
  m_sState[iInvader].fPos         = 0.0f;
  m_sState[iInvader].iChangesLeft = (rand() % 5) + 1;
  m_sState[iInvader].fDelta       = ((float) (rand() % 100) / 100.0f + 0.25f) * ((rand() & 2) - 1) / 16.6f;
  int iAxis = (rand() % 3);
  if (iAxis == 0) m_pcInvader[iInvader]->SetAxis(1.0f, 0.0f, 0.0f);
  else if (iAxis == 1) m_pcInvader[iInvader]->SetAxis(0.0f, 1.0f, 0.0f);
  else m_pcInvader[iInvader]->SetAxis(0.0f, 0.0f, 1.0f);

}

void CDemo::DrawHelpScreen(void) const
{
  CTextGL cHelpText;

  // turn off lighting and draw a simple dark grey square
  glDisable(GL_LIGHTING);
  if (CGameMain::m_bAlphaSupported)
    glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glBegin(GL_QUADS);
  glColor4f(0.1f, 0.1f, 0.1f, 0.9f);
  glVertex3f(-450.0f, -300.0f, -55.0f);
  glVertex3f(-450.0f,  400.0f, -55.0f);
  glVertex3f( 450.0f,  400.0f, -55.0f);
  glVertex3f( 450.0f, -300.0f, -55.0f);
  glEnd();
  if (CGameMain::m_bAlphaSupported)
    glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);

  // format and draw the help text
  cHelpText.SetAlpha(true);
  cHelpText.SetColor(255, 192, 192);
  cHelpText.SetFont(E_FONT_CREDIT_RIVER);
  cHelpText.SetHeight(42.0f);
  cHelpText.SetJustification(E_CENTER);
  cHelpText.SetPosition(0.0f, -290.0f, -60.0f);
  cHelpText.SetText("**HELP**\n\nJust shoot them all. What could be simpler?\nAvoid getting shot by hiding behind the bunkers\nand rack up as many points as you can.\nIf you're really good, you might sneak a peek at\nsome particularly heinous bad-guys.\n\nFor any questions or comments, please email:\nInvasion3D@fascinationsoftware.com\n\nVisit:\nhttp://www.fascinationsoftware.com\n\nPress Escape");
  cHelpText.Draw();
}

void CDemo::DrawSetupScreen(void) const
{
  CTextGL cText;

  // disable depth
  glDisable(GL_DEPTH_TEST);

  // turn off lighting and draw a simple dark grey square
  glDisable(GL_LIGHTING);
  if (CGameMain::m_bAlphaSupported)
    glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glBegin(GL_QUADS);
  glColor4f(0.1f, 0.1f, 0.1f, 0.9f);
  glVertex3f(-450.0f, -300.0f, -55.0f);
  glVertex3f(-450.0f,  400.0f, -55.0f);
  glVertex3f( 450.0f,  400.0f, -55.0f);
  glVertex3f( 450.0f, -300.0f, -55.0f);
  // draw highlight bar
  float fX0, fX1, fY0, fY1;
  if (m_iColumn == 0)
    {
    fX0 = -120.0f;
    fX1 = 130.0f;
    }
  else
    {
    fX0 = 150.0f;
    fX1 = 430.0f;
    }
  fY0 = -296.0f + (float) m_iHiControl * 38.5f;
  fY1 = fY0 + 40.0f;
  if (m_bWaitingForCommand)
    glColor4f(0.25f, 0.7f, 0.25f, 0.6f);
  else
    glColor4f(0.7f, 0.25f, 0.25f, 0.5f);
  glVertex3f(fX0, fY0, -57.0f);
  glVertex3f(fX0, fY1, -57.0f);
  glVertex3f(fX1, fY1, -57.0f);
  glVertex3f(fX1, fY0, -57.0f);
  glEnd();
  if (CGameMain::m_bAlphaSupported)
    glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);

  // build a string with all of the input commands
  char chCommands[256];
  char *pch = chCommands;
  for (int iCmd = E_INPUT_FIRST_COMMAND; iCmd <= E_INPUT_LAST_COMMAND; iCmd++)
    {
    m_pSettings->GetCommandName((ECommand) iCmd, pch);
    strcat(pch, "\n");
    pch += strlen(pch);
    }
  strcat(chCommands, "Resolution\nFullscreen");

  // format and draw the command names
  cText.SetAlpha(true);
  cText.SetColor(255, 255, 255);
  cText.SetFont(E_FONT_BERYLLIUM);
  cText.SetHeight(35.0f);
  cText.SetJustification(E_LEFT);
  cText.SetPosition(-420.0f, -290.0f, -60.0f);
  cText.SetText(chCommands);
  cText.Draw();

  // build a string with all of the keys
  char chKeys[512];
  pch = chKeys;
  for (int iCmd = E_INPUT_FIRST_COMMAND; iCmd <= E_INPUT_LAST_COMMAND; iCmd++)
    {
    SDLKey sKeyDummy;
    m_pSettings->GetKey((ECommand) iCmd, &sKeyDummy, pch);
    strcat(pch, "\n");
    pch += strlen(pch);
    }

  // format and draw the keys
  cText.SetColor(255, 255, 192);
  cText.SetJustification(E_RIGHT);
  cText.SetPosition(120.0f, -290.0f, -60.0f);
  cText.SetText(chKeys);
  cText.Draw();

  // build a string with all of the joystick commands
  char chJoys[512];
  pch = chJoys;
  for (int iCmd = E_INPUT_FIRST_COMMAND; iCmd <= E_INPUT_LAST_COMMAND; iCmd++)
    {
    int iJoystick, iIndex;
    bool bAxis, bPositive;
    m_pSettings->GetJoystickMovement((ECommand) iCmd, &iJoystick, &bAxis, &iIndex, &bPositive);
    if (iJoystick >= 0)
      {
      if (bAxis)
        {
        if (bPositive) sprintf(pch, "+ Axis %i\n", iIndex + 1);
        else sprintf(pch, "- Axis %i\n", iIndex + 1);
        }
      else
        {
        sprintf(pch, "Button %i\n", iIndex + 1);
        }
      }
    else sprintf(pch, "\n");
    pch += strlen(pch);
    }
  if (m_pSettings->GetFullscreen())
    sprintf(pch, "< %i x %i >\n< ON >", m_pSettings->GetScreenWidth(), m_pSettings->GetScreenHeight());
  else
    sprintf(pch, "< %i x %i >\n< OFF >", m_pSettings->GetScreenWidth(), m_pSettings->GetScreenHeight());

  // format and draw the joystick commands
  cText.SetColor(255, 255, 192);
  cText.SetJustification(E_RIGHT);
  cText.SetPosition(420.0f, -290.0f, -60.0f);
  cText.SetText(chJoys);
  cText.Draw();

  // draw instructions
  cText.SetColor(160, 160, 255);
  cText.SetJustification(E_CENTER);
  cText.SetPosition(0.0f, 300.0f, -60.0f);
  cText.SetText("Use the arrow keys and <Enter> to change values.\nPress <Escape> when done.");
  cText.Draw();

  // re-enable depth
  glEnable(GL_DEPTH_TEST);
}

void CDemo::DrawHighScores(void) const
{
  CTextGL cText;
  int     iScores[10];

  // turn off lighting and draw a simple dark grey square
  glDisable(GL_LIGHTING);
  if (CGameMain::m_bAlphaSupported)
    glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glBegin(GL_QUADS);
  glColor4f(0.1f, 0.1f, 0.1f, 0.7f);
  glVertex3f(-390.0f + m_fHSOffsetX, -300.0f + m_fHSOffsetY, -55.0f);
  glVertex3f(-390.0f + m_fHSOffsetX,  340.0f + m_fHSOffsetY, -55.0f);
  glVertex3f( 390.0f + m_fHSOffsetX,  340.0f + m_fHSOffsetY, -55.0f);
  glVertex3f( 390.0f + m_fHSOffsetX, -300.0f + m_fHSOffsetY, -55.0f);
  glEnd();
  if (CGameMain::m_bAlphaSupported)
    glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);

  // build a string with all of the high score names
  char chNames[256];
  char *pch = chNames;
  for (int iPlace = 1; iPlace <= 10; iPlace++)
    {
    iScores[iPlace - 1] = m_pSettings->GetHighScore(iPlace, pch);
    strcat(pch, "\n");
    pch += strlen(pch);
    }

  // format and draw the names
  cText.SetAlpha(true);
  cText.SetColor(255, 255, 255);
  cText.SetFont(E_FONT_CREDIT_RIVER);
  cText.SetHeight(57.0f);
  cText.SetJustification(E_LEFT);
  cText.SetPosition(-360.0f + m_fHSOffsetX, -290.0f + m_fHSOffsetY, -60.0f);
  cText.SetText(chNames);
  cText.Draw();

  // format and draw the scores
  char chScores[256];
  pch = chScores;
  for (int i = 0; i < 10; i++)
    {
    // print score with commas
    char chNumber[32];
    sprintf(chNumber, "%i", iScores[i]);
    int iNumLength = (int) strlen(chNumber);
    for (int j = 0; j < iNumLength; j++)
      {
      *pch++ = chNumber[j];
      if (j != iNumLength - 1 && (j % 3) == ((iNumLength - 1) % 3)) *pch++ = ',';
      }
    *pch++ = '\n';
    }
  *pch++ = 0;
  cText.SetJustification(E_RIGHT);
  cText.SetPosition(360.0f + m_fHSOffsetX, -290.0f + m_fHSOffsetY, -60.0f);
  cText.SetText(chScores);
  cText.Draw();

}

