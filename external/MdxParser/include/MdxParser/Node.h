#pragma once

#include "TracksChunk.h"

#include <string>
#include <array>
#include <stdint.h>
#include <iostream>

namespace MdxParser
{
	class Node
	{
	public:
		Node();
		Node(std::istream& stream, uint32_t version);

		void read(std::istream& stream, uint32_t version);
		void write(std::ostream& stream, uint32_t version);

		uint32_t getNodeInclusiveSize() const;
		std::string getName() const;
		uint32_t getObjectId() const;
		uint32_t getParentId() const;
		uint32_t getFlags() const;
		TracksChunk<translation>& getTranslation(); // KGTR
		TracksChunk<quaternion>& getRotation(); // KGRT
		TracksChunk<scale>& getScale(); // KGSC

		void setName(std::string name);
		void setObjectId(uint32_t objectId);
		void setParentId(uint32_t parentId);
		void setFlags(uint32_t flags);

		bool hasParent();

	protected:
		uint32_t nodeInclusiveSize;
		std::string name;
		uint32_t objectId;
		uint32_t parentId;
		uint32_t flags;	// 0x0: helper
						// 0x1: dont inherit translation
						// 0x2: dont inherit rotation
						// 0x4: dont inherit scaling
						// 0x8: billboarded
						// 0x10: billboarded lock x
						// 0x20: billboarded lock y
						// 0x40: billboarded lock z
						// 0x80: camera anchored
						// 0x100: bone
						// 0x200: light
						// 0x400 event object
						// 0x800: attachment
						// 0x1000 particle emitter
						// 0x2000: collision shape
						// 0x4000: ribbon emitter
						// 0x8000: if particle emitter: emitter uses mdl, if particle emitter 2: unshaded
						// 0x10000: if particle emitter: emitter uses tga, if particle emitter 2: sort primitives far z
						// 0x20000: line emitter
						// 0x40000: unfogged
						// 0x80000: model space
						// 0x100000: xy quad
		TracksChunk<translation> translation; // KGTR
		TracksChunk<quaternion> rotation; // KGRT
		TracksChunk<scale> scale; // KGSC
	};
}