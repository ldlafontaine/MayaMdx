#pragma once

#include "Node.h"

#include <stdint.h>
#include <iostream>

namespace MdxParser
{
	class Bone : public virtual Node
	{
	public:
		Bone();
		Bone(std::istream& stream, uint32_t version);

		void write(std::ostream& stream, uint32_t version);

		uint32_t getInclusiveSize() const;
		uint32_t getGeosetId() const;
		uint32_t getGeosetAnimationId() const;

		void setGeosetId(uint32_t geosetId);
		void setGeosetAnimationId(uint32_t geosetAnimationId);

	protected:
		uint32_t geosetId;
		uint32_t geosetAnimationId;
	};
}

