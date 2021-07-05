#include "GeosetTranslator.h"

#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatArray.h>
#include <maya/MGlobal.h>
#include <maya/MCommandResult.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MPointArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFnSet.h>

#include <stdint.h>
#include <set>
#include <sstream>

GeosetTranslator::GeosetTranslator(MObject dependencyNode) :
	ModelObjectTranslator{ dependencyNode } {
}

GeosetTranslator::GeosetTranslator(MdxParser::Geoset parserGeoset) :
	parserGeoset{ parserGeoset } {
}

MdxParser::Geoset GeosetTranslator::getParserGeoset() const {
	return parserGeoset;
}

MObject GeosetTranslator::createDependencyNode(MObject root) {
	// Create geometry.
	MObject outputMesh{};
	uint32_t numFaces{ parserGeoset.getFacesCount() / 3 };
	uint32_t numVertices{ parserGeoset.getVertexCount() };

	MFloatPointArray vertexArray{};
	for (std::array<float, 3> vertex : parserGeoset.getVertexPositions()) {
		MFloatPoint point{ vertex.at(0), vertex.at(1), vertex.at(2) };
		vertexArray.append(point);
	}

	MIntArray polygonConnects{};
	for (uint16_t face : parserGeoset.getFaces()) {
		polygonConnects.append(face);
	}

	MIntArray polygonCounts{};
	for (uint32_t polygonCount = 0; polygonCount < (polygonConnects.length() / 3); polygonCount++) {
		polygonCounts.append(3); // This implementation assumes that all polygons are triangles, which is not necessarily correct.
	}

	MFnMesh meshFn;
	MObject createdMesh{ meshFn.create(numVertices, numFaces, vertexArray, polygonCounts, polygonConnects, outputMesh) };

	// Set UVs.
	for (MdxParser::TextureCoordinateSet textureCoordinateSet : parserGeoset.getTextureCoordinateSets()) {
		MFloatArray uArray;
		MFloatArray vArray;
		std::vector<std::array<float, 2>> textureCoordinates{ textureCoordinateSet.getTextureCoordinates() };
		for (int i{ 0 }; i < textureCoordinates.size(); i++) {
			uArray.append(textureCoordinates.at(i).at(0));
			vArray.append(textureCoordinates.at(i).at(1) * -1 + 1); // V Coordinates must be flipped.
		}
		meshFn.setUVs(uArray, vArray);
		meshFn.assignUVs(polygonCounts, polygonConnects);
	}

	// Set Normals.
	MVectorArray normalsVectorArray{};
	for (std::array<float, 3> normal : parserGeoset.getVertexNormals()) {
		MVector normalVector{ normal.at(0), normal.at(1), normal.at(2) };
		normalsVectorArray.append(normalVector);
	}

	MIntArray vertexCount{};
	MIntArray vertexList{};
	meshFn.getVertices(vertexCount, vertexList);
	meshFn.setVertexNormals(normalsVectorArray, vertexList);

	// Set Name.
	MFnDagNode dagFn{ createdMesh };
	dagFn.setName("Geoset_0001");

	// Parent into hierarchy.
	dagFn.setObject(root);
	dagFn.addChild(createdMesh);

	// Set dependency node.
	dependencyNode = createdMesh;
	return dependencyNode;
}

void GeosetTranslator::readInfluencesFromParser(std::vector<NodeTranslator> nodeTranslators) {
	MFnDagNode geosetDagFn{ dependencyNode };

	std::vector<uint32_t> matrixIndices{ parserGeoset.getMatrixIndices() };
	std::set<uint32_t> matrixIndicesSet{ matrixIndices.begin(), matrixIndices.end() };
	std::vector<uint32_t> uniqueMatrixIndices{ matrixIndicesSet.begin(), matrixIndicesSet.end() };

	// Create a skin cluster for each polygon object, using the bones corresponding to the geoset's matrix indices as influences.
	std::stringstream command;
	command << "skinCluster " << "-toSelectedBones -rbm " << geosetDagFn.fullPathName();

	for (uint32_t matrixIndex : uniqueMatrixIndices) {
		MObject boneDagNode = nodeTranslators.at(matrixIndex).getDependencyNode();
		MFnDagNode boneDagFn(boneDagNode);
		command << " " << boneDagFn.fullPathName();
	}

	MCommandResult result;
	MGlobal::executeCommand(command.str().c_str(), result);

	// Get resulting skin cluster dependency node.
	MStringArray resultStringArray;
	result.getResult(resultStringArray);
	MString skinClusterName{ resultStringArray[0] };
	MSelectionList selectionList;
	selectionList.add(skinClusterName);
	MObject skinCluster;
	selectionList.getDependNode(0, skinCluster);
	MFnDependencyNode skinDependFn{ skinCluster };

	// Remove bind poses.
	MItDependencyGraph graphIter{ skinCluster, MFn::Type::kDagPose, MItDependencyGraph::kUpstream, MItDependencyGraph::kDepthFirst };
	for (; !graphIter.isDone(); graphIter.next()) {
		MObject bindPose{ graphIter.currentItem() };
		MGlobal::deleteNode(bindPose);
	}

	// Weight vertices.
	std::vector<uint8_t> vertexGroups{ parserGeoset.getVertexGroups() };
	std::set<uint8_t> uniqueVertexGroups{ vertexGroups.begin(), vertexGroups.end() };

	MPlug weightListPlug{ skinDependFn.findPlug("weightList", true) };
	for (unsigned int vertexIndex{ 0 }; vertexIndex < weightListPlug.numElements(); vertexIndex++) {
		MPlug weightListPlugElement{ weightListPlug.elementByLogicalIndex(vertexIndex) }; // Represents a vertex. One such element exists for each vertex.
		MPlug weightsPlug{ weightListPlugElement.child(0) };

		uint32_t vertexGroup{ parserGeoset.getVertexGroups().at(vertexIndex) };

		uint32_t actualMatrixIndex{ 0 };
		for (unsigned int matrixGroupIter{ 0 }; matrixGroupIter < vertexGroup; matrixGroupIter++) {
			actualMatrixIndex += parserGeoset.getMatrixGroups().at(matrixGroupIter);
		}

		uint32_t validMatrixIndex{ matrixIndices.at(actualMatrixIndex) };

		for (unsigned int influenceIndex{ 0 }; influenceIndex < uniqueMatrixIndices.size(); influenceIndex++) {
			MPlug weightsElement{ weightsPlug.elementByLogicalIndex(influenceIndex) }; // Represents an influence object. One such element exists for each influence associated with this object.

			uint32_t influenceMatrixIndex{ uniqueMatrixIndices.at(influenceIndex) };

			if (influenceMatrixIndex == validMatrixIndex) {
				weightsElement.setValue(1.0);
			}
			else {
				weightsElement.setValue(0.0);
			}
		}
	}
}

void GeosetTranslator::readMaterialFromParser(std::map<uint32_t, MObject>& materialIdMap) {
	MObject materialNode{ materialIdMap.at(parserGeoset.getMaterialId()) };
	MFnDependencyNode dependFn{ materialNode };
	MPlugArray destinations;
	dependFn.findPlug("outColor", true).destinations(destinations);
	MObject shadingEngineNode{ destinations[0].node() };

	MFnSet setFn{ shadingEngineNode };
	setFn.addMember(dependencyNode);
}

void GeosetTranslator::writeGeometryToParser() {
	// Get DAG path.
	MDagPath dagPath;
	MDagPath::getAPathTo(dependencyNode, dagPath);
	MFnDagNode dagFn{ dagPath };

	// Restore bind pose.
	MFnDagNode transformDagFn{ dagPath.transform() };
	std::stringstream bindPoseCommand;
	bindPoseCommand << "dagPose -bindPose -restore " << transformDagFn.fullPathName();
	MGlobal::executeCommand(bindPoseCommand.str().c_str());

	MFnMesh meshFn(dagPath);
	int vertexCount{ meshFn.numVertices() };

	// Set vertex positions
	MPointArray vertexList;
	meshFn.getPoints(vertexList, MSpace::kWorld);
	std::vector<std::array<float, 3>> vertexPositions;
	for (MPoint vertex : vertexList) {
		//vertex.cartesianize();
		std::array<float, 3> vertexPosition{ static_cast<float>(vertex[0]), static_cast<float>(vertex[1]), static_cast<float>(vertex[2]) };
		vertexPositions.push_back(vertexPosition);
	}
	parserGeoset.setVertexPositions(vertexPositions);

	// Set vertex normals
	MFloatVectorArray vertexNormalsVectorArray;
	meshFn.getVertexNormals(true, vertexNormalsVectorArray); // should angle weighted be true or false?
	std::vector<std::array<float, 3>> normals;
	for (MFloatVector vertexNormalVector : vertexNormalsVectorArray) {
		std::array<float, 3> normal{ vertexNormalVector[0], vertexNormalVector[1], vertexNormalVector[2] };
		normals.push_back(normal);
	}
	parserGeoset.setVertexNormals(normals);

	// Write Face Type Groups
	// faceTypeGroups seems to be an array of unique face types (points, lines, triangles, quads, etc). Each value only ever appears once.
	std::vector<uint32_t> faceTypeGroups{ 4 };	// This implementation assumes all faces are triangles.
	parserGeoset.setFaceTypeGroups(faceTypeGroups);

	// Write Face Groups
	// faceGroups seems to be an array containing the number of vertices in each faceTypeGroup. (not entirely sure about this)
	std::vector<uint32_t> faceGroups{ static_cast<uint32_t>(vertexCount) };
	parserGeoset.setFaceGroups(faceGroups);

	// Write Faces
	// faces is an array containing the vertex indices, which can be used to map how they are connected.
	MIntArray triangleCounts;
	MIntArray triangleVertices;
	meshFn.getTriangles(triangleCounts, triangleVertices);
	std::vector<uint16_t> faces{ std::begin(triangleVertices), std::end(triangleVertices) };
	parserGeoset.setFaces(faces);

	// Vertex Groups, Matrix Groups, and Matrix Indices can be left at their default values until skin weights are assigned.
	std::vector<uint8_t> vertexGroups;
	for (int vertexGroupIter{ 0 }; vertexGroupIter < vertexCount; vertexGroupIter++) {
		uint8_t vertexGroup{ 0 };
		vertexGroups.push_back(vertexGroup);
	}
	parserGeoset.setVertexGroups(vertexGroups);

	std::vector<uint32_t> matrixGroups;
	matrixGroups.push_back(uint32_t(1));
	parserGeoset.setMatrixGroups(matrixGroups);

	std::vector<uint32_t> matrixIndices;
	matrixIndices.push_back(uint32_t(0));
	parserGeoset.setMatrixIndices(matrixIndices);

	// Write Texture Coordinate Sets
	MFloatArray uArray;
	MFloatArray vArray;
	meshFn.getUVs(uArray, vArray);

	std::vector<std::array<float, 2>> textureCoordinates;
	for (unsigned int uvIter{ 0 }; uvIter < uArray.length(); uvIter++) {
		std::array<float, 2> textureCoordinate{ uArray[uvIter], vArray[uvIter] * -1 + 1 }; // V Coordinates must be flipped.
		textureCoordinates.push_back(textureCoordinate);
	}

	std::vector<MdxParser::TextureCoordinateSet> textureCoordinateSets;
	MdxParser::TextureCoordinateSet textureCoordinateSet;
	textureCoordinateSet.setTextureCoordinates(textureCoordinates);
	textureCoordinateSets.push_back(textureCoordinateSet);
	parserGeoset.setTextureCoordinateSets(textureCoordinateSets);
}

void GeosetTranslator::writeInfluencesToParser(std::map<MObject, uint32_t, DagNodeComparator>& dependencyNodeMap) {
	MFnDagNode geosetDagFn{ dependencyNode };
	uint32_t vertexCount{ parserGeoset.getVertexCount() };
	std::vector<std::vector<uint32_t>> matrixIndices;
	std::vector<uint32_t> matrixGroups;
	std::vector<uint8_t> vertexGroups;

	MItDependencyGraph graphIter{ dependencyNode, MFn::Type::kSkinClusterFilter, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst };
	for (; !graphIter.isDone(); graphIter.next()) {
		MFnDependencyNode skinDependFn{ graphIter.currentItem() };

		// Determine the Object IDs of the dependency nodes influencing the skin cluster.
		std::vector<uint32_t> objectIds;
		MPlug matrixPlug{ skinDependFn.findPlug("matrix", true) };
		for (unsigned int matrixPlugIterator{ 0 }; matrixPlugIterator < matrixPlug.numElements(); ++matrixPlugIterator) {
			MPlug matrixElementPlug{ matrixPlug.elementByLogicalIndex(matrixPlugIterator) };
			MPlug matrixElementPlugSource{ matrixElementPlug.source() };
			MObject matrixElementNode{ matrixElementPlugSource.node() };
			objectIds.push_back(dependencyNodeMap.at(matrixElementNode));
		}

		// Get plug element indices for all influence objects with a weight greater than zero.
		MPlug weightListPlug{ skinDependFn.findPlug("weightList", true) };
		for (unsigned int vertexIndex{ 0 }; vertexIndex < weightListPlug.numElements(); ++vertexIndex) {
			MPlug weightListPlugElement{ weightListPlug.elementByLogicalIndex(vertexIndex) }; // Represents a vertex.
			MPlug weightsPlug{ weightListPlugElement.child(0) };

			std::vector<uint32_t> vertexInfluences;

			for (unsigned int influenceIndex{ 0 }; influenceIndex < weightsPlug.numElements(); ++influenceIndex) {
				MPlug weightsElement{ weightsPlug.elementByLogicalIndex(influenceIndex) }; // Represents an influence object.
				if (weightsElement.asDouble() > 0) {
					uint32_t objectId{ objectIds.at(influenceIndex) };
					vertexInfluences.push_back(objectId);
				}
			}

			// If a matrix group already exists representing these influences, find it and add its index in the matrix indices vector to the vertex groups vector.
			auto findIterator{ std::find(matrixIndices.begin(), matrixIndices.end(), vertexInfluences) };
			if (findIterator != matrixIndices.end()) {
				uint8_t vertexGroup{ static_cast<uint8_t>(std::distance(matrixIndices.begin(), findIterator)) };
				vertexGroups.push_back(vertexGroup);
			}
			else {
				// If no matching matrix group is found, create a new one then push back its index on to the vertex groups vector.
				matrixIndices.push_back(vertexInfluences);
				matrixGroups.push_back(static_cast<uint32_t>(vertexInfluences.size()));
				vertexGroups.push_back(static_cast<uint8_t>(matrixIndices.size() - 1));
			}
		}
	}

	std::vector<uint32_t> matrixIndicesFlattened;
	for (auto matrixGroup : matrixIndices) {
		matrixIndicesFlattened.insert(matrixIndicesFlattened.end(), matrixGroup.begin(), matrixGroup.end());
	}

	parserGeoset.setVertexGroups(vertexGroups);
	parserGeoset.setMatrixGroups(matrixGroups);
	parserGeoset.setMatrixIndices(matrixIndicesFlattened);
}

void GeosetTranslator::writeMaterialToParser(std::vector<MObject>& materialDependencyNodes) {
	MItDependencyGraph graphIter{ dependencyNode, MFn::Type::kShadingEngine, MItDependencyGraph::kDownstream, MItDependencyGraph::kBreadthFirst };
	for (; !graphIter.isDone(); graphIter.next()) {
		auto findIterator{ std::find(materialDependencyNodes.begin(), materialDependencyNodes.end(), graphIter.currentItem()) };
		if (findIterator != materialDependencyNodes.end()) {
			uint32_t materialId{ static_cast<uint32_t>(std::distance(materialDependencyNodes.begin(), findIterator)) };
			parserGeoset.setMaterialId(materialId);
		}
		else {
			uint32_t materialId{ static_cast<uint32_t>(materialDependencyNodes.size()) };
			parserGeoset.setMaterialId(materialId);
			materialDependencyNodes.push_back(graphIter.currentItem());
		}
	}
}