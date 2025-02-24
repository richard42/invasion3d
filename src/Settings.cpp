/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Settings.cpp                                             *
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
**                              04/28/05                                   **
**                                                                         **
** Settings.cpp - contains interface for CSettings class used for handling **
**                input setup and graphical options                        **
****************************************************************************/

#ifdef WIN32
  #include <windows.h>
  #include <shlobj.h>
  #include <shlwapi.h>
#endif
#if defined(__APPLE__)
  #include <SDL.h>
#else
  #include <SDL/SDL.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GameMain.h"
#include "Settings.h"

#if !defined(_MAX_PATH)
  #define _MAX_PATH 2048
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettings class constructor and destructor

CSettings::CSettings()
{
  // set the default setting
  SetDefaults();
  m_iScreenWidth  = 800;
  m_iScreenHeight = 600;
  m_bFullscreen   = false;
}

CSettings::~CSettings()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSettings class public accessors

bool CSettings::SaveToFile(void) const
{
  char chFilepath[_MAX_PATH];

  // get filename to save to
  if (!GetSettingsPathname(chFilepath))
    {
    printf("CSettings::SaveToFile Error: Couldn't get pathname for file to save!\n");
    return false;
    }

  // open the file
  FILE *fOut = fopen(chFilepath, "w");
  if (!fOut)
    {
    printf("CSettings::SaveToFile Error: Couldn't open file \"%s\" to save!\n", chFilepath);
    return false;
    }

  // dump out the keyboard and joystick settings
  for (int iCmd = E_INPUT_FIRST_COMMAND; iCmd <= E_INPUT_LAST_COMMAND; iCmd++)
    {
    // find name for this key
    int iKey;
    for (iKey = 0; iKey < 118; iKey++)
      {
      if (m_asKeyCommands[iCmd] == g_asKeys[iKey].symbol) break;
      }
    if (iKey < 118)
      fprintf(fOut, "Key_%s = %s\n", m_chCommandNames[iCmd], g_asKeys[iKey].chName);
    fprintf(fOut, "Joy_%s = %i,%i,%i,%i\n", m_chCommandNames[iCmd], m_asJoyCommands[iCmd].iJoystick, m_asJoyCommands[iCmd].bAxis, m_asJoyCommands[iCmd].bPositive, m_asJoyCommands[iCmd].iIndex);
    }

  // dump out the screen settings
  fprintf(fOut, "Screen_Full = %i\nScreen_Width = %i\nScreen_Height = %i\n", m_bFullscreen, m_iScreenWidth, m_iScreenHeight);

  // dump out the high score list
  for (int iScore = 0; iScore < 10; iScore++)
    {
    fprintf(fOut, "Score_\"%s\" = %i\n", m_asHighScores[iScore].chName, m_asHighScores[iScore].iScore);
    }

  // close the file
  fclose(fOut);

  // all done
  return true;
}

ECommand CSettings::GetCommand(SDLKey sKeySymbol) const
{
  int iCmd;

  for (iCmd = E_INPUT_FIRST_COMMAND; iCmd <= E_INPUT_LAST_COMMAND; iCmd++)
    {
    if (m_asKeyCommands[iCmd] == sKeySymbol) break;
    }

  return (ECommand) iCmd;
}

ECommand CSettings::GetCommand(int iJoystick, bool bAxis, int iIndex, bool bPositive) const
{
  int iCmd;

  for (iCmd = E_INPUT_FIRST_COMMAND; iCmd <= E_INPUT_LAST_COMMAND; iCmd++)
    {
    if (m_asJoyCommands[iCmd].iJoystick == iJoystick &&
        m_asJoyCommands[iCmd].bAxis     == bAxis &&
        m_asJoyCommands[iCmd].iIndex    == iIndex &&
        m_asJoyCommands[iCmd].bPositive == bPositive) break;
    }

  return (ECommand) iCmd;
}

bool CSettings::GetCommandName(ECommand eCommand, char *pchName) const
{
  // check inputs
  if (eCommand < E_INPUT_FIRST_COMMAND || eCommand > E_INPUT_LAST_COMMAND) return false;
  if (!pchName) return false;

  strcpy(pchName, m_chCommandNames[eCommand]);
  return true;
}

bool CSettings::GetKey(ECommand eCommand, SDLKey *psKeySymbol, char *pchKeyName) const
{
  // check inputs
  if (eCommand < E_INPUT_FIRST_COMMAND || eCommand > E_INPUT_LAST_COMMAND) return false;
  if (!psKeySymbol) return false;

  // set the key symbol
  *psKeySymbol = m_asKeyCommands[eCommand];

  // set the name if necessary
  if (pchKeyName)
    {
    pchKeyName[0] = 0;
    int i;
    for (i = 0; i < 118; i++)
      {
      if (g_asKeys[i].symbol == *psKeySymbol) break;
      }
    if (i < 118) strcpy(pchKeyName, g_asKeys[i].chName);
    }

  return true;
}

bool CSettings::GetJoystickMovement(ECommand eCommand, int *piJoystick, bool *pbAxis, int *piIndex, bool *pbPositive) const
{
  // check inputs
  if (eCommand < E_INPUT_FIRST_COMMAND || eCommand > E_INPUT_LAST_COMMAND) return false;

  // set the values
  if (piJoystick) *piJoystick = m_asJoyCommands[eCommand].iJoystick;
  if (pbAxis)     *pbAxis     = m_asJoyCommands[eCommand].bAxis;
  if (piIndex)    *piIndex    = m_asJoyCommands[eCommand].iIndex;
  if (pbPositive) *pbPositive = m_asJoyCommands[eCommand].bPositive;

  return true;
}

int CSettings::GetHighScore(int iPlace, char *pchName)
{
  if (iPlace < 1 || iPlace > 10) return 0;

  if (pchName) strcpy(pchName, m_asHighScores[iPlace - 1].chName);
  return m_asHighScores[iPlace - 1].iScore;
}

bool CSettings::GetFullscreen() const
{
  return m_bFullscreen;
}

int CSettings::GetScreenWidth() const
{
  return m_iScreenWidth;
}

int CSettings::GetScreenHeight() const
{
  return m_iScreenHeight;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings class public modifiers

bool CSettings::LoadFromFile(void)
{
  char chFilepath[_MAX_PATH];

  // get filename to save to
  if (!GetSettingsPathname(chFilepath))
    {
    printf("CSettings::LoadFromFile Error: Couldn't get pathname for file to save!\n");
    return false;
    }

  // open the file
  FILE *fIn = fopen(chFilepath, "rb");
  if (!fIn)
    { /* couldn't open file. it won't be their the first time invasion3d is run */
    /* so lets try saving the default settings */
    SaveToFile();
    /* if that worked we should be able to load it back */
    fIn = fopen(chFilepath, "rb");
    }
  if (!fIn)
    {
    printf("CSettings::LoadFromFile Error: Couldn't open file \"%s\" to load!\n", chFilepath);
    return false;
    }

  // find out how big this file is
  fseek(fIn, 0, SEEK_END);
  int iLength = ftell(fIn);
  fseek(fIn, 0, SEEK_SET);

  // allocate memory to store the file
  char *pchSettings = (char *) malloc(iLength + 16);
  if (!pchSettings)
    {
    printf("CSettings::LoadFromFile Error: Couldn't allocate %i bytes of memory to put settings file!\n", iLength + 16);
    return false;
    }

  // load it
  if (fread(pchSettings, 1, iLength, fIn) < (size_t) iLength)
    {
    printf("CSettings::LoadFromFile Error: Couldn't read %i bytes from settings file!\n", iLength);
    free(pchSettings);
    return false;
    }
  pchSettings[iLength] = 0;

  // close the file
  fclose(fIn);

  // now parse this thing
  char *pch = pchSettings;
  while (*pch != 0 && (int) (pch - pchSettings) < iLength)
    {
    // look for a keyboard, joystick, or screen setting
    if (strncmp(pch, "Key_", 4) == 0)
      {
      pch += 4;
      // search for command name
      int iCmd;
      for (iCmd = E_INPUT_FIRST_COMMAND; iCmd <= E_INPUT_LAST_COMMAND; iCmd++)
        {
        if (strncmp(pch, m_chCommandNames[iCmd], strlen(m_chCommandNames[iCmd])) == 0) break;
        }
      if (iCmd <= E_INPUT_LAST_COMMAND)
        {
        // skip command name
        pch += strlen(m_chCommandNames[iCmd]);
        // skip white space and equals
        while (*pch == ' ' || *pch == '\t') pch++;
        while (*pch == '=') pch++;
        while (*pch == ' ' || *pch == '\t') pch++;
        // look for key name
        int iKey;
        for (iKey = 0; iKey < 118; iKey++)
          {
          if (strncmp(pch, g_asKeys[iKey].chName, strlen(g_asKeys[iKey].chName)) == 0) break;
          }
        // assign this command if we have found the right key
        if (iKey < 118)
          {
          m_asKeyCommands[iCmd] = g_asKeys[iKey].symbol;
          }
        }
      }
    else if (strncmp(pch, "Joy_", 4) == 0)
      {
      pch += 4;
      // search for command name
      int iCmd;
      for (iCmd = E_INPUT_FIRST_COMMAND; iCmd <= E_INPUT_LAST_COMMAND; iCmd++)
        {
        if (strncmp(pch, m_chCommandNames[iCmd], strlen(m_chCommandNames[iCmd])) == 0) break;
        }
      if (iCmd <= E_INPUT_LAST_COMMAND)
        {
        // skip command name
        pch += strlen(m_chCommandNames[iCmd]);
        // skip white space and equals
        while (*pch == ' ' || *pch == '\t') pch++;
        while (*pch == '=') pch++;
        while (*pch == ' ' || *pch == '\t') pch++;
        // try to get the parameters
        int  iJoystick, iIndex, iAxis, iPositive;
        int iParams = sscanf(pch, "%i,%i,%i,%i", &iJoystick, &iAxis, &iPositive, &iIndex);
        if (iParams == 4)
          {
          // we found the joystick command
          m_asJoyCommands[iCmd].iJoystick = iJoystick;
          m_asJoyCommands[iCmd].bAxis     = (iAxis != 0);
          m_asJoyCommands[iCmd].bPositive = (iPositive != 0);
          m_asJoyCommands[iCmd].iIndex    = iIndex;
          }
        }
      }
    else if (strncmp(pch, "Screen_Full", 11) == 0)
      {
      pch += 11;
      // skip white space and equals
      while (*pch == ' ' || *pch == '\t') pch++;
      while (*pch == '=') pch++;
      while (*pch == ' ' || *pch == '\t') pch++;
      m_bFullscreen = (atoi(pch) != 0);
      }
    else if (strncmp(pch, "Screen_Width", 12) == 0)
      {
      pch += 12;
      // skip white space and equals
      while (*pch == ' ' || *pch == '\t') pch++;
      while (*pch == '=') pch++;
      while (*pch == ' ' || *pch == '\t') pch++;
      int iWidth = atoi(pch);
      if (iWidth != 0) m_iScreenWidth = iWidth;
      }
    else if (strncmp(pch, "Screen_Height", 13) == 0)
      {
      pch += 13;
      // skip white space and equals
      while (*pch == ' ' || *pch == '\t') pch++;
      while (*pch == '=') pch++;
      while (*pch == ' ' || *pch == '\t') pch++;
      int iHeight = atoi(pch);
      if (iHeight != 0) m_iScreenHeight = iHeight;
      }
    else if (strncmp(pch, "Score_\"", 7) == 0)
      {
      pch += 7;
      // grab the name
      char chName[128];
      int i = 0;
      while (*pch != '\"' && *pch != 10 && *pch != 0 && i < 127) chName[i++] = *pch++;
      chName[i] = 0;
      if (*pch == '\"') pch++;
      // limit the length to 9 chars
      chName[9] = 0;
      // skip white space and equals
      while (*pch == ' ' || *pch == '\t') pch++;
      while (*pch == '=') pch++;
      while (*pch == ' ' || *pch == '\t') pch++;
      int iScore = atoi(pch);
      // find out if this is a 'default' high score
      for (i = 0; i < 10; i++)
        if (strcmp(m_asHighScores[i].chName, chName) == 0 && iScore == m_asHighScores[i].iScore) break;
      // add to the list if it's valid and not default
      if (i == 10 && iScore != 0) InsertHighScore(iScore, chName);
      }
    // go to the next line
    while (*pch != 10 && *pch != 0) pch++;
    if (*pch == 10) pch++;
    }

  // free the memory
  free(pchSettings);

  // all done
  return true;
}

bool CSettings::Change(ECommand eCommand, SDLKey sKeySymbol)
{
  // check input
  if (eCommand < E_INPUT_FIRST_COMMAND || eCommand > E_INPUT_LAST_COMMAND) return false;

  // check to see if this key command is already in use
  for (int iCmd = E_INPUT_FIRST_COMMAND; iCmd <= E_INPUT_LAST_COMMAND; iCmd++)
    {
    if (m_asKeyCommands[iCmd] == sKeySymbol)
      {
      // this key is already in use, so swap my old key command into its place
      m_asKeyCommands[iCmd] = m_asKeyCommands[eCommand];
      }
    }

  // change the key command
  m_asKeyCommands[eCommand] = sKeySymbol;
  return true;
}

bool CSettings::Change(ECommand eCommand, int iJoystick, bool bAxis, int iIndex, bool bPositive)
{
  // check input
  if (eCommand < E_INPUT_FIRST_COMMAND || eCommand > E_INPUT_LAST_COMMAND) return false;

  // check to see if this joystick command is already in use
  for (int iCmd = E_INPUT_FIRST_COMMAND; iCmd <= E_INPUT_LAST_COMMAND; iCmd++)
    {
    if (m_asJoyCommands[iCmd].iJoystick == iJoystick && iJoystick != -1 &&
        m_asJoyCommands[iCmd].bAxis     == bAxis &&
        m_asJoyCommands[iCmd].iIndex    == iIndex &&
        m_asJoyCommands[iCmd].bPositive == bPositive)
      {
      // this command is already in use, so remove it from its other place in the list
      m_asJoyCommands[iCmd].iJoystick = -1;
      }
    }

  // change the joystick command
  m_asJoyCommands[eCommand].iJoystick = iJoystick;
  m_asJoyCommands[eCommand].bAxis     = bAxis;
  m_asJoyCommands[eCommand].bPositive = bPositive;
  m_asJoyCommands[eCommand].iIndex    = iIndex;
  return true;
}

bool CSettings::SetScreen(int iWidth, int iHeight, bool bFullscreen)
{
  m_iScreenWidth = iWidth;
  m_iScreenHeight = iHeight;
  m_bFullscreen = bFullscreen;
  return true;
}

bool CSettings::InsertHighScore(int iScore, char *pchName)
{
  // find place in the list
  int iPlace;
  for (iPlace = 0; iPlace < 10; iPlace++)
    {
    if (m_asHighScores[iPlace].iScore <= iScore) break;
    }
  if (iPlace == 10) return false;

  // now move the lower scores down
  for (int i = 9; i > iPlace; i--)
    {
    strcpy(m_asHighScores[i].chName, m_asHighScores[i - 1].chName);
    m_asHighScores[i].iScore = m_asHighScores[i - 1].iScore;
    }

  // then insert this score
  m_asHighScores[iPlace].iScore = iScore;
  strcpy(m_asHighScores[iPlace].chName, pchName);

  // all done
  return true;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings class private functions

#if defined(WIN32)
bool CSettings::GetSettingsPathname(char *pchPathname) const
{
  // check input string
  if (!pchPathname) return false;
  pchPathname[0] = 0;

  // get the pathname for Windows
  if (!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, pchPathname)))
    {
    return false;
    }

  // tack on our filename to the end
  PathAppend(pchPathname, TEXT("Invasion3D.txt"));
  return true;

}
#else
bool CSettings::GetSettingsPathname(char *pchPathname) const
{
  // check input string
  if (!pchPathname) return false;
  pchPathname[0] = 0;

  // get pointer to $HOME environment variable
  char *pchHome = getenv("HOME");
  if (!pchHome) return false;

  // copy pathname
  strcpy(pchPathname, pchHome);
  strcat(pchPathname, "/.invasion3d");

  return true;
}
#endif // defined(WIN32)

void CSettings::SetDefaults(void)
{
  // key commands
  m_asKeyCommands[E_INPUT_LEFT]        = SDLK_LEFT;
  m_asKeyCommands[E_INPUT_RIGHT]       = SDLK_RIGHT;
  m_asKeyCommands[E_INPUT_UP]          = SDLK_UP;
  m_asKeyCommands[E_INPUT_DOWN]        = SDLK_DOWN;
  m_asKeyCommands[E_INPUT_FIRE]        = SDLK_SPACE;
  m_asKeyCommands[E_INPUT_SHIELD]      = SDLK_LALT;
  m_asKeyCommands[E_INPUT_PAUSE]       = SDLK_p;
  m_asKeyCommands[E_INPUT_WEAPON_UP]   = SDLK_PERIOD;
  m_asKeyCommands[E_INPUT_WEAPON_DOWN] = SDLK_COMMA;
  m_asKeyCommands[E_INPUT_WEAPON0]     = SDLK_1;
  m_asKeyCommands[E_INPUT_WEAPON1]     = SDLK_2;
  m_asKeyCommands[E_INPUT_WEAPON2]     = SDLK_3;
  m_asKeyCommands[E_INPUT_WEAPON3]     = SDLK_4;

  // joystick commands
  memset(m_asJoyCommands, 0, E_INPUT_NONE * sizeof(SJoyCmd));
  m_asJoyCommands[E_INPUT_LEFT].bAxis         = true;
  m_asJoyCommands[E_INPUT_RIGHT].bAxis        = true;
  m_asJoyCommands[E_INPUT_RIGHT].bPositive    = true;
  m_asJoyCommands[E_INPUT_UP].bAxis           = true;
  m_asJoyCommands[E_INPUT_UP].iIndex          = 1;
  m_asJoyCommands[E_INPUT_DOWN].bAxis         = true;
  m_asJoyCommands[E_INPUT_DOWN].bPositive     = true;
  m_asJoyCommands[E_INPUT_DOWN].iIndex        = 1;
  m_asJoyCommands[E_INPUT_FIRE].iIndex        = 1;
  m_asJoyCommands[E_INPUT_SHIELD].iIndex      = 2;
  m_asJoyCommands[E_INPUT_PAUSE].iJoystick    = -1;
  m_asJoyCommands[E_INPUT_WEAPON_UP].iIndex   = 5;
  m_asJoyCommands[E_INPUT_WEAPON_DOWN].iIndex = 4;
  m_asJoyCommands[E_INPUT_WEAPON0].iJoystick  = -1;
  m_asJoyCommands[E_INPUT_WEAPON1].iJoystick  = -1; 
  m_asJoyCommands[E_INPUT_WEAPON2].iJoystick  = -1;
  m_asJoyCommands[E_INPUT_WEAPON3].iJoystick  = -1;

  // high scores
  strcpy(m_asHighScores[0].chName, "Pyro420  ");
  strcpy(m_asHighScores[1].chName, "Yashka15 ");
  strcpy(m_asHighScores[2].chName, "Rix      ");
  strcpy(m_asHighScores[3].chName, "WrnchSpnr");
  strcpy(m_asHighScores[4].chName, "C2NOS    ");
  strcpy(m_asHighScores[5].chName, "Xanadu   ");
  strcpy(m_asHighScores[6].chName, "Chain8888");
  strcpy(m_asHighScores[7].chName, "Koshka   ");
  strcpy(m_asHighScores[8].chName, "Mario    ");
  strcpy(m_asHighScores[9].chName, "T.Leary  ");
  m_asHighScores[0].iScore = 100000;
  m_asHighScores[1].iScore = 90000;
  m_asHighScores[2].iScore = 80000;
  m_asHighScores[3].iScore = 70000;
  m_asHighScores[4].iScore = 60000;
  m_asHighScores[5].iScore = 50000;
  m_asHighScores[6].iScore = 40000;
  m_asHighScores[7].iScore = 30000;
  m_asHighScores[8].iScore = 20000;
  m_asHighScores[9].iScore = 10000;

}

/////////////////////////////////////////////////////////////////////////////
// CSettings Command Names

char CSettings::m_chCommandNames[][16] = {"Move Left",
                                          "Move Right",
                                          "Move Up",
                                          "Move Down",
                                          "Fire Weapon",
                                          "Activate Shield",
                                          "Pause",
                                          "Next Weapon",
                                          "Previous Weapon",
                                          "Weapon 1",
                                          "Weapon 2",
                                          "Weapon 3",
                                          "Weapon 4"};
