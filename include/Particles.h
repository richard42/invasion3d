/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Particles.h                                              *
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


#if !defined(PARTICLES_H)
#define PARTICLES_H

#define SPARKLE_SIZE     15.0f
#define SPARKLE_LIFE     2000    // 2 seconds
#define SPARKLE_TERM_VEL 0.90f   // world units per millisecond
#define SPARKLE_INIT_VEL 0.24f   // world units per millisecond
#define POLY_TERM_VEL    1.20f   // world units per millisecond
#define POLY_INIT_VEL    0.42f   // world units per millisecond
#define POLY_LIFE        2000    // 2 seconds

#define GRAVITY_STRENGTH 0.0036f

class CInvader;
class CLaserBase;

// structure used to handle sparkling explosion
typedef struct {unsigned int    uiNumQuads;
                unsigned int    uiTimeCount;
                float         (*pfVertices)[3];
                float         (*pfColors)[4];
                float          *pfFade;
                float         (*pfVelocity)[3];
               } SSparkles;

// structure used to handle polygonal explosion
typedef struct {unsigned int    uiNumQuads[6];
                unsigned int    uiNumQuadColors;
                unsigned int    uiNumTriangles[6];
                unsigned int    uiNumTriColors;
                unsigned int    uiTimeCount;
                unsigned char   aucQuadColors[6][3];
                unsigned char   aucTriColors[6][3];
                float         (*pfQuadVertices)[4][4];
                float         (*pfTriVertices)[3][4];
                float         (*pfQuadNormals)[4][4];
                float         (*pfTriNormals)[3][4];
                float         (*pfQuadVelocity)[4];
                float         (*pfTriVelocity)[4];
                float         (*pfQuadCenter)[4];
                float         (*pfTriCenter)[4];
                unsigned char  *pucQuadRot;
                unsigned char  *pucTriRot;
               } SPolys;

class CParticles
{
public:
  CParticles();
  ~CParticles();

  // static functions
  static bool StaticSetup(void);
  static void StaticShutdown(void);

  // accessor functions
  void Draw(void) const;

  // modifier functions
  void Clear(void);
  void ProcessState(unsigned int uiFrameTime);
  bool AddInvaderSparkle(CInvader *pcInvader);
  bool AddInvaderPoly(CInvader *pcInvader);
  bool AddLaserBase(CLaserBase *pcLaserBase);

private:
  // member data
  SSparkles    m_asSparkles[16];
  unsigned int m_uiNumSparkles;
  SPolys       m_asPolys[16];
  unsigned int m_uiNumPolys;

  SSparkles    m_sLaserBase;
  bool         m_bLaserBase;

  // private functions
  void RemoveSparkle(unsigned int uiNumSparkle);
  void RemovePoly(unsigned int uiNumPoly);

  // static data
  static float m_afColors[12][3];
};

#endif // !defined(PARTICLES_H)
