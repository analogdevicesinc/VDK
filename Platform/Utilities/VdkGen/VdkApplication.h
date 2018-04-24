/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

////////////////////////////////////////////////////////////////////////////
//                                     
//  VdkApplication.h
//  CVdkApplication Object Class Definition File
//   
//  The CVdkApplication class implements the main characteristics of the 
//  application we want to generate VDK files for. Directories and type of 
//  application will be here for example.
// 
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __VDKAPPLICATION_H_
#define __VDKAPPLICATION_H_

#include "stdafx.h"
#include "VdkConfiguration.h"
#include "StdString.h"

class VdkApplication {
public:

	VdkApplication() : csOutputType(kMain), configAssociated(NULL), csInputFile(""), 
		csOutputDirectory(""), csRbldFileName(""), csImportFileName("") {}
	int GenerateDependencies();
	void SetFileNames(CStdString inputFile);
	void SetOutputDirectory();
	void SetConfiguration(CVdkConfiguration* inconfig){ configAssociated = inconfig;}
	void SetApplicationType(CVdkConfigType inConfigType) {csOutputType = inConfigType;}
	CVdkConfigType GetOutputType() {return csOutputType;}

	CStdString csRbldFileName;
	CStdString csImportFileName;

private:
	CStdString csInputFile;
	CStdString csOutputDirectory;
	CVdkConfiguration *configAssociated;
	CStdString csProcessorType;
	CVdkConfigType csOutputType;
};

#endif
