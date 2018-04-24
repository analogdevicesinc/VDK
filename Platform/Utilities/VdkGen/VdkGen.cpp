/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

///////////////////////////////////////////////////////////////////////////////
//                                     
//  VdkGen.cpp
//   
//  Defines the entry point for the console application: vdkgen.exe
// 

//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vdkgen.h"
#include "vdkprojectwnd.h"
#include "VdkConfiguration.h"
#include "VdkApplication.h"
#include "StdString.h"

using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// contants and globals
///////////////////////////////////////////////////////////////////////////////

extern CVdkConfiguration* g_VdkData, * g_first_config_file;
extern CVdkPlatformInfo g_VdkPlatformInfo;
extern CStdString g_VdspInstallDir;
extern vector <int>					   g_NumberOfNodes;
char    szFullPath[MAX_PATH];
bool g_suppress_warnings=false;

VdkApplication g_Project;
CStdString VdkGenVersion;
CStdString VdkGenCompany;
CStdString VdkGenCopyright;

char* options_list[] = {
	"-help", "Displays option's list",
	"-initial_configuration", "Indicates that the .vdk file is not complete yet so there is no error checking",
	"-lib", "Generates import file that should be given to customers when generating a library. Does not generate VDK.cpp",
	"-M", "Generates dependencies for VDK.cpp and VDK.h",
	"-MM", "Same as -M but will continue with the generation of the files",
	"-proc ProcessorName", "Selects the processor VDK.cpp and VDK.h are generated for",
	"-version", "Prints out VdkGen version number",
	0

};

char* private_options_list[] = {
	"-o DirName", "Selects the directory where VDK.cpp and VDK.h will be generated",
	"++VDSP_4.0", "Indicates that the VDSP information will be looked for in the 4.0 location",
	"++VDSP_4.5", "Indicates that the VDSP information will be looked for in the 4.5 location",
	"++VDSP_5.0", "Indicates that the VDSP information will be looked for in the 5.0 location",
	"++install_dir DirName", "Specifies the directory where VDSP is installed",
	"++vdkgen_for_rt", "Indicates that VdkGen is not in the VDSP install directory",
	0

};

bool vdkgen_for_rt = false;

void PrintOptions(){
	cout << "Usage: VdkGen [options] FileName.vdk" <<endl;
	cout << endl;
	cout << "Options may be interspersed with filename in command line" <<endl;
	cout <<endl;

	int i=0;
	while (options_list[i]!=0) {
		cout << options_list[i++] << endl;
		cout << "    " << options_list[i++] <<endl;
	}


	//display the private options if we are using it ourselves
	if (vdkgen_for_rt) {
		i=0;
		while (private_options_list[i]!=0) {
			cout << private_options_list[i++] << endl;
			cout << "    " << private_options_list[i++] <<endl;
		}
	}
}

void ConfigurationCleanup(){
	int configType=kImport;
	CVdkConfiguration* config = g_first_config_file, *next_config;
	
	while (configType!= kEnd) {
		//go to the last of this type of configuration (
		while (config->GetNext((CVdkConfigType)configType))
			config = config->GetNext((CVdkConfigType)configType);
		// delete and move backwards to the first one
		while (config!= g_first_config_file)
		{
			next_config = config->GetPrev((CVdkConfigType)configType);
			delete config;
			config = next_config;
		}

		(configType)++;
	}
	//delete the original one now
	delete g_first_config_file;
}

void CheckLibrariesConsistency() {
	CStdString csValue;
	// check that the timer interrupt is the same for all of the libraries
	CStdString csInitialTimer;
	g_first_config_file->GetSystemData().GetProperty("Timer Interrupt", csInitialTimer);

	for (CVdkConfiguration* config=g_first_config_file;config;config=config->GetNextLibrary())
	{
		if (config->GetSystemData().GetProperty("Timer Interrupt", csValue))
			if (csValue != csInitialTimer)
					VDKGEN_ERROR_NO_FILE((LPCSTR)("Timer in library " + config->GetFileName() + " not consistant with project"));
	}
}
void CheckImportsConsistency(){
	// fill in the number of nodes we are meant to have
	for (unsigned int i=0; i<g_first_config_file->GetNumRoutingNodes() ; i++) {
		g_NumberOfNodes.push_back(i);
	}

	for (CVdkConfiguration* config=g_first_config_file;config;config=config->GetNextImport())
		config->CheckConsistencyWithMaster();

	// check there are no nodes left. If there are, issue an error
	if (!g_NumberOfNodes.empty()) {
		VDKGEN_ERROR_NO_FILE("The following routing nodes were not specified:");
		cerr<<"              ";
		for ( vector<int>::iterator theIterator = g_NumberOfNodes.begin(); 
			theIterator != g_NumberOfNodes.end(); theIterator++)
		{
			cerr << "RoutingNode"<<*theIterator;
			if (theIterator != g_NumberOfNodes.end() -1 ) cout << ", ";
			else cerr <<endl;
		}
		cerr<<"              The missing imports for the specified nodes are required"<<endl;


	}
}

///////////////////////////////////////////////////////////////////////////////
// int main( int, TCHAR*[], TCHAR*[] )
///////////////////////////////////////////////////////////////////////////////

int main( int argc, TCHAR* argv[], TCHAR* envp[] )
{
	CStdString csExecutableName = argv[0];
	CStdString csProcType  = argv[1];
	CStdString csInputFile = argv[2];
	CStdString csOutputPath;		// directory where VDK.cpp VDK.h and rbld files will
								// go. For dependencies might be a relative path but
								// will be converted onto full path

	DWORD dwVerInfoSize;
	DWORD   dwVerHnd=0;        // An 'ignored' parameter, always '0'
	LPSTR   lpVersion;         // String pointer to 'version' text
	LPSTR   lpCopyright;
	LPSTR   lpCompanyName;
	UINT    uVersionLen;

	// get the full path of the executable we are running
	GetModuleFileName (NULL, szFullPath, sizeof(szFullPath));

	// Get the version information:
	dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd);
	if (dwVerInfoSize) {
		LPSTR   lpstrVffInfo;
 		lpstrVffInfo = (char*) malloc (dwVerInfoSize);
		GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpstrVffInfo);
		// the hex number comes from the resource file (block header) 
		char strVersionField[100];
		sprintf(strVersionField, "\\StringFileInfo\\040904b0\\FileVersion");
		char strCopyRightField[100];
		sprintf(strCopyRightField, "\\StringFileInfo\\040904b0\\LegalCopyright");
		char strCompanyField[100];
		sprintf(strCompanyField, "\\StringFileInfo\\040904b0\\CompanyName");
		if (VerQueryValue((LPVOID)lpstrVffInfo,(LPSTR) strVersionField, (LPVOID *)&lpVersion,
			(UINT *)&uVersionLen)) 
		{
			// give the version a nicer look (replace , with . and remove white 
			// spaces
			VdkGenVersion = lpVersion;
			VdkGenVersion.Remove(' ');
			VdkGenVersion.Replace(",",".");
			VerQueryValue((LPVOID)lpstrVffInfo,(LPSTR) strCopyRightField, (LPVOID *)&lpCopyright,
					(UINT *)&uVersionLen); 
			VerQueryValue((LPVOID)lpstrVffInfo,(LPSTR) strCompanyField, (LPVOID *)&lpCompanyName,
					(UINT *)&uVersionLen);
			VdkGenVersion = lpVersion;
			VdkGenVersion.Remove(' ');
			VdkGenVersion.Replace(",",".");
			VdkGenCopyright = lpCopyright;
			VdkGenCompany = lpCompanyName;
			
 		}
		free(lpstrVffInfo);
	}
	
	CStdString csOutputOptionDir; // the directory we get in the command line with the 
								// -o option
	bool generate_dependencies =false, generate_files = true;

	for  (int i=1; i < argc;i++)
	{
		if (!strcmp (argv[i],"-proc"))
		{
			if ( i == argc - 1) {
				cerr <<"VdkGen: No argument specified for -proc option"<<endl;
				return (1);
			}
			csProcType = argv[++i];
		}
		else if ( !strcmp(argv[i] ,"-MM") || !strcmp(argv[i] , "-M") ) {
			generate_dependencies = true;
			if (!strcmp(argv[i] , "-M")) generate_files = false;
		} 
		else if (!strcmp (argv[i],"-si-revision"))
		{
			if ( i == argc - 1) {
				cerr <<"VdkGen: No argument specified for -si-revision option"<<endl;
				return (1);
			}
			g_VdkPlatformInfo.SetSiliconRevision(argv[++i]);
		}
		else if ( !strcmp( argv[i] + strlen(argv[i]) - 4, ".vdk")) csInputFile = argv[i];
		else if ( !strcmp(argv[i],"++vdkgen_for_rt")) { vdkgen_for_rt = true; g_suppress_warnings=true; }
		else if ( !strcmp(argv[i],"-w")) g_suppress_warnings=true;
		else if ( !strcmp(argv[i],"++VDSP_4.0")) g_VdkPlatformInfo.SetVDSPVersion("4.0");
		else if ( !strcmp(argv[i],"++VDSP_4.5")) g_VdkPlatformInfo.SetVDSPVersion("4.5");
		else if ( !strcmp(argv[i],"++VDSP_5.0")) g_VdkPlatformInfo.SetVDSPVersion("5.0");
		else if ( !strcmp(argv[i],"-initial_configuration")) g_VdkPlatformInfo.SetInitConfig(true);
		else if ( !strcmp(argv[i],"++install_dir")) 	g_VdspInstallDir = argv[++i];
		else if ( !strcmp(argv[i],"-o")) csOutputOptionDir = argv[++i];
		else if ( !strcmp(argv[i],"-help")) { PrintOptions(); return(0);}
		else if ( !strcmp(argv[i],"-lib" )) {g_Project.SetApplicationType(kLibrary);}

		else if ( !strcmp(argv[i],"-version")) { 
					cerr << "Analog Devices VdkGen Version " << (LPCSTR) VdkGenVersion <<endl;
					cerr << (LPCSTR) VdkGenCopyright << " " << (LPCSTR) VdkGenCompany << " All rights reserved." <<endl;
		}

		else if (i == 1) csProcType  = argv[1];
		else {
			cerr <<"VdkGen: Wrong command line option " << argv[i] <<endl;
			return (1);
		}
	}

	if (csProcType.IsEmpty() || csInputFile.IsEmpty()) {
		cerr <<"VdkGen:Wrong command line arguments specified"<< endl;
		return (1);
	}

	// we no longer support 0.x silicon revision for danube so we don't go any further if that is the revision selected
	if (csProcType == "ADSP-TS201" || csProcType == "ADSP-TS202" || csProcType == "ADSP-TS203") 
	{
			CStdString  SiRev = g_VdkPlatformInfo.GetSiliconRevision();
			if ( (atof((LPCSTR) SiRev) < 1) && SiRev.CompareNoCase("automatic") !=0 
				&& SiRev.CompareNoCase("none") != 0 && SiRev.CompareNoCase("any") != 0) {
				VDKGEN_ERROR_NO_FILE((LPCSTR)( "VDK does not support " + csProcType + " silicon revision " + SiRev ));
				return(1);
			}
	}

	if( !g_VdkPlatformInfo.FInit(csProcType) )
	{
		const char *psString = csProcType;
		cerr << "VdkGen: Failed to initialize platform information for processor type: " << psString << endl;
		return( 1 );
	}

	// find the name of the rbld file to generate so we can put it in the dependencies
	// and in the file generation

	//get the file name without the path
	g_Project.SetFileNames(csInputFile);
	char ext[10], fname[100],drive[10],dir[MAX_PATH],szOutputPath[MAX_PATH];
	_splitpath((LPCSTR) csInputFile,drive, dir, fname, ext );

	if (csOutputOptionDir != "") { 
		// there was a -o option so the files will be generated in that directory
		if (csOutputOptionDir.Right(1) != "\\")
			csOutputOptionDir += "\\";
		csOutputPath = csOutputOptionDir ;
	}
	else {
		// the files will be generated in the same directory as the .vdk file.
		// At this point this path will be relative if this was what we had in the
		//command line
		_makepath(szOutputPath,drive,dir,"","");
		csOutputPath = szOutputPath;
	}

	// we don't want the dependencies with the full path unless it was specified in
	// the command line

	if( PathIsRelative(csInputFile) )
	{
		TCHAR szDirectory[MAX_PATH];

		if( !GetCurrentDirectory(sizeof(szDirectory), szDirectory) )
		{
			cerr << "VdkGen: Failed to get current directory." << endl;
			return( 1 );
		}

		csInputFile = szDirectory + CStdString("\\") + csInputFile;
		// if there was no -o option we get the full path so we know where
		// to generate the files. If there was a -o flag this is already set
		if (csOutputOptionDir == "")
			csOutputPath = szDirectory + CStdString("\\");
	}

	g_first_config_file = new CVdkConfiguration;
	g_Project.SetConfiguration(g_first_config_file);
	g_VdkData = g_first_config_file;

	if( !g_VdkData->Load(csInputFile) )
	{
		const char *psString = csInputFile;
		cerr << "VdkGen: Failed to load the file: " << psString << endl;
		ConfigurationCleanup();
		return( 1 );
	}

	// we don't want the dependencies with the full path unless it was specified in
	// the command line so the information in g_Project is relative if that was the way
	// specified in the command line

	if (generate_dependencies) 	g_Project.GenerateDependencies();

	// checking that all the imports are consistant. This function will give
	// an error if it finds any problem so we don't need any other code here

	CheckImportsConsistency();
	
//	CheckLibrariesConsistency();

	if (error_found) {
		cerr << "              VDK.cpp and VDK.h will not be generated" << endl; 
		ConfigurationCleanup();
		return (1);
	}
	
	// if we don't need to generate any files we are done
	if (!generate_files) 
			return (0); 

	if (g_Project.GetOutputType() == kLibrary) // when we generate libraries we don't need VDK.cpp added to the project
	{
		if (!GenerateLibraryImportFile(csOutputPath  + g_Project.csImportFileName ) )
		{
			cerr << "VdkGen: Failed to generate import file." << endl;
			ConfigurationCleanup();
			return( 1 );
		}
	}
	else
	{
		if( !GenerateConfigSource(csOutputPath) )
		{
			cerr << "VdkGen: Failed to generate config source file." << endl;
			ConfigurationCleanup();
			return( 1 );
		}
	}

	if( !GenerateConfigHeader(csOutputPath) )
	{
		cerr << "VdkGen: Failed to generate config header file." << endl;
		ConfigurationCleanup();
		return( 1 );
	}

	if ( !GenerateRbldFile( (LPCSTR) (csOutputPath + g_Project.csRbldFileName) ))
	{
		cerr << "VdkGen: Failed to generate VDK files." << endl;
		ConfigurationCleanup();
		return( 1 );
	}

	ConfigurationCleanup();

	return 0;
}


