#pragma once

#include "NodeTranslator.h"

#include <MdxParser/Bone.h>
#include <maya/MObject.h>

class BoneTranslator : public NodeTranslator
{
public:
	BoneTranslator(MObject dependencyNode);
	BoneTranslator(MdxParser::Bone parserBone);

	virtual MdxParser::Node getParserNode() const override;
	MdxParser::Bone getParserBone() const;

	MObject createDependencyNode();

	void writeAttributesToParser(uint32_t objectId);
	virtual void writeParentToParser(std::map<MObject, uint32_t, DagNodeComparator> dependencyNodeMap) override;
	virtual void writeAnimationToParser() override;

protected:
	MdxParser::Bone parserBone;
};