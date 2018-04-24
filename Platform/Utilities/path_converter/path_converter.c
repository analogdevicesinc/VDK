#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LEN 1024

char* Replace (char* inputStr, char* str1, char* str2) {
	unsigned int uiInputLength = strlen(inputStr);
	unsigned int uiNumFound=0, uiOutputLength, uiPrevIndex = 0, uiIndex, var;
	char* pcLookIn = inputStr; // we will move the pointer to keep searching
							   // for instances of str1
	char *pcPrevFound = inputStr, *pcOutput;
	char * pcStrFound; 
	char *outputStr ; // this is the string we will output
    int difference = 0;

	// first we see how many strings of the one we want to replace there are
	// the size of the output string will be greater than it was if str2 is
    // longer than str1. In theory, we can make it shorter is str2 is shorter
	// but we don't bother

	while (pcStrFound = strstr (pcLookIn, str1)) {
			uiNumFound++;
			pcLookIn= pcStrFound + strlen(str1);
	}
	if (difference =(strlen(str2) - strlen(str1)) >0 ) 
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
	var = pcOutput - outputStr;
	if ((unsigned int) (pcOutput - outputStr) >= uiOutputLength) {
		fprintf (stderr, "install_finder error. Memory allocated was not big enough.\n");
		exit (1);
	}
	*pcOutput='\0';
	return (outputStr);

}
int main(int argc, char* argv[], char* envp[] ) {
char pcOS[MAX_LEN] ="Windows";
char *pcBasePath = (char*) malloc(MAX_LEN);
char* input_installer = 0;
char* temp_string=0;
int i=1;
    for  (i=1; i < argc;i++)
    {
		if ( stricmp(argv[i],"-unix_style") == 0 ) 
			strcpy(pcOS,"unix");
		else if (strncmp(argv[i],"-vdsp_install=", strlen("-vdsp_install=")) == 0)
		{
			input_installer = strdup(argv[i] + strlen("-vdsp_install="));
		}
        else {
			fprintf(stderr,"path_converter: Wrong command line option %s\n" ,argv[i] );
            return (-1);
        }
     }

		if (strcmp(pcOS, "unix") ==0 ) {
			pcBasePath = Replace(input_installer," ","\\ ");
		}
		else {
			temp_string = Replace(input_installer,"/","\\");
			pcBasePath = Replace(temp_string,"\\ "," ");
			free(temp_string);

		}
		free (input_installer);

	fprintf(stdout,"%s\n",pcBasePath);
	fflush(stdout);
	return 0;
}

