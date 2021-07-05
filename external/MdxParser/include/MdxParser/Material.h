#pragma once

#include "Layer.h"

#include <string>
#include <vector>
#include <stdint.h>
#include <iostream>

namespace MdxParser
{
    class Material
    {
    public:
        Material();
        Material(std::istream& stream, uint32_t version);

        void write(std::ostream& stream, uint32_t version);

        void addLayer(Layer layer);

        uint32_t getInclusiveSize() const;
        uint32_t getPriorityPlane() const;
        uint32_t getFlags() const;
        std::string getShader() const;
        uint32_t getLayersCount() const;
        std::vector<Layer> getLayers() const;

    protected:
        uint32_t inclusiveSize;
        uint32_t priorityPlane;
        uint32_t flags;
        std::string shader;
        uint32_t layersCount;
        std::vector<Layer> layers;
    };
}