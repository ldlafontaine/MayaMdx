#pragma once

#include "ModelObjectTranslator.h"

#include <MdxParser/Sequence.h>
#include <maya/MObject.h>

class SequenceTranslator : public ModelObjectTranslator
{
public:
	SequenceTranslator(MObject dependencyNode);
	SequenceTranslator(MdxParser::Sequence parserSequence);

	MdxParser::Sequence getParserSequence() const;

	void createTimeSliderBookmark();

	void writeSequenceToParser();

protected:
	MdxParser::Sequence parserSequence;
};