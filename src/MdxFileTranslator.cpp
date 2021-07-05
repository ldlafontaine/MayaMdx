#include "MdxFileTranslator.h"

#include "./translators/ModelTranslator.h"

#include <maya/MArgList.h>
#include <maya/MFileObject.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MPxFileTranslator.h>

#include <iostream>

MdxFileTranslator::MdxFileTranslator(void)
{
	bool m_bShort = false;
}

MdxFileTranslator::~MdxFileTranslator(void)
{
}

MStatus	MdxFileTranslator::reader(const MFileObject& file, const MString& options, FileAccessMode mode)
{
	std::cout << "MdxFileTranslator::reader() beginning execution." << std::endl;
	ModelTranslator modelTranslator;
	modelTranslator.readModelFromFile(file, options);
	std::cout << "MdxFileTranslator::reader() execution completed." << std::endl;
	return MS::kSuccess;
}

MStatus	MdxFileTranslator::writer(const MFileObject& file, const MString& options, FileAccessMode mode)
{
	std::cout << "MdxFileTranslator::writer() beginning execution." << std::endl;
	ModelTranslator modelTranslator;
	modelTranslator.writeModelToFile(file, options);
	std::cout << "MdxFileTranslator::writer() execution completed." << std::endl;
	return MS::kSuccess;
}

bool MdxFileTranslator::haveReadMethod() const {
	return true;
}

bool MdxFileTranslator::haveWriteMethod() const {
	return true;
}

MString MdxFileTranslator::defaultExtension() const {
	return "mdx";
}

MPxFileTranslator::MFileKind MdxFileTranslator::identifyFile(const MFileObject& fileName,
	const char* buffer,
	short size) const {
	const char* str = fileName.resolvedName().asChar();
	unsigned int len = fileName.resolvedName().length();
	if (str[len - 3] == 'm' &&
		str[len - 2] == 'd' &&
		str[len - 1] == 'x')
	{
		return kCouldBeMyFileType;
	}
	return kNotMyFileType;
}

void* MdxFileTranslator::creator() {
	return new MdxFileTranslator;
}