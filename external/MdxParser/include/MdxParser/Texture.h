#pragma once

#include <string>
#include <stdint.h>
#include <iostream>

namespace MdxParser
{
    class Texture
    {
    public:
        Texture();
        Texture(std::istream& stream, uint32_t version);

        void write(std::ostream& stream, uint32_t version);

        void setReplaceableId(uint32_t replaceableId);
        void setFileName(std::string fileName);
        void setFlags(uint32_t flags);

        uint32_t getInclusiveSize() const;
        uint32_t getReplaceableId() const;
        std::string getFileName() const;
        uint32_t getFlags() const;

    protected:
        uint32_t replaceableId;
        std::string fileName;
        uint32_t flags;
    };
}