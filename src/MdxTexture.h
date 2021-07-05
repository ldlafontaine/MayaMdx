#pragma once

#include <CascLib.h>

#include <maya/MPxNode.h>
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFloatVector.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MImage.h>

#include <maya/MPxShadingNodeOverride.h>
#include <maya/MShaderManager.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MRenderUtil.h>
#include <maya/MStateManager.h>

#include <sstream>

class MdxTexture : public MPxNode {
public:
	virtual MStatus compute(const MPlug&, MDataBlock&);
	static  void* creator();
	static  MStatus initialize();

	static void readTextureFromCasc(MString path, char* buffer);

	static const MTypeId nodeTypeId;
	static const MString nodeTypeName;
	static const MString registrantIdString;
	static const MString drawDbClassification;
	static const MString classification;

private:
	MImage image;
	unsigned int imageWidth;
	unsigned int imageHeight;

	static MObject uvCoordAttr;
	static MObject replaceableIdAttr;
	static MObject fileNameAttr;
	static MObject wrapWidthAttr;
	static MObject wrapHeightAttr;
	static MObject outColorAttr;
	//static MObject outAlphaAttr;
};

class MdxTextureOverride : public MHWRender::MPxShadingNodeOverride
{
public:
	static MHWRender::MPxShadingNodeOverride* creator(const MObject& obj);

	~MdxTextureOverride() override;

	MHWRender::DrawAPI supportedDrawAPIs() const override;

	MString fragmentName() const override;
	void getCustomMappings(MHWRender::MAttributeParameterMappingList& mappings) override;

	void updateDG() override;
	void updateShader(MHWRender::MShaderInstance& shader, const MHWRender::MAttributeParameterMappingList& mappings) override;

private:
	MdxTextureOverride(const MObject& obj);

	MObject object;
	MString fileName;
	const MHWRender::MSamplerState* samplerState;
};