#pragma once

#include <stdint.h>
#include <iostream>
#include <vector>
#include <array>

namespace MdxParser
{
	typedef std::array<float, 3> translation;
	typedef std::array<float, 3> scale;
	typedef std::array<float, 3> color;
	typedef std::array<float, 4> quaternion;

	template <typename T> class Track
	{
	public:
		Track(int32_t frame, T value, T inTan, T outTan);
		Track(std::istream& stream, uint32_t interpolationType);

		void read(std::istream& stream, uint32_t interpolationType);
		void write(std::ostream& stream, uint32_t version, uint32_t interpolationType);

		int32_t getFrame() const;
		T getValue();
		T getInTan();
		T getOutTan();

		void setFrame(int32_t frame);
		void setValue(T value);
		void setInTan(T inTan);
		void setOutTan(T outTan);

	protected:
		int32_t frame;
		T value;
		T inTan;
		T outTan;
	};
}