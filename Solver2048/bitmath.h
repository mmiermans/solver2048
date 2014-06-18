#include <stdint.h>

/// <summary>
/// Integer Log2
/// </summary>
/// <returns>Log2 of x or -1 for an input value of 0.</returns>
static inline int log2(int x) {
	int result = 0;
	while (x >>= 1) ++result;
	return result;
}