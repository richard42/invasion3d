/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Intro.h                                                  *
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

#if !defined(INTRO_H)
#define INTRO_H

#define NUM_STARS 4200

class CGameMain;

class CIntro
{
public:
  CIntro();
  ~CIntro();

  // accessor functions
  void Draw(void) const;

  // modifier functions
  void Initialize(CGameMain *pGameMain);
  void ProcessState(unsigned int uiMillisec);
  void ProcessEvents(void);

private:
  // member data
  CGameMain       *m_pGameMain;
  float            m_fStars[NUM_STARS][3];
  float            m_fColors[NUM_STARS][3];
  unsigned int     m_uiStateTime;

  // private functions

};

#endif // defined(INTRO_H)
