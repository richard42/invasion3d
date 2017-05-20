/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Invasion3D - TextGL.h                                                 *
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


#if !defined(TEXTGL_H)
#define TEXTGL_H

#ifdef WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>

// enumerated type for weapon selection
typedef enum {E_FONT_6809 = 0,
              E_FONT_EUPHORIGENIC,
              E_FONT_CREDIT_RIVER,
              E_FONT_VECTROID,
              E_FONT_BERYLLIUM,
              E_FONT_INVALID} eFont;

// enumerated type for justification
typedef enum {E_LEFT = 0,
              E_CENTER,
              E_RIGHT,
              E_JUST_INVALID} EJustify;

// structure for each font
typedef struct {GLuint       glCharTexture;
                unsigned int uiCharPosition[87];
                unsigned int uiCharWidth[87];
                unsigned int uiFontHeight;
                unsigned int uiTextureWidth;
                unsigned int uiTextureHeight;
                unsigned int uiSpaceWidth;
               } sFont;

class CPackage;

class CTextGL
{
public:
  CTextGL();
  ~CTextGL();

  // function to initialize the static data
  static bool InitFonts(CPackage *pcMedia);
  static void DestroyFonts(void);

  // accessor functions
  float GetLineWidth(const char *pch, int iLength) const;
  float Width(void) const { return m_fWidth; }
  float PositionX(void) const { return m_fPositionX; }
  void Draw(void) const;

  // modifier functions
  void SetAlpha(bool bBlend) { m_bAlphaBlend = bBlend; }
  void SetColor(unsigned char uchRed, unsigned char uchGreen, unsigned char uchBlue);
  void SetFont(eFont eType);
  bool SetHeight(float fHeight);
  bool SetJustification(EJustify eJustify);
  void SetPosition(float fX, float fY, float fZ);
  bool SetText(const char *pchText);

private:
  // member data
  GLfloat        m_fPositionX;
  GLfloat        m_fPositionY;
  GLfloat        m_fPositionZ;
  GLfloat        m_fHeight;
  float          m_fWidth;
  bool           m_bAlphaBlend;
  char          *m_pchString;
  unsigned int   m_uiBufferLength;
  eFont          m_eFont;
  EJustify       m_eJustify;
  unsigned char  m_uchColor[3];

  // private accessors
  void DrawLine(const char *pchString, int iLength, float fX, float fY, float fZ) const;

  // private modifiers
  void CalculateWidth(void);

  // private static functions
  static bool CreateFontGL(CPackage *pcMedia, eFont eType, const char *pchBMPName, int iCharSpacing);

  // static data
  static bool          m_bInitialized;
  static unsigned int  m_uiNumFonts;
  static unsigned int  m_uiAsciiConv[256];
  static unsigned char m_uchCharTable[88];
  static sFont         m_asFonts[16];
  static unsigned int  m_uiMaxWidth;
};

#endif // defined(TEXTGL_H)
