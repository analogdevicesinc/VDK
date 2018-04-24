/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

/* Parses command line options */

#pragma warning (disable : 4786)
#include <stdio.h>
#include <string.h>
#include "CmdLine_Parser.h"


/* static variables */
static char program_name[256];

/* Outputs an error */
int
ParseOptionsError(char *str1, char*str2)
{
	fprintf(stderr, "%s %s", str1, str2);
	return -1;
}

/* Prints the options */
void
PrintOptions(CmdLineOptions_t *inOptions, int inNumOptions)
{
	int			i,j;
	int			max_length = 0;
	static int	options_printed = 0;

	/* should we print the options */
	if (options_printed != 0)
		return;
	options_printed = 1;

	/* Find the maximum command length */
	for(i = 0; i < inNumOptions; ++i)
	{
		int temp;
		temp = strlen(inOptions[i].cmd_name);
		if (temp > max_length)
			max_length = temp;
	}

	/* Output the string */
	printf("%s:\n", program_name);

	/* Output the commands */
	for(i = 0; i < inNumOptions; ++i)
	{
		int print_chars;

		/* print the command */
		print_chars = printf("   %s", inOptions[i].cmd_name);

		/* print spaces */
		for(j = 0; j < (max_length + 7) - print_chars; ++j)
			printf(" ");

		/* print what the command does */
		printf("%s\n", inOptions[i].cmd_desc);
	}
}

typedef int (*func_type)(char*);
int
ParseOptions(int argc, char ** argv, CmdLineOptions_t *inOptions, int inNumOptions)
{
	int		found_cmd_num;
	int		in_cmd_num = 0;
	int		test_length;
	int		found;
	int		num_negative;
	int		num_val;
	char	*command_ptr = "";

	/* setup the program name */
	strcpy(program_name, argv[0]);

	/* loop through the command line arguments */
	while (1)
	{
		/* We want to move to the next input if necessary */
		if (*command_ptr == 0)
		{
			if(++in_cmd_num >= argc)
				return 0;
			command_ptr = argv[in_cmd_num];
		}

		/* Find the command in the command list */
		found = -1;
		for (found_cmd_num = 0; (found_cmd_num < inNumOptions) && (found == -1); ++found_cmd_num)
		{
			/* Does the input match the command */
			test_length = strlen(inOptions[found_cmd_num].cmd_name);
			if (strncmp(inOptions[found_cmd_num].cmd_name, command_ptr, test_length) == 0)
			{
				/* We've found a match */
				found = 1;

				switch(inOptions[found_cmd_num].cmd_type)
				{
					case cmd_function:
					case cmd_int:
							/* Do we need to get the next input arg??? */
							if (*(command_ptr + test_length) == 0)
							{
								/* We do, have we run over too many??? */
								if (++in_cmd_num >= argc)
									return ParseOptionsError("Command expects argument: ", command_ptr);
								command_ptr = argv[in_cmd_num];
							}
							else
								command_ptr += test_length;

							/* call the function if that's the type we are */
							if (inOptions[found_cmd_num].cmd_type == cmd_function)
							{
								int ret_val;
								ret_val = ((func_type)(inOptions[found_cmd_num].val))(command_ptr);
								if(ret_val != 0)
									return ret_val;
							}
							else
							{
								if (*command_ptr == '-')
								{
									num_negative = 1;
									++command_ptr;
								}
								else
									num_negative = 0;
								num_val = 0;
								while (*command_ptr >= '0' && *command_ptr <= '9')
									num_val = 10 * num_val + ('0' - *command_ptr++);
								if (num_negative == 0)
									num_val = -num_val;
								if (*command_ptr != 0)
									return ParseOptionsError("Command expects integer: ", command_ptr);
								*((int*)(inOptions[found_cmd_num].val)) = num_val;
							}
							command_ptr += strlen(command_ptr);
						break;
					case cmd_toggle:
							command_ptr += test_length;
							*((int*)(inOptions[found_cmd_num].val)) = !*((int*)(inOptions[found_cmd_num].val));
						break;
					case cmd_bool:
							command_ptr += test_length;
							*((int*)(inOptions[found_cmd_num].val)) = 1;
						break;
					case cmd_options:
							command_ptr += test_length;
							PrintOptions(inOptions, inNumOptions);
						break;
					default:
						break;
				}
			}
		}

		/* Handle unspecified options */
		if (found == -1)
		{
			int ret_val = UnspecifiedOption(command_ptr);
			if (ret_val != 0)
				return ret_val;
			command_ptr += strlen(command_ptr);
		}
	}
	return 0;
}
