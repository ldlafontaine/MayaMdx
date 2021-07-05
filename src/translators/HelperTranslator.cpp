#include "HelperTranslator.h"

#include <maya/MFnDagNode.h>
#include <maya/MDagPath.h>

HelperTranslator::HelperTranslator(MObject dependencyNode) :
	NodeTranslator{ dependencyNode } {
}

HelperTranslator::HelperTranslator(MdxParser::Helper parserHelper) :
	NodeTranslator{ parserHelper } {
}

MdxParser::Node HelperTranslator::getParserNode() const {
	return parserHelper;
}

MdxParser::Helper HelperTranslator::getParserHelper() const {
	return parserHelper;
}

MObject HelperTranslator::createDependencyNode() {
	MFnDagNode dagFn;
	dependencyNode = dagFn.create("locator");
	dagFn.setName(parserNode.getName().c_str());

	return dependencyNode;
}

void HelperTranslator::writeAttributesToParser(uint32_t objectId) {
	MFnDagNode dagFn{ dependencyNode };
	MDagPath dagPath;
	dagFn.getPath(dagPath);
	dagFn.setObject(dagPath.transform());
	parserHelper.setName(dagFn.name().asChar());
	parserHelper.setObjectId(objectId);
}

void HelperTranslator::writeParentToParser(std::map<MObject, uint32_t, DagNodeComparator> dependencyNodeMap) {
	uint32_t parentId{ getParentIdFromScene(dependencyNodeMap) };
	parserHelper.setParentId(parentId);
}

void HelperTranslator::writeAnimationToParser() {
	writeTranslationTracksToParser(parserHelper.getTranslation());
	writeRotationTracksToParser(parserHelper.getRotation());
	writeScaleTracksToParser(parserHelper.getScale());
	parserHelper.getTranslation().setInterpolationType(1);
	parserHelper.getRotation().setInterpolationType(1);
	parserHelper.getScale().setInterpolationType(1);
}