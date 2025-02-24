/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - GameMain.cpp                                             *
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
**                              04/18/05                                   **
**                                                                         **
** GameMain.cpp - contains implementation of CGameMain class               **
****************************************************************************/

#ifdef WIN32
  #include <windows.h>
#endif
#if defined(__APPLE__)
  #include <SDL.h>
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <SDL/SDL.h>
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "GameMain.h"
#include "GameLogic.h"
#include "GLProfile.h"
#include "Settings.h"
#include "Invader10.h"
#include "Invader20.h"
#include "Invader30.h"
#include "InvaderMystery.h"
#include "Intro.h"
#include "Demo.h"
#include "Sound.h"
#include "Particles.h"
#include "TextGL.h"
#include "Textures.h"
#include "Package.h"
#include "Version.h"

// static CGameMain class members
bool CGameMain::m_bAlphaSupported = false;
bool CGameMain::m_bUseLuminance   = false;

/*  The main() function */
int main(int argc, char *argv[])
{
  CGameMain cTheApp;

  // fill in the path to the executable (for linux)
#ifdef WIN32
  cTheApp.SetExePath("");
#else
  char chPath[256];
  strcpy(chPath, argv[0]);
  char *pch = strrchr(chPath, '/');
  if (pch)
    pch[1] = 0;
  else
    chPath[0] = 0;
  cTheApp.SetExePath(chPath);
#endif

  printf("Invasion3D v%s\n", INVASION3D_VERSION);
  printf("Copyright (C) 2005,2011 Richard Goedeken\n");

  // initialize SDL and video mode
  if (!cTheApp.InitSDL())
      {
      return -1;
      }
  // initialize general OpenGL stuff
  if (!cTheApp.InitGL())
      {
      return -2;
      }
  // initialize the 3D models, textures for weapons and text, collision tables, particle tables, and sounds
  if (!cTheApp.StaticInit())
      {
      return -3;
      }
  // create the Intro, Demo, GameLogic, and Sound objects
  if (!cTheApp.InitObjects())
      {
      return -4;
      }

  // run the main loop
  cTheApp.MainLoop();

  // close everything down
  cTheApp.Shutdown();

  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CGameMain class constructor and destructor

CGameMain::CGameMain()
{
  // load the settings
  m_cSettings.LoadFromFile();

  // set up my screen parameters
  m_iScreenWidth  = m_cSettings.GetScreenWidth();
  m_iScreenHeight = m_cSettings.GetScreenHeight();
  m_bFullscreen   = m_cSettings.GetFullscreen();

  // set program mode to Intro and Frame number to 0
#if defined(_DEBUG)
  m_eGameMode    = E_DEMO;
#else
  m_eGameMode    = E_INTRO;
#endif
  m_uiModeTime        = 0;
  m_uiBulletTimeStart = 0;

  // set pointers to 0
  for (int i = 0; i < MAX_JOYSTICKS; i++)
    m_pJoystick[i] = NULL;
  m_pcDemo = NULL;
  m_pcIntro = NULL;

  // clear debug data
#if defined(_DEBUG)
  for (unsigned int ui = 0; ui < 128; ui++) m_fFrameTime[ui] = 0.0f;
  m_iCPUCircIdx = 0;
#endif

}

CGameMain::~CGameMain()
{
}

/////////////////////////////////////////////////////////////////////////////
// CGameMain class public functions

void CGameMain::MainLoop(void)
{
  // initialize local variables
  unsigned int uiFrameTime   = 1;
  unsigned int uiTimeStart   = SDL_GetTicks();

  // get start time
  unsigned int uiNextFrame = uiTimeStart;
  unsigned int uiFramesSkipped = 0;

  // main loop for the Invasion3D game
  while (m_eGameMode != E_QUIT)
    {
    unsigned int uiFrameTimeStart = uiNextFrame;
    // calculate time expansion factor
    float fTimeFactor = 1.0f;
    if (m_uiBulletTimeStart > 0)
      {
      if (m_uiModeTime - m_uiBulletTimeStart < 100)
        fTimeFactor = 1.0f - 0.008f * (m_uiModeTime - m_uiBulletTimeStart);
      else
        fTimeFactor = 0.20f;
      }
    switch (m_eGameMode)
      {
      case E_INTRO:
        m_pcIntro->ProcessEvents();
        m_pcIntro->ProcessState(uiFrameTime);
        break;
      case E_DEMO:
        m_pcDemo->ProcessEvents();
        m_pcDemo->ProcessState(uiFrameTime);
        break;
      case E_GAMERUN:
        m_pcGameLogic->ProcessEvents();
        m_pcGameLogic->ProcessState((unsigned int) (fTimeFactor * uiFrameTime));
        break;
      default:
        break;
      }
    GLProfile profile;
#if defined(GL_PROFILE)
    // set up profiling parameters and set the first timestamp
    profile.bGLFinishAfterEach = false;
    profile.uiFrameTimeMS = 10;
    profile.uiTimeProcess  = SDL_GetTicks();
#endif
    DrawFrame(profile);
#if defined(GL_PROFILE)
    // if we skipped a frame on the last redraw, paint a red box in the top left corner on this frame
    if (uiFramesSkipped > 0)
    {
      glDisable(GL_LIGHTING);
      glBegin(GL_QUADS);
      glColor4f(1.0, 0, 0, 1.0f);
      glVertex3f(-512.0f, -512.0f, -20.0f);
      glVertex3f(-512.0f, -300.0f, -20.0f);
      glVertex3f(-300.0f, -300.0f, -20.0f);
      glVertex3f(-300.0f, -512.0f, -20.0f);
      glEnd();
      glEnable(GL_LIGHTING);
    }
#endif

    // flip the double buffer
    SDL_GL_SwapBuffers();

#if defined(GL_PROFILE)
    // we must issue a GL command to make it wait for frame swap
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,100,-1600, 0,50,0, 0,-1,0);
    profile.uiTimeFlip  = SDL_GetTicks();

    // calculate GL profile timing
    uiFramesSkipped = profile.CalculateTiming(uiFrameTimeStart);
    if (m_eGameMode == E_GAMERUN && uiFramesSkipped > 0)
    {
      unsigned int uiTimeOther = profile.uiTimeClear + profile.uiTimeSetup + profile.uiTimeBases + profile.uiTimeOSD;
      printf("Skipped %i frames (Process: %i  Stars: %i  Shockwave:%i  Invaders: %i  Particles: %i  Weapons: %i  Special: %i  Other: %i  Flip: %i)\n", uiFramesSkipped, profile.uiTimeProcess, profile.uiTimeStars, profile.uiTimeShockwave, profile.uiTimeInvaders, profile.uiTimeParticles, profile.uiTimeWeapons, profile.uiTimeSpecial, uiTimeOther, profile.uiTimeFlip);
    }
#endif
    // calculate frame time to pass to ProcessState() in the next loop
    do 
      {
      //if (uiFrameTime < 5) SDL_Delay(1);
      uiNextFrame = SDL_GetTicks();
      uiFrameTime = (uiNextFrame - uiFrameTimeStart);
      } while(uiFrameTime < 5);  // cap max FPS at 200
    // set frame time in array for FPS display
#ifdef _DEBUG
    m_fFrameTime[m_iCPUCircIdx] = (float) uiFrameTime / 1000.0f;
    m_iCPUCircIdx = (m_iCPUCircIdx + 1) & 127;
#endif
    m_uiModeTime += uiFrameTime;
    }
  return;
}

void CGameMain::SetMode(EGameMode eMode)
{
  // handle startup for new mode
  if (eMode == E_DEMO)
    m_pcDemo->Initialize(this, &m_cSettings);
  else if (eMode == E_GAMERUN)
    m_pcGameLogic->Initialize(this, &m_cSettings, m_pcSound);

  // if the previous mode was Intro, re-set the projection matrix to have a smaller depth of
  // field so that things will look OK with a 16-bit Z buffer
  if (m_eGameMode == E_INTRO)
    {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, 4.0/3.0, 1400.0, 1800.0);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    }

#if defined(GL_PROFILE)
  const char *pccModes[4] = { "Intro", "Demo", "Running", "Quitting" };
  printf("Switching game mode to: %s\n", pccModes[eMode]);
#endif

  // switch modes
  m_eGameMode         = eMode;
  m_uiModeTime        = 0;
  m_uiBulletTimeStart = 0;
}

void CGameMain::SetBulletTime(bool bActive)
{
  if (bActive)
    m_uiBulletTimeStart = m_uiModeTime;
  else
    m_uiBulletTimeStart = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CGameMain class lower-level private functions

void CGameMain::DrawFrame(GLProfile &profile)
{
  // Clear the color and depth buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#if defined(GL_PROFILE)
  profile.uiTimeClear = SDL_GetTicks();
#endif

  switch (m_eGameMode)
    {
    case E_INTRO:
      // setup the coordinate system
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(0,0,-1600, 0,50,0, 0,-1,0);
      // draw the intro
      m_pcIntro->Draw();
      break;
    case E_DEMO:
      {
      // setup the coordinate system
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(0,100,-1600.0f, 0,50,0, 0,-1,0);
      // draw the demo screen
      m_pcDemo->Draw();
      // fade from black
      if (m_uiModeTime <= 1000)
        {
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
        glColor4f(0, 0, 0, (1000 - m_uiModeTime) / 1000.0f);
        glVertex3f(-512.0f, -512.0f, -55.0f);
        glVertex3f(-512.0f,  512.0f, -55.0f);
        glVertex3f( 512.0f,  512.0f, -55.0f);
        glVertex3f( 512.0f, -512.0f, -55.0f);
        glEnd();
        glEnable(GL_LIGHTING);
        }
      break;
      }
    case E_GAMERUN:
    {
      // setup the coordinate system
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(0,100,-1600, 0,50,0, 0,-1,0);
#if defined(GL_PROFILE)
      profile.uiTimeSetup = SDL_GetTicks();
#endif
      // draw the gameplay screen
      m_pcGameLogic->Draw(profile);
      break;
    }
    default:
      break;
    }
}

/////////////////////////////////////////////////////////////////////////////
// Linux compatibility functions

#if !defined(WIN32)

void *_aligned_malloc(size_t sz, size_t alignment)
{
  void    *pvData;

#if defined(__sgi)
  pvData = memalign(alignment, sz);
#else
  if (posix_memalign(&pvData, alignment, sz)) pvData = NULL;
#endif

  return pvData;
}

void _aligned_free(void *pvData)
{
  if (pvData) free(pvData);
}

#endif

/////////////////////////////////////////////////////////////////////////////
// CGameMain class Initialize / Shutdown functions

bool CGameMain::InitSDL(void)
{
    const SDL_VideoInfo* psInfo = NULL;
    int iBPP = 0;
    int iFlags = 0;

    // First, initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
        {
        printf("Video initialization failed: %s\n", SDL_GetError());
        return false;
        }
    atexit(SDL_Quit);

    // activate joystick event notifications
    if (SDL_JoystickEventState(SDL_ENABLE) != SDL_ENABLE)
        {
        printf("Failed to initialize joystick event notification!\n");
        return false;
        }

    // activate each joystick
    int iNumSticks = SDL_NumJoysticks();
    if (iNumSticks > MAX_JOYSTICKS) iNumSticks = MAX_JOYSTICKS;
    for (int i = 0; i < iNumSticks; i++)
      {
      m_pJoystick[i] = SDL_JoystickOpen(i);
      }

    // Get display information.
    psInfo = SDL_GetVideoInfo( );
    iBPP = psInfo->vfmt->BitsPerPixel;

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);      // at least 5 bits of red
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);    // at least 5 bits of green
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);     // at least 5 bits of blue
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);   // at least 16 bit screen depth
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  // use double-buffered screen

    iFlags = SDL_OPENGL;
    if (m_bFullscreen)
        {
        iFlags |= SDL_FULLSCREEN;
        if (SDL_SetVideoMode(m_iScreenWidth, m_iScreenHeight, iBPP, iFlags) == 0)
            {
            printf("Video mode set failed: %s\n", SDL_GetError());
            return false;
            }
        // get current screen resolution
        SDL_Surface *pScreen = SDL_GetVideoSurface();
        m_iScreenWidth = pScreen->w;
        m_iScreenHeight = pScreen->h;
        }
    else
        {
        // create a windowed surface with the given dimensions
        if (SDL_SetVideoMode(m_iScreenWidth, m_iScreenHeight, iBPP, iFlags) == 0)
            {
            printf("Video mode set failed: %s\n", SDL_GetError());
            return false;
            }
        }

    // set window name
    SDL_WM_SetCaption("Invasion 3D! An OpenGL game", "Invasion3D");

    return true;
}

bool CGameMain::InitGL(void)
{
    // Flat shading model
    glShadeModel(GL_FLAT);

    // Culling
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    // Set the clear color
    glClearColor(0, 0, 0, 0);

    // Setup viewport
    glViewport(0, 0, m_iScreenWidth, m_iScreenHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // don't use perspective
    //glOrtho(-600, 600, 500, -400, -500.0f, 500.0f);
    //glOrtho(-200, 200, 150, -150, -500.0f, 500.0f);
    //glOrtho(-100, 100, 75, -75, -500.0f, 500.0f);

    // use perspective
    gluPerspective(30.0, 4.0/3.0, 10.0, 3200.0);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // set up z-buffer parameters
    //glClearDepth(0.0f);
    //glDepthFunc(GL_GEQUAL);	
    glDepthFunc(GL_LESS);	
    glEnable(GL_DEPTH_TEST);

    // set up lighting
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_LIGHTING);
    GLfloat lightAmbient[4]  = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat lightDiffuse[4]  = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightPosition[4] = {1.0f, 0.0f, 1.0f, 0.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightAmbient) ;
    glLightfv(GL_LIGHT1, GL_AMBIENT,  lightDiffuse);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  lightDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightDiffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
    glEnable(GL_LIGHT1);

    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SDL_GL_SwapBuffers();

    // turn alpha on always
    m_bAlphaSupported = true;

    // determine if we're running on a Mach64
    const char *pccRenderer = (const char *) glGetString(GL_RENDERER);
    if (pccRenderer != NULL && strstr(pccRenderer, "Rage Pro") != NULL)
      m_bUseLuminance = false;
    else
      m_bUseLuminance = true;

    return true;
}

bool CGameMain::StaticInit(void)
{
  // get path to media file
  char chMediapath[256];
  strcpy(chMediapath, m_chExePath);
  strcat(chMediapath, "Invaders.dat");

  // read a Package file with our media in it
  CPackage cMedia;
  if (!cMedia.LoadPackage(chMediapath)) return false;

  // initialize the TextGL fonts
  if (!CTextGL::InitFonts(&cMedia)) return false;

  // initialize the textures
  if (!CTextures::InitTextures(&cMedia)) return false;

  // build the 10-point invader model
  if (!CInvader10::InitModel()) return false;
  // build the 20-point invader model
  if (!CInvader20::InitModel()) return false;
  // build the 30-point invader model
  if (!CInvader30::InitModel()) return false;
  // build the Mystery invader model
  if (!CInvaderMystery::InitModel()) return false;

  // setup the collision data arrays
  if (!CInvader::SetupCollisionData()) return false;

  // setup some static arrays for the Particles
  if (!CParticles::StaticSetup()) return false;

  // Sound is a good thing
  m_pcSound = new CSound;
  if (!m_pcSound) return false;

  // setup the CSound static data
  if (!CSound::InitSound(&cMedia)) return false;

  return true;
}

bool CGameMain::InitObjects(void)
{
  // create an Intro object to display the opening sequence
  m_pcIntro = new CIntro();
  if (!m_pcIntro) return false;
  m_pcIntro->Initialize(this);

  // the demo object handles the Attraction Demo (idle screen)
  m_pcDemo = new CDemo;
  if (!m_pcDemo) return false;
  m_pcDemo->Initialize(this, &m_cSettings);

  // the GameLogic object handles the main gameplay
  m_pcGameLogic = new CGameLogic;
  if (!m_pcGameLogic) return false;

  return true;
}

void CGameMain::Shutdown(void)
{
  // destroy objects
  delete m_pcDemo;
  delete m_pcIntro;
  delete m_pcGameLogic;
  delete m_pcSound;

  // destroy the 10-point invader model
  CInvader10::DestroyModel();
  // destroy the 20-point invader model
  CInvader20::DestroyModel();
  // destroy the 30-point invader model
  CInvader30::DestroyModel();
  // destroy the Mystery invader model
  CInvaderMystery::DestroyModel();

  // destroy the fonts
  CTextGL::DestroyFonts();
  // destroy the textures
  CTextures::DestroyTextures();
  // destroy the Particles static data
  CParticles::StaticShutdown();
  // destroy the sound clips
  CSound::DestroySound();

  return;
}

bool CGameMain::ChangeScreen(void)
{
  const SDL_VideoInfo* psInfo = NULL;
  int iBPP = 0;
  int iFlags = 0;

  // set up my screen parameters
  m_iScreenWidth  = m_cSettings.GetScreenWidth();
  m_iScreenHeight = m_cSettings.GetScreenHeight();
  m_bFullscreen   = m_cSettings.GetFullscreen();

  // destroy the fonts and textures
  CTextGL::DestroyFonts();
  CTextures::DestroyTextures();

  // shut down video
  SDL_QuitSubSystem(SDL_INIT_VIDEO);

  // restart video
  SDL_InitSubSystem(SDL_INIT_VIDEO);

  // Get display information.
  psInfo = SDL_GetVideoInfo();
  iBPP = psInfo->vfmt->BitsPerPixel;

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);      // at least 5 bits of red
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);    // at least 5 bits of green
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);     // at least 5 bits of blue
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);   // at least 16 bit screen depth
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  // use double-buffered screen

  iFlags = SDL_OPENGL;
  if (m_bFullscreen)
      {
      iFlags |= SDL_FULLSCREEN;
      if (SDL_SetVideoMode(m_iScreenWidth, m_iScreenHeight, iBPP, iFlags) == 0)
          {
          printf("Video mode set failed: %s\n", SDL_GetError());
          return false;
          }
      // get current screen resolution
      SDL_Surface *pScreen = SDL_GetVideoSurface();
      m_iScreenWidth = pScreen->w;
      m_iScreenHeight = pScreen->h;
      }
  else
      {
      // create a windowed surface with the given dimensions
      if (SDL_SetVideoMode(m_iScreenWidth, m_iScreenHeight, iBPP, iFlags) == 0)
          {
          printf("Video mode set failed: %s\n", SDL_GetError());
          return false;
          }
      }

  // set window name
  SDL_WM_SetCaption("Invasion 3D! An OpenGL game", "Invasion3D");

  // re-start OpenGL
  if (!InitGL()) return false;

  // get path to media file
  char chMediapath[256];
  strcpy(chMediapath, m_chExePath);
  strcat(chMediapath, "Invaders.dat");

  // read a Package file with our media in it
  CPackage cMedia;
  if (!cMedia.LoadPackage(chMediapath)) return false;

  // initialize the TextGL fonts
  if (!CTextGL::InitFonts(&cMedia)) return false;

  // initialize the textures
  if (!CTextures::InitTextures(&cMedia)) return false;

  return true;
}

/////////////////////////////////////////////////////////////////////////////
// CGameMain key conversion table

SKeyConv g_asKeys[] = {{SDLK_BACKSPACE,	  "Backspace"},
                       {SDLK_TAB,		      "Tab"},
                       {SDLK_CLEAR,		    "Clear"},
                       {SDLK_RETURN,	    "Enter"},
                       {SDLK_PAUSE,		    "Pause"},
                       {SDLK_ESCAPE,	    "ESC"},
                       {SDLK_SPACE,		    "Space"},
                       {SDLK_EXCLAIM,     "!"},
                       {SDLK_QUOTEDBL,	  "\""},
                       {SDLK_HASH,		    "#"},
                       {SDLK_DOLLAR,		  "$"},
                       {SDLK_AMPERSAND,	  "&"},
                       {SDLK_QUOTE,		    "'"},
                       {SDLK_LEFTPAREN,	  "("},
                       {SDLK_RIGHTPAREN,  ")"},
                       {SDLK_ASTERISK,	  "*"},
                       {SDLK_PLUS,		    "+"},
                       {SDLK_COMMA,		    ","},
                       {SDLK_MINUS,		    "-"},
                       {SDLK_PERIOD,		  "."},
                       {SDLK_SLASH,		    "/"},
                       {SDLK_0,		        "0"},
                       {SDLK_1,		        "1"},
                       {SDLK_2,		        "2"},
                       {SDLK_3,		        "3"},
                       {SDLK_4,		        "4"},
                       {SDLK_5,		        "5"},
                       {SDLK_6,		        "6"},
                       {SDLK_7,		        "7"},
                       {SDLK_8,		        "8"},
                       {SDLK_9,		        "9"},
                       {SDLK_COLON,		    ":"},
                       {SDLK_SEMICOLON,	  ";"},
                       {SDLK_LESS,		    "<"},
                       {SDLK_EQUALS,		  "="},
                       {SDLK_GREATER,		  ">"},
                       {SDLK_QUESTION,	  "?"},
                       {SDLK_AT,		      "@"},
                       {SDLK_LEFTBRACKET,	"["},
                       {SDLK_BACKSLASH,	  "Backslash"},
                       {SDLK_RIGHTBRACKET,"]"},
                       {SDLK_CARET,		    "^"},
                       {SDLK_UNDERSCORE,  "_"},
                       {SDLK_BACKQUOTE,	  "Grave"},
                       {SDLK_a,		        "a"},
                       {SDLK_b,		        "b"},
                       {SDLK_c,		        "c"},
                       {SDLK_d,		        "d"},
                       {SDLK_e,		        "e"},
                       {SDLK_f,		        "f"},
                       {SDLK_g,		        "g"},
                       {SDLK_h,		        "h"},
                       {SDLK_i,		        "i"},
                       {SDLK_j,		        "j"},
                       {SDLK_k,		        "k"},
                       {SDLK_l,		        "l"},
                       {SDLK_m,		        "m"},
                       {SDLK_n,		        "n"},
                       {SDLK_o,		        "o"},
                       {SDLK_p,		        "p"},
                       {SDLK_q,		        "q"},
                       {SDLK_r,		        "r"},
                       {SDLK_s,		        "s"},
                       {SDLK_t,		        "t"},
                       {SDLK_u,		        "u"},
                       {SDLK_v,		        "v"},
                       {SDLK_w,		        "w"},
                       {SDLK_x,		        "x"},
                       {SDLK_y,		        "y"},
                       {SDLK_z,		        "z"},
                       {SDLK_DELETE,		  "Delete"},
                       {SDLK_KP0,	 	      "Keypad0"},
                       {SDLK_KP1,	 	      "Keypad1"},
                       {SDLK_KP2,	 	      "Keypad2"},
                       {SDLK_KP3,	 	      "Keypad3"},
                       {SDLK_KP4,	 	      "Keypad4"},
                       {SDLK_KP5,	 	      "Keypad5"},
                       {SDLK_KP6,	 	      "Keypad6"},
                       {SDLK_KP7,         "Keypad7"},
                       {SDLK_KP8,	 	      "Keypad8"},
                       {SDLK_KP9,	 	      "Keypad9"},
                       {SDLK_KP_PERIOD,   "Keypad."},
                       {SDLK_KP_DIVIDE,	  "Keypad/"},
                       {SDLK_KP_MULTIPLY, "Keypad*"},
                       {SDLK_KP_MINUS,	  "Keypad-"},
                       {SDLK_KP_PLUS,		  "Keypad+"},
                       {SDLK_KP_ENTER,	  "Keypad-Enter"},
                       {SDLK_KP_EQUALS,	  "Keypad="},
                       {SDLK_UP,	 	      "Up-Arrow"},
                       {SDLK_DOWN,	 	    "Down-Arrow"},
                       {SDLK_RIGHT,	 	    "Right-Arrow"},
                       {SDLK_LEFT,	 	    "Left-Arrow"},
                       {SDLK_INSERT,	 	  "Insert"},
                       {SDLK_HOME,	 	    "Home"},
                       {SDLK_END,	 	      "End"},
                       {SDLK_PAGEUP,	   	"Page-Up"},
                       {SDLK_PAGEDOWN, 	  "Page-Down"},
                       {SDLK_F1,	 	      "F1"},
                       {SDLK_F2,	      	"F2"},
                       {SDLK_F3,	 	      "F3"},
                       {SDLK_F4,	 	      "F4"},
                       {SDLK_F5,	 	      "F5"},
                       {SDLK_F6,	 	      "F6"},
                       {SDLK_F7,	 	      "F7"},
                       {SDLK_F8,	 	      "F8"},
                       {SDLK_F9,	 	      "F9"},
                       {SDLK_F10,	 	      "F10"},
                       {SDLK_F11,	 	      "F11"},
                       {SDLK_F12,	 	      "F12"},
                       {SDLK_F13,	 	      "F13"},
                       {SDLK_F14,	 	      "F14"},
                       {SDLK_F15,	 	      "F15"},
                       {SDLK_RSHIFT,	    "Right-Shift"},
                       {SDLK_LSHIFT,	    "Left-Shift"},
                       {SDLK_RCTRL,	 	    "Right-Ctrl"},
                       {SDLK_LCTRL,	 	    "Left-Ctrl"},
                       {SDLK_RALT,	 	    "Right-Alt"},
                       {SDLK_LALT,	 	    "Left-Alt"}
                       };

