set LibraryVersionNum "5.0.10"
set LibraryVersion "LIB_VERSION=$LibraryVersionNum"

set VisualStudio ""
set  Family ""
set Core ""
set Device ""
set Si_Rev ""
set target_family ""
set target_core ""
set target_device ""
set target_si_rev ""
set Target ""
set instfind_flgs ""
set Vdsp_version_num "5.0"
set Vdsp_version "VDSP_VERSION=$Vdsp_version_num"
set finderversion ""
set adi_dir_dos ""
set Install ""
set install_dir ""
set OutputFile "&"
set VerboseOutput 1
set TOP_DIR [pwd]
set ManifestFile ""
set GenerateManifest 0
set vdsp_make 0
set BuildMake "make.exe"
set IsADIMake 1
set tmk_includes 1
set tmk_dir "../Kernel"
set AssertFlags "NO_ASSERT_TMK=1"

# All 2106x processors
set 2106X [list 21060 21061 21062 21065L]
# All Hammerhead processors
set 2116X [list 21160 21161]
# All SHARC Ex processors
set 2126X [list 21262 21266 21261 21267]
# All SHARC Ex4 processors
set 2136X [list 21362 21363 21364 21365 21366 21367 21368 21369]
# All 2137X processors
set 2137X [list 21371 21375]
# All 2146X processors
set 2146X [list 21462 21465 21467 21469]
# All 2147X processors
set 2147X [list 21472 21475 21477 21478 21479]
# All 2148X processors
set 2148X [list 21482 21484 21485 21486 21487 21489]
# All SHARC processors (i.e. 2106X, Hammerhead, SHARC Ex)
set SHARC [concat $2106X $2116X $2126X $2136X $2137X $2146X $2147X $2148X]
# All Blackfin processors
set Blackfin [list BF535 BF532 BF531 BF533 BF561 BF539 BF534 BF536 BF537 BF566 BF538 BF542 BF544 BF547 BF548 BF549 BF522 BF523 BF524 BF525 BF526 BF527 BF512 BF514 BF516 BF504 BF504F BF506F BF592-A]

# All Nile processors
set TS1xx [list TS101]
# All Danube processors
set TS2xx [list TS201 TS202 TS203]
# All Tigersharc processors
set Tigersharc [concat $TS1xx $TS2xx]

set AllCores [list 10x 2106x 2116x 2126x 2136x 2137x 2146x 2147x 2148x ts1xx ts2xx]
set AllFamilies [list blackfin ts sharc]

#All Blackfin silicon revisions
set BF512_Revisions [list 0.0]
set BF514_Revisions [list 0.0]
set BF516_Revisions [list 0.0]
set BF522_Revisions [list 0.0]
set BF525_Revisions [list 0.0]
set BF527_Revisions [list 0.0]
set BF531_Revisions [list 0.1 0.2 0.3 0.5]
set BF532_Revisions [list 0.1 0.2 0.3 0.5]
set BF533_Revisions [list 0.1 0.2 0.3 0.5]
set BF534_Revisions [list 0.0 0.3]
set BF535_Revisions [list 0.2 1.0 1.1 1.2 1.3]
set BF536_Revisions [list 0.0 0.3]
set BF537_Revisions [list 0.0 0.3]
set BF539_Revisions [list 0.0 0.3]
set BF561_Revisions [list 0.2 0.3 0.4 0.5 any]
set BF566_Revisions [list 0.0 0.1]
set BF538_Revisions [list 0.0 0.3]
set BF542_Revisions [list 0.0]
set BF544_Revisions [list 0.0]
set BF547_Revisions [list 0.0]
set BF548_Revisions [list 0.0]
set BF549_Revisions [list 0.0]

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
set 21369_Revisions [list 0.0]
set 21371_Revisions [list 0.0]
set 21375_Revisions [list 0.0]
set 21462_Revisions [list 0.0]
set 21465_Revisions [list 0.0]
set 21467_Revisions [list 0.0]
set 21469_Revisions [list 0.0]
set 21472_Revisions [list 0.0]
set 21475_Revisions [list 0.0]
set 21477_Revisions [list 0.0]
set 21478_Revisions [list 0.0]
set 21479_Revisions [list 0.0]
set 21482_Revisions [list 0.0]
set 21484_Revisions [list 0.0]
set 21485_Revisions [list 0.0]
set 21486_Revisions [list 0.0]
set 21487_Revisions [list 0.0]
set 21489_Revisions [list 0.0]

# All TS Silicon revisions
set TS101_Revisions [list 0.0 0.1 0.2 0.4 1.0 1.1 1.2]
set TS201_Revisions [list 1.0 1.1 1.2]

# Master list of all supported processors
set AllProcessors [concat $2106X $2116X $2126X $2136X $2137X $2146X $2147X $2148X $Blackfin $Tigersharc ]

proc ShowParameters {} {
    puts "VDK_Build.tcl\n"
    puts "Command-line options:\n"
    puts "-assert"
    puts "        Builds libs to be used with the asserting TMK. "
    puts "-family=<family>"
    puts "        Family to build VDK for. "
    puts "        Families supported to present are:"
    puts "        blackfin sharc ts tigersharc "
    puts "-core=<core>"
    puts "        Core to build VDK for."
    puts "        Cores supported to present are:"
    puts "        Blackfin Family - 10x"
    puts "        SHARC Family - 2106x 2116x 2126x 2136x 2137x 2146x 2147x 2148x"
    puts "        TS Family - TS1XX TS2XX "
    puts "-device=<device>"
    puts "        Device to build VDK for. "
    puts "        Devices supported to present are:"
    puts "        Blackfin Family - BF535 BF532 BF531 BF533 BF561 BF539 BF534 BF536 BF537 BF566 BF538 BF542 BF544 BF547 BF548 BF549 BF522 BF523 BF524 BF525 BF526 BF527 BF512 BF514 BF516 BF504 BF504F BF506F BF592-A"
    puts "        SHARC Family - 21060 21061 21062 21065L 21160 21161 21262 21266 21261 21267 21362 21363 21364 21365 21366 21367 21368 21369 21371 21375 21462 21465 21467 21469 21472 21475 21477 21478 21479 21482 21484 21485 21486 21487 21489"
    puts "        TS Family - TS101 TS201 TS202 TS203 "
    puts "-target=<target1>,<target2>..."
    puts "        Comma-separated list of commands to be executed by gmake."
    puts "        Targets supported to present are:"
    puts "          clean debug release install squeaky "
    puts "          r_i r_e r_n "
    puts "          d_i d_e d_n "
    puts "          squeaky_offsets offsets"
    puts "        Blackfin specific - "
    puts "          r_i_csync r_e_csync r_n_csync "
    puts "          d_i_csync d_e_csync d_n_csync "
    puts "        Sharc specific - "
    puts "          release_swf debug_swf"
    puts "          r_i_swf r_e_swf r_n_swf "
    puts "          d_i_swf d_e_swf d_n_swf "
    puts "        Ts specific - "
    puts "          rel_predicted_branches debug_predicted_branches"
    puts "          rel_np rel_ba rel_ba_np"
    puts "          debug_np debug_ba debug_ba_np"
    puts "          r_i_np r_e_np r_n_np r_i_ba r_e_ba r_n_ba "
    puts "          r_i_ba_np r_e_ba_np r_n_ba_np "
    puts "          d_i_np d_e_np d_n_np d_i_ba d_e_ba d_n_ba "
    puts "          d_i_ba_np d_e_ba_np d_n_ba_np "
    puts "-manifest"
    puts "        Creation of manifest file. To be used with -install_dir"
    puts "        Unless specified otherwise, the file generated will be "
    puts "        install_dir/_manifest-vdk.xml."
    puts "-manifest_file=<filename>"
    puts "        Specifies the file to contain the vdk manifest."
    puts "-make=<executable>"
    puts "        Specifies the make to be used in the build. If the exe "
    puts "        is not in the path, the full path is required (no spaces"
    puts "        allowed)"
    puts "-no_tmk_update"
    puts "        Instructs the script not to copy the header files from"
    puts "        the Kernel folder"
    puts "-install_dir=<dir>"
    puts "        When doing and install which directory the files should be created in"
    puts "        The default is where VDSP++ is installed"
    puts "-output=<filename>"
    puts "        Redirects the output of the make command onto file filename"
    puts "-si-revision=<number|all>"
    puts "        Generates the libraries for the silicon revision specified. All will"
    puts "        generate the libraries for all the silicon revisions."
    puts "-tmk_dir=<dir>"
    puts "        Location of the microkernel where we will copy header files from."
    puts "        Default location ../Kernel"
    puts "-vdsp_version=<4.5|5.0>"
    puts "        Vdsp version to use. Default 4.5."
    puts "-vdsp_install=<dir_name>"
    puts "        Uses dir_name as the install location for VDSP++"
    puts "-vdsp_make"
    puts "        Uses vdsp make instead of the one from cygwin"
    puts "-verbose"
    puts "        Prints to stdout the command lines being run"
    exit 0;
}

##################################################################
# If silicon revision has been specified the procedure checks whether the
# libraries that we need to build are for a different revision (for example
# in TS101 the revision 0.0 and 0.1 mean 0.0 libraries). If the silicon
# revision specified matches what we need to build, the environment
# variable is already set
##################################################################
proc SetSiliconRevision {target_si_rev} {
	global target_device
	global TS101_Revisions
	global TS201_Revisions
	global BF532_Revisions
	global BF534_Revisions
	global BF535_Revisions
	global BF561_Revisions
	global BF566_Revisions
	global BF542_Revisions
	global 21267_Revisions
	global Si_Rev

	if {$target_si_rev == "" || $target_si_rev == "all" } {
	  set Si_Rev SI_REV=all
	}
	# silicon revision is specified. If no device is set then we fail
	if {$target_device == "" } {
		puts "si-revision not allowed without specifying device"
		exit 1
	}

	# device specific silicon revisions
	if {$target_device == "TS101"} {
        if {[lsearch $TS101_Revisions $target_si_rev] == -1} {
			puts "ERROR: $target_si_rev is not a valid revision for TS101 family"
			exit 1
		} elseif {$target_si_rev == 0.0 || $target_si_rev == 0.1 } {
			set Si_Rev SI_REV=0.0
		}
	} elseif {$target_device == "TS201" || $target_device == "TS202"  || $target_device == "TS203" } {
    	if {[lsearch $TS201_Revisions $target_si_rev] == -1} {
			puts "ERROR: $target_si_rev is not a valid revision for TS201 family"
			exit 1
		} elseif {$target_si_rev == 1.0 || $target_si_rev == 1.1 || $target_si_rev == 1.2} {
			set Si_Rev SI_REV=1.0
		}
	} elseif {$target_device == "BF532" || $target_device == "BF533" || $target_device == "BF531"} {
		# BF533 and BF531 revisions are the same as BF532
       	if {[lsearch $BF532_Revisions $target_si_rev] == -1} {
			puts "ERROR: $target_si_rev is not a valid revision for $target_device "
			exit 1
		}
	} elseif {$target_device == "BF534" || $target_device == "BF536"  || $target_device == "BF537" || $target_device == "BF538" || $target_device == "BF539"} {
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
			set Si_Rev SI_REV=0.2
		}
	} elseif {$target_device == "BF542" || $target_device == "BF544" || $target_device == "BF547" || $target_device == "BF548" || $target_device == "BF549"} {
		# BF542, BF544 BF547 BF548 and BF549 revision is the same as BF542
       	if {[lsearch $BF542_Revisions $target_si_rev] == -1} {
			puts "ERROR: $target_si_rev is not a valid revision for $target_device "
			exit 1
		}
	} elseif {$target_device == "BF561"} {
       	if {[lsearch $BF561_Revisions $target_si_rev] == -1} {
			puts "ERROR: $target_si_rev is not a valid revision for $target_device "
			exit 1
		} elseif {$target_si_rev == 0.4 } {
			set Si_Rev SI_REV=0.3
		}
	} elseif {$target_device == "21261" || $target_device == "21262"  || $target_device == "21266" || $target_device == "21267" } {
       	if {[lsearch $21267_Revisions $target_si_rev] == -1} {
			puts "ERROR: $target_si_rev is not a valid revision for $target_device "
			exit 1
		}
	}
}

foreach arg $argv {
	switch -glob -- [string tolower $arg] {

		"-assert"         {
			set AssertFlags "NO_ASSERT_TMK=0"

			}
		"-family=*"         {
			set target_family [string tolower [ split [join [lrange [split $arg "="] 1 end] "="] ","]]
			# we accept tigersharc too
			if { $target_family == "tigersharc" } {
   				set target_family "ts"
			}
			 set Family FAMILY=[string toupper $target_family] }

		"-device=*"         {
			set target_device [ string toupper [ split [join [lrange [split $arg "="] 1 end] "="] ","]]
			set Device DEVICE=[string toupper $target_device] }

		"-core=*"           {
			set target_core [ string tolower [ split [join [lrange [split $arg "="] 1 end] "="] ","]]
			set Core CORE=[string toupper $target_core] }

		"-si-revision=*"    {
			set target_si_rev [ string tolower [ split [join [lrange [split $arg "="] 1 end] "="] ","]]
			set Si_Rev SI_REV=[string tolower $target_si_rev] }

		"-target=*"         {
			set arg [split [join [lrange [split $arg "="] 1 end] "="] ","]
			append Target "$arg " }

		"-make=*"	{
			set BuildMake [ split [join [lrange [split $arg "="] 1 end] "="] ","]
			set IsADIMake 1 }
		"-no_tmk_update" {
			set tmk_includes 0 }
        "-vdsp_install=*" {
            set vdsp_install [join [lrange [split $arg "="] 1 end] "="]
            set instfind_flgs "-vdsp_install=$vdsp_install" }
		"-vdsp_make"	{
			set vdsp_make 1
			set IsADIMake 0 }
		"-vdsp_version=4.5" {
			set Vdsp_version_num 4.5
			set Vdsp_version VDSP_VERSION=$Vdsp_version_num}
		"-vdsp_version=5.0" {
			set Vdsp_version_num 5.0
			set Vdsp_version VDSP_VERSION=$Vdsp_version_num}
		"-output=*"         {
			set curr_dir [pwd]
			set tmp_out [ split [join [lrange [split $arg "="] 1 end] "="] ","]
			set OutputFile ">& $curr_dir/$tmp_out "  }

		"-verbose"          {
			set VerboseOutput 1 }
		"-install_dir=*"         {
			set install_dir [ split [join [lrange [split $arg "="] 1 end] "="] ","]
			set Install UP_INSTALL_DIR=$install_dir
			}
		"-manifest"	{
			set GenerateManifest 1
		}
		"-manifest_file=*"       {
			set ManifestFile  [ split [join [lrange [split $arg "="] 1 end] "="] ","]
			}
		"-tmk_dir=*"         {
			set tmk_dir [ split [join [lrange [split $arg "="] 1 end] "="] ","]
			}

		"-help"             {
			ShowParameters }

		default             {
			puts "\n Command line argument not recognised :\n\n$arg\n\nUse -help to display valid arguments.\n"
			exit 1 }
                }
        }

# Check that the device core and family given in the command line are valid

   if {$target_device != "" && [lsearch $AllProcessors $target_device] == -1} {
       puts "ERROR: Wrong device specified $target_device"
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
            set Family FAMILY=BLACKFIN
	        if { $target_core ==""} {
	        	set target_core 10x
             	set Core CORE=10x
			}

          }

          if {[lsearch $Tigersharc $target_device] != -1} {
	    set target_family ts
            set Family FAMILY=TS
	     if { $target_core ==""} {
               if {[lsearch $TS1xx $target_device] != -1} {
	         set target_core ts1xx
                 set Core CORE=TS1XX
               } elseif {[lsearch $TS2xx $target_device] != -1} {
	         set target_core ts2xx
                 set Core CORE=TS2XX
             }
          }
             if { $target_core != "ts2xx" && $target_core != "ts1xx" } {
                puts "ERROR: With Ts processors the only cores allowed are ts1xx and ts2xx "
                exit 1
	     }

          }

          if {[lsearch $SHARC $target_device] != -1} {
	    set target_family sharc
            set Family FAMILY=SHARC
	     if { $target_core ==""} {
               if {[lsearch $2106X $target_device] != -1} {
	         set target_core 2106x
                 set Core CORE=2106X
               } elseif {[lsearch $2116X $target_device] != -1} {
	         set target_core 2116x
                 set Core CORE=2116X
               } elseif {[lsearch $2126X $target_device] != -1} {
	         set target_core 2126x
                 set Core CORE=2126X
               } elseif {[lsearch $2136X $target_device] != -1} {
	         set target_core 2136x
                 set Core CORE=2136X
               } elseif {[lsearch $2137X $target_device] != -1} {
	         set target_core 2137x
                 set Core CORE=2137X
               } elseif {[lsearch $2146X $target_device] != -1} {
	         set target_core 2146x
                 set Core CORE=2146X
               } elseif {[lsearch $2147X $target_device] != -1} {
	         set target_core 2147x
                 set Core CORE=2147X
               } else {
	         set target_core 2148x
                 set Core CORE=2148X
               }
             }
          }
# we only got the core in the command line so we should work out the family
       } elseif {$target_core != ""} {
         if {$target_core == "2106x" || $target_core == "2116x" || $target_core == "2126x" || $target_core == "2136x" || $target_core == "2137x" || $target_core == "2146x" || $target_core == "2147x" || $target_core == "2148x"} {
            set target_family sharc
            set Family FAMILY=SHARC
         } elseif {$target_core == "10x"} {
	        set target_family blackfin
            set Family FAMILY=BLACKFIN
         } else {
            set target_family ts
            set Family FAMILY=TS
         }
       }
     }

 # Check that device core and family match for blackfin. The family is always
 # set by now

   if {$target_family == "blackfin"} {
      #if { $target_core!= "" } {
	#puts "ERROR: core cannot be specified with blackfin processors"
	#puts "       the core will be ignored"
	#set target_core ""
	#set Core ""
      #}
      if {$target_device != "" && [lsearch $Blackfin $target_device] == -1} {
	puts "ERROR: device $target_device is not a supported device of the blackfin family"
	exit 1
      }
    }

 # Check that device core and family match for ts. The family is always
 # set by now

   if {$target_family == "ts"} {
	if {$target_core != "" && $target_core != "ts1xx" && $target_core != "ts2xx"  } {
	puts "ERROR: core $target_core is not a supported core of the Ts family"
	exit 1
        }

      if {$target_device != "" && [lsearch $Tigersharc $target_device] == -1} {
	puts "ERROR: device $target_device is not a supported device of the TS family"
	exit 1
      }
   }

 # Check that device core and family match for sharc. The family is always
 # set by now

   if {$target_family == "sharc"} {
	if {$target_core != "" && $target_core != "2106x" && $target_core != "2116x" && $target_core != "2126x" && $target_core != "2136x" && $target_core != "2137x" && $target_core != "2146x" && $target_core != "2147x" && $target_core != "2148x"} {
	puts "ERROR: core $target_core is not a supported core of the SHARC family"
	exit 1
        }

      if {$target_device != "" && [lsearch $SHARC $target_device] == -1} {
	puts "ERROR: target $target_device is not a supported device of the SHARC family"
	exit 1
      }
   }

 # Set the silicon revision to the appropriate value (in some cases a silicon
 # revision is equivalent to another so we set it correctly"
 # We also check that the given silicon revision is in range with the supported
 # ones

  if {$target_si_rev != ""} {
	SetSiliconRevision $target_si_rev
  }

# Check that if we want to generate a manifest, the install directory has been
# specified
  if {$GenerateManifest} {
    if {$install_dir == "" } {
	puts "VDK_Build error. Manifest option requires install_dir."
	exit 1
    }
    if {$ManifestFile == ""} {
	set ManifestFile "$install_dir/_manifest-vdk.xml"
    }
  }
# If debug is one of the targets and install is another one, change install to
# install_debug so we don't copy the release libs.
  if { [string first debug $Target ] != -1  || [string first d_ $Target ] != -1 } {
      if { [string first install $Target ] != -1  && [string first install_debug $Target ] == -1 } {
         puts "Changing install to install_debug "
	 regsub -all install $Target install_debug Target
      }
  }

   if { $Vdsp_version_num == "4.5" } {
	set finderversion "-VDSP_4.5"
   } elseif { $Vdsp_version_num == "5.0" } {
	set finderversion "-VDSP_5.0"
   } else {
	set finderversion "-VDSP_4.5"
   }

   #Create the installfinder with MSdev

            if {[array names env DevEnvDir] != ""  && [file exists $env(DevEnvDir)/VCExpress.exe] != 0 } {
                    set VisualStudio VSTUDIO=2005_EXPRESS
            } elseif {[array names env DEVENVDIR] != ""  && [file exists $env(DEVENVDIR)/VCExpress.exe] != 0 } {
                    set VisualStudio VSTUDIO=2005_EXPRESS
            } elseif {[array names env DevEnvDir] != ""  && [file exists $env(DevEnvDir)/devenv.exe] != 0 } {
                    set VisualStudio VSTUDIO=2005_FULL
            } elseif {[array names env DEVENVDIR] != ""  && [file exists $env(DEVENVDIR)/devenv.exe] != 0 } {
                    set VisualStudio VSTUDIO=2005_FULL
            } elseif {[array names env VSINSTALLDIR] != ""  && [file exists $env(VSINSTALLDIR)/devenv.exe] != 0 } {
                    set VisualStudio VSTUDIO=2003
            } elseif {[array names env MSDEVDIR] != ""  && [file exists $env(MSDEVDIR)/Bin/MSDEV.exe] != 0 } {
                    set VisualStudio VSTUDIO=6
            }

  if {$VerboseOutput} {
    puts "Finding install path"
  }

   # Read the registry and see where the install is.
   if {$instfind_flgs == "" } {
       if {$VerboseOutput} {
           puts "utilities\\installfinder\\Debug\\installfinder -unix_style $finderversion"
       }
       if {[catch {set adi_dir [exec utilities\\installfinder\\Debug\\installfinder -unix_style $finderversion] } u] != 0} {
	    puts "The install path was not found"
	    puts $u
	    exit 1
	    #Return -1, compile-time failure.
       }
   } else {
       if {$VerboseOutput} {
           puts "utilities\\path_converter\\path_converter -unix_style $instfind_flgs"
       }
       if {[catch {set adi_dir [exec utilities\\path_converter\\path_converter -unix_style $instfind_flgs] } u] != 0} {
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
	puts "utilities\\path_converter\\path_converter $dir"
  }

  if {[catch {set adi_dir_dos [exec utilities\\path_converter\\path_converter $dir ] } u] != 0} {
        puts "String could not be converted to DOS style"
	    puts $u
	    exit 1
   }

  # Select gmake
  if {$vdsp_make == 1} {
    set BuildMake  $adi_dir/gmake-378
  }

  # If necessary, copy the microkernel header files to the right location
  if {$tmk_includes} {
    if {$VerboseOutput} {
      puts "Copying microkernel header files to common directory"
      puts "cp $tmk_dir/LCK.h common "
      puts "cp $tmk_dir/TMK.h common "
      puts "cp $tmk_dir/ADI_attributes.h common "
    }
      exec cp $tmk_dir/LCK.h common
      exec cp $tmk_dir/TMK.h common
      exec cp $tmk_dir/ADI_attributes.h common

  }

# Add the directory to the path
  append env(PATH) ";$adi_dir_dos"
  if {$VerboseOutput} {
    puts "Adding install dir to path"
    puts "  $adi_dir_dos "
  }
  if {$Target != ""} {
    cd common

  # Start building
    if {$VerboseOutput} {
      puts "Running gmake:"
      puts "  $BuildMake -f makefile.mk  $VisualStudio $Family $Core $Device $Si_Rev $LibraryVersion $Vdsp_version $Target $Install ${AssertFlags} $OutputFile "
    }

    puts "eval exec  $BuildMake -f makefile.mk $VisualStudio $Family $Core $Device $Si_Rev $LibraryVersion $Vdsp_version $Target ADI_MAKE=$IsADIMake $Install TOP_DIR=${TOP_DIR} ${AssertFlags} $OutputFile"
  }


# Generation of manifest file
  if {$GenerateManifest} {
    if {$VerboseOutput} {
      puts "Generating manifest file "
      puts "tclsh83 Utilities/GenerateManifest.tcl -install_dir=$install_dir -output=$ManifestFile"
    }
    eval exec tclsh83 Utilities/GenerateManifest.tcl -install_dir=$install_dir -output=$ManifestFile
  }
