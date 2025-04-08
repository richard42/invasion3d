/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Settings.h                                               *
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


#if !defined(SETTINGS_H)
#define SETTINGS_H

#if defined(__APPLE__)
  #include <SDL.h>
#else
  #include <SDL/SDL.h>
#endif

// input command types
typedef enum {E_INPUT_LEFT = 0,
              E_INPUT_RIGHT,
              E_INPUT_UP,
              E_INPUT_DOWN,
              E_INPUT_FIRE,
              E_INPUT_SHIELD,
              E_INPUT_PAUSE,
              E_INPUT_WEAPON_UP,
              E_INPUT_WEAPON_DOWN,
              E_INPUT_WEAPON0,
              E_INPUT_WEAPON1,
              E_INPUT_WEAPON2,
              E_INPUT_WEAPON3,
              E_INPUT_NONE} ECommand;
#define E_INPUT_FIRST_COMMAND E_INPUT_LEFT
#define E_INPUT_LAST_COMMAND  E_INPUT_WEAPON3

typedef struct {int      iJoystick;
                bool     bAxis;
                bool     bPositive;
                int      iIndex;
               } SJoyCmd;

typedef struct {int      iScore;
                char     chName[10];
               } SHighScore;

class CSettings
{
public:
  CSettings();
  ~CSettings();

  // public accessors
  bool     SaveToFile(void) const;
  ECommand GetCommand(SDLKey sKeySymbol) const;
  ECommand GetCommand(int iJoystick, bool bAxis, int iIndex, bool bPositive) const;
  bool     GetCommandName(ECommand eCommand, char *pchName) const;
  bool     GetKey(ECommand eCommand, SDLKey *psKeySymbol, char *pchKeyName) const;
  bool     GetJoystickMovement(ECommand eCommand, int *piJoystick, bool *pbAxis, int *piIndex, bool *pbPositive) const;
  int      GetHighScore(int iPlace, char *pchName);
  bool     GetFullscreen() const;
  int      GetScreenWidth() const;
  int      GetScreenHeight() const;
  int      GetStereoOffset() const;

  // public modifiers
  bool LoadFromFile(void);
  bool Change(ECommand eCommand, SDLKey sKeySymbol);
  bool Change(ECommand eCommand, int iJoystick, bool bAxis, int iIndex, bool bPositive);
  bool SetScreen(int iWidth, int iHeight, bool bFullscreen);
  bool SetStereoOffset(int iOffset);
  bool InsertHighScore(int iScore, char *pchName);

private:
  // private member data
  int  m_iScreenWidth;
  int  m_iScreenHeight;
  int  m_iStereoOffset;
  bool m_bFullscreen;

  // private functions
  bool GetSettingsPathname(char *pchPathname) const;
  void SetDefaults(void);

  // static member data
  static char m_chCommandNames[E_INPUT_NONE][16];

  // input command tables
  SDLKey   m_asKeyCommands[E_INPUT_NONE];
  SJoyCmd  m_asJoyCommands[E_INPUT_NONE];

  // high score
  SHighScore m_asHighScores[10];

};

#endif // !defined(SETTINGS_H)
