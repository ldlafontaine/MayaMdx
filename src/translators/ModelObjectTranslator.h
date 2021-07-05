#pragma once

#include <MdxParser/Node.h>
#include <maya/MObject.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MPlug.h>

#include <stdint.h>
#include <set>

class ModelObjectTranslator
{
public:
	ModelObjectTranslator();
	ModelObjectTranslator(MObject dependencyNode);

	MObject getDependencyNode() const;

protected:
	MObject dependencyNode;

	MFnAnimCurve::TangentType getTangentType(uint32_t parsedInterpolationType);
	void getKeyframeTimes(std::vector<MPlug> plugs, std::set<int32_t>& keyframeTimes, std::vector<MObject>& animCurves);
};