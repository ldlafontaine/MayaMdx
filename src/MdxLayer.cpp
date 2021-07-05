#include "MdxLayer.h"

const MTypeId MdxLayer::nodeTypeId(0x70034);
const MString MdxLayer::nodeTypeName("MdxLayer");
const MString MdxLayer::registrantIdString{ "MdxTexture" };
const MString MdxLayer::drawDbClassification{ "drawdb/shader/utility/general/MdxLayer" };
const MString MdxLayer::classification{ "utility/general" };

MObject MdxLayer::filterModeAttr;
MObject MdxLayer::unshadedAttr;
MObject MdxLayer::sphereEnvironmentMapAttr;
MObject MdxLayer::twoSidedAttr;
MObject MdxLayer::unfoggedAttr;
MObject MdxLayer::noDepthTestAttr;
MObject MdxLayer::noDepthSetAttr;
MObject MdxLayer::textureAttr;
MObject MdxLayer::alphaAttr;
MObject MdxLayer::outColorAttr;

void* MdxLayer::creator()
{
	return new MdxLayer();
}

MStatus MdxLayer::initialize()
{
	MFnEnumAttribute enumFn;
	filterModeAttr = enumFn.create("filterMode", "fm");
	enumFn.addField("None", 0);
	enumFn.addField("Transparent", 1);
	enumFn.addField("Blend", 2);
	enumFn.addField("Additive", 3);
	enumFn.addField("Add Alpha", 4);
	enumFn.addField("Modulate", 5);
	enumFn.addField("Modulate 2x", 6);

	MFnNumericAttribute numericFn;

	unshadedAttr = numericFn.create("unshaded", "us", MFnNumericData::Type::kBoolean);
	numericFn.addToCategory("flags");
	sphereEnvironmentMapAttr = numericFn.create("sphereEnvironmentMap", "sem", MFnNumericData::Type::kBoolean);
	numericFn.addToCategory("flags");
	twoSidedAttr = numericFn.create("twoSided", "ts", MFnNumericData::Type::kBoolean);
	unfoggedAttr = numericFn.create("unfogged", "uf", MFnNumericData::Type::kBoolean);
	noDepthTestAttr = numericFn.create("noDepthTest", "ndt", MFnNumericData::Type::kBoolean);
	noDepthSetAttr = numericFn.create("noDepthSet", "nds", MFnNumericData::Type::kBoolean);

	textureAttr = numericFn.createColor("texture", "t");
	numericFn.setStorable(true);

	alphaAttr = numericFn.create("alpha", "a", MFnNumericData::Type::kFloat);
	numericFn.setKeyable(true);

	outColorAttr = numericFn.createColor("outColor", "oc");
	numericFn.setStorable(false);
	numericFn.setWritable(false);

	addAttribute(filterModeAttr);
	addAttribute(unshadedAttr);
	addAttribute(sphereEnvironmentMapAttr);
	addAttribute(twoSidedAttr);
	addAttribute(unfoggedAttr);
	addAttribute(noDepthTestAttr);
	addAttribute(noDepthSetAttr);
	addAttribute(textureAttr);
	addAttribute(alphaAttr);
	addAttribute(outColorAttr);

	attributeAffects(textureAttr, outColorAttr);

	return MS::kSuccess;
}

MStatus MdxLayer::compute(const MPlug& plug, MDataBlock& data)
{
	if ((plug != outColorAttr) && (plug.parent() != outColorAttr)) return MS::kUnknownParameter;

	// Compute output values
	MDataHandle inputData = data.inputValue(textureAttr);
	const MFloatVector& color = inputData.asFloatVector();

	// Set output values
	MDataHandle outColorHandle = data.outputValue(outColorAttr);
	MFloatVector& outColor = outColorHandle.asFloatVector();
	outColor = color;
	outColorHandle.setClean();

	return MS::kSuccess;
}