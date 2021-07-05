#include "ModelObjectTranslator.h"

#include <maya/MFnAnimCurve.h>
#include <maya/MItDependencyGraph.h>

ModelObjectTranslator::ModelObjectTranslator() {
}

ModelObjectTranslator::ModelObjectTranslator(MObject dependencyNode) :
	dependencyNode{ dependencyNode } {
}

MObject ModelObjectTranslator::getDependencyNode() const {
	return dependencyNode;
}

MFnAnimCurve::TangentType ModelObjectTranslator::getTangentType(uint32_t parsedInterpolationType) {
	MFnAnimCurve::TangentType tangentType{ MFnAnimCurve::TangentType::kTangentLinear };
	if (parsedInterpolationType > 1) {
		MFnAnimCurve::TangentType tangentType{ MFnAnimCurve::TangentType::kTangentSmooth };
	}
	return tangentType;
}

void ModelObjectTranslator::getKeyframeTimes(std::vector<MPlug> plugs, std::set<int32_t>& keyframeTimes, std::vector<MObject>& animCurves) {
	for (MPlug plug : plugs) {
		MItDependencyGraph graphIter{ plug, MFn::Type::kAnimCurve, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst };
		for (; !graphIter.isDone(); graphIter.next()) {
			MFnAnimCurve animCurve{ graphIter.currentItem() };
			for (unsigned int keyIndex{ 0 }; keyIndex < animCurve.numKeyframes(); keyIndex++) {
				MTime time{ animCurve.time(keyIndex) };
				keyframeTimes.insert(static_cast<int32_t>(time.value()));
			}
			animCurves.push_back(graphIter.currentItem());
		}
	}
}