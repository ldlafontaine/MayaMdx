#pragma once

#include <maya/MPxNode.h>
#include <maya/MIOStream.h>

#include <maya/MString.h>
#include <maya/MPlug.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnStringData.h>
#include <maya/MFloatVector.h>
#include <maya/MFnDependencyNode.h>

// Viewport 2.0 includes
#include <maya/MDrawRegistry.h>
#include <maya/MPxShaderOverride.h>
#include <maya/MDrawContext.h>
#include <maya/MStateManager.h>
#include <maya/MViewport2Renderer.h>
#include <maya/MShaderManager.h>

// Shader fragment includes 
#include <maya/MFragmentManager.h>
#include <maya/MPxShadingNodeOverride.h>
#include <maya/MRenderUtil.h>
#include <maya/MImage.h>

class MdxStandardShader : public MPxNode {
public:
	virtual MStatus compute(const MPlug&, MDataBlock&);
	static  void* creator();
	static  MStatus initialize();

	static const MTypeId nodeTypeId;
	static const MString nodeTypeName;
	static const MString registrantIdString;
	static const MString drawDbClassification;
	static const MString classification;

private:
	static MObject priorityPlaneAttr;
	static MObject constantColorAttr;
	static MObject sortPrimitivesFarZAttr;
	static MObject fullResolutionAttr;
	static MObject shaderAttr;
	static MObject layersAttr;
	static MObject outColorAttr;
};

class MdxStandardShaderOverride : public MHWRender::MPxShaderOverride {
public:
	static MHWRender::MPxShaderOverride* Creator(const MObject& obj);
	virtual ~MdxStandardShaderOverride();
	virtual MString initialize(const MInitContext& initContext, MInitFeedback& initFeedback);
	virtual void updateDG(MObject object);
	virtual void updateDevice();
	virtual MHWRender::MShaderInstance* shaderInstance(MDrawContext& drawContext) const;
	virtual void activateKey(MHWRender::MDrawContext& context, const MString& key);
	virtual void terminateKey(MHWRender::MDrawContext& context, const MString& key);
	virtual bool draw(MHWRender::MDrawContext& context, const MHWRender::MRenderItemList& renderItemList) const;
	virtual MHWRender::DrawAPI supportedDrawAPIs() const;
	virtual bool isTransparent();
	virtual MHWRender::MShaderInstance* nonTexturedShaderInstance(bool& monitor) const;
	virtual bool overridesDrawState();

protected:
	MdxStandardShaderOverride(const MObject& obj);
	static const MHWRender::MBlendState* blendState;
	float transparency;
	float diffuse[4];
	float specular[3];
	float shininess[3];
	float nonTextured[3];
	MHWRender::MShaderInstance* colorShaderInstance;
	MHWRender::MShaderInstance* nonTexturedColorShaderInstance;
	MString fragmentName;
	MObject object;
	MString name;
	const MHWRender::MSamplerState* samplerState;
	MHWRender::MTexture* defaultTexture;
	mutable MString resolvedMapName;
	mutable MString resolvedSamplerName;
	MHWRender::MAttributeParameterMappingList mappings;

	MPlugArray texturePlugs;

private:
	void initializeFragmentShaders();
	void createCustomMappings();
	void assignTexture(MHWRender::MTexture* texture, MHWRender::MTextureManager* textureManager);
};