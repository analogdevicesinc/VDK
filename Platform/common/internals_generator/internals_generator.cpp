/*******************************************************************
  Copyright (C) 2001-2018 Analog Devices, Inc. All Rights Reserved
*******************************************************************/

// internals_generator.cpp : Defines the entry point for the console application.
//

#pragma warning (disable : 4786)
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include "CmdLine_Parser.h"

using namespace std;


int		should_leave_c_tags = 0;
int		should_error_on_reprocess = 0;
int		should_not_skip_infinite_files = 0;
int		should_multi_include = 0;
int		strip_count;		// > 0 when stripping


// globals
string			file_name;
string			output_filename;
vector<string>	include_dirs;
vector<string>	processed_filenames;
vector<string>	processing_filenames;
vector<int>		line_nums;

void
Error(string &str)
{
	cerr << "Error: " << str << endl;
	exit(-1);
}

void
Error (char *str)
{
	Error(string(str));
}


int
UnspecifiedOption(char *str)
{
	if (file_name != "")
	{
		Error("Multiple input files specified");
		return -1;
	}
	file_name = str;
	return 0;
}

int
AddInclude(char *str)
{
	include_dirs.push_back(str);
	return 0;
}

int
OutputFilename(char *str)
{
	if (output_filename != "")
	{
		Error("Multiple output files specified");
		return -1;
	}
	output_filename = str;
	return 0;
}

CmdLineOptions_t options[] =
{
	{ "-C", "Leaves C tags in the file (for debugging)", cmd_bool, &should_leave_c_tags },
	{ "-o", "Specify the output file name", cmd_function, OutputFilename },
	{ "-help", "Displays this message", cmd_options, 0 },
	{ "-I", "Specifies an include directory", cmd_function, AddInclude },
	{ "-multi", "Turns on multiple inclusions of a header file\n"
			"\t\t-- off by default", cmd_bool, &should_multi_include },
	{ "-rep_err", "Turns on error reporting during reprocessing of a file\n"
			"\t\t-- off by default", cmd_bool, &should_error_on_reprocess },
	{ "-skip_off", "Turns off skipping of a file that is being included infinitely\n"
			"\t\t-- on by default", cmd_bool, &should_not_skip_infinite_files },
};

bool
FindIncludeFile(string &inStr)
{
	vector<string>::iterator	i;
	for (i = include_dirs.begin(); i != include_dirs.end(); ++i)
	{

		string t = *i + "\\" + inStr;
		ifstream infile(t.c_str());
		if (infile)
		{
			inStr = t;
			return true;
		}
	}
	return false;
}


void
ProcessFile(ostream &stream, string &inStr)
{
	string				the_line;
	int					line_count = 0;
	string::size_type	loc;
	string::size_type	loc_c_begin;
	string::size_type	loc_cpp_begin;
	bool				in_c_comment = false;
	bool				should_read = true;

	// Open the outputfile
	ifstream infile(inStr.c_str());
	if (infile.bad())
	{
		// Since we couldn't open the output file, print the error
		for (int i = 0; i != processing_filenames.size(); ++i)
		{
			cerr << "While processing file: " << processing_filenames[i];
			cerr << " line: " << line_nums[i] << endl;
		}
		Error("Could not open input file: " + inStr);
	}
	processing_filenames.push_back(inStr);
	processed_filenames.push_back(inStr);

	if (should_leave_c_tags)
		stream << "#line 1 \"" << inStr << '\"' << endl;

	// Process the file
	while (infile)
	{
		// Should we get the next input line????  -- this is used for when
		// we're stripping C comments
		if (should_read == true)
		{
			++line_count;
			getline(infile, the_line);

			// skip black lines
			if ((the_line.length() == 0) && (should_leave_c_tags == false))
				continue;

			// append a nl...
			the_line += '\n';
		}
		should_read = true;

		// Check for the beginning of a c comment
		loc_c_begin = the_line.find("/*");
		if ((loc_c_begin != string::npos) && (in_c_comment == false))
		{
			// Output everything before the comment began
			if ((in_c_comment == false) && (strip_count == 0))
				stream << the_line.substr(0, loc_c_begin);
			in_c_comment = true;
			the_line = the_line.substr(loc_c_begin + 2, the_line.length());
			should_read = false;
		}

		// Check for a C comment
		if (in_c_comment)
		{
			// We are in a comment.  Figure out if we are going to being stripping...
			if (the_line.find("BEGIN_INTERNALS_STRIP") != string::npos)
				++strip_count;
			if (the_line.find("END_INTERNALS_STRIP") != string::npos)
				--strip_count;

			// Figure out if we're done stripping
			string::size_type	loc_c_end = the_line.find("*/");
			if (loc_c_end != string::npos)
			{
				in_c_comment = false;
				the_line = the_line.substr(loc_c_end + 2, the_line.length());
				if (strip_count == 0)
					stream << ' ';
				if (the_line.length() != 0)
					should_read = false;
			}
			else
				the_line = "";
			continue;
		}

		// Rip out C++ comments
		loc_cpp_begin = the_line.find("//");
		if (loc_cpp_begin != string::npos)
		{
			// We are in a comment.  Figure out if we are going to being stripping...
			if (the_line.find("BEGIN_INTERNALS_STRIP") != string::npos)
				++strip_count;
			if (the_line.find("END_INTERNALS_STRIP") != string::npos)
				--strip_count;

			// Output before the C++ comment
			if (strip_count == 0)
				stream << the_line.substr(0, loc_cpp_begin) << endl;
			the_line = "";
			continue;
		}

		// See if we have an include line
		loc = the_line.find_first_not_of(" \t");
		if (loc != string::npos)
		{
			// See if we have found a preprocessor line
			if (the_line[loc] == '#')
			{
				// See if it's an include line
				loc = the_line.find_first_not_of(" \t", loc + 1);
				if (the_line.find("include", loc) == loc)
				{
					// Get the filename
					loc = the_line.find_first_of(" \t\"<", loc);
					loc = the_line.find_first_not_of(" \t\"<", loc);
					string		name = the_line.substr(loc);
					name = name.substr(0, name.find_first_of(" \t\">"));

					// Find the include file
					if(FindIncludeFile(name) == true)
					{
						// Make sure the filename is unique or should be skipped
						vector<string>::iterator t;
						t = find(processed_filenames.begin(), processed_filenames.end(), name);
						if (t != processed_filenames.end())
						{
							if (should_error_on_reprocess)
								Error("Reprocess of file detected on filename: " + name +
										" in file " + inStr);
							if (should_multi_include == 0)
							{
								stream << endl;
								continue;
							}
							t = find(processing_filenames.begin(), processing_filenames.end(), name);
							if (t != processing_filenames.end())
							{
								if (should_not_skip_infinite_files != 0)
									Error("Infinite recursion detected in filename: " + inStr);
								if (strip_count == 0)
									stream << endl;
								continue;
							}
						}

						// Save off the line information & process the file
						line_nums.push_back(line_count);
						ProcessFile(stream, name);

						// Restore the line information
						line_nums.pop_back();
						if ((should_leave_c_tags) && (strip_count == 0))
							stream << "#line " << line_count + 1 << " \"" << inStr << '\"' << endl;
						continue;
					}
				}
			}
		}
		if (strip_count == 0)
			stream << the_line;
	}

	processing_filenames.pop_back();

	// output an easy to catch error
	if (in_c_comment)
		Error("Expected end of C comment in file: " + inStr);
}


int
main(int argc, char* argv[])
{
	ofstream	stream;
	bool		should_stdout = true;

	// Parse the command line
	if (ParseOptions(argc, argv, options, sizeof(options)/sizeof(CmdLineOptions_t)) != 0)
		Error("Error in command line parser");

	// Test to make sure that we have a filename we should process
	if (file_name.length() == 0)
		Error("No input files specified");

	// Figure out the output
	if (output_filename.length() != 0)
	{
		should_stdout = false;
		stream.open(output_filename.c_str());
		if (stream.bad())
			Error("Couldn't open " + output_filename + " for output");
	}
	else
		output_filename = file_name;

	// Add the current directory to the include path
	include_dirs.push_back(".");

	// Get the preprocessor guard name
	string header_mark = output_filename;
    string::size_type loc = header_mark.find_last_of("/") + 1;
    if (loc != string::npos)        // strip the path (if necessary)
        header_mark = header_mark.substr(loc, header_mark.length());
    loc = header_mark.find('.');    // Change the .h to _h
	if (loc != string::npos)
		header_mark.replace(loc, 1, "_");
	header_mark += '_';
	transform(header_mark.begin(), header_mark.end(), header_mark.begin(), toupper);    // make the guard uppercase

	// Process the file
	if (should_stdout == false)
	{
		stream << "#ifndef " << header_mark << endl;
		stream << "#define " << header_mark << endl;
		ProcessFile(stream, file_name);
		stream << "#endif /* " << header_mark << "*/\n\n";
	}
	else
	{
		cout << "#ifndef " << header_mark << endl;
		cout << "#define " << header_mark << endl;
		ProcessFile(cout, file_name);
		cout << "#endif /* " << header_mark << "*/\n\n";
	}


	return 0;
}
