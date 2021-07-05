#pragma once

#include "Extent.h"
#include "TextureCoordinateSet.h"

#include <string>
#include <vector>
#include <array>
#include <stdint.h>
#include <iostream>

namespace MdxParser
{
    class Geoset
    {
    public:
        Geoset();
        Geoset(std::istream& stream, uint32_t version);

        void write(std::ostream& stream, uint32_t version);

        uint32_t getInclusiveSize() const;
        uint32_t getVertexCount() const;
        std::vector<std::array<float, 3>> getVertexPositions() const;
        uint32_t getNormalCount() const;
        std::vector<std::array<float, 3>> getVertexNormals() const;
        uint32_t getFaceTypeGroupsCount() const;
        std::vector<uint32_t> getFaceTypeGroups() const;
        uint32_t getFaceGroupsCount() const;
        std::vector<uint32_t> getFaceGroups() const;
        uint32_t getFacesCount() const;
        std::vector<uint16_t> getFaces() const;
        uint32_t getVertexGroupsCount() const;
        std::vector<uint8_t> getVertexGroups() const;
        uint32_t getMatrixGroupsCount() const;
        std::vector<uint32_t> getMatrixGroups() const;
        uint32_t getMatrixIndicesCount() const;
        std::vector<uint32_t> getMatrixIndices() const;
        uint32_t getMaterialId() const;
        uint32_t getSelectionGroup() const;
        uint32_t getSelectionFlags() const;
        uint32_t getLod() const;
        std::string getLodName() const;
        Extent getExtent() const;
        uint32_t getExtentsCount() const;
        std::vector<Extent> getSequenceExtents() const;
        uint32_t getTextureCoordinateSetsCount() const;
        std::vector<TextureCoordinateSet> getTextureCoordinateSets() const;

        void setInclusiveSize(uint32_t inclusiveSize);
        void setVertexCount(uint32_t vertexCount);
        void setVertexPositions(std::vector<std::array<float, 3>> vertexPositions);
        void setNormalCount(uint32_t normalCount);
        void setVertexNormals(std::vector<std::array<float, 3>> vertexNormals);
        void setFaceTypeGroupsCount(uint32_t faceTypeGroupsCount);
        void setFaceTypeGroups(std::vector<uint32_t> faceTypeGroups);
        void setFaceGroupsCount(uint32_t faceGroupsCount);
        void setFaceGroups(std::vector<uint32_t> faceGroups);
        void setFacesCount(uint32_t facesCount);
        void setFaces(std::vector<uint16_t> faces);
        void setVertexGroupsCount(uint32_t vertexGroupsCount);
        void setVertexGroups(std::vector<uint8_t> vertexGroups);
        void setMatrixGroupsCount(uint32_t matrixGroupsCount);
        void setMatrixGroups(std::vector<uint32_t> matrixGroups);
        void setMatrixIndicesCount(uint32_t matrixIndicesCount);
        void setMatrixIndices(std::vector<uint32_t> matrixIndices);
        void setMaterialId(uint32_t materialId);
        void setSelectionGroup(uint32_t selectionGroup);
        void setSelectionFlags(uint32_t selectionFlags);
        void setLod(uint32_t lod);
        void setLodName(std::string lodName);
        void setExtent(Extent extent);
        void setExtentsCount(uint32_t exentsCount);
        void setSequenceExtents(std::vector<Extent> sequenceExtents);
        void setTextureCoordinateSetsCount(uint32_t textureCoordinateSetsCount);
        void setTextureCoordinateSets(std::vector<TextureCoordinateSet> textureCoordinateSets);

    protected:
        uint32_t inclusiveSize;
        uint32_t vertexCount;
        std::vector<std::array<float, 3>> vertexPositions;
        uint32_t normalCount;
        std::vector<std::array<float, 3>> vertexNormals;
        uint32_t faceTypeGroupsCount;
        std::vector<uint32_t> faceTypeGroups;
        uint32_t faceGroupsCount;
        std::vector<uint32_t> faceGroups;
        uint32_t facesCount;
        std::vector<uint16_t> faces;
        uint32_t vertexGroupsCount;
        std::vector<uint8_t> vertexGroups;
        uint32_t matrixGroupsCount;
        std::vector<uint32_t> matrixGroups;
        uint32_t matrixIndicesCount;
        std::vector<uint32_t> matrixIndices;
        uint32_t materialId;
        uint32_t selectionGroup;
        uint32_t selectionFlags;
        uint32_t lod;
        std::string lodName;
        Extent extent;
        uint32_t extentsCount;
        std::vector<Extent> sequenceExtents;
        uint32_t tangentsCount;
        std::vector<std::array<float, 4>> tangents;
        uint32_t skinCount;
        std::vector<uint8_t> skin;
        uint32_t textureCoordinateSetsCount;
        std::vector<TextureCoordinateSet> textureCoordinateSets;
    };
}