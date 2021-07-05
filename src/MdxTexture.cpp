#include "MdxTexture.h"

const MTypeId MdxTexture::nodeTypeId(0x70035);
const MString MdxTexture::nodeTypeName("MdxTexture");
const MString MdxTexture::registrantIdString{ "MdxTexture" };
const MString MdxTexture::drawDbClassification{ "drawdb/shader/utility/general/MdxTexture" };
const MString MdxTexture::classification{ "utility/general:" + drawDbClassification };

MObject MdxTexture::uvCoordAttr;
MObject MdxTexture::replaceableIdAttr;
MObject MdxTexture::fileNameAttr;
MObject MdxTexture::wrapWidthAttr;
MObject MdxTexture::wrapHeightAttr;
MObject MdxTexture::outColorAttr;

void* MdxTexture::creator()
{
	return new MdxTexture();
}

MStatus MdxTexture::initialize()
{
	MFnNumericAttribute numericFn;

	MObject uCoord = numericFn.create("uCoord", "u", MFnNumericData::kFloat);
	MObject vCoord = numericFn.create("vCoord", "v", MFnNumericData::kFloat);
	uvCoordAttr = numericFn.create("uvCoord", "uv", uCoord, vCoord);
	numericFn.setHidden(true);

	replaceableIdAttr = numericFn.create("replaceableId", "rid", MFnNumericData::Type::kInt);

	MFnTypedAttribute typedFn;

	fileNameAttr = typedFn.create("fileName", "fn", MFnData::Type::kString);
	typedFn.setUsedAsFilename(true);

	wrapWidthAttr = numericFn.create("wrapWidth", "ww", MFnNumericData::Type::kBoolean);
	wrapHeightAttr = numericFn.create("wrapHeight", "wh", MFnNumericData::Type::kBoolean);

	outColorAttr = numericFn.createColor("outColor", "oc");
	numericFn.setStorable(false);
	numericFn.setWritable(false);
	numericFn.setAffectsAppearance(true);
	
	addAttribute(uvCoordAttr);
	addAttribute(replaceableIdAttr);
	addAttribute(fileNameAttr);
	addAttribute(wrapWidthAttr);
	addAttribute(wrapHeightAttr);
	addAttribute(outColorAttr);

	attributeAffects(uvCoordAttr, outColorAttr);
	attributeAffects(fileNameAttr, outColorAttr);
	attributeAffects(replaceableIdAttr, outColorAttr);

	return MS::kSuccess;
}

MStatus MdxTexture::compute(const MPlug& plug, MDataBlock& data)
{
	if ((plug != outColorAttr) && (plug.parent() != outColorAttr)) return MS::kUnknownParameter;

	/* OLD IMPLEMENTATION
	// Get image
	if (!image.pixels()) {
		MDataHandle fileNameHandle = data.inputValue(fileNameAttr);
		MString fileName = fileNameHandle.asString();
		if (fileName.length() <= 0)
		fileNameHandle.setClean();

		image.readFromFile(fileName);
		image.getSize(imageWidth, imageHeight);
	}

	// Read pixels
	MFloatVector computedColor{ 0.0f, 0.0f, 0.0f };
	float computedAlpha{ 1.0f };

	unsigned char* pixels = image.pixels();
	if (pixels && imageWidth > 0 && imageHeight > 0)
	{
		float2& uv = data.inputValue(uvCoordAttr).asFloat2();
		float u = uv[0]; if (u < 0.0f) u = 0.0f; if (u > 1.0f) u = 1.0f;
		float v = uv[1]; if (v < 0.0f) v = 0.0f; if (v > 1.0f) v = 1.0f;

		static const size_t pixelSize = 4;
		size_t rowOffset = (size_t)(v * (imageHeight - 1)) * imageWidth;
		size_t colOffset = (size_t)(u * (imageWidth - 1));
		const unsigned char* pixel = pixels + ((rowOffset + colOffset) * pixelSize);

		computedColor[0] = ((float)pixel[0]) / 255.0f;
		computedColor[1] = ((float)pixel[1]) / 255.0f;
		computedColor[2] = ((float)pixel[2]) / 255.0f;
		computedAlpha = ((float)pixel[3]) / 255.0f;
	}

	// Set output values
	MDataHandle outColorHandle = data.outputValue(outColorAttr);
	MFloatVector& outColor = outColorHandle.asFloatVector();
	outColor = computedColor;
	outColorHandle.setClean();
	*/

	// Below is the temporary implementation until CascLib can be implemented.
	MDataHandle outColorHandle = data.outputValue(outColorAttr);
	MFloatVector& outColor = outColorHandle.asFloatVector();
	outColor = MFloatVector(1.0f, 0.0f, 0.0f);

	return MS::kSuccess;
}

void MdxTexture::readTextureFromCasc(MString path, char* buffer) {

	/// Access wc3's data files
	HANDLE storage;

	if (!CascOpenStorage(L"D:\\Program Files (x86)\\Warcraft III", 0, &storage)) {
		std::cerr << "CascLib failed to open storage: " << GetLastError() << std::endl;
	}
	else {
		std::cout << "CascLib has opened storage successfully!" << std::endl;
	}

	std::stringstream pathStream;
	pathStream << "*:" << path.asChar();

	CASC_FIND_DATA findData;
	HANDLE findHandle = CascFindFirstFile(storage, pathStream.str().c_str(), &findData, NULL);

	if (!findHandle || findHandle == INVALID_HANDLE_VALUE) {
		std::cerr << "CascLib failed to find file: " << GetLastError() << std::endl;
	}

	std::cout << "CascLib has found a matching file at \"" << findData.szFileName << "\"." << std::endl;

	HANDLE file;

	/// Attempt to open the file
	if (!CascOpenFile(storage, findData.szFileName, 0, CASC_OPEN_BY_NAME, &file)) {
		std::cerr << "CascLib failed to open file: " << GetLastError() << std::endl;
	}

	/// Read the file in chunks
	DWORD bytesRead = 1;

	while (bytesRead > 0) {
		if (!CascReadFile(file, buffer, sizeof(*buffer), &bytesRead)) {
			break;
		}
	}

	/// Cleanup
	if (file != NULL) CascCloseFile(file);
	if (findHandle != NULL) CloseHandle(findHandle);
	if (storage != NULL) CascCloseStorage(storage);
}

MHWRender::MPxShadingNodeOverride* MdxTextureOverride::creator(const MObject& obj) {
	return new MdxTextureOverride(obj);
}

MdxTextureOverride::MdxTextureOverride(const MObject& obj)
	: MPxShadingNodeOverride(obj), object{ obj } {
}

MdxTextureOverride::~MdxTextureOverride() { }

MHWRender::DrawAPI MdxTextureOverride::supportedDrawAPIs() const {
	return MHWRender::kOpenGL | MHWRender::kDirectX11 | MHWRender::kOpenGLCoreProfile;
}

MString MdxTextureOverride::fragmentName() const {
	return "mayaFileTexture";
}

void MdxTextureOverride::getCustomMappings(MHWRender::MAttributeParameterMappingList& mappings) {
	MHWRender::MAttributeParameterMapping mapParameterMapping("map", "", true, true);
	mappings.append(mapParameterMapping);

	MHWRender::MAttributeParameterMapping samplerParameterMapping("textureSampler", "", true, true);
	mappings.append(samplerParameterMapping);
}

void MdxTextureOverride::updateDG() {
	MStatus status;
	MFnDependencyNode node(object, &status);
	if (status) {
		node.findPlug("fileName", true).getValue(fileName);
	}
}

void MdxTextureOverride::updateShader(MHWRender::MShaderInstance& shader, const MHWRender::MAttributeParameterMappingList& mappings) {
	const MHWRender::MAttributeParameterMapping* mapParameterMapping = mappings.findByParameterName("map");
	MString mapParameterName = mapParameterMapping->resolvedParameterName();

	const MHWRender::MAttributeParameterMapping* samplerParameterMapping = mappings.findByParameterName("textureSampler");
	MString samplerParameterName = samplerParameterMapping->resolvedParameterName();

	MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer();
	if (!renderer) return;

	if (!samplerState) {
		MHWRender::MSamplerStateDesc samplerStateDesc;
		samplerStateDesc.filter = MHWRender::MSamplerState::kAnisotropic;
		samplerStateDesc.maxAnisotropy = 16;
		samplerState = MHWRender::MStateManager::acquireSamplerState(samplerStateDesc);
		shader.setParameter(samplerParameterName, samplerState);
	}

	MHWRender::MTextureManager* textureManager = renderer->getTextureManager();
	if (!textureManager) return;

	char buffer[0x10000];
	MdxTexture::readTextureFromCasc(fileName, buffer);

	MTextureDescription textureDescription;
	textureDescription.setToDefault2DTexture();
	textureDescription.fWidth = 128;
	textureDescription.fHeight = 128;

	MHWRender::MTexture* texture = textureManager->acquireTexture("", textureDescription, buffer, true);

	MHWRender::MTextureAssignment textureAssignment;
	textureAssignment.texture = texture;
	shader.setParameter(mapParameterName, textureAssignment);
	textureManager->releaseTexture(texture);
}