/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif

#define VC_EXTRALEAN          // Exclude rarely-used stuff from Windows header

#ifdef V6_COMPILER
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                   // MFC support for Windows Common Contro
#endif
#endif

#include <iostream>
#include <shlwapi.h>

#define MAX_LEN 1024

char* Replace (char* inputStr, char* str1, char* str2) {
	unsigned int uiInputLength = strlen(inputStr);
	unsigned int uiNumFound=0, uiOutputLength, uiPrevIndex = 0, uiIndex;
	char* pcLookIn = inputStr; // we will move the pointer to keep searching
							   // for instances of str1
	char *pcPrevFound = inputStr, *pcOutput;
	char * pcStrFound; 
	char *outputStr ; // this is the string we will output

	// first we see how many strings of the one we want to replace there are
	// the size of the output string will be greater than it was if str2 is
    // longer than str1. In theory, we can make it shorter is str2 is shorter
	// but we don't bother

	while (pcStrFound = strstr (pcLookIn, str1)) {
			uiNumFound++;
			pcLookIn= pcStrFound + strlen(str1);
	}
	if (int difference =(strlen(str2) - strlen(str1)) >0 ) 
		uiOutputLength = uiInputLength + uiNumFound * difference + 1 ;
	else 
		uiOutputLength = uiInputLength + 1 ;

	outputStr = (char*) malloc(uiOutputLength);

	// search and replace all instances
	pcLookIn = inputStr;
	pcOutput = outputStr;
	while (pcStrFound = strstr(pcLookIn, str1)) {
		uiIndex = pcStrFound - inputStr;
		// copy the characters from the last instance to the current one
		strncpy(pcOutput, pcPrevFound ,uiIndex - uiPrevIndex); 
		// copy the new string instead of the old one
		strncpy(pcOutput + uiIndex - uiPrevIndex, str2, strlen(str2));

		// move the pointers to the next position
		pcOutput += (uiIndex - uiPrevIndex) + strlen(str2);
		pcLookIn = pcStrFound + strlen(str1);

		// store for next time
		uiPrevIndex = uiIndex + strlen(str1);
		pcPrevFound = pcStrFound + strlen(str1);
	}
	// copy to the end of the string
	strncpy(pcOutput, pcLookIn, uiInputLength - uiPrevIndex);
	pcOutput += (uiInputLength - uiPrevIndex);
	unsigned int var = pcOutput - outputStr;
	if (pcOutput - outputStr >= uiOutputLength) {
		fprintf (stderr, "install_finder error. Memory allocated was not big enough.\n");
		exit (1);
	}
	*pcOutput='\0';
	return (outputStr);

}

int main(int argc, TCHAR* argv[], TCHAR* envp[] ){
char pcVDSPVersion[]="4.5";
char pcOS[MAX_LEN] ="Windows";
char *pcBasePath = (char*) malloc(MAX_LEN);
char *pcOutputStr;
DWORD dwType = REG_SZ, dwSize = MAX_PATH;
DWORD dwResult;
char* input_installer = 0;

// Process the command line
    for  (int i=1; i < argc;i++)
    {
        if (strcmp (argv[i],"-proc") == 0)
        {
            if ( i == argc - 1) {
				std::cerr <<"installfinder: No argument specified for -proc option"<<std::endl;
               return (-1);
            }
        }
		else if ( stricmp(argv[i],"-VDSP_4.5") == 0) 
			strcpy(pcVDSPVersion, "4.5");
		else if ( stricmp(argv[i],"-VDSP_5.0" ) == 0) 
			strcpy(pcVDSPVersion, "5.0");
		else if ( stricmp(argv[i],"-unix_style") == 0 ) 
			strcpy(pcOS,"unix");
		else if (strncmp(argv[i],"-vdsp_install=", strlen("-vdsp_install=")) == 0)
		{
			input_installer = strdup(argv[i] + strlen("-vdsp_install="));
		}
        else {
			std::cerr <<"installfinder: Wrong command line option " << argv[i] <<std::endl;
            return (-1);
        }
     }

// Read the registry entries 

	if (input_installer == NULL) {
		if (strcmp (pcVDSPVersion, "4.5") == 0) {
			dwResult = SHGetValue( HKEY_LOCAL_MACHINE,
				         "Software\\Analog Devices\\VisualDSP++ 4.5",
						 "Install Path", &dwType,
						 pcBasePath, &dwSize );
		}
		else if (strcmp (pcVDSPVersion, "5.0") == 0) {
			dwResult = SHGetValue( HKEY_LOCAL_MACHINE,
				         "Software\\Analog Devices\\VisualDSP++ 5.0",
						 "Install Path", &dwType,
						 pcBasePath, &dwSize );
		}
		if (strcmp(pcOS, "unix") ==0 ) {
			pcOutputStr =  Replace(pcBasePath,"\\","/");
			free (pcBasePath);
			pcBasePath = Replace(pcOutputStr," ","\\ ");
			free(pcOutputStr); 
		}
	}
	else { // we had a path in the command line
		if (strcmp(pcOS, "unix") ==0 ) {
			pcBasePath = Replace(input_installer," ","\\ ");
		}
		else {
			pcBasePath = Replace(input_installer,"/","\\");
		}
		free (input_installer);
	}

   std::cout << pcBasePath <<std::endl;

   free(pcBasePath);

   return 0;
}
