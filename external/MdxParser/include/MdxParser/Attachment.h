#pragma once

#include "Node.h"
#include "TracksChunk.h"

#include <string>
#include <stdint.h>
#include <iostream>

namespace MdxParser 
{
	class Attachment : public virtual Node 
	{
	public:
		Attachment();
		Attachment(std::istream& stream, uint32_t version);

		void write(std::ostream& stream, uint32_t version);

		uint32_t getInclusiveSize() const;
		std::string getPath() const;
		uint32_t getAttachmentId() const;
		TracksChunk<float> getVisibility() const;

	protected:
		uint32_t inclusiveSize;
		std::string path;
		uint32_t attachmentId;
		TracksChunk<float> visibility; // KATV
	};
}

