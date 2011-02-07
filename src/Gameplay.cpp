/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Gameplay.cpp                                             *
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
**                              05/05/05                                   **
**                                                                         **
** Gameplay.cpp - contains interface for static CGameplay class, used to   **
**                store some tables used to tweak gameplay settings.       **
**                                                                         **
****************************************************************************/

#include "Gameplay.h"

// number and speed of stars in the 'star field'
int   CGameplay::m_iNumStars     = 500;
float CGameplay::m_fStarSpeed    = 0.03f;  // world units per millisecond

// interval (in frames) between appearances of the "Mystery" invader, and its speed
int   CGameplay::m_iMysteryInterval = 15000;       // milliseconds
float CGameplay::m_fMysterySpeed    = -0.105f;     // world units per millisecond

// the speed increase for the Invaders as a function of the number of edge columns knocked out
float CGameplay::m_fSpeedBoost[8]   = {0.0f, 1.0f, 2.0f, 3.0f, 5.0f, 7.0f, 11.0f, 15.0f};

// speed and force of expanding shockwave
float CGameplay::m_fShockwaveSpeed  = 3.0f;  // world units per millisecond
float CGameplay::m_fShockwaveForce  = 2.1f;

// force of a bullet impact
float CGameplay::m_fRecoilForce     = 1.2f;  // delta velocity in world units per millisecond

// amount of damage to apply to colliding invaders, per unit velocity in the direction of collision
float CGameplay::m_fCollideDamage   = 16.0f;

// Projectile damage               laser  gun  cannon  invader
int   CGameplay::m_iProjDamage[] = {50,   25,  500,    35};

// Projectile speed                  LASER, BULLET, CANNONBALL,  INVADER,  QUAD_LASER
float CGameplay::m_fProjSpeed[]  = {-0.24f, -0.54f,     -0.42f,     0.3f,       0.12f};

// Projectile reloading time       LASER,  BULLET,  CANNONBALL,  INVADER,  QUAD_LASER
int   CGameplay::m_iReloadTime[] = {500,      170,         850,      850,         850};

// maximum speed of movement for laser base
float CGameplay::m_fBaseMoveSpeedLR = 0.24f;
float CGameplay::m_fBaseMoveSpeedUD = 0.18f;

// pieces of ammo given at the start of each wave
int   CGameplay::m_iExtraGunAmmo[20]    = { 0,10,10,10, 15,15,15,15, 20,20,20,20, 25,25,25,25, 30,30,30,30};
int   CGameplay::m_iExtraCannonAmmo[20] = { 0, 1, 2, 2,  3, 3, 4, 4,  5, 5, 6, 6,  7, 7, 7, 7,  8, 8, 8, 8};

// Invader full health by wave
int   CGameplay::m_iInvaderHealth[][20]= {{3,3,3,3,     6,6,6,6,     9,9,9,9,     15,15,15,15, 25,25,25,25}, // E_INVADER_10
                                          {5,5,5,5,     15,15,15,15, 20,20,20,20, 30,30,30,30, 40,40,40,40}, // E_INVADER_20
                                          {7,7,7,7,     20,20,20,20, 35,35,35,35, 50,50,50,50, 65,65,65,65}, // E_INVADER_30
                                          {10,10,10,10, 10,10,10,10, 10,10,10,10, 10,10,10,10, 10,10,10,10}, // E_INVADER_MYSTERY
                                          {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100}, // E_INVADER_ERRR
                                          {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100} }; // E_INVADER_IGNIGNOKT
// Bunker health by wave
int   CGameplay::m_iBunkerHealth[20] = {300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300};

// number of milliseconds between calls to replenish matrix of 'alive' invaders
int   CGameplay::m_iReplenishTime = 500;

// the inverse of the probability that each empty spot will be filled inside the matrix replenish function
int   CGameplay::m_iChanceRefillTop[20]   = {7,7,7,7, 6,6,6,6, 5,5,5,5, 4,4,4,4, 2,2,2,2};
int   CGameplay::m_iChanceRefillInner[20] = {9,9,9,9, 7,7,7,7, 5,5,5,5, 3,3,3,3, 2,2,2,2};

// number of invaders which appear to re-fill the matrix for each wave
int   CGameplay::m_iExtraInvaders[20] = {0,6,12,18, 24,30,36,42, 48,54,60,66, 72,78,84,90, 96,102,108,114};

// amount of time (seconds) allowed for each wave to get a bonus
int   CGameplay::m_iBonusTime[20]     = {60,66,72,78, 84,90,96,102, 108,114,120,126, 132,138,144,150, 156,162,168,174};

// inverse probability that an invader who is able to fire will shoot during 1/60s when Laser Base is directly underneath
int   CGameplay::m_iCloseShotChance[20] = {100,97,95,92, 90,87,85,82, 80,77,73,70, 65,60,55,50, 45,40,20,10};

// inverse probability that an invader who is able to fire will shoot during 1/60 second
int   CGameplay::m_iFarShotChance[20]   = {800,775,750,725, 700,675,650,625, 600,575,550,525, 500,475,450,425, 400,300,200,150};

// number of seconds before Invaders start doing the Kamikaze dive, for each wave
int   CGameplay::m_iKamikazeCounter[20] = {60,55,50,45, 40,38,36,34, 32,30,28,26, 24,22,20,18, 16,14,12,10};

// other parameters for the Kamikaze Dive move
int   CGameplay::m_iKamikazeChance      = 100;   // inverse probability that an Invader will go kamikaze if he can
float CGameplay::m_fKamikazeFlipSpeed   = 0.36f; // degrees per millisecond, for a 180 degree front flip
float CGameplay::m_fKamikazeRotSpeed    = 0.6f;  // degrees per millisecond, as the invader is diving
float CGameplay::m_fKamikazeDashSpeed   = 0.48f; // in screen units per millisecond, as the Invader dives down

// first wave in which the Mooninites may appear
int   CGameplay::m_iFirstMoonWave       = 12;

// inverse probabilities that certain events will trigger the Mooninites to appear
int   CGameplay::m_iMoonTrigger3Chance  = 3;   // Mystery ship shot down by player
int   CGameplay::m_iMoonTrigger4Chance  = 4;  // > 6 enemy shots are on the screen
int   CGameplay::m_iMoonTrigger5Chance  = 5;  // Entire column of invaders shot down consecutively
