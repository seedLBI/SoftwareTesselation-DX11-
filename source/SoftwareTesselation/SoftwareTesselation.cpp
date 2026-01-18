#include "SoftwareTesselation.h"

#include <algorithm>
#include <cmath>

std::vector<UINT> makeIndexes(const size_t& subdivision) {

	auto getIndex = [&subdivision](const size_t& x, const size_t& y) -> size_t {
		return y * subdivision + x;
	};

	const size_t count = subdivision - 1;

	std::vector<UINT> output;
	output.reserve(count * count);

	for (size_t y = 0; y < count; y++) {
		for (size_t x = 0; x < count; x++) {

			output.push_back(getIndex(x, y));
			output.push_back(getIndex(x, y + 1));
			output.push_back(getIndex(x + 1, y + 1));

			output.push_back(getIndex(x, y));
			output.push_back(getIndex(x + 1, y + 1));
			output.push_back(getIndex(x + 1, y));

		}
	}

	return output;
}
