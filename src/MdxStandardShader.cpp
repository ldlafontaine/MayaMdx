#include "MdxStandardShader.h"

const MTypeId MdxStandardShader::nodeTypeId(0x70033);
const MString MdxStandardShader::nodeTypeName("MdxStandardShader");
const MString MdxStandardShader::registrantIdString{ "MdxStandardShaderRegistrantId" };
const MString MdxStandardShader::drawDbClassification{ "drawdb/shader/surface/MdxStandardShader" };
const MString MdxStandardShader::classification{ "shader/surface:" + drawDbClassification };

MObject MdxStandardShader::priorityPlaneAttr;
MObject MdxStandardShader::constantColorAttr;
MObject MdxStandardShader::sortPrimitivesFarZAttr;
MObject MdxStandardShader::fullResolutionAttr;
MObject MdxStandardShader::shaderAttr;
MObject MdxStandardShader::layersAttr;
MObject MdxStandardShader::outColorAttr;

void* MdxStandardShader::creator()
{
	return new MdxStandardShader();
}

MStatus MdxStandardShader::initialize()
{
	MFnNumericAttribute numericFn;

	priorityPlaneAttr = numericFn.create("priorityPlane", "pp", MFnNumericData::Type::kInt);
	constantColorAttr = numericFn.create("constantColor", "cc", MFnNumericData::Type::kBoolean);
	sortPrimitivesFarZAttr = numericFn.create("sortPrimitivesFarZ", "sp", MFnNumericData::Type::kBoolean);
	fullResolutionAttr = numericFn.create("fullResolution", "fr", MFnNumericData::Type::kBoolean);

	MFnTypedAttribute typedFn;

	shaderAttr = typedFn.create("shader", "s", MFnData::Type::kString);

	layersAttr = numericFn.createColor("layers", "l");
	numericFn.setArray(true);
	numericFn.setAffectsAppearance(true);

	outColorAttr = numericFn.createColor("outColor", "oc");
	numericFn.setStorable(false);
	numericFn.setWritable(false);
	numericFn.setAffectsAppearance(true);

	addAttribute(priorityPlaneAttr);
	addAttribute(constantColorAttr);
	addAttribute(sortPrimitivesFarZAttr);
	addAttribute(fullResolutionAttr);
	addAttribute(shaderAttr);
	addAttribute(layersAttr);
	addAttribute(outColorAttr);

	attributeAffects(layersAttr, outColorAttr);

	return MS::kSuccess;
}

MStatus MdxStandardShader::compute(const MPlug& plug, MDataBlock& data)
{
	if ((plug != outColorAttr) && (plug.parent() != outColorAttr)) return MS::kUnknownParameter;

	// Compute output value
	MArrayDataHandle layersHandle = data.inputArrayValue(layersAttr);
	unsigned int layersCount{ layersHandle.elementCount() };
	MFloatVector computedColor{ 0.0f, 0.0f, 0.0f };
	for (unsigned int i{ layersHandle.elementIndex() }; i < layersCount; i++) {
		computedColor += layersHandle.inputValue().asFloatVector();
		layersHandle.next();
	}
	if (layersCount > 0) computedColor /= static_cast<float>(layersHandle.elementCount());
	//std::cout << "Color: { " << computedColor.x << ", " << computedColor.y << ", " << computedColor.z << " }." << std::endl;

	// Set output values
	MDataHandle outColorHandle = data.outputValue(outColorAttr);
	outColorHandle.setMFloatVector(computedColor);
	outColorHandle.setClean();

	return MS::kSuccess;
}

MdxStandardShaderOverride::MdxStandardShaderOverride(const MObject& obj)
	: MHWRender::MPxShaderOverride(obj)
	, colorShaderInstance(NULL)
	, nonTexturedColorShaderInstance(NULL)
	, transparency(0.0f)
	, samplerState(NULL)
	, fragmentName(MString(""))
	, defaultTexture(NULL) {

	//std::cout << "MdxStandardShaderOverride constructed." << std::endl;

	diffuse[0] = diffuse[1] = diffuse[2] = diffuse[3] = 0.0f;
	specular[0] = specular[1] = specular[2] = 0.0f;
	nonTextured[0] = 0.5f; nonTextured[1] = 0.5f; nonTextured[2] = 0.5f;

	// Create a shader instance to use for drawing
	MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer();
	const MHWRender::MShaderManager* shaderManager = renderer ? renderer->getShaderManager() : NULL;
	if (!shaderManager) return;

	// Create texture fragment shader
	initializeFragmentShaders();
	createCustomMappings();

	if (!colorShaderInstance) {
		colorShaderInstance = shaderManager->getStockShader(MHWRender::MShaderManager::k3dBlinnShader);
		colorShaderInstance->addInputFragment(fragmentName, MString("output"), MString("diffuseColor"));
	}
	if (!nonTexturedColorShaderInstance) {
		nonTexturedColorShaderInstance = shaderManager->getStockShader(MHWRender::MShaderManager::k3dBlinnShader);
		nonTexturedColorShaderInstance->setParameter("diffuseColor", &nonTextured[0]);
	}
}

MdxStandardShaderOverride::~MdxStandardShaderOverride() {
	MHWRender::MRenderer* theRenderer = MHWRender::MRenderer::theRenderer();
	if (theRenderer) {
		const MHWRender::MShaderManager* shaderManager = theRenderer->getShaderManager();
		if (shaderManager) {
			if (colorShaderInstance) shaderManager->releaseShader(colorShaderInstance);
			if (nonTexturedColorShaderInstance) shaderManager->releaseShader(nonTexturedColorShaderInstance);
			colorShaderInstance = NULL;
		}

		if (defaultTexture) {
			MHWRender::MTextureManager* textureManager = theRenderer->getTextureManager();
			if (textureManager) {
				if (defaultTexture) textureManager->releaseTexture(defaultTexture);
				defaultTexture = NULL;
			}
		}
	}
}

MHWRender::MPxShaderOverride* MdxStandardShaderOverride::Creator(const MObject& obj) {
	return new MdxStandardShaderOverride(obj);
}

MString MdxStandardShaderOverride::initialize(const MInitContext& initContext, MInitFeedback& initFeedback) {
	std::cout << "MdxStandardShaderOverride::initialize() execution began." << std::endl;

	if (colorShaderInstance) {
		addShaderSignature(*colorShaderInstance);
	}

	MString empty;
	MHWRender::MVertexBufferDescriptor positionDescriptor(empty, MHWRender::MGeometry::kPosition, MHWRender::MGeometry::kFloat, 3);
	MHWRender::MVertexBufferDescriptor normalDescriptor(empty, MHWRender::MGeometry::kNormal, MHWRender::MGeometry::kFloat, 3);
	MHWRender::MVertexBufferDescriptor textureDescriptor(empty, MHWRender::MGeometry::kTexture, MHWRender::MGeometry::kFloat, 2);

	addGeometryRequirement(positionDescriptor);
	addGeometryRequirement(normalDescriptor);
	addGeometryRequirement(textureDescriptor);

	return MString("MdxStandardShader Override");
}

void MdxStandardShaderOverride::updateDG(MObject object) {
	std::cout << "MdxStandardShaderOverride::updateDG() execution began." << std::endl;

	if (object == MObject::kNullObj) return;

	MFnDependencyNode node(object);

	MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer();
	if (!renderer) return;

	MHWRender::MTextureManager* textureManager = renderer->getTextureManager();
	if (!textureManager) return;

	MPlug layersPlug{ node.findPlug("layers", true) };
	int elementsCount = layersPlug.numElements();
	if (elementsCount > 0) {
		for (int i{ 0 }; i < elementsCount; i++) {
			texturePlugs.append(layersPlug.elementByPhysicalIndex(i));
		}
	}

	name = node.name();
}

void MdxStandardShaderOverride::updateDevice() {
	std::cout << "MdxStandardShaderOverride::updateDevice() execution began." << std::endl;

	if (colorShaderInstance) {
		// Update shader to mark it as drawing with transparency or not.
		colorShaderInstance->setIsTransparent(isTransparent());
		colorShaderInstance->setParameter("specularColor", &specular[0]);

		// Update texture shader
		MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer();

		if (resolvedMapName.length() == 0) {
			const MHWRender::MAttributeParameterMapping* mapParameterMapping = mappings.findByParameterName("map");
			resolvedMapName = mapParameterMapping->resolvedParameterName();
		}

		if (resolvedSamplerName.length() == 0) {
			const MHWRender::MAttributeParameterMapping* samplerParameterMapping = mappings.findByParameterName("textureSampler");
			resolvedSamplerName = samplerParameterMapping->resolvedParameterName();
		}

		// Set the parameters
		if (resolvedMapName.length() > 0 && resolvedSamplerName.length() > 0) {

			MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer();
			if (!renderer) return;

			MHWRender::MTextureManager* textureManager = renderer->getTextureManager();
			if (!textureManager) return;

			// Set sampler to linear-wrap
			if (!samplerState) {
				MHWRender::MSamplerStateDesc samplerStateDesc;
				samplerStateDesc.filter = MHWRender::MSamplerState::kAnisotropic;
				samplerStateDesc.maxAnisotropy = 16;
				samplerState = MHWRender::MStateManager::acquireSamplerState(samplerStateDesc);
			}
			if (samplerState) {
				colorShaderInstance->setParameter(resolvedSamplerName, *samplerState);
			}

			// Acquire textures from layers and process
			MImage image;
			unsigned int imageWidth;
			unsigned int imageHeight;

			if (texturePlugs.length() > 0) {
				MPlug texturePlug{ texturePlugs[0] };
				if (texturePlug.isDestination()) {
					// Calling MTextureManager::acquireTexture() with an MPlug as a parameter uses the texture produced by the node's compute function.
					// The output of its override's updateShader() function is not used.
					MHWRender::MTexture* textureFromPlug = textureManager->acquireTexture("", texturePlug, 512, 512);

					if (textureFromPlug) {
						int rowPitch = 0;
						size_t slicePitch = 0;
						unsigned char* pixelData = (unsigned char*)textureFromPlug->rawData(rowPitch, slicePitch);
						image.setPixels(pixelData, 512, 512);
						image.getSize(imageWidth, imageHeight);
						image.verticalFlip();
						MTexture::freeRawData(pixelData);

						MHWRender::MTextureDescription textureDescription;
						textureDescription.setToDefault2DTexture();
						textureDescription.fHeight = imageHeight;
						textureDescription.fWidth = imageWidth;
						MHWRender::MTexture* texture = textureManager->acquireTexture("", textureDescription, image.pixels(), false);
						image.release();

						// Assign texture
						if (texture) {
							assignTexture(texture, textureManager);
							textureManager->releaseTexture(texture);
						}
					}
				}
			}
			else {
				unsigned char defaultImagePixelData[4];
				for (int i = 0; i < 4; ++i) {
					defaultImagePixelData[i] = (unsigned char)(diffuse[i] * 255);
				}
				image.setPixels(defaultImagePixelData, 1, 1);

				if (!defaultTexture) {
					MHWRender::MTextureDescription defaultTextureDescription;
					defaultTextureDescription.setToDefault2DTexture();
					defaultTextureDescription.fHeight = 1;
					defaultTextureDescription.fWidth = 1;
					defaultTexture = textureManager->acquireTexture("", defaultTextureDescription, defaultImagePixelData, false);
				}
				else {
					defaultTexture->update(image, false);
				}
				if (defaultTexture) {
					// It is cached by OGS, update to make sure texture has been updated.								
					assignTexture(defaultTexture, textureManager);
				}
			}
		}
	}

	if (nonTexturedColorShaderInstance) {
		nonTexturedColorShaderInstance->setParameter("diffuseColor", &nonTextured[0]);
	}
}

MHWRender::MShaderInstance* MdxStandardShaderOverride::shaderInstance(MDrawContext& drawContext) const {
	return colorShaderInstance;
}

void MdxStandardShaderOverride::activateKey(MHWRender::MDrawContext& context, const MString& key) {
	colorShaderInstance->bind(context);
}

void MdxStandardShaderOverride::terminateKey(MHWRender::MDrawContext& context, const MString& key) {
	colorShaderInstance->unbind(context);
}

// Use custom shader with custom blend state if required for transparency handling.
bool MdxStandardShaderOverride::draw(MHWRender::MDrawContext& context, const MHWRender::MRenderItemList& renderItemList) const {

	MHWRender::MStateManager* stateManager = context.getStateManager();

	// initialize MdxStandardShader blend state once
	if (blendState == NULL) {
		MHWRender::MBlendStateDesc blendStateDesc;

		for (int i = 0; i < (blendStateDesc.independentBlendEnable ? MHWRender::MBlendState::kMaxTargets : 1); ++i) {
			blendStateDesc.targetBlends[i].blendEnable = true;
			blendStateDesc.targetBlends[i].sourceBlend = MHWRender::MBlendState::kSourceAlpha;
			blendStateDesc.targetBlends[i].destinationBlend = MHWRender::MBlendState::kInvSourceAlpha;
			blendStateDesc.targetBlends[i].blendOperation = MHWRender::MBlendState::kAdd;
			blendStateDesc.targetBlends[i].alphaSourceBlend = MHWRender::MBlendState::kOne;
			blendStateDesc.targetBlends[i].alphaDestinationBlend = MHWRender::MBlendState::kInvSourceAlpha;
			blendStateDesc.targetBlends[i].alphaBlendOperation = MHWRender::MBlendState::kAdd;
		}

		blendStateDesc.blendFactor[0] = 1.0f;
		blendStateDesc.blendFactor[1] = 1.0f;
		blendStateDesc.blendFactor[2] = 1.0f;
		blendStateDesc.blendFactor[3] = 1.0f;

		blendState = stateManager->acquireBlendState(blendStateDesc);
	}

	// Save old blend state
	const MHWRender::MBlendState* pOldBlendState = stateManager->getBlendState();

	bool needBlending = false;
	if (transparency > 0.0f) {
		needBlending = true;
		stateManager->setBlendState(blendState);
	}

	// Activate all the shader passes and draw using internal draw methods.
	unsigned int passCount = colorShaderInstance->getPassCount(context);
	for (unsigned int i = 0; i < passCount; i++) {
		colorShaderInstance->activatePass(context, i);
		MHWRender::MPxShaderOverride::drawGeometry(context);
	}

	// Restore blend state
	if (needBlending) {
		stateManager->setBlendState(pOldBlendState);
	}
	return true;
}

MHWRender::DrawAPI MdxStandardShaderOverride::supportedDrawAPIs() const {
	return (MHWRender::kOpenGL | MHWRender::kDirectX11 | MHWRender::kOpenGLCoreProfile);
}

bool MdxStandardShaderOverride::isTransparent() {
	return (transparency > 0.0f);
}

MHWRender::MShaderInstance* MdxStandardShaderOverride::nonTexturedShaderInstance(bool& monitor) const {
	if (nonTexturedColorShaderInstance) {
		monitor = true;
		return nonTexturedColorShaderInstance;
	}
	return NULL;
}

bool MdxStandardShaderOverride::overridesDrawState() {
	return true;
}

void MdxStandardShaderOverride::initializeFragmentShaders() {
	static const MString sFragmentName("fileTexturePluginFragment");
	static const char* sFragmentBody =
		"<fragment uiName=\"fileTexturePluginFragment\" name=\"fileTexturePluginFragment\" type=\"plumbing\" class=\"ShadeFragment\" version=\"1.0\">"
		"	<description><![CDATA[Simple file texture fragment]]></description>"
		"	<properties>"
		"		<float2 name=\"uvCoord\" semantic=\"mayaUvCoordSemantic\" flags=\"varyingInputParam\" />"
		"		<texture2 name=\"map\" />"
		"		<sampler name=\"textureSampler\" />"
		"	</properties>"
		"	<values>"
		"	</values>"
		"	<outputs>"
		"		<float4 name=\"output\" />"
		"	</outputs>"
		"	<implementation>"
		"	<implementation render=\"OGSRenderer\" language=\"Cg\" lang_version=\"2.100000\">"
		"		<function_name val=\"fileTexturePluginFragment\" />"
		"		<source><![CDATA["
		"float4 fileTexturePluginFragment(float2 uv, texture2D map, sampler2D mapSampler) \n"
		"{ \n"
		"	uv -= floor(uv); \n"
		"	uv.y = 1.0f - uv.y; \n"
		"	float4 color = tex2D(mapSampler, uv); \n"
		"	return color.rgba; \n"
		"} \n]]>"
		"		</source>"
		"	</implementation>"
		"	<implementation render=\"OGSRenderer\" language=\"HLSL\" lang_version=\"11.000000\">"
		"		<function_name val=\"fileTexturePluginFragment\" />"
		"		<source><![CDATA["
		"float4 fileTexturePluginFragment(float2 uv, Texture2D map, sampler mapSampler) \n"
		"{ \n"
		"	uv -= floor(uv); \n"
		"	uv.y = 1.0f - uv.y; \n"
		"	float4 color = map.Sample(mapSampler, uv); \n"
		"	return color.rgba; \n"
		"} \n]]>"
		"		</source>"
		"	</implementation>"
		"	<implementation render=\"OGSRenderer\" language=\"GLSL\" lang_version=\"3.0\">"
		"		<function_name val=\"fileTexturePluginFragment\" />"
		"		<source><![CDATA["
		"vec4 fileTexturePluginFragment(vec2 uv, sampler2D mapSampler) \n"
		"{ \n"
		"	uv -= floor(uv); \n"
		"	uv.y = 1.0f - uv.y; \n"
		"	vec4 color = texture(mapSampler, uv); \n"
		"	return color.rgba; \n"
		"} \n]]>"
		"		</source>"
		"	</implementation>"
		"	</implementation>"
		"</fragment>";

	MHWRender::MRenderer* theRenderer = MHWRender::MRenderer::theRenderer();
	if (theRenderer) {
		MHWRender::MFragmentManager* fragmentManager = theRenderer->getFragmentManager();
		if (fragmentManager) {
			// Add fragments if needed
			bool fragmentAdded = fragmentManager->hasFragment(sFragmentName);
			if (!fragmentAdded) fragmentAdded = (sFragmentName == fragmentManager->addShadeFragmentFromBuffer(sFragmentBody, false));
			if (fragmentAdded) fragmentName = sFragmentName;
		}
	}
}

void MdxStandardShaderOverride::createCustomMappings() {
	MHWRender::MAttributeParameterMapping mapMapping("map", "", false, true);
	mappings.append(mapMapping);

	MHWRender::MAttributeParameterMapping textureSamplerMapping("textureSampler", "", false, true);
	mappings.append(textureSamplerMapping);
}

void MdxStandardShaderOverride::assignTexture(MHWRender::MTexture* texture, MHWRender::MTextureManager* textureManager) {
	MHWRender::MTextureAssignment textureAssignment;
	textureAssignment.texture = texture;
	colorShaderInstance->setParameter(resolvedMapName, textureAssignment);
}

const MHWRender::MBlendState* MdxStandardShaderOverride::blendState = NULL;