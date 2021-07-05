#pragma once

#include "ModelObjectTranslator.h"
#include "NodeTranslator.h"
#include "BoneTranslator.h"
#include "../DagNodeComparator.h"

#include <MdxParser/Geoset.h>
#include <maya/MObject.h>

class GeosetTranslator : public ModelObjectTranslator
{
public:
	GeosetTranslator(MObject dependencyNode);
	GeosetTranslator(MdxParser::Geoset parserGeoset);

	MdxParser::Geoset getParserGeoset() const;

	MObject createDependencyNode(MObject root);

	void readInfluencesFromParser(std::vector<NodeTranslator> nodeTranslators);
	void readMaterialFromParser(std::map<uint32_t, MObject>& materialIdMap);

	void writeGeometryToParser();
	void writeInfluencesToParser(std::map<MObject, uint32_t, DagNodeComparator>& dependencyNodeMap);
	void writeMaterialToParser(std::vector<MObject>& materialDependencyNodes);

protected:
	MdxParser::Geoset parserGeoset;
};