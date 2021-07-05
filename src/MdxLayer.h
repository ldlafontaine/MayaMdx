#pragma once

#include <maya/MPxNode.h>
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFloatVector.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>

#include <maya/MPxShadingNodeOverride.h>
#include <maya/MShaderManager.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MRenderUtil.h>
#include <maya/MStateManager.h>
#include <maya/MImage.h>

class MdxLayer : public MPxNode {
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
	static MObject filterModeAttr;
	static MObject unshadedAttr;
	static MObject sphereEnvironmentMapAttr;
	static MObject twoSidedAttr;
	static MObject unfoggedAttr;
	static MObject noDepthTestAttr;
	static MObject noDepthSetAttr;
	static MObject textureAttr;
	static MObject textureAnimationAttr;
	static MObject alphaAttr;
	//static MObject emissiveGainAttr;
	//static MObject fresnelColorAttr;
	//static MObject fresnelOpacityAttr;
	//static MObject fresnelTeamColorAttr;
	static MObject outColorAttr;
};