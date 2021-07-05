#pragma once

#include <stdint.h>
#include <array>
#include <iostream>

namespace MdxParser
{
	class Extent
	{
	public:
		Extent();
		Extent(std::istream& stream);

		void write(std::ostream& stream, uint32_t version);

		void setBoundsRadius(float boundsRadius);
		void setMinimum(std::array<float, 3> minimum);
		void setMaximum(std::array<float, 3> maximum);

		uint32_t getInclusiveSize() const;
		float getBoundsRadius() const;
		std::array<float, 3> getMinimum() const;
		std::array<float, 3> getMaximum() const;

	protected:
		float boundsRadius;
		std::array<float, 3> minimum;
		std::array<float, 3> maximum;
	};
}