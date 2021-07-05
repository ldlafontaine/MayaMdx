#include "DagNodeComparator.h"

#include <maya/MFnDagNode.h>
#include <maya/MString.h>

#include <string>

bool DagNodeComparator::operator()(const MObject& a, const MObject& b) const {
	MFnDagNode dagFn{ a };
	std::string aPathName{ dagFn.fullPathName().asChar() };
	dagFn.setObject(b);
	std::string bPathName{ dagFn.fullPathName().asChar() };
	return aPathName < bPathName;
};