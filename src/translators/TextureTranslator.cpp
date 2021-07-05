#include "TextureTranslator.h"

#include "../MdxTexture.h"

#include <maya/MFnDependencyNode.h>

TextureTranslator::TextureTranslator(MObject dependencyNode) :
	ModelObjectTranslator{ dependencyNode } {
}

TextureTranslator::TextureTranslator(MdxParser::Texture parserTexture) :
	parserTexture{ parserTexture } {
}

MdxParser::Texture TextureTranslator::getParserTexture() const {
	return parserTexture;
}

void TextureTranslator::createDependencyNode() {
	MFnDependencyNode dependFn;
	dependencyNode = dependFn.create(MdxTexture::nodeTypeId);
	MPlug fileNamePlug{ dependFn.findPlug("fileName", true) };
	MPlug replaceableIdPlug{ dependFn.findPlug("replaceableId", true) };
	fileNamePlug.setString(parserTexture.getFileName().c_str());
	replaceableIdPlug.setInt(parserTexture.getReplaceableId());
}

void TextureTranslator::writeAttributesToParser() {
	MFnDependencyNode dependFn{ dependencyNode };
	MPlug fileNamePlug{ dependFn.findPlug("fileName", true) };
	MPlug replaceableIdPlug{ dependFn.findPlug("replaceableId", true) };
	parserTexture.setFileName(fileNamePlug.asString().asChar());
	parserTexture.setReplaceableId(replaceableIdPlug.asInt());
}