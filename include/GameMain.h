/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - GameMain.h                                               *
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


#if !defined(GAMEMAIN_H)
#define GAMEMAIN_H

#include <SDL/SDL.h>
#include <string.h>
#include "Settings.h"

class CDemo;
class CIntro;
class CSound;
class CGameLogic;

#define MAX_JOYSTICKS 4

typedef struct {SDLKey  symbol;
                char    chName[16];
                } SKeyConv;

typedef enum {E_INTRO,
              E_DEMO,
              E_GAMERUN,
              E_QUIT} EGameMode;


// global key conversion table
extern SKeyConv g_asKeys[118];

// Linux compatibility functions
#if !defined(WIN32)
  typedef long long int __int64;
  typedef long long int LARGE_INTEGER;
  void QueryPerformanceCounter(__int64 *liValue);
  void QueryPerformanceFrequency(__int64 *liFreq);
  void *_aligned_malloc(size_t sz, size_t alignment);
  void _aligned_free(void *pvData);
#endif


class CGameMain
{
public:
  CGameMain();
  ~CGameMain();

  // public functions
  bool InitSDL(void);
  bool InitGL(void);
  void MainLoop(void);
  bool StaticInit(void);
  bool InitObjects(void);
  void Shutdown(void);
  void SetMode(EGameMode eMode);
  void SetBulletTime(bool bActive);
  bool ChangeScreen(void);
  void SetExePath(const char *chExePath) { strcpy(m_chExePath, chExePath); }

  // public static data members
  static bool m_bAlphaSupported;
  static bool m_bUseLuminance;

  // debug data and functions
#if defined(_DEBUG)
  float     m_fFrameTime[128];
  int       m_iCPUCircIdx;
  float     GetFPS(void) const { float f1 = 0.0001f; for (int i = 0; i < 128; i++) f1 += m_fFrameTime[i]; return 128.0f / f1; }
#endif

private:
  // main sub-functions
  void DrawFrame(void);

  // SDL joystick pointers
  SDL_Joystick *m_pJoystick[MAX_JOYSTICKS];

  // object arrays
  CSettings    m_cSettings;
  CDemo       *m_pcDemo;
  CIntro      *m_pcIntro;
  CSound      *m_pcSound;
  CGameLogic  *m_pcGameLogic;

  // general member data
  unsigned int m_uiModeTime;
  unsigned int m_uiBulletTimeStart;
  int          m_iScreenWidth;
  int          m_iScreenHeight;
  bool         m_bFullscreen;
  bool         m_bQuit;
  EGameMode    m_eGameMode;
  char         m_chExePath[256];

};

#endif // !defined(GAMEMAIN_H)
