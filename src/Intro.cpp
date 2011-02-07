/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Intro.cpp                                                *
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
**                              05/26/05                                   **
**                                                                         **
** Intro.cpp - contains implementation of CIntro class, which displays the **
**             opening sequence.                                           **
**                                                                         **
****************************************************************************/

#ifdef WIN32
  #include <windows.h>
#endif
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <stdlib.h>

#include "Intro.h"
#include "GameMain.h"
#include "Invader10.h"
#include "Invader20.h"
#include "Invader30.h"
#include "InvaderMystery.h"
#include "TextGL.h"

/////////////////////////////////////////////////////////////////////////////
// CIntro class constructor and destructor

CIntro::CIntro()
{
}

CIntro::~CIntro()
{
}

/////////////////////////////////////////////////////////////////////////////
// CIntro accessors

void CIntro::Draw(void) const
{
  unsigned int uiTime = m_uiStateTime;

  // draw the stars
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, m_fStars);
  glColorPointer(3, GL_FLOAT, 0, m_fColors);
  glDrawArrays(GL_POINTS, 0, NUM_STARS);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);

  // draw the text
  if (uiTime > 6000 && uiTime < 15000)
    {
    uiTime -= 6000;
    unsigned int uiMsg = (uiTime / 3000);
    uiTime = uiTime % 3000;
    float fOffsetZ = 0.0f;
    float fSize = 1.0f;
    if (uiTime < 500)
      {
      fOffsetZ = 1600.0f - (float) uiTime * 3.2f;
      fSize = uiTime * 0.002f;
      }
    else if (uiTime > 2000)
      fOffsetZ = (2000 - (int) uiTime) * 3.2f;
    // setup text
    CTextGL cTitle;
    cTitle.SetAlpha(true);
    cTitle.SetColor(255, 255, 255);
    cTitle.SetJustification(E_CENTER);
    // set strings and invaders
    switch (uiMsg)
      {
      case 0:
        cTitle.SetFont(E_FONT_EUPHORIGENIC);
        cTitle.SetHeight(60.0f * fSize);
        cTitle.SetPosition(0.0f, -90.0f * fSize, fOffsetZ);
        cTitle.SetText("Fascination\nSoftware\nPresents");
        break;
      case 1:
        cTitle.SetFont(E_FONT_EUPHORIGENIC);
        cTitle.SetHeight(50.0f * fSize);
        cTitle.SetPosition(0.0f, -50.0f * fSize, fOffsetZ);
        cTitle.SetText("An OpenGL / SDL\nGame!");
        break;
      case 2:
        cTitle.SetFont(E_FONT_VECTROID);
        cTitle.SetHeight(60.0f * fSize);
        cTitle.SetPosition(0.0f, -30.0f * fSize, fOffsetZ);
        cTitle.SetText("Invasion 3D");
        break;
      }
    // draw text
    if (uiTime <= 2500) cTitle.Draw();
    }

  if (uiTime < 15000) return;
  uiTime -= 15000;

  // draw invaders
  if (uiTime < 4000)
    {
    unsigned int uiInv = (uiTime / 1000);
    uiTime = uiTime % 1000;
    float fOffsetZ = 1600.0f - (float) uiTime * 3.2f;
    switch (uiInv)
      {
      case 0:
        {
        CInvader10 cInvader;
        cInvader.SetPosition(-64.0f, -64.0f, fOffsetZ);
        if ((uiTime & 128) != 0) cInvader.ToggleLegs(true);
        cInvader.Draw();
        break;
        }
      case 1:
        {
        CInvader20 cInvader;
        cInvader.SetPosition(48.0f, -48.0f, fOffsetZ);
        if ((uiTime & 128) != 0) cInvader.ToggleLegs(true);
        cInvader.Draw();
        break;
        }
      case 2:
        {
        CInvader30 cInvader;
        cInvader.SetPosition(64.0f, 64.0f, fOffsetZ);
        if ((uiTime & 128) != 0) cInvader.ToggleLegs(true);
        cInvader.Draw();
        break;
        }
      case 3:
        {
        CInvaderMystery cInvader;
        cInvader.SetPosition(-32.0f, 32.0f, fOffsetZ);
        cInvader.SetAxis(0.0f, 1.0f, 0.0f);
        cInvader.SetRotation(uiTime * 0.36f);
        cInvader.Draw();
        break;
        }
      }
    }

}

/////////////////////////////////////////////////////////////////////////////
// CIntro Initialize

void CIntro::Initialize(CGameMain *pGameMain)
{
  // set pointer to main class for switching modes
  m_pGameMain = pGameMain;

  // set state time to 0
  m_uiStateTime = 0;

  // set positions of the stars
  for (unsigned int ui = 0; ui < NUM_STARS; ui++)
    {
    m_fStars[ui][0] = ((float) rand() / RAND_MAX) * 2000.0f - 1000.0f;
    m_fStars[ui][1] = ((float) rand() / RAND_MAX) * 2000.0f - 1000.0f;
    m_fStars[ui][2] = ((float) rand() / RAND_MAX) * 3200.0f - 1600.0f;
    float fColor = ((float) rand() / RAND_MAX) * 0.2f;
    m_fColors[ui][0] = fColor;
    m_fColors[ui][1] = fColor;
    m_fColors[ui][2] = fColor;
    }

}

/////////////////////////////////////////////////////////////////////////////
// CIntro Event Handling

void CIntro::ProcessEvents(void)
{
  SDL_Event event;

  // Grab all the events off the queue and toss them
  while (SDL_PollEvent(&event))
    {
    }
}


/////////////////////////////////////////////////////////////////////////////
// CIntro State Processing

void CIntro::ProcessState(unsigned int uiMillisec)
{
  m_uiStateTime += uiMillisec;

  // pull stars forward
  for (unsigned int ui = 0; ui < NUM_STARS; ui++)
    {
    m_fStars[ui][2] -= uiMillisec * 0.5f;
    float fColor = m_fColors[ui][0] + uiMillisec * 0.0002f;
    if (fColor > 1.0f) fColor = 1.0f;
    m_fColors[ui][0] = fColor;
    m_fColors[ui][1] = fColor;
    m_fColors[ui][2] = fColor;
    if (m_fStars[ui][2] < -1600.0f)
      {
      // loop star back to front
      m_fStars[ui][0] = ((float) rand() / RAND_MAX) * 2000.0f - 1000.0f;
      m_fStars[ui][1] = ((float) rand() / RAND_MAX) * 2000.0f - 1000.0f;
      m_fStars[ui][2] += 3200.0f;
      fColor = ((float) rand() / RAND_MAX) * 0.2f;
      m_fColors[ui][0] = fColor;
      m_fColors[ui][1] = fColor;
      m_fColors[ui][2] = fColor;
      }
    }

  // switch to demo mode at the end
  if (m_uiStateTime > 20000) m_pGameMain->SetMode(E_DEMO);
}

/////////////////////////////////////////////////////////////////////////////
// CIntro private modifiers

