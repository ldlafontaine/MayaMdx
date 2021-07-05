#pragma once

#include "NodeTranslator.h"

#include <MdxParser/Helper.h>
#include <maya/MObject.h>

class HelperTranslator : public NodeTranslator
{
public:
	HelperTranslator(MObject dependencyNode);
	HelperTranslator(MdxParser::Helper parserHelper);

	virtual MdxParser::Node getParserNode() const override;
	MdxParser::Helper getParserHelper() const;

	MObject createDependencyNode();

	void writeAttributesToParser(uint32_t objectId);
	virtual void writeParentToParser(std::map<MObject, uint32_t, DagNodeComparator> dependencyNodeMap) override;
	virtual void writeAnimationToParser() override;

protected:
	MdxParser::Helper parserHelper;
};