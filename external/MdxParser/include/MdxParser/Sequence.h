#pragma once

#include "Extent.h"

#include <string>
#include <stdint.h>
#include <iostream>

namespace MdxParser
{
	class Sequence
	{
	public:
		Sequence();
		Sequence(std::istream& stream, uint32_t version);

		void write(std::ostream& stream, uint32_t version);

		void setName(std::string name);
		void setStartFrame(uint32_t startFrame);
		void setEndFrame(uint32_t endFrame);
		void setMoveSpeed(float moveSpeed);
		void setFlags(uint32_t flags);
		void setRarity(float rarity);
		void setSyncPoint(uint32_t syncPoint);
		void setExtent(Extent extent);

		uint32_t getInclusiveSize() const;
		std::string getName() const;
		uint32_t getStartFrame() const;
		uint32_t getEndFrame() const;
		float getMoveSpeed() const;
		uint32_t getFlags() const;
		float getRarity() const;
		uint32_t getSyncPoint() const;
		Extent getExtent() const;

	protected:
		std::string name;
		uint32_t startFrame;
		uint32_t endFrame;
		float moveSpeed;
		uint32_t flags;
		float rarity;
		uint32_t syncPoint;
		Extent extent;
	};
}