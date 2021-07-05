#include "MaterialTranslator.h"

#include "../MdxStandardShader.h"
#include "../MdxLayer.h"

#include <maya/MFnDependencyNode.h>
#include <maya/MDGModifier.h>
#include <maya/MItDependencyGraph.h>

MaterialTranslator::MaterialTranslator(MObject dependencyNode) :
	ModelObjectTranslator{ dependencyNode } {
}

MaterialTranslator::MaterialTranslator(MdxParser::Material parserMaterial) :
	parserMaterial{ parserMaterial } {
}

MdxParser::Material MaterialTranslator::getParserMaterial() const {
	return parserMaterial;
}

void MaterialTranslator::createDependencyNode() {
	MFnDependencyNode dependFn;
	MDGModifier dgModifier;

	// Create shader and find plugs
	dependencyNode = dependFn.create(MdxStandardShader::nodeTypeId);
	MPlug materialOutColorPlug{ dependFn.findPlug("outColor", true) };

	// Create shading group and find plugs
	shadingEngineDependencyNode = dependFn.create("shadingEngine");
	MPlug surfaceShaderPlug{ dependFn.findPlug("surfaceShader", true) };

	// Connect shader to shading engine
	dgModifier.connect(materialOutColorPlug, surfaceShaderPlug);
}

void MaterialTranslator::readLayersFromParser(std::map<uint32_t, MObject>& textureIdMap) {
	MDGModifier dgModifier;

	MFnDependencyNode dependFn{ dependencyNode };
	MPlug materialLayersPlug{ dependFn.findPlug("layers", true) };

	auto layers{ parserMaterial.getLayers() };
	for (int i{ 0 }; i < layers.size(); ++i) {
		MdxParser::Layer layer{ layers.at(i) };
		MObject layerNode = dependFn.create(MdxLayer::nodeTypeId);

		// Set layer attributes.
		MPlug layerFilterModePlug{ dependFn.findPlug("filterMode", true) };
		layerFilterModePlug.setInt(layer.getFilterMode());

		// Set layer texture.
		MPlug layerTexturePlug{ dependFn.findPlug("texture", true) };
		MPlug layerOutColorPlug{ dependFn.findPlug("outColor", true) };

		MObject textureNode{ textureIdMap.at(layer.getTextureId()) };
		dependFn.setObject(textureNode);
		MPlug textureOutColorPlug{ dependFn.findPlug("outColor", true) };

		// Connect texture to layer
		dgModifier.connect(textureOutColorPlug, layerTexturePlug);

		// Connect layer to shader
		dgModifier.connect(layerOutColorPlug, materialLayersPlug.elementByLogicalIndex(i));
	}
}

void MaterialTranslator::writeAttributesToParser() {
	// Query plugs for attributes like Constant Color, Priority Plane, Sort Primitives Far Z and set the corresponding properties in parserMaterial.
}

void MaterialTranslator::writeLayersToParser(std::vector<MObject>& textureDependencyNodes) {

	MItDependencyGraph graphIter{ dependencyNode, MFn::Type::kInvalid, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst };
	for (; !graphIter.isDone(); graphIter.next()) {
		MFnDependencyNode dependFn{ graphIter.currentItem() };
		if (dependFn.typeId() != MdxLayer::nodeTypeId) continue;

		MdxParser::Layer layer;

		// Set layer attributes.
		MPlug layerFilterModePlug{ dependFn.findPlug("filterMode", true) };
		layer.setFilterMode(layerFilterModePlug.asInt());

		// Set texture ID.
		MPlug texturePlug{ dependFn.findPlug("texture", true) };
		MObject textureDependencyNode{ texturePlug.source().node() };
		auto textureDependencyNodeIterator{ std::find(textureDependencyNodes.begin(), textureDependencyNodes.end(), textureDependencyNode) };
		if (textureDependencyNodeIterator != textureDependencyNodes.end()) {
			uint32_t textureId{ static_cast<uint32_t>(std::distance(textureDependencyNodes.begin(), textureDependencyNodeIterator)) };
			layer.setTextureId(textureId);
		}
		else {
			uint32_t textureId{ static_cast<uint32_t>(textureDependencyNodes.size()) };
			layer.setTextureId(textureId);
			textureDependencyNodes.push_back(textureDependencyNode);
		}

		// Add layer to parser material.
		parserMaterial.addLayer(layer);
	}
}