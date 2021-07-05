#pragma once

#include <maya/MObject.h>

struct DagNodeComparator
{
	bool operator()(const MObject& a, const MObject& b) const;
};

