#include <maya/MFileIO.h>
#include <maya/MFileObject.h>
#include <maya/MObject.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MPxFileTranslator.h>

#pragma once

class MdxFileTranslator: public MPxFileTranslator {

public:
	MdxFileTranslator(void);
	~MdxFileTranslator(void);
	MStatus	reader(const MFileObject& file, const MString& optionsString, FileAccessMode mode);
	MStatus writer(const MFileObject& file, const MString& optionsString, FileAccessMode mode);
	bool haveReadMethod() const;
	bool haveWriteMethod() const;
	MString defaultExtension() const;
	MFileKind identifyFile(const MFileObject& fileName, const char* buffer, short size) const;
	static void* creator();

private:
	bool m_bShort;
};