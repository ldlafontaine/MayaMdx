#include "NodeTranslator.h"

#include <maya/MFnDagNode.h>
#include <maya/MFnTransform.h>
#include <maya/MVector.h>
#include <maya/MDagPath.h>
#include <maya/MMatrix.h>
#include <maya/MPlug.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MQuaternion.h>
#include <maya/MEulerRotation.h>
#include <maya/MVector.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MStatus.h>

#include <set>
#include <limits>

NodeTranslator::NodeTranslator(MObject dependencyNode) :
	ModelObjectTranslator{ dependencyNode } {
}

NodeTranslator::NodeTranslator(MdxParser::Node parserNode) :
	parserNode{ parserNode } {
}

MdxParser::Node NodeTranslator::getParserNode() const {
	return parserNode;
}

void NodeTranslator::readPositionFromParser(std::vector<std::array<float,3>> pivots) {
	MFnTransform transformFn{ dependencyNode };
	std::array<float, 3> pivot{ pivots.at(parserNode.getObjectId()) };
	MVector position{ pivot.at(0), pivot.at(1), pivot.at(2) };
	transformFn.setTranslation(position, MSpace::kTransform);
}

void NodeTranslator::readParentFromParser(std::map<uint32_t, MObject>& objectIdMap, MObject root) {
	MFnDagNode dagFn{ dependencyNode };

	if (parserNode.hasParent()) {
		MObject parentDagNode = objectIdMap.at(parserNode.getParentId());
		dagFn.setObject(parentDagNode);
		MDagPath dagPath;
		dagFn.getPath(dagPath);

		MMatrix parentMatrix{ dagFn.transformationMatrix() };
		dagFn.setObject(dependencyNode);
		MMatrix transformationMatrix{ dagFn.transformationMatrix() };

		dagFn.setObject(parentDagNode);
		dagFn.addChild(dependencyNode);

		MFnTransform transformFn{ dependencyNode };
		transformFn.set(transformationMatrix * dagPath.inclusiveMatrixInverse());
	}
	else {
		dagFn.setObject(root);
		dagFn.addChild(dependencyNode);
	}
}

void NodeTranslator::readAnimationFromParser() {
	readTranslationTracksFromParser();
	readRotationTracksFromParser();
	readScaleTracksFromParser();
}

void NodeTranslator::readTranslationTracksFromParser() {
	MFnDependencyNode dependFn{ dependencyNode };
	MPlug translateXPlug{ dependFn.findPlug("translateX", true) };
	MPlug translateYPlug{ dependFn.findPlug("translateY", true) };
	MPlug translateZPlug{ dependFn.findPlug("translateZ", true) };
	std::vector<MFnAnimCurve> animCurves(3);
	std::vector<MPlug> plugs{ translateXPlug, translateYPlug, translateZPlug };

	// Get tangent type.
	MFnAnimCurve::TangentType tangentType{ getTangentType(parserNode.getTranslation().getInterpolationType()) };

	// Get initial value.
	MFnTransform transformFn{ dependencyNode };
	MVector initialTranslation{ transformFn.getTranslation(MSpace::kTransform) };

	for (auto track : parserNode.getTranslation().getTracks()) {
		MTime frame{ static_cast<double>(track.getFrame()) };

		// Get track value relative to initial value.
		MVector offset{ static_cast<double>(track.getValue().at(0)), static_cast<double>(track.getValue().at(1)), static_cast<double>(track.getValue().at(2)) };
		MVector value{ initialTranslation + offset };

		// Set track keyframe.
		for (int plugsIter{ 0 }; plugsIter < plugs.size(); plugsIter++) {
			animCurves.at(plugsIter).create(plugs.at(plugsIter));
			animCurves.at(plugsIter).addKeyframe(frame, value[plugsIter], tangentType, tangentType);
		}
	}
}

void NodeTranslator::readRotationTracksFromParser() {
	MFnDependencyNode dependFn{ dependencyNode };
	MPlug rotateXPlug{ dependFn.findPlug("rotateX", true) };
	MPlug rotateYPlug{ dependFn.findPlug("rotateY", true) };
	MPlug rotateZPlug{ dependFn.findPlug("rotateZ", true) };
	std::vector<MFnAnimCurve> animCurves(3);
	std::vector<MPlug> plugs{ rotateXPlug, rotateYPlug, rotateZPlug };

	// Get tangent type.
	MFnAnimCurve::TangentType tangentType{ getTangentType(parserNode.getRotation().getInterpolationType()) };

	// Get initial value.
	MFnTransform transformFn{ dependencyNode };
	MEulerRotation initialRotation;
	transformFn.getRotation(initialRotation);

	for (auto track : parserNode.getRotation().getTracks()) {
		MTime frame{ static_cast<double>(track.getFrame()) };

		// Get track value relative to initial value.
		MQuaternion trackValueQuaternion{ static_cast<double>(track.getValue().at(0)), 
											static_cast<double>(track.getValue().at(1)),
											static_cast<double>(track.getValue().at(2)), 
											static_cast<double>(track.getValue().at(3)) };
		transformFn.setRotation({ initialRotation.x, initialRotation.y, initialRotation.z });
		transformFn.rotateBy(trackValueQuaternion, MSpace::kTransform);
		MEulerRotation value;
		transformFn.getRotation(value);

		// Set track keyframe.
		for (int plugsIter{ 0 }; plugsIter < plugs.size(); plugsIter++) {
			animCurves.at(plugsIter).create(plugs.at(plugsIter));
			animCurves.at(plugsIter).addKeyframe(frame, value[plugsIter], tangentType, tangentType);
		}
	}
}

void NodeTranslator::readScaleTracksFromParser() {
	MFnDependencyNode dependFn{ dependencyNode };
	MPlug scaleXPlug{ dependFn.findPlug("scaleX", true) };
	MPlug scaleYPlug{ dependFn.findPlug("scaleY", true) };
	MPlug scaleZPlug{ dependFn.findPlug("scaleZ", true) };
	std::vector<MFnAnimCurve> animCurves(3);
	std::vector<MPlug> plugs{ scaleXPlug, scaleYPlug, scaleZPlug };

	// Get tangent type.
	MFnAnimCurve::TangentType tangentType{ getTangentType(parserNode.getScale().getInterpolationType()) };

	for (auto track : parserNode.getScale().getTracks()) {
		MTime frame{ static_cast<double>(track.getFrame()) };
		double value[3]{ static_cast<double>(track.getValue().at(0)), static_cast<double>(track.getValue().at(1)), static_cast<double>(track.getValue().at(2)) };

		// Set track keyframe.
		for (int plugsIter{ 0 }; plugsIter < plugs.size(); plugsIter++) {
			animCurves.at(plugsIter).create(plugs.at(plugsIter));
			animCurves.at(plugsIter).addKeyframe(frame, value[plugsIter], tangentType, tangentType);
		}
	}
}

void NodeTranslator::writeParentToParser(std::map<MObject, uint32_t, DagNodeComparator> dependencyNodeMap) {}

void NodeTranslator::writeAnimationToParser() {}

void NodeTranslator::writeTranslationTracksToParser(MdxParser::TracksChunk<MdxParser::translation>& translationTracksChunk) {
	MStatus status;

	// Get transform from dependency node, if possible.
	MFnDagNode dagFn{ dependencyNode };
	MDagPath dagPath;
	dagFn.getPath(dagPath);
	MObject transformNode = dagPath.transform(&status);
	MFnTransform transformFn{ dependencyNode };
	if (status == MStatus::kSuccess) transformFn.setObject(transformNode);

	// Get plugs.
	MPlug translateXPlug{ transformFn.findPlug("translateX", true) };
	MPlug translateYPlug{ transformFn.findPlug("translateY", true) };
	MPlug translateZPlug{ transformFn.findPlug("translateZ", true) };
	std::vector<MPlug> plugs{ translateXPlug, translateYPlug, translateZPlug };
	
	// Get initial value.
	MVector initialTranslation{ transformFn.getTranslation(MSpace::kTransform) };

	// Find keyframe times for each attribute.
	std::set<int32_t> keyframeTimes;
	std::vector<MObject> animCurves;
	getKeyframeTimes(plugs, keyframeTimes, animCurves);

	// Evaluate the output of each attribute at each keyframe.
	for (int32_t keyframeTime : keyframeTimes) {

		MdxParser::translation value{ 0 };
		MdxParser::translation inTan{ 0 };
		MdxParser::translation outTan{ 0 };

		for (int animCurveIter{ 0 }; animCurveIter < animCurves.size(); ++animCurveIter) {
			MFnAnimCurve animCurve{ animCurves.at(animCurveIter) };
			double interpolatedValue{ animCurve.evaluate(MTime(static_cast<double>(keyframeTime))) };
			value.at(animCurveIter) = static_cast<float>(interpolatedValue) - static_cast<float>(initialTranslation[animCurveIter]);
		}

		translationTracksChunk.addTrack(keyframeTime, value, inTan, outTan);
	}
}

void NodeTranslator::writeRotationTracksToParser(MdxParser::TracksChunk<MdxParser::quaternion>& rotationTracksChunk) {
	MStatus status;

	// Get transform from dependency node, if possible.
	MFnDagNode dagFn{ dependencyNode };
	MDagPath dagPath;
	dagFn.getPath(dagPath);
	MObject transformNode = dagPath.transform(&status);
	MFnTransform transformFn{ dependencyNode };
	if (status == MStatus::kSuccess) transformFn.setObject(transformNode);

	// Get plugs.
	MPlug rotateXPlug{ transformFn.findPlug("rotateX", true) };
	MPlug rotateYPlug{ transformFn.findPlug("rotateY", true) };
	MPlug rotateZPlug{ transformFn.findPlug("rotateZ", true) };
	std::vector<MPlug> plugs{ rotateXPlug, rotateYPlug, rotateZPlug };

	// Find keyframe times for each attribute.
	std::set<int32_t> keyframeTimes;
	std::vector<MObject> animCurves;
	getKeyframeTimes(plugs, keyframeTimes, animCurves);

	// Evaluate the output of each attribute at each keyframe.
	for (int32_t keyframeTime : keyframeTimes) {

		MdxParser::quaternion value{ 0 };
		MdxParser::quaternion inTan{ 0 };
		MdxParser::quaternion outTan{ 0 };

		MEulerRotation evaluatedEuler;
		for (int animCurveIter{ 0 }; animCurveIter < animCurves.size(); ++animCurveIter) {
			MFnAnimCurve animCurve{ animCurves.at(animCurveIter) };
			double interpolatedValue{ animCurve.evaluate(MTime(static_cast<double>(keyframeTime))) };
			evaluatedEuler[animCurveIter] = interpolatedValue;
		}
		MQuaternion quaternion{ evaluatedEuler.asQuaternion() };
		value = { static_cast<float>(quaternion.x), static_cast<float>(quaternion.y), static_cast<float>(quaternion.z), static_cast<float>(quaternion.w) };

		rotationTracksChunk.addTrack(keyframeTime, value, inTan, outTan);
	}
}

void NodeTranslator::writeScaleTracksToParser(MdxParser::TracksChunk<MdxParser::scale>& scaleTracksChunk) {
	MStatus status;

	// Get transform from dependency node, if possible.
	MFnDagNode dagFn{ dependencyNode };
	MDagPath dagPath;
	dagFn.getPath(dagPath);
	MObject transformNode = dagPath.transform(&status);
	MFnTransform transformFn{ dependencyNode };
	if (status == MStatus::kSuccess) transformFn.setObject(transformNode);

	// Get plugs.
	MPlug scaleXPlug{ transformFn.findPlug("scaleX", true) };
	MPlug scaleYPlug{ transformFn.findPlug("scaleY", true) };
	MPlug scaleZPlug{ transformFn.findPlug("scaleZ", true) };
	std::vector<MPlug> plugs{ scaleXPlug, scaleYPlug, scaleZPlug };

	// Find keyframe times for each attribute.
	std::set<int32_t> keyframeTimes;
	std::vector<MObject> animCurves;
	getKeyframeTimes(plugs, keyframeTimes, animCurves);

	// Evaluate the output of each attribute at each keyframe.
	for (int32_t keyframeTime : keyframeTimes) {

		MdxParser::scale value{ 0 };
		MdxParser::scale inTan{ 0 };
		MdxParser::scale outTan{ 0 };

		for (int animCurveIter{ 0 }; animCurveIter < animCurves.size(); ++animCurveIter) {
			MFnAnimCurve animCurve{ animCurves.at(animCurveIter) };
			double interpolatedValue{ animCurve.evaluate(MTime(static_cast<double>(keyframeTime))) };
			value.at(animCurveIter) = static_cast<float>(interpolatedValue);
		}

		scaleTracksChunk.addTrack(keyframeTime, value, inTan, outTan);
	}
}

uint32_t NodeTranslator::getParentIdFromScene(std::map<MObject, uint32_t, DagNodeComparator>& dependencyNodeMap) {
	MStatus status;
	uint32_t parentId{ std::numeric_limits<uint32_t>::max() };
	MFnDagNode dagFn{ dependencyNode };

	// Get transform from dependency node, if possible.
	MDagPath dagPath;
	dagFn.getPath(dagPath);
	MObject transformNode = dagPath.transform(&status);
	if (status == MStatus::kSuccess) dagFn.setObject(transformNode);

	// Get parent.
	dagFn.setObject(transformNode);
	MObject parentNode{ dagFn.parent(0) };

	// Get shape node from parent, if one exists.
	dagFn.setObject(parentNode);
	MDagPath parentDagPath;
	dagFn.getPath(parentDagPath);
	status = parentDagPath.extendToShape();
	if (status == MStatus::kSuccess) parentNode = parentDagPath.node();

	// Get parent ID.
	auto mapIterator{ dependencyNodeMap.find(parentNode) };
	if (mapIterator != dependencyNodeMap.end()) parentId = mapIterator->second;
	return parentId;
}