#include "MdxFileTranslator.h"
#include "MdxStandardShader.h"
#include "MdxLayer.h"
#include "MdxTexture.h"

#include <maya/MStatus.h>
#include <maya/MObject.h>
#include <maya/MFnPlugin.h>
#include <maya/MStreamUtils.h>
#include <maya/MDrawRegistry.h>

#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>
#include <vector>
#include <sstream>

#ifdef WIN32
#pragma comment(lib,"Foundation.lib")
#pragma comment(lib,"OpenMaya.lib")
#pragma comment(lib,"OpenMayaFx.lib")
#pragma comment(lib,"Image.lib")
#pragma comment(lib,"OpenMayaAnim.lib")
#pragma comment(lib,"OpenMayaRender.lib")
#endif

#ifdef WIN32
#define MLL_EXPORT extern __declspec(dllexport) 
#else
#define MLL_EXPORT
#endif

const char* translatorOptionScript{ "MdxFileTranslatorScript" };
const char* translatorDefaultOptions{ "-namesonly=0;" };

MLL_EXPORT MStatus initializePlugin(MObject obj)
{
	MFnPlugin plugin(obj, "Lucas LaFontaine", "1.0", "Any");
	MStatus status;

	std::cout.set_rdbuf(MStreamUtils::stdOutStream().rdbuf());
	std::cerr.set_rdbuf(MStreamUtils::stdErrorStream().rdbuf());

	// Register MdxFileTranslator
	status = plugin.registerFileTranslator("Warcraft 3 MDX", "none", MdxFileTranslator::creator, (char*)translatorOptionScript, (char*)translatorDefaultOptions);
	if (status != MS::kSuccess) status.perror("MayaExportCommand::registerFileTranslator");

	// Register MdxStandardShader and MdxStandardShaderOverride
	status = plugin.registerNode(MdxStandardShader::nodeTypeName, MdxStandardShader::nodeTypeId, MdxStandardShader::creator, MdxStandardShader::initialize, MPxNode::kDependNode, &MdxStandardShader::classification);
	if (status != MS::kSuccess) return status;
	MHWRender::MDrawRegistry::registerShaderOverrideCreator(MdxStandardShader::drawDbClassification, MdxStandardShader::registrantIdString, MdxStandardShaderOverride::Creator);
	if (status != MS::kSuccess) return status;

	// Register MdxLayer
	status = plugin.registerNode(MdxLayer::nodeTypeName, MdxLayer::nodeTypeId, MdxLayer::creator, MdxLayer::initialize, MPxNode::kDependNode, &MdxLayer::classification);
	//status = MHWRender::MDrawRegistry::registerShadingNodeOverrideCreator(MdxLayer::drawDbClassification, MdxLayer::registrantIdString, MdxLayerOverride::creator);

	// Register MdxTexture and MdxTextureOverride
	status = plugin.registerNode(MdxTexture::nodeTypeName, MdxTexture::nodeTypeId, MdxTexture::creator, MdxTexture::initialize, MPxNode::kDependNode, &MdxTexture::classification);
	status = MHWRender::MDrawRegistry::registerShadingNodeOverrideCreator(MdxTexture::drawDbClassification, MdxTexture::registrantIdString, MdxTextureOverride::creator);

	return status;

}

MLL_EXPORT MStatus uninitializePlugin(MObject obj)
{
	MFnPlugin plugin(obj);
	MStatus status;

	// Deregister MdxFileTranslator
	status = plugin.deregisterFileTranslator("Warcraft 3 MDX");
	if (status != MS::kSuccess) {
		status.perror("MayaExportCommand::deregisterFileTranslator");
	}

	// Deregister MdxStandardShader and MdxStandardShaderOverride
	status = plugin.deregisterNode(MdxStandardShader::nodeTypeId);
	status = MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(MdxStandardShader::drawDbClassification, MdxStandardShader::registrantIdString);

	// Deregister MdxLayer
	status = plugin.deregisterNode(MdxLayer::nodeTypeId);
	//status = MHWRender::MDrawRegistry::deregisterShadingNodeOverrideCreator(MdxLayer::drawDbClassification, MdxLayer::registrantIdString);

	// Deregister MdxTexture and MdxTextureOverride
	status = plugin.deregisterNode(MdxTexture::nodeTypeId);
	status = MHWRender::MDrawRegistry::deregisterShadingNodeOverrideCreator(MdxTexture::drawDbClassification, MdxTexture::registrantIdString);

	return status;
}