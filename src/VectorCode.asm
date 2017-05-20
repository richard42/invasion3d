;/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
; *   Invasion3D - VectorCode.asm                                           *
; *   Homepage: http://code.google.com/p/invasion3d/                        *
; *   Copyright (C) 2005-2011 Richard Goedeken                              *
; *                                                                         *
; *   This program is free software; you can redistribute it and/or modify  *
; *   it under the terms of the GNU General Public License as published by  *
; *   the Free Software Foundation; either version 2 of the License, or     *
; *   (at your option) any later version.                                   *
; *                                                                         *
; *   This program is distributed in the hope that it will be useful,       *
; *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
; *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
; *   GNU General Public License for more details.                          *
; *                                                                         *
; *   You should have received a copy of the GNU General Public License     *
; *   along with this program; if not, write to the                         *
; *   Free Software Foundation, Inc.,                                       *
; *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

%ifdef CFG_WIN32
  %macro Function 1
      global _%1
  %define %1 _%1
  %endmacro
%else
  %macro Function 1
      global %1
  %endmacro
%endif

Function asmMixerMMX
Function asmParticleQuadSSE
Function asmParticleTriSSE

;***************************************************************************
;***                                Code                                 ***
;***************************************************************************
segment .text

;------------------------------------------------------------------------------
; void asmMixerMMX(short *psClips[], unsigned int uiNumClips, unsigned int uiNumSamples, short *psOutput);
;
; Mixes 16-bit sound clips together to form a single output stream
;
; psClips      - table of pointers to sound clips
; uiNumClips   - number of sound effect clips to mix together
; uiNumSamples - number of 16-bit samples to process
; psOutput     - output buffr
;
align 16
asmMixerMMX:
  push ebp
  mov ebp, esp
  pushad

  ; first, clear the output buffer
  xor eax, eax
  mov edi, [ebp+20]             ; psOutput
  mov ecx, [ebp+16]             ; uiNumSamples
  shr ecx, 1
  rep stosd
  
  ; start of main loop
  mov eax, [ebp+12]             ; uiNumClips
  mov esi, [ebp+8]              ; psClips
ammMainLoop:
  mov edi, [ebp+20]             ; pSOutput
  cmp eax, 1
  je near ammMixOne
  cmp eax, 2
  je near ammMixTwo
  cmp eax, 3
  je ammMixThree

  ; mix four and repeat main loop
  mov eax, [esi]                ; address of first clip to mix
  mov ebx, [esi+4]              ; address of second clip to mix
  mov ecx, [esi+8]              ; address of third clip to mix
  mov edx, [esi+12]             ; address of fourth clip to mix
  mov esi, [ebp+16]             ; uiNumSamples
amm4Loop:
  movq mm0, [edi]               ; get existing value from output buffer
  movq mm1, [edi+8]             ; get existing value from output buffer
  paddsw mm0, [eax]
  paddsw mm1, [eax+8]
  add eax, 16
  paddsw mm0, [ebx]
  paddsw mm1, [ebx+8]
  add ebx, 16
  paddsw mm0, [ecx]
  paddsw mm1, [ecx+8]
  add ecx, 16
  paddsw mm0, [edx]
  paddsw mm1, [edx+8]
  add edx, 16
  movq [edi], mm0
  movq [edi+8], mm1
  add edi, 16
  sub esi, byte 8               ; we just did 8 samples
  jg amm4Loop
  mov eax, [ebp+12]             ; uiNumClips
  mov esi, [ebp+8]              ; psClips
  sub eax, 4
  add esi, 16
  mov [ebp+12], eax
  mov [ebp+8], esi
  test eax, eax
  jne ammMainLoop
  jmp ammDone
  
ammMixThree:  
  ; mix three and end
  mov eax, [esi]                ; address of first clip to mix
  mov ebx, [esi+4]              ; address of second clip to mix
  mov ecx, [esi+8]              ; address of third clip to mix
  mov esi, [ebp+16]             ; uiNumSamples
amm3Loop:
  movq mm0, [edi]               ; get existing value from output buffer
  movq mm1, [edi+8]             ; get existing value from output buffer
  paddsw mm0, [eax]
  paddsw mm1, [eax+8]
  add eax, 16
  paddsw mm0, [ebx]
  paddsw mm1, [ebx+8]
  add ebx, 16
  paddsw mm0, [ecx]
  paddsw mm1, [ecx+8]
  add ecx, 16
  movq [edi], mm0
  movq [edi+8], mm1
  add edi, 16
  sub esi, byte 8               ; we just did 8 samples
  jg amm3Loop
  jmp ammDone

ammMixTwo:
  ; mix two and end
  mov eax, [esi]                ; address of first clip to mix
  mov ebx, [esi+4]              ; address of second clip to mix
  mov esi, [ebp+16]             ; uiNumSamples
amm2Loop:
  movq mm0, [edi]               ; get existing value from output buffer
  movq mm1, [edi+8]             ; get existing value from output buffer
  paddsw mm0, [eax]
  paddsw mm1, [eax+8]
  add eax, 16
  paddsw mm0, [ebx]
  paddsw mm1, [ebx+8]
  add ebx, 16
  movq [edi], mm0
  movq [edi+8], mm1
  add edi, 16
  sub esi, byte 8               ; we just did 8 samples
  jg amm2Loop
  jmp ammDone

ammMixOne:
  ; mix one and end
  mov eax, [esi]                ; address of first clip to mix
  mov esi, [ebp+16]             ; uiNumSamples
amm1Loop:
  movq mm0, [edi]               ; get existing value from output buffer
  movq mm1, [edi+8]             ; get existing value from output buffer
  paddsw mm0, [eax]
  paddsw mm1, [eax+8]
  add eax, 16
  movq [edi], mm0
  movq [edi+8], mm1
  add edi, 16
  sub esi, byte 8               ; we just did 8 samples
  jg amm1Loop
  
ammDone:
  ; end of routine
  emms
  popad
  pop ebp
  ret

;------------------------------------------------------------------------------
; void asmParticleQuadSSE(int iQuads, float (*pfVertex)[3], float *pfCenter,
;                         float *pfVelocity, unsigned char *pucRot, float (*pfSinCos)[2],
;                         float (*pfCosSin)[2], unsigned int uiFrameTime);
;
; Applies velocity and rotation to particle engine quads using SSE instructions
;
; iQuads      - number of quads to process
; pfVertex    - float (*)[4][4] array of vertex coordinates
; pfCenter    - float (*)[4] array of quad centers
; pfVelocity  - float (*)[4] array of quad velocities
; pucRot      - array of [0,31] indices into sin/cos arrays for angular amount
; pfSinCos    - float[32][2] array of -sin, +cos values
; pfCosSin    - float[32][2] array of +cos, +sin values
; uiFrameTime - number of milliseconds in this frame
;
align 16
asmParticleQuadSSE:
  push ebp
  mov ebp, esp
  pushad

  ; load up the registers
  movd mm0, [ebp+36]
  mov ecx, [ebp+8]              ; iQuads
  mov edi, [ebp+12]             ; pfVertex
  punpckldq mm0, mm0
  mov esi, [ebp+16]             ; pfCenter
  mov eax, [ebp+20]             ; pfVelocity
  cvtpi2ps xmm0, mm0
  mov ebx, [ebp+24]             ; pucRot
  mov edx, [ebp+28]             ; pfSinCos
  mov ebp, [ebp+32]             ; pfCosSin
  movlhps xmm0, xmm0            ; 4 x uiFrameTime

  xorps xmm3, xmm3              ; clear the high 2 dwords in these registers
  xorps xmm4, xmm4
  xorps xmm5, xmm5
  xorps xmm7, xmm7
pqLoop1:
  ; load up all the registers for the rotation
  push ebx                      ; pucRot pointer
  movzx ebx, byte [ebx]         ; ebx == angular index for this quad
  movlps xmm6, [esi]            ; xmm6 = CenterY, CenterX
  movlps xmm2, [edi]            ; xmm2 = VertexY, VertexX
  movaps xmm1, xmm6
  unpcklps xmm2, xmm2           ; xmm2 = vY, vY, vX, vX
  unpcklps xmm6, xmm6           ; xmm6 = cY, cY, cX, cX
  movhlps xmm3, xmm2            ; xmm3 = vY, vY
  movhlps xmm7, xmm6            ; xmm7 = cY, cY
  movlps xmm4, [edx+ebx*8]      ; xmm4 = +cos_t, -sin_t
  movlps xmm5, [ebp+ebx*8]      ; xmm5 = +sin_t, +cos_t
  ; calculate the rotation around the center point for vertex 0 and store
  mulps xmm3, xmm4
  mulps xmm2, xmm5
  mulps xmm7, xmm4
  mulps xmm6, xmm5
  addps xmm3, xmm2
  addps xmm7, xmm6
  addps xmm3, xmm1
  subps xmm3, xmm7
  movlps [edi], xmm3
  ; now do vertex 1
  movlps xmm2, [edi+16]         ; xmm2 = VertexY, VertexX
  movaps xmm6, xmm1             ; xmm6 = CenterY, CenterX
  unpcklps xmm2, xmm2           ; xmm2 = vY, vY, vX, vX
  unpcklps xmm6, xmm6           ; xmm6 = cY, cY, cX, cX
  movhlps xmm3, xmm2            ; xmm3 = vY, vY
  movhlps xmm7, xmm6            ; xmm7 = cY, cY
  ; calculate the rotation around the center point for vertex 1 and store
  mulps xmm3, xmm4
  mulps xmm2, xmm5
  mulps xmm7, xmm4
  mulps xmm6, xmm5
  addps xmm3, xmm2
  addps xmm7, xmm6

  addps xmm3, xmm1
  subps xmm3, xmm7
  movlps [edi+16], xmm3
  ; now do vertex 2
  movlps xmm2, [edi+32]         ; xmm2 = VertexY, VertexX
  movaps xmm6, xmm1             ; xmm6 = CenterY, CenterX
  unpcklps xmm2, xmm2           ; xmm2 = vY, vY, vX, vX
  unpcklps xmm6, xmm6           ; xmm6 = cY, cY, cX, cX
  movhlps xmm3, xmm2            ; xmm3 = vY, vY
  movhlps xmm7, xmm6            ; xmm7 = cY, cY
  ; calculate the rotation around the center point for vertex 2 and store
  mulps xmm3, xmm4
  mulps xmm2, xmm5
  mulps xmm7, xmm4
  mulps xmm6, xmm5
  addps xmm3, xmm2
  addps xmm7, xmm6
  addps xmm3, xmm1
  subps xmm3, xmm7
  movlps [edi+32], xmm3
  ; now do vertex 3
  movlps xmm2, [edi+48]         ; xmm2 = VertexY, VertexX
  movaps xmm6, xmm1             ; xmm6 = CenterY, CenterX
  unpcklps xmm2, xmm2           ; xmm2 = vY, vY, vX, vX
  unpcklps xmm6, xmm6           ; xmm6 = cY, cY, cX, cX
  movhlps xmm3, xmm2            ; xmm3 = vY, vY
  movhlps xmm7, xmm6            ; xmm7 = cY, cY
  ; calculate the rotation around the center point for vertex 3 and store
  mulps xmm3, xmm4
  mulps xmm2, xmm5
  mulps xmm7, xmm4
  mulps xmm6, xmm5
  addps xmm3, xmm2
  addps xmm7, xmm6
  addps xmm3, xmm1
  subps xmm3, xmm7
  movlps [edi+48], xmm3
  ; calculate velocity to apply
  movaps xmm1, [eax]
  mulps xmm1, xmm0
  ; load the vertex and center coordinates, add the velocity, and store
  movaps xmm2, [edi]
  movaps xmm3, [edi+16]
  movaps xmm4, [edi+32]
  movaps xmm5, [edi+48]
  movaps xmm6, [esi]
  addps xmm2, xmm1
  addps xmm3, xmm1
  addps xmm4, xmm1
  addps xmm5, xmm1
  addps xmm6, xmm1
  movaps [edi], xmm2
  movaps [edi+16], xmm3
  movaps [edi+32], xmm4
  movaps [edi+48], xmm5
  movaps [esi], xmm6
  ; advance pointers to next quad and loop
  pop ebx
  add edi, byte 64
  add esi, byte 16
  add eax, byte 16
  add ebx, byte 1
  dec ecx
  jne near pqLoop1
  
  popad
  pop ebp
  emms
  ret

;------------------------------------------------------------------------------
; void asmParticleTriSSE(int iTris, float (*pfVertex)[3], float *pfCenter,
;                        float *pfVelocity, unsigned char *pucRot, float (*pfSinCos)[2],
;                        float (*pfCosSin)[2], unsigned int uiFrameTime);
;
; Applies velocity and rotation to particle engine triangles using SSE instructions
;
; iTris       - number of triangles to process
; pfVertex    - float (*)[3][4] array of vertex coordinates
; pfCenter    - float (*)[4] array of triangle centers
; pfVelocity  - float (*)[4] array of triangle velocities
; pucRot      - array of [0,31] indices into sin/cos arrays for angular amount
; pfSinCos    - float[32][2] array of -sin, +cos values
; pfCosSin    - float[32][2] array of +cos, +sin values
; uiFrameTime - number of milliseconds in this frame
;
align 16
asmParticleTriSSE:
  push ebp
  mov ebp, esp
  pushad

  ; load up the registers
  movd mm0, [ebp+36]
  mov ecx, [ebp+8]              ; iQuads
  mov edi, [ebp+12]             ; pfVertex
  punpckldq mm0, mm0
  mov esi, [ebp+16]             ; pfCenter
  mov eax, [ebp+20]             ; pfVelocity
  cvtpi2ps xmm0, mm0
  mov ebx, [ebp+24]             ; pucRot
  mov edx, [ebp+28]             ; pfSinCos
  mov ebp, [ebp+32]             ; pfCosSin
  movlhps xmm0, xmm0            ; 4 x uiFrameTime

  xorps xmm3, xmm3              ; clear the high 2 dwords in these registers
  xorps xmm4, xmm4
  xorps xmm5, xmm5
  xorps xmm7, xmm7
ptLoop1:
  ; load up all the registers for the rotation
  push ebx                      ; pucRot pointer
  movzx ebx, byte [ebx]         ; ebx == angular index for this quad
  movlps xmm6, [esi]            ; xmm6 = CenterY, CenterX
  movlps xmm2, [edi]            ; xmm2 = VertexY, VertexX
  movaps xmm1, xmm6
  unpcklps xmm2, xmm2           ; xmm2 = vY, vY, vX, vX
  unpcklps xmm6, xmm6           ; xmm6 = cY, cY, cX, cX
  movhlps xmm3, xmm2            ; xmm3 = vY, vY
  movhlps xmm7, xmm6            ; xmm7 = cY, cY
  movlps xmm4, [edx+ebx*8]      ; xmm4 = +cos_t, -sin_t
  movlps xmm5, [ebp+ebx*8]      ; xmm5 = +sin_t, +cos_t
  ; calculate the rotation around the center point for vertex 0 and store
  mulps xmm3, xmm4
  mulps xmm2, xmm5
  mulps xmm7, xmm4
  mulps xmm6, xmm5
  addps xmm3, xmm2
  addps xmm7, xmm6
  addps xmm3, xmm1
  subps xmm3, xmm7
  movlps [edi], xmm3
  ; now do vertex 1
  movlps xmm2, [edi+16]         ; xmm2 = VertexY, VertexX
  movaps xmm6, xmm1             ; xmm6 = CenterY, CenterX
  unpcklps xmm2, xmm2           ; xmm2 = vY, vY, vX, vX
  unpcklps xmm6, xmm6           ; xmm6 = cY, cY, cX, cX
  movhlps xmm3, xmm2            ; xmm3 = vY, vY
  movhlps xmm7, xmm6            ; xmm7 = cY, cY
  ; calculate the rotation around the center point for vertex 1 and store
  mulps xmm3, xmm4
  mulps xmm2, xmm5
  mulps xmm7, xmm4
  mulps xmm6, xmm5
  addps xmm3, xmm2
  addps xmm7, xmm6
  addps xmm3, xmm1
  subps xmm3, xmm7
  movlps [edi+16], xmm3
  ; now do vertex 2
  movlps xmm2, [edi+32]         ; xmm2 = VertexY, VertexX
  movaps xmm6, xmm1             ; xmm6 = CenterY, CenterX
  unpcklps xmm2, xmm2           ; xmm2 = vY, vY, vX, vX
  unpcklps xmm6, xmm6           ; xmm6 = cY, cY, cX, cX
  movhlps xmm3, xmm2            ; xmm3 = vY, vY
  movhlps xmm7, xmm6            ; xmm7 = cY, cY
  ; calculate the rotation around the center point for vertex 2 and store
  mulps xmm3, xmm4
  mulps xmm2, xmm5
  mulps xmm7, xmm4
  mulps xmm6, xmm5
  addps xmm3, xmm2
  addps xmm7, xmm6
  addps xmm3, xmm1
  subps xmm3, xmm7
  movlps [edi+32], xmm3
  ; calculate velocity to apply
  movaps xmm1, [eax]
  mulps xmm1, xmm0
  ; load the vertex and center coordinates, add the velocity, and store
  movaps xmm2, [edi]
  movaps xmm3, [edi+16]
  movaps xmm4, [edi+32]
  movaps xmm5, [esi]
  addps xmm2, xmm1
  addps xmm3, xmm1
  addps xmm4, xmm1
  addps xmm5, xmm1
  movaps [edi], xmm2
  movaps [edi+16], xmm3
  movaps [edi+32], xmm4
  movaps [esi], xmm5
  ; advance pointers to next quad and loop
  pop ebx
  add edi, byte 48
  add esi, byte 16
  add eax, byte 16
  add ebx, byte 1
  dec ecx
  jne near ptLoop1
  
  popad
  pop ebp
  emms
  ret

;***************************************************************************
;***                                Data                                 ***
;***************************************************************************
segment .data
