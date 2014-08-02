#pragma once
#include <string.h>
#include <float.h>

#include "bitmath.h"
#include "boardconstants.h"

class BoardHashTable
{
public:
	static const int sizePower = 27; // 27 = 512 MB
	static const int sizeMask = (1<<sizePower) - 1;
	static const int size = 1<<sizePower;
	const float nullValue = FLT_MAX;

	BoardHashTable() { table = new float[size]; clear(); }
	~BoardHashTable() { delete[] table; }

	inline void clear() {
		// Convert two sequential 32 bit floats to an 64 bit integer.
		union {
			struct { float a; float b; } input;
			uint64_t output;
		} data;
		data.input.a = nullValue;
		data.input.b = nullValue;
		uint64_t mask = data.output;

		// Clear table
		uint64_t* end = (uint64_t*)(table + size - 1);
		for (uint64_t* p = (uint64_t*)table; p < end; p++) {
			*p = mask;
		}

		// Set last one separately.
		table[size - 1] = nullValue;
	}

	inline float getValue(uint64_t index) {
		return table[index];
	}

	inline void putValue(uint64_t index, float v) {
		table[index] = v;
	}

	static inline uint64_t getIndex(Board h) {
		h = mix_fasthash(__ac_Wang_hash(h));
		h ^= (h >> sizePower);
		h ^= (h >> (64 - sizePower));
		return h & sizeMask;
	}

private:

	static inline Board mix_fasthash(Board h) {
		h ^= (h) >> 23;
		h *= 0x2127599bf4325c37ULL;
		h ^= (h) >> 47;
		return h;
	}

	static inline Board mix_murmur2(Board h) {
		(h) *= 0xc6a4a7935bd1e995ULL;
		(h) ^= (h) >> 47;
		(h) *= 0xc6a4a7935bd1e995ULL;
		return h;
	}

	static inline Board __ac_Wang_hash(Board key) {
		key += ~(key << 15);
		key ^= (key >> 10);
		key += (key << 3);
		key ^= (key >> 6);
		key += ~(key << 11);
		key ^= (key >> 16);
		return key;
	}

	float* table;
};
