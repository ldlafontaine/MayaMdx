#pragma once

#include "Node.h"

#include <stdint.h>
#include <iostream>

namespace MdxParser
{
	class Helper : public virtual Node
	{
	public:
		Helper();
		Helper(std::istream& stream, uint32_t version);

		void write(std::ostream& stream, uint32_t version);

		uint32_t getInclusiveSize() const;
	};
}