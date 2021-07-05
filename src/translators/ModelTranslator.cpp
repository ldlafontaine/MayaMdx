#include "ModelTranslator.h"

#include "../DagNodeComparator.h"

#include <maya/MFnDagNode.h>
#include <maya/MGlobal.h>
#include <maya/MAnimControl.h>
#include <maya/MTime.h>
#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MFnTransform.h>
#include <maya/MVector.h>
#include <maya/MItDependencyNodes.h>

#include <sstream>
#include <map>

const MTypeId ModelTranslator::timeSliderBookmarkTypeId{ 1414742605 };

ModelTranslator::ModelTranslator() {
}

void ModelTranslator::readModelFromFile(const MFileObject& file, const MString& options) {
	// Parse file.
	const char* filePath{ file.resolvedFullName().asChar() };
	parserModel = MdxParser::Model{ filePath };

	// Construct root node.
	MFnDagNode dagFn;
	root = dagFn.create("transform");
	dagFn.setName(parserModel.getName().c_str());

	// Construct scene.
	setPreferences();
	readGeosetsFromParser();
	readNodesFromParser();
	readMaterialsFromParser();
	readAnimationsFromParser();
	readSequencesFromParser();
}

void ModelTranslator::readGeosetsFromParser() {
	std::cout << "Reading geosets to scene." << std::endl;

	// Create dependency nodes for all geosets.
	for (auto parserGeoset : parserModel.getGeosets()) {
		GeosetTranslator geosetTranslator{ parserGeoset };
		geosetTranslator.createDependencyNode(root);
		geosetTranslators.push_back(geosetTranslator);
	}
}

void ModelTranslator::readNodesFromParser() {
	std::cout << "Reading nodes to scene." << std::endl;

	std::map<uint32_t, MObject> objectIdMap;

	// Create dependency nodes for all bones.
	for (auto parserBone : parserModel.getBones()) {
		BoneTranslator boneTranslator{ parserBone };
		boneTranslator.createDependencyNode();
		boneTranslators.push_back(boneTranslator);
		objectIdMap.insert({ parserBone.getObjectId(), boneTranslator.getDependencyNode() });
	}

	// Create dependency nodes for all helpers.
	for (auto parserHelper : parserModel.getHelpers()) {
		HelperTranslator helperTranslator{ parserHelper };
		helperTranslator.createDependencyNode();
		helperTranslators.push_back(helperTranslator);
		objectIdMap.insert({ parserHelper.getObjectId(), helperTranslator.getDependencyNode() });
	}

	auto nodeTranslators{ getNodeTranslators() };

	// Set position and hierarchy of all node dependency nodes.
	for (auto nodeTranslator : nodeTranslators) nodeTranslator.readPositionFromParser(parserModel.getPivots());
	for (auto nodeTranslator : nodeTranslators) nodeTranslator.readParentFromParser(objectIdMap, root);

	// Set skin weights for each geoset.
	for (auto geosetTranslator : geosetTranslators) {
		geosetTranslator.readInfluencesFromParser(nodeTranslators);
	}

	// Rebuild a single, consolidated bind pose.
	std::stringstream bindPoseCommand;
	bindPoseCommand << "dagPose -save -bindPose ";
	for (auto nodeTranslator : nodeTranslators) {
		MFnDagNode bindPoseDagFn{ nodeTranslator.getDependencyNode() };
		bindPoseCommand << bindPoseDagFn.name() << " ";
	}
	MGlobal::executeCommand(bindPoseCommand.str().c_str());
}

void ModelTranslator::readMaterialsFromParser() {
	std::cout << "Reading materials to scene." << std::endl;

	std::map<uint32_t, MObject> textureIdMap;
	std::map<uint32_t, MObject> materialIdMap;

	// Create dependency nodes for all textures.
	for (auto parserTexture : parserModel.getTextures()) {
		TextureTranslator textureTranslator{ parserTexture };
		textureTranslator.createDependencyNode();
		uint32_t textureId{ static_cast<uint32_t>(textureTranslators.size()) };
		textureTranslators.push_back(textureTranslator);
		textureIdMap.insert({ textureId, textureTranslator.getDependencyNode() });
	}

	// Create dependency nodes for all materials.
	for (auto parserMaterial : parserModel.getMaterials()) {
		MaterialTranslator materialTranslator{ parserMaterial };
		materialTranslator.createDependencyNode();
		materialTranslator.readLayersFromParser(textureIdMap);
		uint32_t materialId{ static_cast<uint32_t>(materialTranslators.size()) };
		materialTranslators.push_back(materialTranslator);
		materialIdMap.insert({ materialId, materialTranslator.getDependencyNode() });
	}

	for (auto it = geosetTranslators.begin(); it != geosetTranslators.end(); ++it) {
		it->readMaterialFromParser(materialIdMap);
	}
}

void ModelTranslator::readAnimationsFromParser() {
	std::cout << "Reading animations to scene." << std::endl;

	for (auto nodeTranslator : getNodeTranslators()) {
		nodeTranslator.readAnimationFromParser();
	}
}

void ModelTranslator::readSequencesFromParser() {
	std::cout << "Reading sequences to scene." << std::endl;

	MGlobal::executeCommand("loadPlugin -quiet timeSliderBookmark;");

	uint32_t animationEndTime{ 0 };
	for (auto parsedSequence : parserModel.getSequences()) {
		SequenceTranslator sequenceTranslator{ parsedSequence };

		// Create Time Slider Bookmark
		sequenceTranslator.createTimeSliderBookmark();

		// Query animation length to determine playback options.
		uint32_t endFrame{ parsedSequence.getEndFrame() };
		if (endFrame > animationEndTime) {
			animationEndTime = endFrame;
		}
	}

	// Set Playback Options
	MAnimControl animControl;
	MTime endTime{ static_cast<double>(animationEndTime) };
	animControl.setAnimationEndTime(endTime);
	animControl.setMaxTime(endTime);
	double playbackSpeed{ MTime().asUnits(MTime::Unit::kMilliseconds) };
	animControl.setPlaybackSpeed(playbackSpeed);
	animControl.setCurrentTime(MTime{ 1.0 });
}

void ModelTranslator::setPreferences() {
	std::cout << "Setting preferences." << std::endl;

	if (MGlobal::isZAxisUp() == false) {
		MGlobal::setZAxisUp(true);
	}
}

void ModelTranslator::writeModelToFile(const MFileObject& file, const MString& options) {

	// Identify root node.
	MItDag dagIter{ MItDag::kBreadthFirst };
	for (; !dagIter.isDone(); dagIter.next()) {
		MDagPath dagPath;
		dagIter.getPath(dagPath);
		MFnDagNode dagFn{ dagPath };

		if (!dagPath.hasFn(MFn::kTransform) || dagPath.hasFn(MFn::kCamera)) continue;

		parserModel.setName(dagFn.partialPathName().asChar());
		break;
	}

	// FOR TESTING PURPOSES ONLY: Write extent.
	MdxParser::Extent extent;
	extent.setMinimum({ -50, -50, -50 });
	extent.setMaximum({ 50, 50, 50 });
	parserModel.setExtent(extent);

	// Translate data into parser model.
	writeGeosetsToParser();
	writeNodesToParser();
	writeMaterialsToParser();
	writeAnimationsToParser();
	writeSequencesToParser();
	writeModelToParser();

	// Write to file.
	const char* filePath{ file.resolvedFullName().asChar() };
	parserModel.write(filePath);
}

void ModelTranslator::writeGeosetsToParser() {
	std::cout << "Writing geosets to parser." << std::endl;

	// Iterate through each polygonal mesh object and build a representative geoset.
	MItDag dagIter{ MItDag::kDepthFirst };
	//dagIter.reset(root, MItDag::kDepthFirst); // Set root node for export selection.
	for (; !dagIter.isDone(); dagIter.next()) {
		MDagPath dagPath;
		dagIter.getPath(dagPath);
		MFnDagNode dagFn{ dagPath };

		// Skip iteration if the current item is an intermediate object or doesn't support the correct function type.
		if (dagFn.isIntermediateObject() || !dagPath.hasFn(MFn::kMesh) || dagPath.hasFn(MFn::kTransform)) continue;

		GeosetTranslator geosetTranslator{ dagIter.currentItem() };
		geosetTranslator.writeGeometryToParser();
		geosetTranslators.push_back(geosetTranslator);
	}
}

void ModelTranslator::writeNodesToParser() {
	std::cout << "Writing nodes to parser." << std::endl;

	std::map<MObject, uint32_t, DagNodeComparator> dependencyNodeMap;

	MStatus status;

	// Translate bones and bone pivots.
	MItDag dagIter{ MItDag::kDepthFirst };
	for (; !dagIter.isDone(); dagIter.next()) {
		if (!dagIter.currentItem().hasFn(MFn::kJoint)) continue;

		BoneTranslator boneTranslator{ dagIter.currentItem() };
		uint32_t objectId{ static_cast<uint32_t>(parserModel.getPivotsCount()) };
		boneTranslator.writeAttributesToParser(objectId);
		boneTranslators.push_back(boneTranslator);
		dependencyNodeMap.insert({ dagIter.currentItem(), objectId });

		MDagPath dagPath;
		dagIter.getPath(dagPath);
		writePivotToParser(dagPath);
	}

	// Translate bone influences.
	for (auto it = geosetTranslators.begin(); it != geosetTranslators.end(); ++it) {
		it->writeInfluencesToParser(dependencyNodeMap);
	}

	// Translate helpers and helper pivots.
	dagIter.reset();
	for (; !dagIter.isDone(); dagIter.next()) {
		if (!dagIter.currentItem().hasFn(MFn::kLocator)) continue;

		HelperTranslator helperTranslator{ dagIter.currentItem() };
		uint32_t objectId{ static_cast<uint32_t>(parserModel.getPivotsCount()) };
		helperTranslator.writeAttributesToParser(objectId);
		helperTranslators.push_back(helperTranslator);
		dependencyNodeMap.insert({ dagIter.currentItem(), objectId });

		MDagPath dagPath;
		dagIter.getPath(dagPath);
		dagPath.getAPathTo(dagPath.transform(), dagPath);
		writePivotToParser(dagPath);
	}

	// Translate node hierarchy.
	for (auto it = boneTranslators.begin(); it != boneTranslators.end(); ++it) {
		it->writeParentToParser(dependencyNodeMap);
	}

	for (auto it = helperTranslators.begin(); it != helperTranslators.end(); ++it) {
		it->writeParentToParser(dependencyNodeMap);
	}
}

void ModelTranslator::writeMaterialsToParser() {
	std::cout << "Writing materials to parser." << std::endl;

	// Identify material dependency nodes.
	std::vector<MObject> materialDependencyNodes;
	for (auto it = geosetTranslators.begin(); it != geosetTranslators.end(); ++it) {
		it->writeMaterialToParser(materialDependencyNodes);
	}

	// Use dependency nodes to construct material translators and to identify texture dependency nodes.
	std::vector<MObject> textureDependencyNodes;
	for (auto it = materialDependencyNodes.begin(); it != materialDependencyNodes.end(); ++it) {
		MaterialTranslator materialTranslator{ *it };
		materialTranslator.writeAttributesToParser();
		materialTranslator.writeLayersToParser(textureDependencyNodes);
		materialTranslators.push_back(materialTranslator);
	}

	for (auto it = textureDependencyNodes.begin(); it != textureDependencyNodes.end(); ++it) {
		TextureTranslator textureTranslator{ *it };
		textureTranslator.writeAttributesToParser();
		textureTranslators.push_back(textureTranslator);
	}
}

void ModelTranslator::writeAnimationsToParser() {
	std::cout << "Writing animations to parser." << std::endl;

	for (auto it = boneTranslators.begin(); it != boneTranslators.end(); ++it) {
		it->writeAnimationToParser();
	}

	for (auto it = helperTranslators.begin(); it != helperTranslators.end(); ++it) {
		it->writeAnimationToParser();
	}
}

void ModelTranslator::writeSequencesToParser() {
	std::cout << "Writing sequences to parser." << std::endl;

	MItDependencyNodes dependencyNodesIter;
	for (; !dependencyNodesIter.isDone(); dependencyNodesIter.next()) {
		MFnDependencyNode dependFn{ dependencyNodesIter.thisNode() };
		if (dependFn.typeId() != timeSliderBookmarkTypeId) continue;

		SequenceTranslator sequenceTranslator{ dependencyNodesIter.thisNode() };
		sequenceTranslator.writeSequenceToParser();
		sequenceTranslators.push_back(sequenceTranslator);
	}
}

void ModelTranslator::writePivotToParser(MDagPath dagPath) {
	MFnTransform transformFn{ dagPath };
	MVector translationVector{ transformFn.getTranslation(MSpace::kWorld) };
	std::array<float, 3> pivot{ static_cast<float>(translationVector.x), static_cast<float>(translationVector.y), static_cast<float>(translationVector.z) };
	parserModel.addPivot(pivot);
}

void ModelTranslator::writeModelToParser() {
	for (auto boneTranslator : boneTranslators) parserModel.addBone(boneTranslator.getParserBone());
	boneTranslators.clear();
	for (auto geosetTranslator : geosetTranslators) parserModel.addGeoset(geosetTranslator.getParserGeoset());
	geosetTranslators.clear();
	for (auto helperTranslator : helperTranslators) parserModel.addHelper(helperTranslator.getParserHelper());
	helperTranslators.clear();
	for (auto materialTranslator : materialTranslators) parserModel.addMaterial(materialTranslator.getParserMaterial());
	materialTranslators.clear();
	for (auto sequenceTranslator : sequenceTranslators) parserModel.addSequence(sequenceTranslator.getParserSequence());
	sequenceTranslators.clear();
	for (auto textureTranslator : textureTranslators) parserModel.addTexture(textureTranslator.getParserTexture());
	textureTranslators.clear();
}

std::vector<NodeTranslator> ModelTranslator::getNodeTranslators() {
	std::vector<NodeTranslator> nodeTranslators;

	for (auto boneTranslator : boneTranslators) {
		nodeTranslators.push_back(boneTranslator);
	}
	for (auto helperTranslator : helperTranslators) {
		nodeTranslators.push_back(helperTranslator);
	}

	return nodeTranslators;
}