# dorun.tcl	gets file to load/run from enviroment variable RUNDXE and 
#		executes it
#
# NOTE: if you find that the debugger stays in the "running" state, make 
#	sure the "dspbreakpoint" line near the end of the file is 
#	uncommented.  This should only be a problem with the coff debugger.

	# The enviroment variable RUNDXE should be set.  This represents the 
	# executable to load and run.  If not set, or not a file, then
	# exit with an error

	# Get the enviroment variable, and strip quotes if needed

	set default_dxe [lindex [ array get env RUNDXE ] 1 ]
	if { [ expr 0 == [ string length $default_dxe ] ] } {
		error "Environment variable RUNDXE not set"
	}
   	set default_dxe [string trim $default_dxe "\"" ]

	# lets make sure that the file exists ...
	if { [ expr 0 == [file isfile $default_dxe ] ] } {
    		error "file $default_dxe either dosen't exist or is not a regular file"
    	}

	# load file into simulator, checking for errors ...
	set return_code [ catch {
   		dspload "$default_dxe" -wait
	} return_error_string ] 

	if { $return_code} {
   		error "dspload of file to be executed ( $default_dxe ) returned \"$return_error_string\""
   	}

	# the following line should only be needed if you are running with the 
	# "coff" symbol manager
	#dspbreakpoint [ dsplookupsymbol _exit ]

	#run the executable, waiting for it to complete
	dsprun -wait

# the "exit" is required to force the termination of the debugger after 
# execution of the app
exit
