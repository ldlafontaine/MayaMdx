#pragma once

#include "ModelObjectTranslator.h"
#include "../DagNodeComparator.h"

#include <MdxParser/Node.h>
#include <maya/MObject.h>

#include <stdint.h>
#include <map>

class NodeTranslator : public ModelObjectTranslator
{
public:
	NodeTranslator(MObject dependencyNode);
	NodeTranslator(MdxParser::Node parserNode);

	virtual MdxParser::Node getParserNode() const;

	void readPositionFromParser(std::vector<std::array<float, 3>> pivots);
	void readParentFromParser(std::map<uint32_t, MObject>& objectIdMap, MObject root);
	void readAnimationFromParser();

	virtual void writeParentToParser(std::map<MObject, uint32_t, DagNodeComparator> dependencyNodeMap);
	virtual void writeAnimationToParser();

protected:
	MdxParser::Node parserNode;

	void readTranslationTracksFromParser();
	void readRotationTracksFromParser();
	void readScaleTracksFromParser();

	void writeTranslationTracksToParser(MdxParser::TracksChunk<MdxParser::translation>& translationTracksChunk);
	void writeRotationTracksToParser(MdxParser::TracksChunk<MdxParser::quaternion>& rotationTracksChunk);
	void writeScaleTracksToParser(MdxParser::TracksChunk<MdxParser::scale>& scaleTracksChunk);

	uint32_t getParentIdFromScene(std::map<MObject, uint32_t, DagNodeComparator>& dependencyNodeMap);
};