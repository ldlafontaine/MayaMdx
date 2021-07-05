#pragma once

#include "Node.h"

#include <string>
#include <array>
#include <vector>
#include <stdint.h>
#include <iostream>

namespace MdxParser
{
	class CollisionShape : public virtual Node
	{
	public:
		CollisionShape();
		CollisionShape(std::istream& stream, uint32_t version);

		void write(std::ostream& stream, uint32_t version);

		uint32_t getInclusiveSize() const;
		uint32_t getType() const;
		std::vector<std::array<float, 3>> getVertices() const;
		float getRadius() const;

	protected:
		uint32_t inclusiveSize;
		uint32_t type;
		std::vector<std::array<float, 3>> vertices;
		float radius;
	};
}