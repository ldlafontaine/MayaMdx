#include "SequenceTranslator.h"

#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>
#include <maya/MTime.h>
#include <maya/MString.h>

SequenceTranslator::SequenceTranslator(MObject dependencyNode) :
	ModelObjectTranslator{ dependencyNode } {
}

SequenceTranslator::SequenceTranslator(MdxParser::Sequence parserSequence) :
	parserSequence{ parserSequence } {
}

MdxParser::Sequence SequenceTranslator::getParserSequence() const {
	return parserSequence;
}

void SequenceTranslator::createTimeSliderBookmark() {
	uint32_t startFrame{ parserSequence.getStartFrame() };
	uint32_t endFrame{ parserSequence.getEndFrame() };

	MFnDependencyNode dependFn;
	dependFn.create("timeSliderBookmark");
	MPlug namePlug{ dependFn.findPlug("name", true) };
	MPlug startPlug{ dependFn.findPlug("timeRangeStart", true) };
	MPlug stopPlug{ dependFn.findPlug("timeRangeStop", true) };

	namePlug.setString(parserSequence.getName().c_str());
	startPlug.setMTime(MTime{ static_cast<double>(startFrame) });
	stopPlug.setMTime(MTime{ static_cast<double>(endFrame) });
}

void SequenceTranslator::writeSequenceToParser() {
	MFnDependencyNode dependFn{ dependencyNode };
	MPlug namePlug{ dependFn.findPlug("name", true) };
	MPlug timeRangeStartPlug{ dependFn.findPlug("timeRangeStart", true) };
	MPlug timeRangeStopPlug{ dependFn.findPlug("timeRangeStop", true) };


	const char* nameValue{ namePlug.asString().asChar() };
	double timeRangeStartValue{ timeRangeStartPlug.asMTime().value() };
	double timeRangeStopValue{ timeRangeStopPlug.asMTime().value() };

	parserSequence.setName(nameValue);
	parserSequence.setStartFrame(static_cast<uint32_t>(timeRangeStartValue));
	parserSequence.setEndFrame(static_cast<uint32_t>(timeRangeStopValue));
}