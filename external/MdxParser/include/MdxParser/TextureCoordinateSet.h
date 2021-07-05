#pragma once

#include <vector>
#include <array>
#include <stdint.h>
#include <iostream>

namespace MdxParser
{
	class TextureCoordinateSet
	{
	public:
		TextureCoordinateSet();
		TextureCoordinateSet(std::istream& stream);

		void write(std::ostream& stream, uint32_t version);

		uint32_t getInclusiveSize() const;
		uint32_t getCount() const;
		std::vector<std::array<float, 2>> getTextureCoordinates() const;

		void setTextureCoordinates(std::vector<std::array<float, 2>> textureCoordinates);

	protected:
		uint32_t count;
		std::vector<std::array<float, 2>> textureCoordinates;
	};
}