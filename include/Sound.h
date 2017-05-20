/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Sound.h                                                  *
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


#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

#if !defined(SOUND_H)
#define SOUND_H

class CPackage;

// enumerated type for Sound clips
typedef enum {E_SOUND_MOON_0 = 0,
              E_SOUND_MOON_1,
              E_SOUND_MOON_2A,
              E_SOUND_MOON_2B,
              E_SOUND_MOON_3,
              E_SOUND_MOON_4,
              E_SOUND_MOON_5,
              E_SOUND_MOON_6,
              E_SOUND_MOON_7,
              E_SOUND_SHOOT_0,
              E_SOUND_SHOOT_1,
              E_SOUND_SHOOT_2,
              E_SOUND_SHOOT_3,
              E_SOUND_SHOOT_4,
              E_SOUND_HIT_0,
              E_SOUND_HIT_1,
              E_SOUND_HIT_2,
              E_SOUND_HIT_3,
              E_SOUND_HIT_4,
              E_SOUND_EXPLODE_0,
              E_SOUND_EXPLODE_1,
              E_SOUND_EXPLODE_2,
              E_SOUND_EXPLODE_3,
              E_SOUND_KAMIKAZE,
              E_SOUND_SHIELD,
              E_SOUND_EXTRALIFE,
              E_SOUND_GAMEOVER,
              E_SOUND_MUSIC_1,
              E_SOUND_MUSIC_2,
              E_SOUND_MYSTERY,
              E_SOUND_INVALID} ESound;

class CSound
{
public:
  CSound();
  ~CSound();

  // function to initialize the static data
  static bool InitSound(CPackage *pcMedia);
  static void DestroySound(void);

  // accessor functions
  void Pause(void) const   { SDL_PauseAudio(1); }
  void Unpause(void) const { SDL_PauseAudio(0); }

  // modifier functions
  bool PlaySoundClip(ESound eClipType);
  void StopSoundClip(ESound eClipType);

private:
  // member data
  unsigned int   m_uiActiveSounds;
  ESound         m_eClipType[32];
  unsigned int   m_uiClipOffset[32];

  SDL_AudioSpec  m_AudioSpec;

  // private accessors

  // private static functions
  static bool LoadClip(CPackage *pcMedia, ESound eClipType, const char *pccFilename, unsigned int uiVolume);
  static void AudioCallback(void *pvUserData, Uint8 *pucBuffer, int iBufferBytes);

  // static data
  static SDL_mutex    *m_pMutex;
  static bool          m_bInitialized;
  static void         *m_pvAudioData[E_SOUND_INVALID];
  static unsigned int  m_uiClipLength[E_SOUND_INVALID];
  static int           m_iBufferLength;
  const static int     m_ciBufferLength;
};

#endif // defined(SOUND_H)
