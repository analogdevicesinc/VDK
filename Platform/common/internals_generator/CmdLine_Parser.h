/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

// Describe the command line option parser

#ifndef CMDLINE_PARSER_H_
#define CMDLINE_PARSER_H_


typedef enum
{
	cmd_function,
	cmd_toggle,
	cmd_bool,
	cmd_int,
	cmd_options

} cmd_t;

struct CmdLineOptions_t
{
	char*		cmd_name;
	char*		cmd_desc;
	cmd_t		cmd_type;
	void		*val;
};


extern int ParseOptions(int, char**, CmdLineOptions_t*, int);
extern int UnspecifiedOption(char*);


#endif // CMDLINE_PARSER_H_

