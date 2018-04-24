/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#include "stdafx.h"

#include "VdkApplication.h"
#include "StdString.h"

void VdkApplication::SetFileNames(CStdString inInputFile) {
	csInputFile = inInputFile;
	//get the file name without the path
	char ext[10], fname[100],drive[10],dir[MAX_PATH];
	_splitpath((LPCSTR) csInputFile,drive, dir, fname, ext );
	csRbldFileName = fname + CStdString( ".rbld");
	csImportFileName = fname + CStdString( ".imp");
}

int VdkApplication::GenerateDependencies() {
	unsigned int count;
		if (csOutputType != kLibrary) // for library generation we don't have VDK.cpp
			cout << (LPCSTR)csOutputDirectory << "VDK.h: $(VDSP)\\"<<(LPCSTR) (g_VdkPlatformInfo.GetRelativeTemplatePath()) <<"VDK.cpp.tf "<<endl;
		cout << (LPCSTR)csOutputDirectory << "VDK.h: $(VDSP)\\"<<(LPCSTR)(g_VdkPlatformInfo.GetRelativeTemplatePath())<<"VDK.h.tf " << endl;
		cout << (LPCSTR)csOutputDirectory << "VDK.h: $(VDSP)\\"<<(LPCSTR) (g_VdkPlatformInfo.GetRelativeTemplatePath()) <<"VDKGen.exe "<<endl;
		cout << (LPCSTR)csOutputDirectory << "VDK.h: "<< (LPCTSTR) csInputFile << endl;
		cout << (LPCSTR)csOutputDirectory << "VDK.h: "<< (LPCTSTR) (csOutputDirectory + csRbldFileName) << endl;

		for (count=0; count < configAssociated->GetNumImports(); count++) {
			cout << (LPCSTR)csOutputDirectory << "VDK.h: "<< (LPCTSTR) (configAssociated->GetImport(count)) << endl;
		}

		if (csOutputType != kLibrary) // for library generation we don't have VDK.cpp
		{
			cout << (LPCSTR)csOutputDirectory << "VDK.cpp: $(VDSP)\\"<<(LPCSTR) (g_VdkPlatformInfo.GetRelativeTemplatePath()) <<"VDK.cpp.tf "<<endl;
			cout << (LPCSTR)csOutputDirectory << "VDK.cpp: $(VDSP)\\"<<(LPCSTR)(g_VdkPlatformInfo.GetRelativeTemplatePath())<<"VDK.h.tf " << endl;
			cout << (LPCSTR)csOutputDirectory << "VDK.cpp: $(VDSP)\\"<<(LPCSTR) (g_VdkPlatformInfo.GetRelativeTemplatePath()) <<"VDKGen.exe "<<endl;
			cout << (LPCSTR)csOutputDirectory << "VDK.cpp: "<< (LPCTSTR) csInputFile << endl;
			cout << (LPCSTR)csOutputDirectory << "VDK.cpp: "<< (LPCTSTR) (csOutputDirectory + csRbldFileName) << endl;
			for (count=0; count < configAssociated->GetNumImports(); count++) {
				cout << (LPCSTR)csOutputDirectory << "VDK.cpp: "<< (LPCTSTR) (configAssociated->GetImport(count)) << endl;
			}
		}

		if (csOutputType != kLibrary) // for library generation we don't have VDK.cpp
			cout << (LPCSTR) (csOutputDirectory + csRbldFileName) <<": $(VDSP)\\"<<(LPCSTR) (g_VdkPlatformInfo.GetRelativeTemplatePath()) <<"VDK.cpp.tf "<<endl;
		cout << (LPCSTR) (csOutputDirectory + csRbldFileName) <<": $(VDSP)\\"<<(LPCSTR)(g_VdkPlatformInfo.GetRelativeTemplatePath())<<"VDK.h.tf " << endl;
		cout << (LPCSTR) (csOutputDirectory + csRbldFileName) <<": $(VDSP)\\"<<(LPCSTR)(g_VdkPlatformInfo.GetRelativeTemplatePath()) <<"VDKGen.exe "<<endl;
		cout << (LPCSTR) (csOutputDirectory + csRbldFileName) <<": "<< (LPCTSTR) csInputFile << endl;
		cout << (LPCSTR) (csOutputDirectory + csRbldFileName) <<": "<< (LPCTSTR) (csOutputDirectory + csRbldFileName) << endl;
		for (count=0; count < configAssociated->GetNumImports(); count++) {
			cout << (LPCSTR)(csOutputDirectory + csRbldFileName) << ": "<< (LPCTSTR) (configAssociated->GetImport(count)) << endl;
		}
		return 0;


}
