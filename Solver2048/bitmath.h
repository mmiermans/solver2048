#pragma once

#include <stdint.h>
#include "FastRand.h"
#include <stdlib.h>
#include <intrin.h>
#include <time.h>

#pragma intrinsic(_BitScanReverse64)

class BitMath
{
public:
	BitMath::BitMath() {
	}

	BitMath::~BitMath() {
	}

	/// <summary>
	/// Counts the number of 1's using the super-fast Hamming weight algorithm.
	/// </summary>
	/// <returns>Number of high bits in x.</returns>
	static inline int popCount(uint64_t x) {
		const uint64_t m1 = 0x5555555555555555; //binary: 0101...
		const uint64_t m2 = 0x3333333333333333; //binary: 00110011..
		const uint64_t m4 = 0x0f0f0f0f0f0f0f0f; //binary:  4 zeros,  4 ones ...
		const uint64_t m8 = 0x00ff00ff00ff00ff; //binary:  8 zeros,  8 ones ...
		const uint64_t m16 = 0x0000ffff0000ffff; //binary: 16 zeros, 16 ones ...
		const uint64_t m32 = 0x00000000ffffffff; //binary: 32 zeros, 32 ones
		const uint64_t hff = 0xffffffffffffffff; //binary: all ones
		const uint64_t h01 = 0x0101010101010101; //the sum of 256 to the power of 0,1,2,3...

		x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
		x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits 
		x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits 
		return (x * h01) >> 56;  //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ... 
	}

	/// <summary>
	/// Initializes the fast random number generator.
	/// </summary>
	static void initRng(fastrand* fr) {
		uint32_t prngSeed[8];
		uint16_t *sptr = (uint16_t *)prngSeed;

		srand((unsigned int)(time(NULL) + clock()));

		// Randomize the seed values
		for (uint8_t i = 0; i < 8; i++) {
			prngSeed[i] = rand();
		}

		// Initialize the PRNG
		InitFastRand(sptr[0], sptr[1],
					 sptr[2], sptr[3],
					 sptr[4], sptr[5],
					 sptr[6], sptr[7],
					 sptr[8], sptr[9],
					 sptr[10], sptr[11],
					 sptr[12], sptr[13],
					 sptr[14], sptr[15],
					 fr);
	}

};
