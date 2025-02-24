/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Ticker.cpp                                               *
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
**                              05/19/05                                   **
**                                                                         **
** Ticker.cpp - contains implementation of CTicker class, used to print a  **
**              line of text at the bottom of the screen.                  **
**                                                                         **
****************************************************************************/

#ifdef WIN32
  #include <windows.h>
#endif
#if defined(__APPLE__)
  #include <OpenGL/gl.h>
#else
  #include <GL/gl.h>
#endif
#include <stdio.h>
#include <stdarg.h>

#include "Ticker.h"
#include "TextGL.h"

// static data declaration
const float CTicker::m_cfTickerSpeed = 0.24f;
const float CTicker::m_cfSeparation  = 50.0f;
const float CTicker::m_cfLineHeight  = 30.0f;
const float CTicker::m_cfLinePosY    = 444.0f;

/////////////////////////////////////////////////////////////////////////////
// CTicker constructor and destructor

CTicker::CTicker()
{
  m_uiNumActive = 0;
  m_uiFirstActive = 0;

  // set the attributes of the messages
  for (unsigned int ui = 0; ui < 8; ui++)
    {
    m_cStrings[ui].SetAlpha(true);
    m_cStrings[ui].SetColor(255, 0, 0);
    m_cStrings[ui].SetFont(E_FONT_6809);
    m_cStrings[ui].SetHeight(m_cfLineHeight);
    m_cStrings[ui].SetJustification(E_LEFT);
    }
}

CTicker::~CTicker()
{
}

/////////////////////////////////////////////////////////////////////////////
// CTicker public accessor functions

void CTicker::Draw(void) const
{
  // draw all of the messages on the screen
  for (unsigned int uiMsg = 0; uiMsg < m_uiNumActive; uiMsg++)
    {
    unsigned int uiIdx = (m_uiFirstActive + uiMsg) & 7;
    m_cStrings[uiIdx].Draw();
    if (m_cStrings[uiIdx].PositionX() + m_cStrings[uiIdx].Width() > 640.0f) break;
    }
}


/////////////////////////////////////////////////////////////////////////////
// CTicker public modifier functions

bool CTicker::AddMessage(const char *pchMessage, ...)
{
  va_list vaList;
  char pchString[1024];

  // check input conditions
  if (!pchMessage) return false;
  if (m_uiNumActive == 8) return false;

  // initialize pointer to variable argument list
  va_start(vaList, pchMessage);

  // print query string with expanded % terms
  vsprintf(pchString, pchMessage, vaList);

  // get the index of the CTextGL object for this message
  unsigned int uiNew = (m_uiFirstActive + m_uiNumActive) & 7;

  // get the starting X coordinate of this message
  float fStartX = 640.0f;
  if (m_uiNumActive > 0)
    {
    unsigned int uiPreceding = (m_uiFirstActive + m_uiNumActive - 1) & 7;
    float fEndX = m_cStrings[uiPreceding].PositionX() + m_cStrings[uiPreceding].Width() + m_cfSeparation;
    if (fEndX > fStartX) fStartX = fEndX;
    }

  // set up this text object
  m_cStrings[uiNew].SetText(pchString);
  m_cStrings[uiNew].SetPosition(fStartX, m_cfLinePosY, 0.0f);

  // increment the number of active messages
  m_uiNumActive++;

  // all done
  return true;
}

void CTicker::AdvanceState(unsigned int uiFrameTime)
{
  if (m_uiNumActive == 0) return;

  // move all of the messages to the left
  for (unsigned int uiMsg = 0; uiMsg < m_uiNumActive; uiMsg++)
    {
    unsigned int uiIdx = (m_uiFirstActive + uiMsg) & 7;
    float fNewX = m_cStrings[uiIdx].PositionX() - m_cfTickerSpeed * (float) uiFrameTime;
    m_cStrings[uiIdx].SetPosition(fNewX, m_cfLinePosY, 0.0f);
    }

  // if the first message is off the screen, then get rid of it
  if (m_cStrings[m_uiFirstActive].PositionX() + m_cStrings[m_uiFirstActive].Width() < -640.0f)
    {
    m_uiFirstActive = (m_uiFirstActive + 1) & 7;
    m_uiNumActive--;
    }
}
