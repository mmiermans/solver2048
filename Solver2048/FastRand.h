/*****************************************************************************
	
	Super-Fast MWC1616 Pseudo-Random Number Generator 
	for Intel/AMD Processors (using SSE or SSE4 instruction set)
	Copyright (c) 2012, Ivan Dimkovic
	All rights reserved.

	Redistribution and use in source and binary forms, with or without 
	modification, are permitted provided that the following conditions are met:

	Redistributions of source code must retain the above copyright notice, 
	this list of conditions and the following disclaimer.
	
	Redistributions in binary form must reproduce the above copyright notice, 
	this list of conditions and the following disclaimer in the documentation 
	and/or other materials provided with the distribution.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
	PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
	OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#ifndef FASTRAND_H_INCLUDED
#define FASTRAND_H_INCLUDED

#include <stdint.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>

//
// Structure holding MWC1616 data

typedef struct fastrand_t {

	//
	// MWC1616 data

	uint32_t a[4];
	uint32_t b[4];
	uint32_t mask[4];
	uint32_t m1[4];
	uint32_t m2[4];

	//
	// Result (4 32-bit random numbers)

	uint32_t res[4];

} fastrand;

//
// MWC1616 Initializer
// Choose random values for aX, bX, cX and dX parameters

static void InitFastRand( 
	uint16_t a1, uint16_t c1,
	uint16_t b1, uint16_t d1,
	uint16_t a2, uint16_t c2,
	uint16_t b2, uint16_t d2,
	uint16_t a3, uint16_t c3,
	uint16_t b3, uint16_t d3,
	uint16_t a4, uint16_t c4,
	uint16_t b4, uint16_t d4,
	fastrand *f)
{

	//
	// Initialize MWC1616 masks and multipliers
	// Default values of 18000 and 30903 used
	// for multipliers

	uint8_t i;
	
	for(i=0;i<4;i++) {
		f->mask[i]=0xFFFF;
		f->m1[i]=0x4650;
		f->m2[i]=0x78B7;
	}

	f->a[0]=((uint32_t )c1<<16) | a1;
	f->a[1]=((uint32_t )c2<<16) | a2;
	f->a[2]=((uint32_t )c3<<16) | a3;
	f->a[3]=((uint32_t )c4<<16) | a4;
	f->b[0]=((uint32_t )d1<<16) | b1;
	f->b[1]=((uint32_t )d2<<16) | b2;
	f->b[2]=((uint32_t )d3<<16) | b3;
	f->b[3]=((uint32_t )d4<<16) | b4;
}

///////////////////////////////
// MWC1616 --> SSE version  //
///////////////////////////////

static __inline void FastRand_SSE(fastrand *f)
{
	__m128i a = _mm_load_si128((const __m128i *)&f->a[0]);
	__m128i b = _mm_load_si128((const __m128i *)&f->b[0]);

	const __m128i mask = _mm_load_si128((const __m128i *)&f->mask[0]);
	const __m128i m1 = _mm_load_si128((const __m128i *)&f->m1[0]);
	const __m128i m2 = _mm_load_si128((const __m128i *)&f->m2[0]);

	__m128i ashift = _mm_srli_epi32(a, 0x10);
	__m128i amask = _mm_and_si128(a, mask);
	__m128i amullow = _mm_mullo_epi16(amask, m1);
	__m128i amulhigh = _mm_mulhi_epu16(amask, m1);
	__m128i amulhigh_shift = _mm_slli_epi32(amulhigh, 0x10);
	__m128i amul = _mm_or_si128(amullow, amulhigh_shift);
	__m128i anew = _mm_add_epi32(amul, ashift);
	_mm_store_si128((__m128i *)&f->a[0], anew);

	__m128i bshift = _mm_srli_epi32(b, 0x10);
	__m128i bmask = _mm_and_si128(b, mask);
	__m128i bmullow = _mm_mullo_epi16(bmask, m2);
	__m128i bmulhigh = _mm_mulhi_epu16(bmask, m2);
	__m128i bmulhigh_shift = _mm_slli_epi32(bmulhigh, 0x10);
	__m128i bmul = _mm_or_si128(bmullow, bmulhigh_shift);
	__m128i bnew = _mm_add_epi32(bmul, bshift);
	_mm_store_si128((__m128i *)&f->b[0], bnew);

	__m128i bmasknew = _mm_and_si128(bnew, mask);
	__m128i ashiftnew = _mm_slli_epi32(anew, 0x10);
	__m128i res = _mm_add_epi32(ashiftnew, bmasknew);
	_mm_store_si128((__m128i *)&f->res[0], res);
}

///////////////////////////////
// MWC1616 --> SSE4 version  //
///////////////////////////////

static inline void FastRand_SSE4(fastrand *f)
{
	__m128i a = _mm_load_si128((const __m128i *)f->a);
	__m128i b = _mm_load_si128((const __m128i *)f->b);

	const __m128i mask = _mm_load_si128((const __m128i *)f->mask);
	const __m128i m1 = _mm_load_si128((const __m128i *)f->m1);
	const __m128i m2 = _mm_load_si128((const __m128i *)f->m2);

	__m128i amask = _mm_and_si128(a, mask);
	__m128i ashift = _mm_srli_epi32(a, 0x10);
	__m128i amul = _mm_mullo_epi32(amask, m1);
	__m128i anew = _mm_add_epi32(amul, ashift);
	_mm_store_si128((__m128i *)f->a, anew);

	__m128i bmask = _mm_and_si128(b, mask);
	__m128i bshift = _mm_srli_epi32(b, 0x10);
	__m128i bmul = _mm_mullo_epi32(bmask, m2);
	__m128i bnew = _mm_add_epi32(bmul, bshift);
	_mm_store_si128((__m128i *)f->b, bnew);

	__m128i bmasknew = _mm_and_si128(bnew, mask);
	__m128i ashiftnew = _mm_slli_epi32(anew, 0x10);
	__m128i res = _mm_add_epi32(ashiftnew, bmasknew);
	_mm_store_si128((__m128i *)f->res, res);
}

#endif
