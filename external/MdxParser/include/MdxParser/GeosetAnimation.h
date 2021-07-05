#pragma once

#include "TracksChunk.h"

#include <string>
#include <array>
#include <stdint.h>
#include <iostream>

namespace MdxParser
{
	class GeosetAnimation
	{
	public:
		GeosetAnimation();
		GeosetAnimation(std::istream& stream, uint32_t version);

		void write(std::ostream& stream, uint32_t version);

		uint32_t getInclusiveSize() const;
		float getAlpha() const;
		uint32_t getFlags() const;
		std::array<float, 3> getColor() const;
		uint32_t getGeosetId() const;
		TracksChunk<float> getAlphaTracks() const;
		TracksChunk<std::array<float, 3>> getColorTracks() const;

	protected:
		uint32_t inclusiveSize;
		float alpha;
		uint32_t flags;
		std::array<float, 3> color;
		uint32_t geosetId;
		TracksChunk<float> alphaTracks; // KGAO
		TracksChunk<std::array<float, 3>> colorTracks; // KGAC
	};
}