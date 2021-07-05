#pragma once

#include "Track.h"

#include <string>
#include <vector>
#include <stdint.h>
#include <iostream>

namespace MdxParser
{
	template <typename T> class TracksChunk
	{
	public:
		TracksChunk();
		TracksChunk(std::string tagValue);
		TracksChunk(std::string tagValue, std::istream& stream);

		void write(std::ostream& stream, uint32_t version);

		void addTrack(Track<T> track);
		void addTrack(int32_t frame, T value, T inTan, T outTan);

		std::string getTag() const;
		uint32_t getTracksCount() const;
		uint32_t getInterpolationType() const;
		uint32_t getGlobalSequenceId() const;
		std::vector<Track<T>>& getTracks();

		void setInterpolationType(uint32_t interpolationType);
		void setGlobalSequenceId(uint32_t globalSequenceId);

	protected:
		std::string tag;
		uint32_t tracksCount;
		uint32_t interpolationType;	// 0: none
									// 1: linear
									// 2: hermite
									// 3: bezier
		uint32_t globalSequenceId;
		std::vector<Track<T>> tracks;
	};
}