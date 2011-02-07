/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Ticker.h                                                 *
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


#if !defined(TICKER_H)
#define TICKER_H

#ifdef WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>

#include "TextGL.h"

class CTicker
{
public:
  CTicker();
  ~CTicker();

  // accessor functions
  void Draw(void) const;

  // modifier functions
  bool AddMessage(const char *pchMessage, ...);
  void AdvanceState(unsigned int uiFrameTime);
  void Clear(void) { m_uiNumActive = 0; }

private:
  // member data
  CTextGL        m_cStrings[8];
  unsigned int   m_uiNumActive;
  unsigned int   m_uiFirstActive;

  // const static data
  const static float m_cfTickerSpeed;
  const static float m_cfSeparation;
  const static float m_cfLineHeight;
  const static float m_cfLinePosY;
};

#endif // defined(TICKER_H)
