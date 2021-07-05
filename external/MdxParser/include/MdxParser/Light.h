#pragma once

#include "Node.h"
#include "TracksChunk.h"

#include <string>
#include <array>
#include <stdint.h>
#include <iostream>

namespace MdxParser
{
	class Light : public virtual Node
	{
	public:
		Light();
		Light(std::istream& stream, uint32_t version);

		void write(std::ostream& stream, uint32_t version);

		uint32_t getInclusiveSize() const;
		uint32_t getType() const;
		float getAttenuationStart() const;
		float getAttenuationEnd() const;
		std::array<float, 3> getColor() const;
		float getIntensity() const;
		std::array<float, 3> getAmbientColor() const;
		float getAmbientIntensity() const;
		TracksChunk<float> getAttenuationStartTracks() const;
		TracksChunk<float> getAttenuationEndTracks() const;
		TracksChunk<std::array<float, 3>> getColorTracks() const;
		TracksChunk<float> getIntensityTracks() const;
		TracksChunk<float> getAmbientIntensityTracks() const;
		TracksChunk<std::array<float, 3>> getAmbientColorTracks() const;
		TracksChunk<float> getVisibilityTracks() const;

	protected:
		uint32_t inclusiveSize;
		uint32_t type;	// 0: omni light
						// 1: directional light
						// 2: ambient light
		float attenuationStart;
		float attenuationEnd;
		std::array<float, 3> color;
		float intensity;
		std::array<float, 3> ambientColor;
		float ambientIntensity;
		TracksChunk<float> attenuationStartTracks; // KLAS
		TracksChunk<float> attenuationEndTracks; // KLAE
		TracksChunk<std::array<float, 3>> colorTracks; // KLAC
		TracksChunk<float> intensityTracks; // KLAI
		TracksChunk<float> ambientIntensityTracks; // KLBI
		TracksChunk<std::array<float, 3>> ambientColorTracks; // KLBC
		TracksChunk<float> visibilityTracks; // KLAV
	};
}