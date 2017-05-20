/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Demo.h                                                   *
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

#if !defined(DEMO_H)
#define DEMO_H

#ifdef WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>

#include "Invader.h"
#include "InvaderMystery.h"
#include "Bunker.h"
#include "LaserBase.h"
#include "TextGL.h"

// definitions controlling the time intervals when the high score display comes and goes
#define SCORE_OFF_TIME 20
#define SCORE_ON_TIME  15

class CGameMain;
class CSettings;

typedef struct {int    iMillisecLeft;
                bool   bMoving;
                float  fDelta;
                float  fPos;
                float  fExtent;
                int    iChangesLeft;
                } SInvaderState;

class CDemo
{
public:
  CDemo();
  ~CDemo();

  // accessor functions
  void Draw(void) const;

  // modifier functions
  void Initialize(CGameMain *pGameMain, CSettings *pSettings);
  void ProcessState(unsigned int uiMillisec);
  void ProcessEvents(void);

private:
  // member data
  CInvader  *m_pcInvader[48];
  CInvader  *m_pcMystery;
  CBunker    m_Bunker[4];
  CLaserBase m_LaserBase;
  CTextGL    m_Text[3];
  CGameMain *m_pGameMain;
  CSettings *m_pSettings;

  // data for Setup Screen
  bool       m_bSetupMode;
  int        m_iHiControl;
  int        m_iColumn;
  bool       m_bWaitingForCommand;
  bool       m_bPreviousFullscreen;
  int        m_iPreviousScreenWidth;
  // data for Help Screen
  bool       m_bHelpScreen;
  // data for High Scores
  bool       m_bHighScores;
  int        m_iHSCountdown;
  int        m_iHSDirection;
  float      m_fHSOffsetX;
  float      m_fHSOffsetY;

  SInvaderState  m_sState[48];
  float          m_fMysteryAngle;

  // private functions
  void SetRandomState(int iInvader);
  void SetupProcessEvent(SDL_Event *pEvent);
  void DrawHelpScreen(void) const;
  void DrawHighScores(void) const;
  void DrawSetupScreen(void) const;

};

#endif // defined(DEMO_H)
