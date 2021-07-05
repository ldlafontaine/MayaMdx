#pragma once

#include "ModelObjectTranslator.h"

#include <MdxParser/Texture.h>
#include <maya/MObject.h>

#include <map>

class TextureTranslator : public ModelObjectTranslator
{
public:
	TextureTranslator(MObject dependencyNode);
	TextureTranslator(MdxParser::Texture parserTexture);

	MdxParser::Texture getParserTexture() const;

	void createDependencyNode();

	void writeAttributesToParser();

protected:
	MdxParser::Texture parserTexture;
	MObject shadingEngineDependencyNode;
};