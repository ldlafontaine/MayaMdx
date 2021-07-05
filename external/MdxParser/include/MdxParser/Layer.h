#pragma once

#include "TracksChunk.h"

#include <string>
#include <array>
#include <stdint.h>
#include <iostream>

namespace MdxParser
{
    class Layer
    {
    public:
        Layer();
        Layer(std::istream& stream, uint32_t version);

        void write(std::ostream& stream, uint32_t version);

        void setFilterMode(uint32_t filterMode);
        void setShadingFlags(uint32_t shadingFlags);
        void setTextureId(uint32_t textureId);
        void setTextureAnimationId(uint32_t textureAnimationId);
        void setCoordId(uint32_t coordId);

        uint32_t getInclusiveSize() const;
        uint32_t getFilterMode() const;
        uint32_t getShadingFlags() const;
        uint32_t getTextureId() const;
        uint32_t getTextureAnimationId() const;
        uint32_t getCoordId() const;

    protected:
        uint32_t inclusiveSize;
        uint32_t filterMode;    // 0: none
                                // 1: transparent
                                // 2: blend
                                // 3: additive
                                // 4: add alpha
                                // 5: modulate
                                // 6: modulate 2x
        uint32_t shadingFlags;  // 0x1: unshaded
                                // 0x2: sphere environment map
                                // 0x4: ?
                                // 0x8: ?
                                // 0x10: two sided
                                // 0x20: unfogged
                                // 0x40: no depth test
                                // 0x80: no depth set
        uint32_t textureId;
        uint32_t textureAnimationId;
        uint32_t coordId;
        float alpha;
        float emissiveGain;
        std::array<float, 3> fresnelColor;
        float fresnelOpacity;
        float fresnelTeamColor;
        TracksChunk<uint32_t> textureIdTracks; // KMTF
        TracksChunk<float> alphaTracks; // KMTA
        TracksChunk<float> emissiveGainTracks; // KMTE
        TracksChunk<std::array<float, 3>> fresnelColorTracks; // KFC3
        TracksChunk<float> fresnelAlphaTracks; // KFCA
        TracksChunk<float> fresnelTeamColorTracks; // KFTC
    };
}