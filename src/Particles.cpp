/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - Particles.cpp                                            *
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
**                              05/03/05                                   **
**                                                                         **
** Particles.cpp - contains interface for CParticles class - used to       **
**                 process and draw fragments of exploding objects         **
**                                                                         **
****************************************************************************/

#ifdef WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>

#include "GameMain.h" // for m_bAlphaSupported flag and Linux aligned_malloc functions
#include "Particles.h"
#include "Textures.h"
#include "Invader.h"
#include "LaserBase.h"

// assembly language functions
#if !defined(NO_SSE)
  extern "C" void asmParticleQuadSSE(int iQuads, float (*pfVertex)[4], float *pfCenter, float *pfVelocity, unsigned char *pucRot, float (*pfSinCos)[2], float (*pfCosSin)[2], unsigned int uiFrameTime);
  extern "C" void asmParticleTriSSE(int iTris, float (*pfVertex)[4], float *pfCenter, float *pfVelocity, unsigned char *pucRot, float (*pfSinCos)[2], float (*pfCosSin)[2], unsigned int uiFrameTime);
#endif

// static data
float CParticles::m_afColors[12][3] = { {1.0f,  0.5f,  0.25f}, {1.0f,  0.5f,  0.25f}, {1.0f,  1.0f,  0.25f}, {0.5f,  1.0f,  0.25f},
                                        {0.25f, 1.0f,  0.25f}, {0.25f, 1.0f,  0.75f}, {0.25f, 1.0f,  1.0f},  {0.25f, 0.5f,  1.0f},
                                        {0.25f, 0.25f, 1.0f},  {0.5f,  0.25f, 1.0f},  {1.0f,  0.25f, 1.0f},  {1.0f,  0.25f, 0.5f} };


/////////////////////////////////////////////////////////////////////////////
// CParticles constructor/destructor

CParticles::CParticles()
{
  m_uiNumSparkles = 0;
  m_uiNumPolys    = 0;
  m_bLaserBase    = false;
}

CParticles::~CParticles()
{
  Clear();
}

void CParticles::Clear(void)
{
  // get rid of everything in the particle engine
  for (unsigned int ui = 0; ui < m_uiNumSparkles; ui++)
    RemoveSparkle(0);
  m_uiNumSparkles = 0;

  for (unsigned int ui = 0; ui < m_uiNumPolys; ui++)
    RemovePoly(0);
  m_uiNumPolys = 0;

  if (m_bLaserBase)
    {
    // free memory
    free(m_sLaserBase.pfVertices);
    free(m_sLaserBase.pfVelocity);
    m_bLaserBase = false;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CParticles static functions

bool CParticles::StaticSetup(void)
{
  return true;
}

void CParticles::StaticShutdown(void)
{
}

/////////////////////////////////////////////////////////////////////////////
// CParticles accessor functions

void CParticles::Draw(void) const
{
  // set up OpenGL for sparkles
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  if (CGameMain::m_bAlphaSupported)
    glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D, CTextures::GetTextureID(E_TEX_PARTICLE));

  // draw the sparkles
  for (unsigned int uiSparkle = 0; uiSparkle < m_uiNumSparkles; uiSparkle++)
    {
    const SSparkles *psSparkle = &m_asSparkles[uiSparkle];
    glBegin(GL_QUADS);
    for (unsigned int uiVertex = 0; uiVertex < psSparkle->uiNumQuads; uiVertex++)
      {
      if (psSparkle->pfColors[uiVertex][3] > 0.0f)
        {
        float fX = psSparkle->pfVertices[uiVertex][0];
        float fY = psSparkle->pfVertices[uiVertex][1];
        float fZ = psSparkle->pfVertices[uiVertex][2];
        glColor4fv(psSparkle->pfColors[uiVertex]);
        glTexCoord2d(0.0f, 0.0f); glVertex3f(fX - SPARKLE_SIZE, fY - SPARKLE_SIZE, fZ);
        glTexCoord2d(0.0f, 1.0f); glVertex3f(fX - SPARKLE_SIZE, fY + SPARKLE_SIZE, fZ);
        glTexCoord2d(1.0f, 1.0f); glVertex3f(fX + SPARKLE_SIZE, fY + SPARKLE_SIZE, fZ);
        glTexCoord2d(1.0f, 0.0f); glVertex3f(fX + SPARKLE_SIZE, fY - SPARKLE_SIZE, fZ);
        }
      }
    glEnd();
    }

  // draw exploding Laser Base
  if (m_bLaserBase)
    {
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, CTextures::GetTextureID(E_TEX_LASER));
    glBegin(GL_QUADS);
    glColor3ub(255,255,255);
    for (unsigned int uiVertex = 0; uiVertex < m_sLaserBase.uiNumQuads; uiVertex++)
      {
      if ((rand() & 7) < 3)  // only draw a random 3/8ths of the points 
        {
        float fX = m_sLaserBase.pfVertices[uiVertex][0];
        float fY = m_sLaserBase.pfVertices[uiVertex][1];
        float fZ = m_sLaserBase.pfVertices[uiVertex][2];
        glTexCoord2d(0.0f, 0.0f); glVertex3f(fX - 15.0f, fY - 15.0f, fZ);
        glTexCoord2d(0.0f, 1.0f); glVertex3f(fX - 15.0f, fY + 15.0f, fZ);
        glTexCoord2d(1.0f, 1.0f); glVertex3f(fX + 15.0f, fY + 15.0f, fZ);
        glTexCoord2d(1.0f, 0.0f); glVertex3f(fX + 15.0f, fY - 15.0f, fZ);
        }
      }
    glEnd();
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }

  // set up OpenGL for polygons
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  if (CGameMain::m_bAlphaSupported)
    glDisable(GL_BLEND);
  glDisable(GL_CULL_FACE);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  GLfloat pfSpecular[4]  = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat pfShininess[1] = {32.0f};
  glMaterialfv(GL_FRONT, GL_SPECULAR, pfSpecular);
  glMaterialfv(GL_FRONT, GL_SHININESS, pfShininess);

  // draw the quads for the Polygons
  for (unsigned int uiPoly = 0; uiPoly < m_uiNumPolys; uiPoly++)
    {
    const SPolys *psPoly       = &m_asPolys[uiPoly];
    glVertexPointer(3, GL_FLOAT, 4 * sizeof(float), psPoly->pfQuadVertices);
    glNormalPointer(GL_FLOAT, 4 * sizeof(float), psPoly->pfQuadNormals);
    unsigned int uiIndex = 0;
    for (unsigned int uiColor = 0; uiColor < psPoly->uiNumQuadColors; uiColor++)
      {
      const unsigned char *pucColor = psPoly->aucQuadColors[uiColor];
      GLint piDiffuse[4] = {pucColor[0] << 23, pucColor[1] << 23, pucColor[2] << 23, 0x7fffffff};
      GLint piAmbient[4] = {pucColor[0] << 21, pucColor[1] << 21, pucColor[2] << 21, 0x7fffffff};
      glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
      glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
      unsigned int uiNumVertices = psPoly->uiNumQuads[uiColor] * 4;
      glDrawArrays(GL_QUADS, uiIndex, uiNumVertices);
      uiIndex += uiNumVertices;
      }
    }

  // draw the triangles for the Polygons
  for (unsigned int uiPoly = 0; uiPoly < m_uiNumPolys; uiPoly++)
    {
    const SPolys *psPoly       = &m_asPolys[uiPoly];
    glVertexPointer(3, GL_FLOAT, 4 * sizeof(float), psPoly->pfTriVertices);
    glNormalPointer(GL_FLOAT, 4 * sizeof(float), psPoly->pfTriNormals);
    unsigned int uiIndex = 0;
    for (unsigned int uiColor = 0; uiColor < psPoly->uiNumTriColors; uiColor++)
      {
      const unsigned char *pucColor = psPoly->aucTriColors[uiColor];
      GLint piDiffuse[4] = {pucColor[0] << 23, pucColor[1] << 23, pucColor[2] << 23, 0x7fffffff};
      GLint piAmbient[4] = {pucColor[0] << 21, pucColor[1] << 21, pucColor[2] << 21, 0x7fffffff};
      glMaterialiv(GL_FRONT, GL_DIFFUSE, piDiffuse);
      glMaterialiv(GL_FRONT, GL_AMBIENT, piAmbient);
      unsigned int uiNumVertices = psPoly->uiNumTriangles[uiColor] * 3;
      glDrawArrays(GL_TRIANGLES, uiIndex, uiNumVertices);
      uiIndex += uiNumVertices;
      }
    }

  // restore OpenGL
  glEnable(GL_CULL_FACE);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
}

/////////////////////////////////////////////////////////////////////////////
// CParticles modifier functions

void CParticles::ProcessState(unsigned int uiFrameTime)
{
  // first, handle the sparkles
  for (unsigned int uiSparkle = 0; uiSparkle < m_uiNumSparkles; uiSparkle++)
    {
    SSparkles *psSparkle = &m_asSparkles[uiSparkle];
    // first increment the frame counter
    psSparkle->uiTimeCount += uiFrameTime;
    // get rid of this object if it's too old
    if (psSparkle->uiTimeCount > SPARKLE_LIFE)
      {
      RemoveSparkle(uiSparkle);
      uiSparkle--;
      continue;
      }
    // apply velocity and color fading
    float *pfVertex   = (float *) psSparkle->pfVertices;
    float *pfVelocity = (float *) psSparkle->pfVelocity;
    for (unsigned int uiVertex = 0; uiVertex < psSparkle->uiNumQuads; uiVertex++)
      {
      pfVertex[0] += pfVelocity[0] * uiFrameTime;
      pfVertex[1] += pfVelocity[1] * uiFrameTime;
      pfVertex[2] += pfVelocity[2] * uiFrameTime;
      pfVertex   += 3;
      pfVelocity += 3;
      float fFade = psSparkle->pfFade[uiVertex] * uiFrameTime;
      if (psSparkle->pfColors[uiVertex][3] > fFade)
        psSparkle->pfColors[uiVertex][3] -= fFade;
      else
        psSparkle->pfColors[uiVertex][3] = 0;
      }
    // apply gravity
    pfVelocity = (float *) psSparkle->pfVelocity;
    for (unsigned int uiVertex = 0; uiVertex < psSparkle->uiNumQuads; uiVertex++)
      {
      if (pfVelocity[1] < SPARKLE_TERM_VEL) pfVelocity[1] += GRAVITY_STRENGTH * (float) uiFrameTime;
      pfVelocity += 3;
      }
    // done with this "Sparkles" object
    }

  // handle the exploding Laser Base
  if (m_bLaserBase)
    {
    // first increment the frame counter
    m_sLaserBase.uiTimeCount += uiFrameTime;
    // get rid of this if it's too old
    if (m_sLaserBase.uiTimeCount > 5000)
      {
      // free memory
      free(m_sLaserBase.pfVertices);
      free(m_sLaserBase.pfVelocity);
      m_bLaserBase = false;
      }
    else
      {
      // apply velocity and gravity
      float *pfVertex   = (float *) m_sLaserBase.pfVertices;
      float *pfVelocity = (float *) m_sLaserBase.pfVelocity;
      for (unsigned int uiVertex = 0; uiVertex < m_sLaserBase.uiNumQuads; uiVertex++)
        {
        pfVertex[0] += pfVelocity[0] * (float) uiFrameTime;
        pfVertex[1] += pfVelocity[1] * (float) uiFrameTime;
        pfVertex[2] += pfVelocity[2] * (float) uiFrameTime;
        if (pfVelocity[1] < 1.08f) pfVelocity[1] += 0.00216f * uiFrameTime;
        pfVertex   += 3;
        pfVelocity += 3;
        }
      }
    }

  float fSin[32], fCos[32], fSinCos[32][2], fCosSin[32][2];
  if (m_uiNumPolys > 0)
    {
    // calculate sin and cosine values for this time interval
    for (int i = 0; i < 32; i++)
      {
      float fRad = (i - 15) * (int) uiFrameTime * 0.0008f;
      fSinCos[i][0] = -sin(fRad);
      fSin[i] = fCosSin[i][1] = sin(fRad);
      fCos[i] = fSinCos[i][1] = fCosSin[i][0] = cos(fRad);
      }
    }

  // next, handle the polygons
  for (unsigned int uiPoly = 0; uiPoly < m_uiNumPolys; uiPoly++)
    {
    SPolys *psPoly = &m_asPolys[uiPoly];
    // first increment the frame counter
    psPoly->uiTimeCount += uiFrameTime;
    // get rid of this object if it's too old
    if (psPoly->uiTimeCount > POLY_LIFE)
      {
      RemovePoly(uiPoly);
      uiPoly--;
      continue;
      }
    // get total number of quads and triangles
    unsigned int uiTotalQuads = 0;
    unsigned int uiTotalTris  = 0;
    for (unsigned int ui = 0; ui < psPoly->uiNumQuadColors; ui++) uiTotalQuads += psPoly->uiNumQuads[ui];
    for (unsigned int ui = 0; ui < psPoly->uiNumTriColors; ui++)  uiTotalTris  += psPoly->uiNumTriangles[ui];
    // apply rotation and velocity to quads
    float (*pfQuadVertex)[4] = (float (*)[4]) psPoly->pfQuadVertices;
    float  *pfQuadCenter     = (float *)      psPoly->pfQuadCenter;
    float  *pfVelocity       = (float *)      psPoly->pfQuadVelocity;
#if !defined(NO_SSE)
    asmParticleQuadSSE(uiTotalQuads, pfQuadVertex, pfQuadCenter, pfVelocity, psPoly->pucQuadRot, fSinCos, fCosSin, uiFrameTime);
#else
    for (unsigned int uiQuad = 0; uiQuad < uiTotalQuads; uiQuad++)
      {
      float fVelX = pfVelocity[0] * (float) uiFrameTime;
      float fVelY = pfVelocity[1] * (float) uiFrameTime;
      float fVelZ = pfVelocity[2] * (float) uiFrameTime;
      unsigned char ucRotIdx = psPoly->pucQuadRot[uiQuad];
      float fCentX = pfQuadCenter[0];
      float fCentY = pfQuadCenter[1];
      for (unsigned int uiVert = 0; uiVert < 4; uiVert++)
        {
        // first, apply rotation
        float fX = pfQuadVertex[uiVert][0] * fCos[ucRotIdx] - pfQuadVertex[uiVert][1] * fSin[ucRotIdx] - fCentX * fCos[ucRotIdx] + fCentY * fSin[ucRotIdx] + fCentX;
        float fY = pfQuadVertex[uiVert][0] * fSin[ucRotIdx] + pfQuadVertex[uiVert][1] * fCos[ucRotIdx] - fCentX * fSin[ucRotIdx] - fCentY * fCos[ucRotIdx] + fCentY;
        // apply velocity to vertex
        pfQuadVertex[uiVert][0] = fX + fVelX;
        pfQuadVertex[uiVert][1] = fY + fVelY;
        pfQuadVertex[uiVert][2] += fVelZ;
        }
      // apply velocity to center
      pfQuadCenter[0] += fVelX;
      pfQuadCenter[1] += fVelY;
      pfQuadCenter[2] += fVelZ;
      // advance pointers
      pfQuadVertex   += 4;
      pfQuadCenter   += 4;
      pfVelocity     += 4;
      }
#endif
    // apply gravity to quads
    pfVelocity = (float *) psPoly->pfQuadVelocity;
    for (unsigned int uiQuad = 0; uiQuad < uiTotalQuads; uiQuad++)
      {
      if (pfVelocity[1] < POLY_TERM_VEL) pfVelocity[1] += GRAVITY_STRENGTH * (float) uiFrameTime;
      pfVelocity += 4;
      }
    // apply velocity to triangles
    float (*pfTriVertex)[4]  = (float (*)[4]) psPoly->pfTriVertices;
    float  *pfTriCenter      = (float *)      psPoly->pfTriCenter;
    pfVelocity               = (float *)      psPoly->pfTriVelocity;
#if !defined(NO_SSE)
    asmParticleTriSSE(uiTotalTris, pfTriVertex, pfTriCenter, pfVelocity, psPoly->pucTriRot, fSinCos, fCosSin, uiFrameTime);
#else
    for (unsigned int uiTri = 0; uiTri < uiTotalTris; uiTri++)
      {
      float fVelX = pfVelocity[0] * (float) uiFrameTime;
      float fVelY = pfVelocity[1] * (float) uiFrameTime;
      float fVelZ = pfVelocity[2] * (float) uiFrameTime;
      unsigned char ucRotIdx = psPoly->pucTriRot[uiTri];
      float fCentX = pfTriCenter[0];
      float fCentY = pfTriCenter[1];
      for (unsigned int uiVert = 0; uiVert < 3; uiVert++)
        {
        // first, apply rotation
        float fX = pfTriVertex[uiVert][0] * fCos[ucRotIdx] - pfTriVertex[uiVert][1] * fSin[ucRotIdx] - fCentX * fCos[ucRotIdx] + fCentY * fSin[ucRotIdx] + fCentX;
        float fY = pfTriVertex[uiVert][0] * fSin[ucRotIdx] + pfTriVertex[uiVert][1] * fCos[ucRotIdx] - fCentX * fSin[ucRotIdx] - fCentY * fCos[ucRotIdx] + fCentY;
        // apply velocity to vertex
        pfTriVertex[uiVert][0] = fX + fVelX;
        pfTriVertex[uiVert][1] = fY + fVelY;
        pfTriVertex[uiVert][2] += fVelZ;
        }
      // apply velocity to center
      pfTriCenter[0] += fVelX;
      pfTriCenter[1] += fVelY;
      pfTriCenter[2] += fVelZ;
      // advance pointers
      pfTriVertex   += 3;
      pfTriCenter   += 4;
      pfVelocity    += 4;
      }
#endif
    // apply gravity to triangles
    pfVelocity = (float *) psPoly->pfTriVelocity;
    for (unsigned int uiTri = 0; uiTri < uiTotalTris; uiTri++)
      {
      if (pfVelocity[1] < POLY_TERM_VEL) pfVelocity[1] += GRAVITY_STRENGTH * (float) uiFrameTime;
      pfVelocity += 4;
      }
    // done with this "Polys" object
    }

}

bool CParticles::AddInvaderSparkle(CInvader *pcInvader)
{
  // find out how many vertices this dude has
  unsigned int uiVertices = pcInvader->CountVertices();

  // allocate memory for tables
  float (*pfVertex)[3] = (float (*)[3]) malloc(uiVertices * 3 * sizeof(float));
  if (!pfVertex) return false;
  float (*pfVelocity)[3] = (float (*)[3]) malloc(uiVertices * 3 * sizeof(float));
  if (!pfVelocity) { free(pfVertex); return false; }
  float (*pfColors)[4] = (float (*)[4]) malloc(uiVertices * 4 * sizeof(float));
  if (!pfColors) { free(pfVertex); free(pfVelocity); return false; }
  float *pfFade = (float *) malloc(uiVertices * sizeof(float));
  if (!pfFade)  { free(pfVertex); free(pfVelocity); free(pfColors); return false; }

  // if we have no free objects, nuke the oldest one
  if (m_uiNumSparkles == 16) RemoveSparkle(0);

  // fiil in the new Sparkle object
  m_asSparkles[m_uiNumSparkles].pfVertices   = pfVertex;
  m_asSparkles[m_uiNumSparkles].pfVelocity   = pfVelocity;
  m_asSparkles[m_uiNumSparkles].pfColors     = pfColors;
  m_asSparkles[m_uiNumSparkles].pfFade       = pfFade;
  m_asSparkles[m_uiNumSparkles].uiTimeCount  = 0;
  m_asSparkles[m_uiNumSparkles].uiNumQuads   = uiVertices;
  m_uiNumSparkles++;

  // get invader position and pointer to its vertices
  float fPosX, fPosY, fPosZ;
  pcInvader->GetPosition(&fPosX, &fPosY, &fPosZ);
  float (*pfSrcVertex)[3] = (float (*)[3]) pcInvader->GetVertices();

  // get the current velocity of the invader
  float fVelX, fVelY;
  pcInvader->GetVelocity(&fVelX, &fVelY);

  // fill in the velocity and vertex tables
  for (unsigned int uiVertex = 0; uiVertex < uiVertices; uiVertex++)
    {
    float fX = pfSrcVertex[uiVertex][0];
    float fY = pfSrcVertex[uiVertex][1];
    float fZ = pfSrcVertex[uiVertex][2];
    float fR = sqrt(fX * fX + fY * fY + fZ * fZ);
    pfVertex[uiVertex][0] = fX + fPosX;
    pfVertex[uiVertex][1] = fY + fPosY;
    pfVertex[uiVertex][2] = fZ + fPosZ;
    pfVelocity[uiVertex][0] = fX * (SPARKLE_INIT_VEL * (96 + (rand() & 63)) / 128.0f) / fR + fVelX;
    pfVelocity[uiVertex][1] = fY * (SPARKLE_INIT_VEL * (96 + (rand() & 63)) / 128.0f) / fR + fVelY;
    pfVelocity[uiVertex][2] = fZ * (SPARKLE_INIT_VEL * (96 + (rand() & 63)) / 128.0f) / fR;
    }

  // fill in the colors and fade tables
  for (unsigned int uiVertex = 0; uiVertex < uiVertices; uiVertex++)
    {
    int iColor = rand() % 12;
    pfColors[uiVertex][0] = m_afColors[iColor][0];
    pfColors[uiVertex][1] = m_afColors[iColor][1];
    pfColors[uiVertex][2] = m_afColors[iColor][2];
    pfColors[uiVertex][3] = 1.0f;
    pfFade[uiVertex] = ((rand() & 63) + 2) / 4096.0f;
    }

  return true;
}

bool CParticles::AddInvaderPoly(CInvader *pcInvader)
{
  // find out how many vertices this dude has
  unsigned int uiQuads = pcInvader->CountQuads();
  unsigned int uiTris  = pcInvader->CountTriangles();

  unsigned char *pucQuadRot = NULL;
  unsigned char *pucTriRot = NULL;
  if (uiQuads > 0)
    {
    pucQuadRot = (unsigned char *) malloc(uiQuads);
    if (!pucQuadRot) return false;
    }
  if (uiTris > 0)
    {
    pucTriRot = (unsigned char *) malloc(uiTris);
    if (!pucTriRot) { free(pucQuadRot); return false; }
    }

  // allocate memory for tables
  float (*pfQuadVertices)[4][4] = NULL;
  float (*pfTriVertices)[3][4] = NULL;
  float (*pfQuadNormals)[4][4] = NULL;
  float (*pfTriNormals)[3][4] = NULL;
  float (*pfQuadVelocity)[4] = NULL;
  float (*pfTriVelocity)[4] = NULL;
  float (*pfQuadCenter)[4] = NULL;
  float (*pfTriCenter)[4] = NULL;
  if (uiQuads > 0)
    {
    pfQuadVertices = (float (*)[4][4]) _aligned_malloc(uiQuads * 4 * 4 * sizeof(float), 16);
    if (!pfQuadVertices) return false;
    pfQuadVelocity = (float (*)[4]) _aligned_malloc(uiQuads * 4 * sizeof(float), 16);
    if (!pfQuadVelocity) { free(pfQuadVertices); return false; }
    pfQuadNormals = (float (*)[4][4]) _aligned_malloc(uiQuads * 4 * 4 * sizeof(float), 16);
    if (!pfQuadNormals) { free(pfQuadVelocity); free(pfQuadVertices); return false; }
    pfQuadCenter = (float (*)[4]) _aligned_malloc(uiQuads * 4 * sizeof(float), 16);
    if (!pfQuadCenter) { free(pfQuadNormals); free(pfQuadVelocity); free(pfQuadVertices); return false; }
    }
  if (uiTris > 0)
    {
    pfTriVertices = (float (*)[3][4]) _aligned_malloc(uiTris * 3 * 4 * sizeof(float), 16);
    if (!pfTriVertices) { free(pfQuadCenter); free(pfQuadNormals); free(pfQuadVelocity); free(pfQuadVertices); return false; }
    pfTriVelocity = (float (*)[4]) _aligned_malloc(uiTris * 4 * sizeof(float), 16);
    if (!pfTriVelocity) { free(pfTriVertices); free(pfQuadCenter); free(pfQuadNormals); free(pfQuadVelocity); free(pfQuadVertices); return false; }
    pfTriNormals = (float (*)[3][4]) _aligned_malloc(uiTris * 3 * 4 * sizeof(float), 16);
    if (!pfTriNormals) { free(pfTriVelocity); free(pfTriVertices); free(pfQuadCenter); free(pfQuadNormals); free(pfQuadVelocity); free(pfQuadVertices); return false; }
    pfTriCenter = (float (*)[4]) _aligned_malloc(uiTris * 4 * sizeof(float), 16);
    if (!pfTriNormals) { free(pfTriNormals); free(pfTriVelocity); free(pfTriVertices); free(pfQuadCenter); free(pfQuadNormals); free(pfQuadVelocity); free(pfQuadVertices); return false; }
    }

  // if we have no free objects, nuke the oldest one
  if (m_uiNumPolys == 16) RemovePoly(0);

  // fiil in the new Polys object
  m_asPolys[m_uiNumPolys].pfQuadVertices  = pfQuadVertices;
  m_asPolys[m_uiNumPolys].pfQuadVelocity  = pfQuadVelocity;
  m_asPolys[m_uiNumPolys].pfQuadNormals   = pfQuadNormals;
  m_asPolys[m_uiNumPolys].pfTriVertices   = pfTriVertices;
  m_asPolys[m_uiNumPolys].pfTriVelocity   = pfTriVelocity;
  m_asPolys[m_uiNumPolys].pfTriNormals    = pfTriNormals;
  m_asPolys[m_uiNumPolys].pfQuadCenter    = pfQuadCenter;
  m_asPolys[m_uiNumPolys].pfTriCenter     = pfTriCenter;
  m_asPolys[m_uiNumPolys].pucQuadRot      = pucQuadRot;
  m_asPolys[m_uiNumPolys].pucTriRot       = pucTriRot;
  m_asPolys[m_uiNumPolys].uiNumQuadColors = 0;
  m_asPolys[m_uiNumPolys].uiNumTriColors  = 0;
  m_asPolys[m_uiNumPolys].uiTimeCount     = 0;
  m_uiNumPolys++;

  // get the current velocity of the invader
  float fVelX, fVelY;
  pcInvader->GetVelocity(&fVelX, &fVelY);

  // fill in some random velocities
  for (unsigned int uiQ = 0; uiQ < uiQuads; uiQ++)
    {
    float fAngle1 = (float) (rand() & 1023) * (float) M_PI / 512.0f;
    float fAngle2 = (float) (rand() & 1023) * (float) M_PI / 1023.0f;
    float fR = POLY_INIT_VEL * (96 + (rand() & 63)) / 128.0f;
    pfQuadVelocity[uiQ][0] = fVelX + fR * sin(fAngle2) * sin(fAngle1);
    pfQuadVelocity[uiQ][1] = fVelY + fR * cos(fAngle2);
    pfQuadVelocity[uiQ][2] = fR * sin(fAngle2) * cos(fAngle1);
    pfQuadVelocity[uiQ][3] = 0.0f;
    }
  for (unsigned int uiT = 0; uiT < uiTris; uiT++)
    {
    float fAngle1 = (float) (rand() & 1023) * (float) M_PI / 512.0f;
    float fAngle2 = (float) (rand() & 1023) * (float) M_PI / 1023.0f;
    float fR = POLY_INIT_VEL * (96 + (rand() & 63)) / 128.0f;
    pfTriVelocity[uiT][0] = fVelX + fR * sin(fAngle2) * sin(fAngle1);
    pfTriVelocity[uiT][1] = fVelY + fR * cos(fAngle2);
    pfTriVelocity[uiT][2] = fR * sin(fAngle2) * cos(fAngle1);
    pfTriVelocity[uiT][3] = 0.0f;
    }

  // random rotational indices
  for (unsigned int uiQ = 0; uiQ < uiQuads; uiQ++)
    {
    pucQuadRot[uiQ] = (rand() & 31);
    }
  for (unsigned int uiT = 0; uiT < uiTris; uiT++)
    {
    pucTriRot[uiT] = (rand() & 31);
    }

  // clear arrays
  if (uiQuads > 0)
    {
    memset(pfQuadVertices, 0, uiQuads * 4 * 4 * sizeof(float));
    memset(pfQuadNormals, 0, uiQuads * 4 * 4 * sizeof(float));
    }
  if (uiTris > 0)
    {
    memset(pfTriVertices, 0, uiTris * 3 * 4 * sizeof(float));
    memset(pfTriNormals, 0, uiTris * 3 * 4 * sizeof(float));
    }

  // get the vertices for the quads and triangles from the Invader object
  pcInvader->GetParticlePolys(m_asPolys + m_uiNumPolys - 1);

  // fix the normals so they're always pointing at us, and adjust the vertices by the invader center position
  float fPosX, fPosY, fPosZ;
  pcInvader->GetPosition(&fPosX, &fPosY, &fPosZ);
  for (unsigned int uiQ = 0; uiQ < uiQuads; uiQ++)
    {
    for (unsigned int uiV = 0; uiV < 4; uiV++)
      {
      pfQuadVertices[uiQ][uiV][0] += fPosX;
      pfQuadVertices[uiQ][uiV][1] += fPosY;
      pfQuadVertices[uiQ][uiV][2] += fPosZ;
      if (pfQuadNormals[uiQ][uiV][2] > 0)
        {
        pfQuadNormals[uiQ][uiV][0] = -pfQuadNormals[uiQ][uiV][0];
        pfQuadNormals[uiQ][uiV][1] = -pfQuadNormals[uiQ][uiV][1];
        pfQuadNormals[uiQ][uiV][2] = -pfQuadNormals[uiQ][uiV][2];
        }
      }
    }
  for (unsigned int uiT = 0; uiT < uiTris; uiT++)
    {
    for (unsigned int uiV = 0; uiV < 3; uiV++)
      {
      pfTriVertices[uiT][uiV][0] += fPosX;
      pfTriVertices[uiT][uiV][1] += fPosY;
      pfTriVertices[uiT][uiV][2] += fPosZ;
      if (pfTriNormals[uiT][uiV][2] > 0)
        {
        pfTriNormals[uiT][uiV][0] = -pfTriNormals[uiT][uiV][0];
        pfTriNormals[uiT][uiV][1] = -pfTriNormals[uiT][uiV][1];
        pfTriNormals[uiT][uiV][2] = -pfTriNormals[uiT][uiV][2];
        }
      }
    }

  // calculate the polygon centers
  for (unsigned int uiQ = 0; uiQ < uiQuads; uiQ++)
    {
    pfQuadCenter[uiQ][0] = (pfQuadVertices[uiQ][0][0] + pfQuadVertices[uiQ][1][0] + pfQuadVertices[uiQ][2][0] + pfQuadVertices[uiQ][3][0]) / 4.0f;
    pfQuadCenter[uiQ][1] = (pfQuadVertices[uiQ][0][1] + pfQuadVertices[uiQ][1][1] + pfQuadVertices[uiQ][2][1] + pfQuadVertices[uiQ][3][1]) / 4.0f;
    pfQuadCenter[uiQ][2] = (pfQuadVertices[uiQ][0][2] + pfQuadVertices[uiQ][1][2] + pfQuadVertices[uiQ][2][2] + pfQuadVertices[uiQ][3][2]) / 4.0f;
    pfQuadCenter[uiQ][3] = 0.0f;
    }
  for (unsigned int uiT = 0; uiT < uiTris; uiT++)
    {
    pfTriCenter[uiT][0] = (pfTriVertices[uiT][0][0] + pfTriVertices[uiT][1][0] + pfTriVertices[uiT][2][0]) / 3.0f;
    pfTriCenter[uiT][1] = (pfTriVertices[uiT][0][1] + pfTriVertices[uiT][1][1] + pfTriVertices[uiT][2][1]) / 3.0f;
    pfTriCenter[uiT][2] = (pfTriVertices[uiT][0][2] + pfTriVertices[uiT][1][2] + pfTriVertices[uiT][2][2]) / 3.0f;
    pfTriCenter[uiT][3] = 0.0f;
    }

  return true;
}

bool CParticles::AddLaserBase(CLaserBase *pcLaserBase)
{
  // find out how many vertices the LaserBase has
  unsigned int uiVertices = pcLaserBase->CountVertices();

  // allocate memory for tables
  float (*pfVertex)[3] = (float (*)[3]) malloc(uiVertices * 3 * sizeof(float));
  if (!pfVertex) return false;
  float (*pfVelocity)[3] = (float (*)[3]) malloc(uiVertices * 3 * sizeof(float));
  if (!pfVelocity) { free(pfVertex); return false; }

  // fiil in the LaserBase struct
  m_sLaserBase.pfVertices   = pfVertex;
  m_sLaserBase.pfVelocity   = pfVelocity;
  m_sLaserBase.pfColors     = NULL;
  m_sLaserBase.pfFade       = NULL;
  m_sLaserBase.uiTimeCount  = 0;
  m_sLaserBase.uiNumQuads   = uiVertices;

  // set the flag
  m_bLaserBase = true;

  // get invader position and pointer to its vertices
  float fPosX, fPosY, fPosZ;
  pcLaserBase->GetPosition(&fPosX, &fPosY, &fPosZ);
  float (*pfSrcVertex)[3] = (float (*)[3]) pcLaserBase->GetVertices();

  // fill in the velocity and vertex tables
  for (unsigned int uiVertex = 0; uiVertex < uiVertices; uiVertex++)
    {
    // generate a pair of gaussian random variables
    float fPhi = ((rand() & 1023) + 1) / 1024.0f;
    float fR   = ((rand() & 1023) + 1) / 1024.0f;
    float fG1   = cos(2 * 3.14159265f * fPhi) * sqrt(-2.0f * log(fR));
    float fG2   = sin(2 * 3.14159265f * fPhi) * sqrt(-2.0f * log(fR));
    float fX = pfSrcVertex[uiVertex][0];
    float fY = pfSrcVertex[uiVertex][1];
    float fZ = pfSrcVertex[uiVertex][2];
    pfVertex[uiVertex][0] = fX + fPosX;
    pfVertex[uiVertex][1] = fY + fPosY;
    pfVertex[uiVertex][2] = fZ + fPosZ + (rand() & 127) / 128.0f;
    pfVelocity[uiVertex][0] = 0.18f * fG1;
    pfVelocity[uiVertex][1] = -0.6f + fG2 * 0.3f;
    pfVelocity[uiVertex][2] = 0.0f;
    }

  return true;
}

/////////////////////////////////////////////////////////////////////////////
// CParticles private functions

void CParticles::RemoveSparkle(unsigned int uiNumSparkle)
{
  // make sure this is a valid Sparkle
  if (uiNumSparkle >= m_uiNumSparkles) return;

  // free memory
  free(m_asSparkles[uiNumSparkle].pfVertices);
  free(m_asSparkles[uiNumSparkle].pfColors);
  free(m_asSparkles[uiNumSparkle].pfFade);
  free(m_asSparkles[uiNumSparkle].pfVelocity);

  // fill in the gap in our table
  memcpy(&m_asSparkles[uiNumSparkle], &m_asSparkles[uiNumSparkle+1], (m_uiNumSparkles - uiNumSparkle - 1) * sizeof(SSparkles));
  m_uiNumSparkles--;
}

void CParticles::RemovePoly(unsigned int uiNumPoly)
{
  // make sure this is a valid Poly
  if (uiNumPoly >= m_uiNumPolys) return;

  // free memory
  if (m_asPolys[uiNumPoly].pfQuadVertices) _aligned_free(m_asPolys[uiNumPoly].pfQuadVertices);
  if (m_asPolys[uiNumPoly].pfQuadNormals) _aligned_free(m_asPolys[uiNumPoly].pfQuadNormals);
  if (m_asPolys[uiNumPoly].pfQuadVelocity) _aligned_free(m_asPolys[uiNumPoly].pfQuadVelocity);
  if (m_asPolys[uiNumPoly].pfQuadCenter) _aligned_free(m_asPolys[uiNumPoly].pfQuadCenter);
  if (m_asPolys[uiNumPoly].pucQuadRot) free(m_asPolys[uiNumPoly].pucQuadRot);

  if (m_asPolys[uiNumPoly].pfTriVertices) _aligned_free(m_asPolys[uiNumPoly].pfTriVertices);
  if (m_asPolys[uiNumPoly].pfTriNormals) _aligned_free(m_asPolys[uiNumPoly].pfTriNormals);
  if (m_asPolys[uiNumPoly].pfTriVelocity) _aligned_free(m_asPolys[uiNumPoly].pfTriVelocity);
  if (m_asPolys[uiNumPoly].pfTriCenter) _aligned_free(m_asPolys[uiNumPoly].pfTriCenter);
  if (m_asPolys[uiNumPoly].pucTriRot) free(m_asPolys[uiNumPoly].pucTriRot);

  // fill in the gap in our table
  memcpy(&m_asPolys[uiNumPoly], &m_asPolys[uiNumPoly+1], (m_uiNumPolys - uiNumPoly - 1) * sizeof(SPolys));
  m_uiNumPolys--;
}
