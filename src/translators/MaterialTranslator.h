#pragma once

#include "ModelObjectTranslator.h"

#include <MdxParser/Material.h>
#include <maya/MObject.h>

#include <map>

class MaterialTranslator : public ModelObjectTranslator
{
public:
	MaterialTranslator(MObject dependencyNode);
	MaterialTranslator(MdxParser::Material parserMaterial);

	MdxParser::Material getParserMaterial() const;

	void createDependencyNode();
	void readAttributesFromParser();
	void readLayersFromParser(std::map<uint32_t, MObject>& textureIdMap);

	void writeAttributesToParser();
	void writeLayersToParser(std::vector<MObject>& textureDependencyNodes);

protected:
	MdxParser::Material parserMaterial;
	MObject shadingEngineDependencyNode;
};