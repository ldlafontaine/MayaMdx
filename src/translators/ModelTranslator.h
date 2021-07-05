#pragma once

#include "BoneTranslator.h"
#include "GeosetTranslator.h"
#include "HelperTranslator.h"
#include "MaterialTranslator.h"
#include "NodeTranslator.h"
#include "SequenceTranslator.h"
#include "TextureTranslator.h"

#include <MdxParser/Model.h>

#include <maya/MFileObject.h>
#include <maya/MString.h>
#include <maya/MObject.h>
#include <maya/MTypeId.h>

class ModelTranslator
{
public:
	ModelTranslator();

	void readModelFromFile(const MFileObject& file, const MString& options);
	void writeModelToFile(const MFileObject& file, const MString& options);

protected:
	static const MTypeId timeSliderBookmarkTypeId;

	MdxParser::Model parserModel;
	MObject root;

	std::vector<GeosetTranslator> geosetTranslators;
	std::vector<BoneTranslator> boneTranslators;
	std::vector<HelperTranslator> helperTranslators;
	std::vector<MaterialTranslator> materialTranslators;
	std::vector<SequenceTranslator> sequenceTranslators;
	std::vector<TextureTranslator> textureTranslators;

	void readGeosetsFromParser();
	void readNodesFromParser();
	void readMaterialsFromParser();
	void readAnimationsFromParser();
	void readSequencesFromParser();
	void setPreferences();

	void writeGeosetsToParser();
	void writeNodesToParser();
	void writeMaterialsToParser();
	void writeAnimationsToParser();
	void writeSequencesToParser();
	void writePivotToParser(MDagPath dagPath);
	void writeModelToParser();

	std::vector<NodeTranslator> getNodeTranslators();
};