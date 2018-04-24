#
# invoke_debugger.tcl
#
# this file was created since a batch file can't see the registry, 
# dosen't wait for a windows app to complete before continuing, ...
#
# it requires:
# the "registry" package to determine the installed location of VisualDSP
# the enviroment variable "REGPROCESSOR" to be set to the "session_name" (ADSP-21062, ...)

	# determine instalation location for VisualDSP
	package require registry 1.0 

	set default_proc [lindex [ array get env REGPROCESSOR ] 1 ]
	if { [ expr 0 == [ string length $default_proc ] ] } {
		puts "Environment variable REGPROCESSOR not set"
		exit 1
	}

	set vdsp_version [lindex [ array get env VDSP_VERSION ] 1 ]
	if { [ expr 0 == [ string length $vdsp_version ] ] } {
		puts "Environment variable VDSP_VERSION not set"
		exit 1
	}
	set vdsp_flag "-VDSP_$vdsp_version"

	set common_dir [ file dirname $argv0 ] 
        if {[catch {set adi_dir [exec $common_dir\\..\\utilities\\installfinder\\Debug\\installfinder -unix_style $vdsp_flag ] } u] != 0} {
        puts "The install path was not found"
        puts $u
        exit 1
        #Return -1, compile-time failure.
   }
        if {[catch {set adi_dir_dos [exec $common_dir\\..\\utilities\\installfinder\\Debug\\installfinder $vdsp_flag ] } u] != 0} {
        puts "The install path was not found"
        puts $u
        exit 1
        #Return -1, compile-time failure.
   }
	# The enviroment variable RUNDXE should be set.  This represents the 
	# executable to load and run.  If not set, or not a file, then
	# exit with an error

	# Get the enviroment variable, and strip quotes if needed

	set default_dxe [lindex [ array get env RUNDXE ] 1 ]
	if { [ expr 0 == [ string length $default_dxe ] ] } {
		puts "Environment variable RUNDXE not set"
		exit 1
	}
   	set default_dxe [string trim $default_dxe "\"" ]

	# lets make sure that the file exists ...
	if { [ expr 0 == [file isfile $default_dxe ] ] } {
    		puts "file $default_dxe either dosen't exist or is not a regular file"
		exit 1
    	}


	# log files that are "cat'ed" to the screen at end of run
	#
	#"tcl" log file
	set output_file "$adi_dir_dos\Data\\VisualDSP_Log.txt" 

	# debugger "stdout" log file
	# the std i/o support always puts the stdout.txt file in c:\windows\temp
	#set stdout_file "c:/windows/temp/stdout.txt"
	set stdout_file [file join $env(TEMP) stdout.txt]

	# for both the log files, delete them if they currently exist.
	if { [ file exists $output_file ] } {
		file delete $output_file
	}
	if { [ file exists $stdout_file ] } {
		file delete $stdout_file
	}


	# expand the name of "dorun.tcl" with the location of this file
	# so don't have to be running from the directory where they are
	# both located.
	set dorun_with_dir [ file join [ file dirname $argv0 ] generate_offsets.tcl  ]

	# invoke the debugger indicating the "session_name" to use.
	if { $default_proc == "ADSP-BF535" } {
	   # We use the command line sim which is quicker and we send the
           # output to a temporary file
	   set  cmd_line  "$adi_dir/jitccs535.exe $default_dxe >$stdout_file 2>&1"
	   set IDE_USED "No"
	} elseif { $default_proc == "ADSP-BF533" } {
	   # We use the command line sim which is quicker and we send the
           # output to a temporary file
	   set  cmd_line  "$adi_dir/jitccs532.exe $default_dxe >$stdout_file 2>&1 "
	   set IDE_USED "No"
	} else {
	   set cmd_line "$adi_dir/System/idde -f $dorun_with_dir -s $default_proc"
	   set IDE_USED "Yes"
	}
        eval exec $cmd_line

	# We are done running the test.  Write the contents of the tcl 
	# log file to STDOUT.
	if { [ file exists $output_file ] } {

		# Open the file for reading and echo the contents to STDOUT.
		set f [ open $output_file r ]
		set t 0
		while {[gets $f line] >= 0} {
			if { $t != 0 } {
				puts $line
			}
			set t 1
		}
		close $f
	} elseif {$IDE_USED == "Yes" } {
	# If we used the IDDE and there is no file, there was an error so 
	# we record that. If the command line sim was used, the output goes to
	# $TEMP
	   puts $output_file
	}

	# Also, write the contents of the stdout log file to STDOUT.
	if { [ file exists $stdout_file ] } {

		#puts "Start of stdout"

		# Open the file for reading and echo the contents to STDOUT.
		set f [ open $stdout_file r ]
		while {[gets $f line] >= 0} {
			puts $line
		}
		close $f
	}
