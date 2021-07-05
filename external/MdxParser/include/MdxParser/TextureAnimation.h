#pragma once

#include "TracksChunk.h"

#include <string>
#include <array>
#include <stdint.h>
#include <iostream>

namespace MdxParser
{
	class TextureAnimation
	{
	public:
		TextureAnimation();
		TextureAnimation(std::istream& stream, uint32_t version);

		void write(std::ostream& stream, uint32_t version);

		uint32_t getInclusiveSize() const;
		TracksChunk<std::array<float, 3>>& getTranslation();
		TracksChunk<std::array<float, 4>>& getRotation();
		TracksChunk<std::array<float, 3>>& getScaling();

	protected:
		uint32_t inclusiveSize;
		TracksChunk<std::array<float, 3>> translation; // KTAT
		TracksChunk<std::array<float, 4>> rotation; // KTAR
		TracksChunk<std::array<float, 3>> scaling; // KTAS
	};
}