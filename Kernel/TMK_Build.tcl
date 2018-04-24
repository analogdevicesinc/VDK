set VDSPVersion "VisualDSP++ 5.0"
set LibraryVersion "5.0.10"

set ReleaseBuild 1
set target_family ""
set target_core ""
set target_device ""
set target_si_rev ""
# This variable indicates whether we are building the TMK library or a ROM image
set BuildType "library"
set Target ""
set instfind_flgs ""
set Vdsp_version_num "5.0"
set finderversion ""
set finderplatform ""
set adi_dir_dos ""
set adi_dir ""
set Install ""
set install_dir ""
set OutputFile "&"
# Where the shell script output will go.
set Script_OutputFile ""
set VerboseOutput 0

# Top directory in DOS and UNIX format
set Trinity_dir [pwd]
set Trinity_dir_dos [exec cygpath -w $Trinity_dir]
puts $Trinity_dir_dos
set Trinity_dir_unix [exec pwd]

# Stores which compiler driver we will use (ccblkfn ccts )
set Compiler ""

# We will use VersionCOpt to store the options that we are building with and we
# want to add to the version information. We don't want to do all options 
# because some like +small_debug are not visible to users (or not significant)
set VersionCOpt ""
set VersionProcessor ""

set cmdline_LibraryTypes ""

# Indicates whether the TMK version that includes lots of assertions is to be
# used. This requires an special version of VDK built for it.
set AssertingTMK 0

# Contains the commands that we will run in the build process. Needs to be 
# created as binary
set sh_commands  [open sh_commands.sh w]
fconfigure $sh_commands -translation binary

####################################################################
# All processors supported - whether we make libraries or not
####################################################################

####
# SHARC processors
####

# All 2106x processors
set 2106X [list 21060 21061 21062 21065L]
# All Hammerhead processors
set 2116X [list 21160 21161]
# All SHARC Ex processors
set 2126X [list 21262 21266 21261 21267]
# All SHARC Ex4 processors
set 2136X [list 21362 21363 21364 21365 21366 21367 21368 21369]
# All 2137x processors
set 2137X [list 21371 21375]
# All 2146x processors
set 2146X [list 21462 21465 21467 21469]
# All 2147x processors
set 2147X [list 21479]
# All 2148x processors
set 2148X [list 21489]
# All SHARC processors (i.e. 2106X, Hammerhead, SHARC Ex)
set SHARC [concat $2106X $2116X $2126X $2136X $2137X $2146X $2147X $2148X]

####
# Blackfin processors
####

set Blackfin [list BF535 BF532 BF531 BF533 BF561 BF539 BF534 BF536 BF537 BF542 BF544 BF547 BF548 BF549 BF522 BF523 BF524 BF525 BF526 BF527 BF512 BF514 BF516 BF518 BF542M BF544M BF547M BF548M BF549M BF504 BF504F BF506F BF592-A]

####
# TigerSHARC processors
####

set AllCores [list 2106x 2116x 2126x 2136x 2137x 2146x 2147x 2148x ]
set AllFamilies [list blackfin sharc]

####################################################################
# Silicon revisions accepted by this script
####################################################################
#All Blackfin silicon revisions
set BF531_Revisions [list 0.1 0.2 0.3 0.5]
set BF532_Revisions [list 0.1 0.2 0.3 0.5 any]
set BF533_Revisions [list 0.1 0.2 0.3 0.5]
set BF534_Revisions [list 0.0 0.3]
set BF535_Revisions [list 0.2 1.0 1.1 1.2 1.3 any]
set BF536_Revisions [list 0.0 0.3]
set BF537_Revisions [list 0.0 0.3]
set BF539_Revisions [list 0.0 0.3]
set BF561_Revisions [list 0.2 0.3 0.4 0.5 any]
set BF506F_Revisions [list 0.0 any none]
set BF592A_Revisions [list any]

#All SHARC silicon revisions
set 21060_Revisions [list 2.1 3.0 3.1]
set 21061_Revisions [list 0.1]
set 21062_Revisions [list 2.0 2.1 3.0]
set 21065_Revisions [list 0.0 0.1 0.2 0.3]
set 21160_Revisions [list 0.0 0.1 1.1 1.2]
set 21161_Revisions [list 0.3 1.0 1.1 1.2]
set 21261_Revisions [list 0.0 0.1]
set 21262_Revisions [list 0.1]
set 21266_Revisions [list 0.1 0.2]
set 21267_Revisions [list 0.0 0.1 0.2]
set 21362_Revisions [list 0.0]
set 21363_Revisions [list 0.0 0.1 0.2]
set 21364_Revisions [list 0.0 0.1 0.2]
set 21365_Revisions [list 0.0 0.2]
set 21366_Revisions [list 0.0 0.1 0.2]
set 21367_Revisions [list 0.0]
set 21368_Revisions [list 0.0]
set 21369_Revisions [list 0.0 0.1]
set 21371_Revisions [list 0.0]
set 21375_Revisions [list 0.0]
set 21462_Revisions [list 0.0]
set 21465_Revisions [list 0.0]
set 21467_Revisions [list 0.0]
set 21469_Revisions [list 0.0]
set 21479_Revisions [list any]
set 21489_Revisions [list any]

####################################################################
# Master list of all supported processors
set AllProcessors [concat $2106X $2116X $2126X $2136X $2137X $2146X $2147X $Blackfin ]

####################################################################
# Help command for this script
####################################################################
proc ShowParameters {} {
	puts "TMK_Build.tcl\n"
	puts "Command-line options:\n"
	puts "-assert"
	puts "        Build the asserting TMK"
	puts "-family=<family>"
	puts "        Family to build the microkernel for. "
	puts "        Families supported to present are:"
	puts "        blackfin sharc "
	puts "-core=<core>"
	puts "        Core to build the microkernel for."
	puts "        Cores supported to present are:"
	puts "        SHARC Family - 2106x 2116x 2126x 2136x 2137x 2146x"
	puts "-device=<device>"
	puts "        Device to build VDK for. "
	puts "        Devices supported to present are:"
	puts "        Blackfin Family - BF535 BF532 BF531 BF533 BF561 BF539 BF534 BF536 BF537 BF541 BF542 BF544 BF547 BF548 BF549 BF522 BF523 BF524 BF525 BF526 BF527 BF512 BF514 BF516 BF518 BF542M BF544M BF547M BF548M BF549M BF504 BF504F BF506F BF592-A"
	puts "        SHARC Family - 21060 21061 21062 21065L 21160 21161 21262 21266 21261 21267 21362 21363 21364 21365 21366 21367 21368 21369 21371 21375 21462 21465 21467 21469 21479 21489"
	puts "-target=<target1>,<target2>..."
	puts "        Comma-separated list of commands to be executed by gmake."
	puts "        Targets supported to present are:"
	puts "          clean debug release install squeaky "
	puts "-install_dir=<dir>"
	puts "        When doing and install which directory the files should be created in" 
	puts "        The default is where VDSP++ 3.0 or 3.5 is installed"
	puts "-si-revision=<number|all>"
	puts "        Generates the libraries for the silicon revision specified. All will"
	puts "        generate the libraries for all the silicon revisions."
    	puts "-vdsp_install=<dir_name>"
    	puts "        Uses dir_name as the install location for VDSP++"
	puts "-vdsp_version=<4.5|5.0>"
    	puts "        Vdsp version to use. Default 5.0."
	puts "-output=<filename>"
	puts "        Redirects the output of the make command onto file filename"
	puts "-verbose"
	puts "        Prints to stdout the command lines being run"
	exit 0;
}

####################################################################
# Parse the options given in the command line
####################################################################
foreach arg $argv {
	switch -glob -- [string tolower $arg] {

		"-assert"         {       
			set AssertingTMK 1
			}
		"-family=*"         {       
			set target_family [string tolower [ split [join [lrange [split $arg "="] 1 end] "="] ","]] 
			}

		"-device=*"         {       
			set target_device [ string toupper [ split [join [lrange [split $arg "="] 1 end] "="] ","]] 
			}

		"-core=*"           {       
			set target_core [ string tolower [ split [join [lrange [split $arg "="] 1 end] "="] ","]] 
			}

		"-si-revision=*"    {
			set target_si_rev [ string tolower [ split [join [lrange [split $arg "="] 1 end] "="] ","]] 
			}

		"-target=*"         {  
			set arg [split [join [lrange [split $arg "="] 1 end] "="] ","]  
			
			set found [lsearch [string tolower $arg] release]
			if { $found != -1 } {
				set ReleaseBuild 1
				set arg [lreplace $arg $found $found clean build]
			}

			set found [lsearch [string tolower $arg] debug]
			if { $found != -1 } {
				set ReleaseBuild 0
				set arg [lreplace $arg $found $found clean build]
			}
			append Target "$arg " }

		"-word_addressing" {
			set cmdline_LibraryTypes word_addressing }
		"-byte_addressing" {
			set cmdline_LibraryTypes byte_addressing }
        "-vdsp_install=*" {
            set vdsp_install [join [lrange [split $arg "="] 1 end] "="]
            set instfind_flgs "-vdsp_install=$vdsp_install" }
		"-vdsp_version=4.5" {
			set Vdsp_version_num 4.5  }
		"-vdsp_version=5.0" {
			set Vdsp_version_num 5.0  }
		"-output=*"         {   
			set curr_dir [pwd]
			set tmp_out [ split [join [lrange [split $arg "="] 1 end] "="] ","]  
			eval exec rm -f $curr_dir/$tmp_out
			eval exec touch $curr_dir/$tmp_out
			set Script_OutputFile ">> $curr_dir/$tmp_out "
			set OutputFile ""  }

		"-verbose"          {
			set VerboseOutput 1 }
		"-install_dir=*"         {   
			set install_dir [ split [join [lrange [split $arg "="] 1 end] "="] ","]  
			set Install UP_INSTALL_DIR=$install_dir
			}

		"-help"             {       
			ShowParameters }

		default             {       
			puts "\n Command line argument not recognised :\n\n$arg\n\nUse -help to display valid arguments.\n"  
			exit 1 }
                }
        }

# if we support a processor that we don't build libraries for, change the target
# device to the one we build libraries for
proc SelectProcessor {inDevice} {
	global target_device
	if {$target_device == "21060" || $target_device == "21061" || $target_device == "21062" || $target_device == "21065L" } {
		set target_device "21060"
	} elseif {$target_device == "21160" || $target_device == "21161"} {
		set target_device "21160"
	} elseif {$target_device == "21261" || $target_device == "21262" 
	       || $target_device == "21266" || $target_device == "21267" } {
		set target_device "21261"
	} elseif {$target_device == "21362" || $target_device == "21363" 
	       || $target_device == "21364" || $target_device == "21365" 
	       || $target_device == "21366" } {
		set target_device "21364"
	} elseif {$target_device == "21371" || $target_device == "21375" } {
		set target_device "21371"
	} elseif {$target_device == "21367" || $target_device == "21368"
	       || $target_device == "21369" } {
		set target_device "21369"
	} elseif {$target_device == "21462" || $target_device == "21465" || $target_device == "21467"} {
		set target_device "21469"
	} elseif {$target_device == "21489" } {
		set target_device "21479"
	} elseif {$target_device == "BF531" || $target_device == "BF532"} {
		set target_device "BF533"
	} elseif {$target_device == "BF524" || $target_device == "BF526"} {
		set target_device "BF522"
	} elseif {$target_device == "BF525" || $target_device == "BF527"} {
		set target_device "BF523"
	} elseif {$target_device == "BF544" || $target_device == "BF547" || $target_device == "BF548" || $target_device == "BF549" || $target_device == "BF542M" || $target_device == "BF544M" || $target_device == "BF547M" || $target_device == "BF548M" || $target_device == "BF549M"} {
		set target_device "BF542"
	} elseif {$target_device == "BF514" || $target_device == "BF516" || $target_device == "BF518" } {
		set target_device "BF512"
	} elseif {$target_device == "BF504F" || $target_device == "BF504"} {
		set target_device "BF506F"
	}
}
# Check that the device core and family given in the command line are valid

   if {$target_device != "" && [lsearch $AllProcessors $target_device] == -1} {
       puts "ERROR: Wrong device specified"
       puts "Supported devices: $AllProcessors"
       exit 1
   }

   if {$target_core != "" && [lsearch $AllCores $target_core] == -1} {
       puts "ERROR: Wrong core specified"
       puts "Core specified $target_core Supported cores: $AllCores"
       exit 1
   }

   if {$target_family != "" && [lsearch $AllFamilies $target_family] == -1} {
       puts "ERROR: Wrong family specified"
       puts "Supported families: $AllFamilies"
       exit 1
   }

   if { $target_family ==""} {
# The device has been specified so set the family and core appropriately
       if {$target_device != "" } {
         if {[lsearch $Blackfin $target_device] != -1} {
	    set target_family blackfin
          }
  
          if {[lsearch $SHARC $target_device] != -1} {
	    set target_family sharc
	     if { $target_core ==""} {
               if {[lsearch $2106X $target_device] != -1} {
	         set target_core 2106x
               } elseif {[lsearch $2116X $target_device] != -1} {
	         set target_core 2116x
               } elseif {[lsearch $2126X $target_device] != -1} {
	         set target_core 2126x
               } elseif {[lsearch $2136X $target_device] != -1} {
	         set target_core 2136x
               } elseif {[lsearch $2137X $target_device] != -1} {
	         set target_core 2137x
               } else {
	         set target_core 2146x
               }
             }
          }
# we only got the core in the command line so we should work out the family
       } elseif {$target_core != ""} {
         if {$target_core == "2106x" || $target_core == "2116x" || $target_core == "2126x" || $target_core == "2136x" || $target_core == "2137x" || $target_core == "2146x"} {
            set target_family sharc
         }
       }
     }

 # Check that device core and family match for blackfin. The family is always
 # set by now

   if {$target_family == "blackfin"} {
      if { $target_core!= "" } {
	puts "ERROR: core cannot be specified with blackfin processors"
	puts "       the core will be ignored"
	set target_core ""
      }
      if {$target_device != "" && [lsearch $Blackfin $target_device] == -1} {
	puts "ERROR: device $target_device is not a supported device of the blackfin family"
	exit 1
      }
    }


 # Check that device core and family match for sharc. The family is always
 # set by now

   if {$target_family == "sharc"} {
	if {$target_core != "" && $target_core != "2106x" && $target_core != "2116x" && $target_core != "2126x" && $target_core != "2136x" && $target_core != "2137x" && $target_core != "2146x"} {
	puts "ERROR: core $target_core is not a supported core of the SHARC family"
	exit 1
        }

      if {$target_device != "" && [lsearch $SHARC $target_device] == -1} {
	puts "ERROR: target $target_device is not a supported device of the SHARC family"
	exit 1
      }
   }

 # Check that si-revision matches for ts. The family is always
 # set by now

      if {$target_si_rev != ""} {
	if {$target_si_rev != "none" && $target_si_rev != "any" && $target_si_rev != "all" } {
	if {$target_device == "BF532" || $target_device == "BF533" || $target_device == "BF531"} {
		set target_device "BF532"
		# BF533 and BF531 revisions are the same as BF532
        	if {[lsearch $BF532_Revisions $target_si_rev] == -1} {
		puts "ERROR: $target_si_rev is not a valid revision for $target_device "
		exit 1
		} 
	} elseif {$target_device == "BF534" || $target_device == "BF536"  || $target_device == "BF537" || $target_device == "BF538" || $target_device == "BF539"} {
		set target_device "BF534"
		# BF536 BF537 BF538 and BF539 revisions are the same as BF534
        	if {[lsearch $BF534_Revisions $target_si_rev] == -1} {
		puts "ERROR: $target_si_rev is not a valid revision for $target_device "
		exit 1
		} 
	} elseif {$target_device == "BF535"} {
        	if {[lsearch $BF535_Revisions $target_si_rev] == -1} {
		puts "ERROR: $target_si_rev is not a valid revision for $target_device "
		exit 1
		} elseif {$target_si_rev == 0.2 || $target_si_rev == 1.0 || $target_si_rev == 1.1 || $target_si_rev == 1.2 || $target_si_rev == 1.3}  {
			set target_si_rev 0.2 
		}
	} elseif {$target_device == "BF561"} {
        	if {[lsearch $BF561_Revisions $target_si_rev] == -1} {
		puts "ERROR: $target_si_rev is not a valid revision for $target_device "
		exit 1
		}
	} elseif {$target_device == "BF504" || $target_device == "BF504F" } {
		set target_device "BF506F"
		# BF533 and BF531 revisions are the same as BF532
        	if {[lsearch $BF506F_Revisions $target_si_rev] == -1} {
		puts "ERROR: $target_si_rev is not a valid revision for $target_device "
		exit 1
		} 
	} elseif {$target_device == "21261" || $target_device == "21262"  || $target_device == "21266" || $target_device == "21267" } {
        	if {[lsearch $21267_Revisions $target_si_rev] == -1} {
		puts "ERROR: $target_si_rev is not a valid revision for $target_device "
		exit 1
		} 
		}
	} elseif {$target_device == "" } {
			puts "si-revision not allowed without specifying device"
			exit 1
		}
      }

  if {$target_device != ""} {
	SelectProcessor $target_device 
  }

# If debug is one of the targets and install is another one, change install to 
# install_debug so we don't copy the release libs.
  if { [string first debug $Target ] != -1  || [string first d_ $Target ] != -1 } {
      if { [string first install $Target ] != -1  && [string first install_debug $Target ] == -1 } {
         puts "Changing install to install_debug "
	 regsub -all install $Target install_debug Target
      }
  }

####################################################################
# Selection of VisualDSP to use
####################################################################

# Find where VisualDSP is installed. We need to know if we are
# VDSP 4.5 or VDSP 5.0. We will be reading the registry entries.

   if { $Vdsp_version_num == "4.5" } {
	set finderversion "-VDSP_4.5"
   } elseif { $Vdsp_version_num == "5.0" } {
	set finderversion "-VDSP_5.0"
   }

   if {$instfind_flgs == "" } {
   #Create the installfinder with MSdev
      if {[array names env DevEnvDir] != ""  && [file exists $env(DevEnvDir)/VCExpress.exe] != 0 } {
              puts "  $env(DevEnvDir)/VCExpress.exe ../Platform/Utilities/installfinder/installfinder_2005.sln /build Debug"
      } elseif {[array names env DEVENVDIR] != ""  && [file exists $env(DEVENVDIR)/VCExpress.exe] != 0 } {
              puts "  $env(DEVENVDIR)/VCExpress.exe Utilities/installfinder/installfinder_2005.sln /build Debug"
      } elseif {[array names env DevEnvDir] != ""  && [file exists $env(DevEnvDir)/devenv.exe] != 0 } {
              puts "  $env(DevEnvDir)/devenv.exe ../Platform/Utilities/installfinder/installfinder_2005.sln /build Debug"
      } elseif {[array names env DEVENVDIR] != ""  && [file exists $env(DEVENVDIR)/devenv.exe] != 0 } {
              puts "  $env(DevEnvDir)/devenv.exe Utilities/installfinder/installfinder_2005.sln /build Debug"
      } elseif {[array names env VSINSTALLDIR] != ""  && [file exists $env(VSINSTALLDIR)/devenv.exe] != 0 } {
              puts "  $env(VSINSTALLDIR)/devenv.exe ../Platform/Utilities/installfinder/installfinder.sln /build Debug"
      } elseif {[array names env MSDEVDIR] != ""  && [file exists $env(MSDEVDIR)/Bin/MSDEV.exe] != 0 } {
              puts "  $env(MSDEVDIR)/Bin/MSDEV.exe ../Platform/Utilities/installfinder/installfinder.dsw /make "
      } else {
              puts " Microsoft Visual Studio not found. Exiting..."
              exit
      }
  
     if {[array names env MSDEVDIR] != ""  && [file exists $env(MSDEVDIR)/Bin/MSDEV.exe] != 0 } {
       if {[catch {[exec $env(MSDEVDIR)/Bin/MSDev ../Platform/Utilities/installfinder/installfinder.dsw /make ]} make_error] != 0} {
                          puts $make_error
       }
    } elseif { [array names env VSINSTALLDIR] != ""  && [file exists $env(VSINSTALLDIR)/devenv.exe] != 0 } {
       if {[catch {[exec $env(VSINSTALLDIR)/devenv.exe ../Platform/Utilities/installfinder/installfinder.sln /build Debug]} make_error] != 0} {
                          puts $make_error
       }
    } elseif { [array names env DevEnvDir] != ""  && [file exists $env(DevEnvDir)/devenv.exe] != 0 } {
       if {[catch {[exec $env(DevEnvDir)/devenv.exe ../Platform/Utilities/installfinder/installfinder_2005.sln /build Debug]} make_error] != 0} {
                               puts $make_error
       }
    } elseif { [array names env DevEnvDir] != ""  && [file exists $env(DevEnvDir)/VCExpress.exe] != 0 } {
       if {[catch {[exec $env(DevEnvDir)/VCExpress.exe ../Platform/Utilities/installfinder/installfinder_2005.sln /build Debug]} make_error] != 0} {
                          puts $make_error
       }
    } elseif { [array names env DEVENVDIR] != ""  && [file exists $env(DEVENVDIR)/devenv.exe] != 0 } {
       if {[catch {[exec $env(DEVENVDIR)/devenv.exe ../Platform/Utilities/installfinder/installfinder_2005.sln /build Debug]} make_error] != 0} {
                               puts $make_error
       }
    } elseif { [array names env DEVENVDIR] != ""  && [file exists $env(DEVENVDIR)/VCExpress.exe] != 0 } {
       if {[catch {[exec $env(DEVENVDIR)/VCExpress.exe ../Platform/Utilities/installfinder/installfinder_2005.sln /build Debug]} make_error] != 0} {
                          puts $make_error
       }
     }
   }


  if {$VerboseOutput} {
    puts "Finding install path"
  }

   # Read the registry and see where the install is.
   if {$instfind_flgs == "" } {
       if {$VerboseOutput} {
           puts "..\\Platform\\utilities\\installfinder\\Debug\\installfinder -unix_style $finderversion"
       }
       if {[catch {set adi_dir [exec ..\\Platform\\utilities\\installfinder\\Debug\\installfinder -unix_style $finderversion] } u] != 0} {
	    puts "The install path was not found"
	    puts $u
	    exit 1
	    #Return -1, compile-time failure.
       }
   } else {
       if {$VerboseOutput} {
           puts "..\\Platform\\utilities\\path_converter\\path_converter -unix_style $instfind_flgs"
       }
       if {[catch {set adi_dir [exec ..\\Platform\\utilities\\path_converter\\path_converter -unix_style $instfind_flgs] } u] != 0} {
	    puts "String could not be converted"
	    puts $u
	    exit 1
	    #Return -1, compile-time failure.
      }
   }
  # Set dir for path_converter
  set dir "-vdsp_install=$adi_dir"

  # Convert the path to DOS style
  if {$VerboseOutput} {
    puts "Converting install path to DOS"
	puts "..\\Platform\\utilities\\path_converter\\path_converter $dir"
  }

  # if {[catch {set adi_dir_dos [exec cygpath -w $dir ] } u] != 0} {
  if {[catch {set adi_dir_dos [exec ..\\Platform\\utilities\\path_converter\\path_converter $dir ] } u] != 0} {
        puts "String could not be converted to DOS style"
	    puts $u
	    exit 1
   }

  if {$install_dir == ""} {
	set install_dir  $adi_dir
  }

# Add the directory to the path
  append env(PATH) ";$adi_dir_dos"
  if {$VerboseOutput} {
    puts "Adding install dir to path"
    puts "** DOS:  $adi_dir_dos "
    puts "** UNIX: $adi_dir"
    puts ""
  }

####################################################################
# Generation of version information file
####################################################################

proc GenerateVersionInformation {processor_built_for SpecialOptions} {
	global LibraryVersion
	set VersionInfo [open version.txt w]
	set date [clock seconds]
	set todayDate [clock format $date -format "%a %b %e %Y %T"]
	puts $VersionInfo "Library_Built:  $todayDate"
	puts $VersionInfo "Library_Name: TMK Library"
	puts $VersionInfo "Library_Version: $LibraryVersion"
	puts $VersionInfo "Platform: $processor_built_for"
	puts $VersionInfo "SpecialOptions: $SpecialOptions"
	close $VersionInfo
}

proc AddToVersionCOpt {new_option} {
	global VersionCOpt
	append VersionCOpt "$new_option "
}

set revisions_to_build ""
set processors_to_build ""

####################################################################
# Processors and si revisions we build libraries for when family/core 
# is specified
####################################################################

set blackfin_processors {BF532 BF534 BF535 BF561 BF522 BF523 BF512 BF506F BF542 BF592-A}
set BF506F_revisions {any}
set BF512_revisions {any}
set BF522_revisions {any}
set BF523_revisions {any}
set BF534_revisions {0.0 0.3 any}
set BF532_revisions {0.1 0.2 0.3 0.5 none any}
set BF535_revisions {0.2 none any}
set BF561_revisions {0.2 0.3 0.5 none any}
set BF542_revisions {any}
set BF592A_revisions {0.1 any}

set sharc_processors {21060 21160 21261 21364 21369 21371 21469 21479}
set 2106x_processors {21060}
set 2116x_processors {21160}
set 2126x_processors {21262}
set 2136x_processors {21364 21369}
set 2137x_processors {21371}
set 2146x_processors {21469}
set 2147x_processors {21479}

# 21060 and 21160 only have one set of libraries in the lib folder
set 21060_revisions {ignore}
set 21160_revisions {ignore}
set 21261_revisions {0.0 any none}
set 21364_revisions {0.0 any none}
set 21369_revisions {0.1 any}
set 21371_revisions {0.0 any none}
set 21469_revisions {any none}
set 21479_revisions {any}

set which_silicon ""

####################################################################
# Addition of processors not specified in the command line
####################################################################

# For certain processors, the real libraries we need to build either don't 
# correspond to that processor or we need libraries from other ones

proc AddOtherProcessors {inDevice} {
	global processors_to_build
	global which_silicon
	if {$inDevice == "21369" } {
		if { [lsearch $processors_to_build 21364] == -1 } {
			append processors_to_build " 21364"
		} 
	}

	if {$inDevice == "BF533"} {
		if { [lsearch $processors_to_build BF532] == -1 } {
			append processors_to_build " BF532"
		} 
		set found [lsearch $processors_to_build BF533]
		# delete this processor from the list
		set processors_to_build [lreplace $processors_to_build $found $found ]
	}

	if {$inDevice == "BF538"} {
		if { [lsearch $processors_to_build BF534] == -1 } {
			append processors_to_build " BF534"
		} 
		set found [lsearch $processors_to_build BF538]
		# delete this processor from the list
		set processors_to_build [lreplace $processors_to_build $found $found ]
	}
	if {$inDevice == "BF539"} {
		if { [lsearch $processors_to_build BF534] == -1 } {
			append processors_to_build " BF534"
		} 
		# delete this processor from the list
		set found [lsearch $processors_to_build BF539]
		set processors_to_build [lreplace $processors_to_build $found $found ]
	}

	if {$inDevice == "BF504" || $inDevice == "BF504F"} {
		if { [lsearch $processors_to_build BF06F] == -1 } {
			append processors_to_build " BF506F"
		} 
		set found [lsearch $processors_to_build BF504F]
		# delete this processor from the list
		set processors_to_build [lreplace $processors_to_build $found $found ]
		set found [lsearch $processors_to_build BF504]
		# delete this processor from the list
		set processors_to_build [lreplace $processors_to_build $found $found ]
	}

}

####################################################################
# Set whether different types of libraries (for the same silicon revision)
# need to be built (byte adressing, swf etc)
####################################################################

proc SelectLibraryTypes {inDevice} {
	global LibraryTypes
	global VerboseOutput
	global Blackfin
	if {$inDevice == "21160"} {
		set LibraryTypes "normal swf"
	} elseif {$inDevice == "21469" || $inDevice == "21479"} {
		set LibraryTypes "nw sw"
	} elseif {[lsearch $Blackfin $inDevice] != -1} {
        	set LibraryTypes "normal sov"
	} else {
		set LibraryTypes "ignored"
	}

	if {$VerboseOutput} {
		puts "SelectLibraryTypes for $inDevice returned $LibraryTypes"
	}

}

####################################################################
# Select silicon revisions
####################################################################
# If no si revision is specified select which ones should be built
proc SelectRevisions {inDevice} {
	global revisions_to_build
	global BF506F_revisions 
	global BF512_revisions 
	global BF522_revisions 
	global BF523_revisions 
	global BF534_revisions 
	global BF532_revisions 
	global BF535_revisions 
	global BF542_revisions 
	global BF561_revisions 
	global BF592A_revisions 
	global 21060_revisions
	global 21160_revisions
	global 21261_revisions
	global 21364_revisions
	global 21369_revisions
	global 21371_revisions
	global 21469_revisions
	global 21479_revisions
	global VerboseOutput

	if { $inDevice == "BF535" } {
		set revisions_to_build $BF535_revisions
	} elseif { $inDevice == "BF532" } {
		set revisions_to_build $BF532_revisions
	} elseif { $inDevice == "BF534" } {
		set revisions_to_build $BF534_revisions
	} elseif { $inDevice == "BF561" } {
		set revisions_to_build $BF561_revisions
	} elseif { $inDevice == "BF506F" } {
		set revisions_to_build $BF506F_revisions
	} elseif { $inDevice == "BF512" } {
		set revisions_to_build $BF512_revisions
	} elseif { $inDevice == "BF522" } {
		set revisions_to_build $BF522_revisions
	} elseif { $inDevice == "BF523" } {
		set revisions_to_build $BF523_revisions
	} elseif { $inDevice == "BF542" } {
		set revisions_to_build $BF542_revisions
	} elseif { $inDevice == "BF592-A" } {
		set revisions_to_build $BF592A_revisions
	} elseif { $inDevice == "21060" } {
		set revisions_to_build $21060_revisions
	} elseif { $inDevice == "21160" } {
		set revisions_to_build $21160_revisions
	} elseif { $inDevice == "21261" } {
		set revisions_to_build $21261_revisions
	} elseif { $inDevice == "21364" } {
		set revisions_to_build $21364_revisions
	} elseif { $inDevice == "21369" } {
		set revisions_to_build $21369_revisions
	} elseif { $inDevice == "21371" } {
		set revisions_to_build $21371_revisions
	} elseif { $inDevice == "21469" } {
		set revisions_to_build $21469_revisions
	} elseif { $inDevice == "21479" } {
		set revisions_to_build $21479_revisions
	}

	if {$VerboseOutput} {
		puts "SelectRevisions for $inDevice returned $revisions_to_build"
	}
	
}

####################################################################
# Select the name of the library that we are going to create
# For example all blackfin (but not wireless) are called TMK-BF532
# and wireless are called TMK-BF535
####################################################################

proc SelectLibraryName {inDevice outFile} {
	global VerboseOutput
	global BuildType
	if {$inDevice == "BF561"} {
		set myLibName "LIBRARY=TMK-BF561"
	} elseif {$inDevice == "BF532" || $inDevice == "BF534" || $inDevice == "BF522" || $inDevice == "BF523" || $inDevice == "BF512" || $inDevice == "BF506F" || $inDevice == "BF542" || $inDevice == "BF592-A"} {
		set myLibName "LIBRARY=TMK-BF532"
	} elseif {$inDevice == "BF535" } {
		set myLibName "LIBRARY=TMK-BF535"
	} elseif {$inDevice == "21060" } {
		set myLibName "LIBRARY=TMK-210XX"
	} elseif {$inDevice == "21160" } {
		set myLibName "LIBRARY=TMK-211XX"
	} elseif {$inDevice == "21261" } {
		set myLibName "LIBRARY=TMK-212XX"
	} elseif {$inDevice == "21364" || $inDevice == "21369"} {
		set myLibName "LIBRARY=TMK-213XX"
	} elseif {$inDevice == "21371" } {
		set myLibName "LIBRARY=TMK-2137X"
	} elseif {$inDevice == "21469" || $inDevice == "21479"} {
		set myLibName "LIBRARY=TMK-2146X"
	}

	if {$VerboseOutput} {
		puts "SelectLibraryName for $inDevice returned $myLibName"
	}

	puts  $outFile $myLibName
}

####################################################################
# Select the family install folder for the particular processor. 
####################################################################

proc SelectInstallFamilyFolder {inDevice inTargetFamily} {
	global VerboseOutput
	if {$inDevice == "21160"} {
    	set myInstallFolder "211xx"
	} elseif {$inDevice == "21060" } {
    	set myInstallFolder "21k"
	} elseif {$inDevice == "21261" } {
    	set myInstallFolder "212xx"
	} elseif {$inDevice == "21364" } {
    	set myInstallFolder "213xx"
	} elseif {$inDevice == "21369" } {
    	set myInstallFolder "213xx"
	} elseif {$inDevice == "21371" } {
    	set myInstallFolder "213xx"
	} elseif {$inDevice == "21469" || $inDevice == "21479"} {
    	set myInstallFolder "214xx"
	} else {
    	set myInstallFolder $inTargetFamily
	}
	if {$VerboseOutput} {
		puts "SelectInstallFamilyFolder for $inDevice returned $myInstallFolder"
	}
	return $myInstallFolder
}

####################################################################
# Set compiler and procesors to build
####################################################################

   if { $target_family == "blackfin" } {
	set Compiler ccblkfn
	if { $target_device == "" } {
		set processors_to_build $blackfin_processors
	} else  {
		set processors_to_build $target_device
	}
   } elseif { $target_family == "sharc" } {
	set Compiler cc21k	
	if { $target_device == "" } {
	  if { $target_core == "2106x" } {
		set processors_to_build $2106x_processors
	  	} elseif { $target_core == "2116x" } {
			set processors_to_build $2116x_processors 
	  		} elseif { $target_core == "2126x" } {
				set processors_to_build $2126x_processors 
	  			} elseif { $target_core == "2136x" } {
					set processors_to_build $2136x_processors 
	  				} elseif { $target_core == "2137x" } {
						set processors_to_build $2137x_processors 
	  					} elseif { $target_core == "2146x" } {
							set processors_to_build $2146x_processors 
						} else  {
							set processors_to_build $sharc_processors
						}
	} else  {
		set processors_to_build $target_device
	}
   }

   if { $target_si_rev == "" } {
	set which_silicon all
   } else { 
	set which_silicon $target_si_rev
   }

# Check whether this processor and silicon revision combination requires
# building other libraries

foreach device $processors_to_build {
	AddOtherProcessors $device
}

if {$VerboseOutput} {
	puts "Creating variables.mk with the options required"
	puts "Libs to build:"
}
 
####################################################################
# Main loop
# Loop setting all the options and command line for particular processors
####################################################################

foreach device $processors_to_build {

	set LibraryTypes $cmdline_LibraryTypes
	set revisions_to_build ""

	if { $which_silicon == "all" } {
		SelectRevisions $device
	} else {
		set revisions_to_build $target_si_rev
	}
	if { $LibraryTypes == "" } {
		SelectLibraryTypes $device
	}
	
	if {$revisions_to_build == "" } {
		puts "**** ERROR. No silicon revisions found for $device"
		exit 1
	}

	foreach lib_type $LibraryTypes {
		foreach revision $revisions_to_build {
			# Go to the build folder
			if { $device == "21261"} {
				if { $revision == "any"} {
					set install_folder [concat 2126x_any]
				} else {
					set install_folder [concat 2126x_rev_${revision}]
				}
			} elseif { $device == "21364" || $device == "21371" } {
				if { $revision == "any"} {
					set install_folder [concat 2136x_any]
				} else {
					set install_folder [concat 2136x_rev_${revision}]
				}
			} elseif { $device == "21369"} {
				if { $revision == "0.1"} {
					set install_folder [concat 2136x_LX3_rev_0.1]
				} elseif {$revision == "any"} {
                    set install_folder [concat 21369_rev_any]
                } else {
                    puts "**** ERROR. Revision unrecognised for 21369"
                    exit 1
                }
			} elseif { $device == "21469" } {
				set install_folder [concat 21469_rev_${revision}]
			} elseif { $device == "21479" } {
				set install_folder [concat 21479_rev_${revision}]
			} elseif { $revision == "ignore" } {
				set install_folder ""
			} else {
				set install_folder [concat [string tolower ${device}]_rev_${revision}]
			}

			if { $lib_type == "byte_addressing" } {
				if { $revision == "ignore" } {
					set folder_name BA
				} else {
					set folder_name [concat ${install_folder}_ba]
				}
			} elseif { $lib_type == "swf" } {
				if { $revision == "ignore" } {
					set folder_name SWF
				} else {
					set folder_name [concat ${install_folder}_swf]
				}
			} elseif { $lib_type == "sw" } {
				if { $revision == "ignore" } {
					set folder_name SW
				} else {
					set folder_name [concat ${install_folder}_sw]
				}
			} elseif { $lib_type == "nw" } {
				if { $revision == "ignore" } {
					set folder_name NW
				} else {
					set folder_name [concat ${install_folder}_nw]
				}
            } elseif { $lib_type == "sov" } {
                if { $revision == "ignore" } {
                    set folder_name SOV
                } else {
                    set folder_name [concat ${install_folder}_sov]
                }
			} else {
				set folder_name ${install_folder}
			}


			
    		if {$VerboseOutput} {
				puts "   $target_family/$device/$folder_name"
			}

			if { ![file exists $target_family/$device/$folder_name] } {
				exec mkdir -p $target_family/$device/$folder_name
			}
			puts $sh_commands "echo "
			puts $sh_commands "echo \"** Building $target_family/$device/$folder_name ** $Script_OutputFile\" "
			puts $sh_commands "cd $target_family/$device/$folder_name"
			cd $target_family/$device/$folder_name
	
			# Create file with options in the right directory
   			set vbles_outout  [open variables.mk w]
	
			puts $vbles_outout " LIBDIR=$target_family/$device/$folder_name"
			# Add the library name to variables.mk
			SelectLibraryName $device $vbles_outout

			puts $vbles_outout "CC=$Compiler"
			puts $vbles_outout "FAMILY=$target_family"

			set install_family [SelectInstallFamilyFolder $device $target_family]
			if {$revision == "none"} {
				puts $vbles_outout "INSTALL_DIR=$install_dir/$install_family/lib"
			} else {
				puts $vbles_outout "INSTALL_DIR=$install_dir/$install_family/lib/$install_folder"
			}
   			if { $Vdsp_version_num != "4.0" } {
			    puts $vbles_outout "ATTRIBUTES=-file-attr ADI_OS=TMK -file-attr prefersMem=internal -file-attr prefersMemNum=30"
			}
			puts $vbles_outout "CCOPTS=-I. -I $Trinity_dir_dos"
			# -include TMK_warning_removal.h"

			# we are going to start with a new build so reset the 
			# VersionCOpt that we are putting in the version information
			set VersionCOpt ""

			if {$revision != "ignore"} {
				puts $vbles_outout "CCOPTS+=-si-revision $revision"
				AddToVersionCOpt "-si-revision $revision"
			} elseif {$device == "21060" || $device == "21160"} {
				puts $vbles_outout "CCOPTS+=-si-revision any"
				AddToVersionCOpt "-si-revision any"
			}
			if {$ReleaseBuild == 0} {
				puts $vbles_outout "CCOPTS+=-g"
				AddToVersionCOpt "-g"
			} else {
				puts $vbles_outout "CCOPTS+=-O"
				puts $vbles_outout "CCOPTS+= +small_debug"
				AddToVersionCOpt "-O"
			}
			if {$AssertingTMK == 0} {
				puts $vbles_outout "CCOPTS+=-DNDEBUG"
				puts $vbles_outout "CCOPTS+=-DTMK_INLINE=inline -DLCK_INLINE=inline "
			}
			if {$BuildType == "ROMIMG"} {
				puts $vbles_outout "BUILD_TYPE=ROM"
				puts $vbles_outout "CCOPTS+=-DBUILD_ROM_IMAGE"
				puts $vbles_outout "CCOPTS+=+data_threshold=0"
			} elseif {$BuildType == "ROMDEF"} {
				puts $vbles_outout "BUILD_TYPE=ROM"
				puts $vbles_outout "REMOVE_COMMON_OBJS=YES"
				puts $vbles_outout "CCOPTS+=-DBUILD_ROM_DEFINITION"
				puts $vbles_outout "CCOPTS+=+data_threshold=0"
            }

			puts $vbles_outout "CCOPTS+=-double-size-any"
			if { $target_family == "blackfin" } {
				puts $vbles_outout "CCOPTS+=-no-assume-vols-are-mmrs"
				puts $vbles_outout "CCOPTS+=-flags-asm -I\"$Trinity_dir_dos\" "
			} elseif { $target_family == "sharc" } {
				puts $vbles_outout "CCOPTS+=-no-circbuf"
				puts $vbles_outout "CCOPTS+=-flags-asm -Wwarn-error"

            }

			puts $vbles_outout "CCOPTS+=-Werror-warnings "
			puts $vbles_outout "CCOPTS+=-proc ADSP-$device"
			set VersionProcessor "ADSP-$device"

			if { $device == "21371" } {
				# this workaround must only be in non-assembly files. For
				# assembly we force the code to be in internal memory where
				# the anomaly does not apply
				puts  $vbles_outout "ASMOPTS += -flags-asm -no-anomaly-workaround,09000011"
			}
			if { $lib_type == "byte_addressing" } {
				puts  $vbles_outout "OPT=_ba"
				puts  $vbles_outout "CCOPTS +=-char-size-8"
				AddToVersionCOpt "-char-size-8"
			} elseif { $lib_type == "swf" } {
				puts  $vbles_outout "OPT=_SWF"
				puts  $vbles_outout "CCOPTS +=-workaround swfa"
				puts  $vbles_outout "CCOPTS +=-flags-asm -swf_screening_fix -flags-asm -D_ADI_SWFA"
			} elseif { $lib_type == "nw" } {
				puts  $vbles_outout "OPT=_nwc"
				puts  $vbles_outout "CCOPTS +=-nwc"
            } elseif { $lib_type == "sov" } {
                puts  $vbles_outout "OPT=_sov"
                puts  $vbles_outout "CCOPTS +=-DSTACK_OVERFLOW_DETECTION"
			}
   			close $vbles_outout
	
			# Copy the command line to run to the shell script
			puts $sh_commands "make -f $Trinity_dir/makefile.mk  $Target CC=$Compiler VPATH=$Trinity_dir_unix $Script_OutputFile 2>&1 "

			# Add a command to stop if there has been any problem with the 
			# previous make
			puts $sh_commands "if \[ $? -ne 0 \]; then cd $Trinity_dir; exit 1; fi"

			# Generate the version information file in the correct folder
			GenerateVersionInformation $VersionProcessor $VersionCOpt

			# Return to the top directory for the next processor
			puts $sh_commands "cd $Trinity_dir"
			cd $Trinity_dir
			}
		}
	}
	puts $sh_commands "echo \"**** ALL DONE ****\""
   	close $sh_commands
    if {$VerboseOutput} {
		puts "Building TMK"
		puts "sh sh_commands.sh $OutputFile"
	}

    exec  sh sh_commands.sh $OutputFile

