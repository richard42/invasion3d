/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Sound.cpp                                                *
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
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "Sound.h"
#include "Package.h"

// assembly language functions
#if !defined(NO_MMX)
  extern "C" void asmMixerMMX(short *psClips[], unsigned int uiNumClips, unsigned int uiNumSamples, short *psOutput);
#endif

// static data declaration
bool          CSound::m_bInitialized = false;
void         *CSound::m_pvAudioData[E_SOUND_INVALID];
unsigned int  CSound::m_uiClipLength[E_SOUND_INVALID];
const int     CSound::m_ciBufferLength = 1024;			// desired buffer length
int           CSound::m_iBufferLength = 0;			// actual buffer length
SDL_mutex    *CSound::m_pMutex = NULL;

/////////////////////////////////////////////////////////////////////////////
// CSound public static functions

bool CSound::InitSound(CPackage *pcMedia)
{

  // create the mutex
  m_pMutex = SDL_CreateMutex();

  // Mooninite sound clips
  if (!LoadClip(pcMedia, E_SOUND_MOON_0,  "YouWillBeDestroyed.wav",          64)) return false;
  if (!LoadClip(pcMedia, E_SOUND_MOON_1,  "OurCultureIsAdvanced.wav",        64)) return false;
  if (!LoadClip(pcMedia, E_SOUND_MOON_2A, "LetUsLeaveThisPrimitiveRock.wav", 64)) return false;
  if (!LoadClip(pcMedia, E_SOUND_MOON_2B, "OhManImToasted.wav",              64)) return false;
  if (!LoadClip(pcMedia, E_SOUND_MOON_3,  "WeSmokeAsWeShootTheBird.wav",     64)) return false;
  if (!LoadClip(pcMedia, E_SOUND_MOON_4,  "NoOneCanDefeatTheQuadLaser.wav",  64)) return false;
  if (!LoadClip(pcMedia, E_SOUND_MOON_5,  "IHopeHeCanSeeThis.wav",           64)) return false;
  if (!LoadClip(pcMedia, E_SOUND_MOON_6,  "SomeWouldSayThatTheEarthIsOurMoon.wav", 64)) return false;
  if (!LoadClip(pcMedia, E_SOUND_MOON_7,  "MoonSpanking.wav",                64)) return false;

  // Gunshot sound clips
  if (!LoadClip(pcMedia, E_SOUND_SHOOT_0, "misc320.wav",  24)) return false;
  if (!LoadClip(pcMedia, E_SOUND_SHOOT_1, "scifi004.wav",  24)) return false;
  if (!LoadClip(pcMedia, E_SOUND_SHOOT_2, "battle016.wav", 40)) return false;
  if (!LoadClip(pcMedia, E_SOUND_SHOOT_3, "battle032.wav", 40)) return false;
  if (!LoadClip(pcMedia, E_SOUND_SHOOT_4, "cannon03.wav",  32)) return false;

  // impacts and explosion sound clips
  if (!LoadClip(pcMedia, E_SOUND_HIT_0, "scifi040.wav",   32)) return false;
  if (!LoadClip(pcMedia, E_SOUND_HIT_1, "pipebang.wav",   32)) return false;
  if (!LoadClip(pcMedia, E_SOUND_HIT_2, "musical004.wav", 20)) return false;
  if (!LoadClip(pcMedia, E_SOUND_HIT_3, "cartoon073.wav", 32)) return false;
  if (!LoadClip(pcMedia, E_SOUND_HIT_4, "smash.wav",      32)) return false;
  if (!LoadClip(pcMedia, E_SOUND_EXPLODE_0, "battle038.wav", 42)) return false;
  if (!LoadClip(pcMedia, E_SOUND_EXPLODE_1, "battle025.wav", 32)) return false;
  if (!LoadClip(pcMedia, E_SOUND_EXPLODE_2, "bomb07.wav",    32)) return false;
  if (!LoadClip(pcMedia, E_SOUND_EXPLODE_3, "battle006.wav", 64)) return false;

  // other sound clips
  if (!LoadClip(pcMedia, E_SOUND_KAMIKAZE,  "cartoon018.wav", 64)) return false;
  if (!LoadClip(pcMedia, E_SOUND_SHIELD,    "scifi026.wav",   32)) return false;
  if (!LoadClip(pcMedia, E_SOUND_EXTRALIFE, "Halleluj.wav",   40)) return false;
  if (!LoadClip(pcMedia, E_SOUND_GAMEOVER,  "musical032.wav", 40)) return false;

  // looping clips
  if (!LoadClip(pcMedia, E_SOUND_MUSIC_1,   "beatplus.wav",   40)) return false;
  if (!LoadClip(pcMedia, E_SOUND_MUSIC_2,   "nicebeat.wav",   64)) return false;
  if (!LoadClip(pcMedia, E_SOUND_MYSTERY,   "cartoon189.wav", 16)) return false;

  m_bInitialized = true;
  return true;
}

void CSound::DestroySound(void)
{
  if (m_bInitialized)
    {
    // destroy the mutex
    SDL_DestroyMutex(m_pMutex);
    // free the memory for the audio clips
    for (unsigned int ui = 0; ui < E_SOUND_INVALID; ui++)
      {
      free(m_pvAudioData[ui]);
      }
    // set the initialized flag to false
    m_bInitialized = false;
    }

}

/////////////////////////////////////////////////////////////////////////////
// CSound constructor and destructor

CSound::CSound()
{
  // set up desired audio specs
  m_AudioSpec.freq     = 22050;
  m_AudioSpec.format   = AUDIO_S16LSB;
  m_AudioSpec.channels = 1;
  m_AudioSpec.samples  = m_ciBufferLength;
  m_AudioSpec.callback = AudioCallback;
  m_AudioSpec.userdata = (void *) this;

  // Open the audio device
  SDL_OpenAudio(&m_AudioSpec, NULL);
  if (m_AudioSpec.samples < m_ciBufferLength)
    m_iBufferLength = m_AudioSpec.samples;
  else
    m_iBufferLength = m_ciBufferLength;

  // set active sounds to 0
  m_uiActiveSounds = 0;

  // activate the sound
  SDL_PauseAudio(0);
}

CSound::~CSound()
{
  SDL_CloseAudio();
}

/////////////////////////////////////////////////////////////////////////////
// CSound accessor functions



/////////////////////////////////////////////////////////////////////////////
// CSound modifier functions

bool CSound::PlaySoundClip(ESound eClipType)
{
  // first, grab the mutex
  if (SDL_mutexP(m_pMutex) == -1) return false;

  // now check to make sure we can add one more sound
  if (m_uiActiveSounds == 32)
    {
    // error - too many sounds playing!
    SDL_mutexV(m_pMutex);
    return false;
    }

  // add it to the arrays
  m_eClipType[m_uiActiveSounds] = eClipType;
  m_uiClipOffset[m_uiActiveSounds] = 0;
  m_uiActiveSounds++;

  // that's all!
  SDL_mutexV(m_pMutex);
  return true;
}

void CSound::StopSoundClip(ESound eClipType)
{
  // first, grab the mutex
  if (SDL_mutexP(m_pMutex) == -1) return;

  // now try to find the sound clip
  for (unsigned int ui = 0; ui < m_uiActiveSounds; ui++)
    {
    if (m_eClipType[ui] == eClipType)
      {
      // remove this sound from our lists
      memcpy(m_eClipType + ui,    m_eClipType + ui + 1,    (m_uiActiveSounds - ui - 1) * sizeof(ESound));
      memcpy(m_uiClipOffset + ui, m_uiClipOffset + ui + 1, (m_uiActiveSounds - ui - 1) * sizeof(unsigned int));
      m_uiActiveSounds--;
      break;
      }
    }

  // let go of the mutex
  SDL_mutexV(m_pMutex);
  return;
}


/////////////////////////////////////////////////////////////////////////////
// private static functions

void CSound::AudioCallback(void *pvUserData, Uint8 *pucBuffer, int iBufferBytes)
{
  CSound *pSound = (CSound *) pvUserData;

  // first grab the mutex
  if (SDL_mutexP(m_pMutex) == -1) return;

  // handle the case where there are no sounds playing
  if (pSound->m_uiActiveSounds == 0)
    {
    memset(pucBuffer, 0, (size_t) iBufferBytes);
    SDL_mutexV(m_pMutex);
    return;
    }

  // first set up pointers into the sound data
  short *sClips[32];
  unsigned int uiSounds = pSound->m_uiActiveSounds;
  for (unsigned int ui = 0; ui < uiSounds; ui++)
    {
    sClips[ui] = (short *) pSound->m_pvAudioData[pSound->m_eClipType[ui]] + pSound->m_uiClipOffset[ui];
    }

  // now mix the sounds
#if !defined(NO_MMX)
    asmMixerMMX(sClips, uiSounds, m_iBufferLength, (short *) pucBuffer);
#else
  short *psOutput = (short *) pucBuffer;
  for (unsigned int uiWord = 0; uiWord < m_iBufferLength; uiWord++)
    {
    int iSample = 0;
    for (unsigned int ui = 0; ui < uiSounds; ui++)
      {
      iSample += (int) *sClips[ui]++;

      }
    iSample = ((iSample < -32767) ? -32767 : ((iSample > 32767) ? 32767 : iSample));
    *psOutput++ = iSample;
    }
#endif

  // advance the states and eliminate any sounds which have reached the end of their clip
  for (unsigned int ui = 0; ui < uiSounds; ui++)
    {
    pSound->m_uiClipOffset[ui] += m_iBufferLength;
    if (pSound->m_uiClipOffset[ui] >= pSound->m_uiClipLength[pSound->m_eClipType[ui]])
      {
      // if this is a looping clip, then loop back to the beginning
      if (pSound->m_eClipType[ui] >= E_SOUND_MUSIC_1)
        {
        pSound->m_uiClipOffset[ui] -= pSound->m_uiClipLength[pSound->m_eClipType[ui]];
        }
      else
        {
        // else remove this sound from our lists
        memcpy(pSound->m_eClipType + ui,    pSound->m_eClipType + ui + 1,    (uiSounds - ui - 1) * sizeof(ESound));
        memcpy(pSound->m_uiClipOffset + ui, pSound->m_uiClipOffset + ui + 1, (uiSounds - ui - 1) * sizeof(unsigned int));
        pSound->m_uiActiveSounds--;
        uiSounds--;
        ui--;
        }
      }
    }

  // all done
  SDL_mutexV(m_pMutex);
}

bool CSound::LoadClip(CPackage *pcMedia, ESound eClipType, const char *pccFilename, unsigned int uiVolume)
{
  // open the WAV file
  unsigned int uiFile = pcMedia->OpenFile(pccFilename);
  if (uiFile == 0)
    {
    printf("CSound Error: Couldn't open WAV file: %s\n", pccFilename);
    return false;
    }

  // get the length
  unsigned int uiLength = pcMedia->Length(uiFile);

  // check the length
  if (uiLength < 44)
    {
    printf("CSound Error: Invalid WAV file length: %i, %s\n", uiLength, pccFilename);
    return false;
    }

  // allocate memory for the wave file
  unsigned char *pchWave = (unsigned char *) malloc(uiLength);
  if (!pchWave)
    {
    printf("CSound Error: Couldn't allocate %i bytes of memory for WAV file\n", uiLength);
    return false;
    }

  // read the file
  if (pcMedia->ReadBytes(uiFile, pchWave, uiLength) != uiLength)
    {
    printf("CSound Error: Couldn't read %i bytes from WAV file: %s\n", uiLength, pccFilename);
    free(pchWave);
    return false;
    }

  // check the very first header
  if (strncmp((char *) pchWave, "RIFF", 4) != 0)
    {
    printf("CSound Error: Invalid WAV file (RIFF tag not found): %s\n", pccFilename);
    free(pchWave);
    return false;
    }
  if (strncmp((char *) pchWave + 8, "WAVE", 4) != 0)
    {
    printf("CSound Error: Invalid WAV file (WAVE tag not found): %s\n", pccFilename);
    free(pchWave);
    return false;
    }

  // check the next header
  if (strncmp((char *) pchWave + 12, "fmt ", 4) != 0)
    {
    printf("CSound Error: Invalid WAV file ('fmt ' tag not found): %s\n", pccFilename);
    free(pchWave);
    return false;
    }
  unsigned int uiSBC1Size = (pchWave[0x13] << 24) | (pchWave[0x12] << 16) | (pchWave[0x11] << 8) | pchWave[0x10];
  unsigned int uiAudioFormat = (pchWave[0x15] << 8) | pchWave[0x14];
  unsigned int uiChannels = (pchWave[0x17] << 8) | pchWave[0x16];
  unsigned int uiSampleRate = (pchWave[0x1b] << 24) | (pchWave[0x1a] << 16) | (pchWave[0x19] << 8) | pchWave[0x18];
  unsigned int uiBitsSample  = (pchWave[0x23] << 8) | pchWave[0x22];
  if (uiAudioFormat != 1)
    {
    printf("CSound Error: Invalid AudioFormat type: %i (1 == PCM expected)\n", uiAudioFormat);
    free(pchWave);
    return false;
    }
  if (uiChannels != 1)
    {
    printf("CSound Error: Invalid channels: %i (1 == mono expected)\n", uiChannels);
    free(pchWave);
    return false;
    }
  if (uiSampleRate != 22050)
    {
    printf("CSound Error: Invalid Sample rate: %i (22,050 expected)\n", uiSampleRate);
    free(pchWave);
    return false;
    }
  if (uiBitsSample != 8)
    {
    printf("CSound Error: Invalid BitsSample type: %i (8 expected)\n", uiBitsSample);
    free(pchWave);
    return false;
    }

  // go to sub-chunk 2
  unsigned char *pchIn = pchWave + uiSBC1Size + 20;

  // check the last header
  if (strncmp((char *) pchIn, "data", 4) != 0)
    {
    printf("CSound Error: Invalid WAV file ('data' tag not found): %s\n", pccFilename);
    free(pchWave);
    return false;
    }
  unsigned int uiDataSize = (pchWave[0x7] << 24) | (pchIn[0x6] << 16) | (pchIn[0x5] << 8) | pchIn[0x4];
  if (uiDataSize + uiSBC1Size + 28 > uiLength)
    {
    printf("CSound Error: DataSize too long: %i\n", uiDataSize);
    free(pchWave);
    return false;
    }

  // go to audio data
  pchIn += 8;

  // calculate number of samples to allocate (it should be a multiple of m_iBufferLength)
  unsigned int uiMemLength;
  if ((uiDataSize % m_iBufferLength) == 0)
    uiMemLength = uiDataSize + m_iBufferLength;
  else
    uiMemLength = ((uiDataSize / m_iBufferLength) + 2) * m_iBufferLength;

  // allocate memory for static clip data
  short *psClip = (short *) malloc(uiMemLength * sizeof(short));
  if (!psClip)
    {
    printf("CSound Error: Couldn't allocate %i bytes of memory for sound clip\n", uiMemLength);
    free(pchWave);
    return false;
    }
  memset(psClip, 0, uiMemLength * sizeof(short));

  // convert the clip to 16-bit
  for (unsigned int ui = 0; ui < uiDataSize; ui++)
    {
    psClip[ui] = (*pchIn++ - 128) * uiVolume;
    }

  // free the original wave file buffer
  free(pchWave);

  // set up the static data entry
  m_pvAudioData[eClipType] = psClip;
  m_uiClipLength[eClipType] = uiDataSize;

  // for the looping clips, copy the beginning onto the end
  if (eClipType >= E_SOUND_MUSIC_1)
    {
    memcpy(psClip + uiDataSize, psClip, (uiMemLength - uiDataSize) * sizeof(short));
    }

  // all done
  return true;
}
