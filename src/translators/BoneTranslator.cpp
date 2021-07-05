#include "BoneTranslator.h"

#include <maya/MFnDagNode.h>

BoneTranslator::BoneTranslator(MObject dependencyNode) :
	NodeTranslator{ dependencyNode } {
}

BoneTranslator::BoneTranslator(MdxParser::Bone parserBone) :
	NodeTranslator{ parserBone } {
}

MdxParser::Node BoneTranslator::getParserNode() const {
	return parserBone;
}

MdxParser::Bone BoneTranslator::getParserBone() const {
	return parserBone;
}

MObject BoneTranslator::createDependencyNode() {
	MFnDagNode dagFn;
	dependencyNode = dagFn.create("joint");
	dagFn.setName(parserNode.getName().c_str());

	return dependencyNode;
}

void BoneTranslator::writeAttributesToParser(uint32_t objectId) {
	MFnDagNode dagFn{ dependencyNode };
	parserBone.setName(dagFn.name().asChar());
	parserBone.setObjectId(objectId);
}

void BoneTranslator::writeParentToParser(std::map<MObject, uint32_t, DagNodeComparator> dependencyNodeMap) {
	uint32_t parentId{ getParentIdFromScene(dependencyNodeMap) };
	parserBone.setParentId(parentId);
}

void BoneTranslator::writeAnimationToParser() {
	writeTranslationTracksToParser(parserBone.getTranslation());
	writeRotationTracksToParser(parserBone.getRotation());
	writeScaleTracksToParser(parserBone.getScale());
	parserBone.getTranslation().setInterpolationType(1);
	parserBone.getRotation().setInterpolationType(1);
	parserBone.getScale().setInterpolationType(1);
}