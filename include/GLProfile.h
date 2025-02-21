/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - GLProfile.h                                              *
 *   Homepage: http://code.google.com/p/invasion3d/                        *
 *   Copyright (C) 2025 Richard Goedeken                                   *
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


#if !defined(GLPROFILE_H)
#define GLPROFILE_H

class GLProfile
{
public:
  GLProfile() { }
  ~GLProfile() { }

  unsigned int CalculateTiming(unsigned int uiFrameTimeStart)
  {
      unsigned int uiSkippedFrames = 0;
      if (uiTimeFlip - uiFrameTimeStart >= uiFrameTimeMS * 3 / 2)
        uiSkippedFrames = ((uiTimeFlip - uiFrameTimeStart + uiFrameTimeMS/2) / uiFrameTimeMS) - 1;
      uiTimeFlip -= uiTimeSpecial;
      uiTimeSpecial -= uiTimeWeapons;
      uiTimeWeapons -= uiTimeParticles;
      uiTimeParticles -= uiTimeOSD;
      uiTimeOSD -= uiTimeBases;
      uiTimeBases -= uiTimeInvaders;
      uiTimeInvaders -= uiTimeShockwave;
      uiTimeShockwave -= uiTimeStars;
      uiTimeStars -= uiTimeSetup;
      uiTimeSetup -= uiTimeClear;
      uiTimeClear -= uiTimeProcess;
      uiTimeProcess -= uiFrameTimeStart;
      return uiSkippedFrames;
  }

  // profiling parameters
  bool bGLFinishAfterEach;
  unsigned int uiFrameTimeMS;

  // time counters
  unsigned int uiTimeProcess;
  unsigned int uiTimeClear;
  unsigned int uiTimeSetup;
  unsigned int uiTimeStars;
  unsigned int uiTimeShockwave;
  unsigned int uiTimeInvaders;
  unsigned int uiTimeBases;
  unsigned int uiTimeOSD;
  unsigned int uiTimeParticles;
  unsigned int uiTimeWeapons;
  unsigned int uiTimeSpecial;
  unsigned int uiTimeFlip;
};

#endif // GLPROFILE_H
