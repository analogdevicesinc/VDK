# Declare our variables
  set SearchDir ""
  set Output ""

# Process the command line
  foreach arg $argv {
      switch -glob -- [string tolower $arg] {
          "-install_dir=*"         {
              set SearchDir [ split [join [lrange [split $arg "="] 1 end] "="] ","]
          }
          "-output=*"         {
              set Output [ split [join [lrange [split $arg "="] 1 end] "="] ","]
          }
      }
  }

# See if the output directory exists. If the file also exists remove it so it 
# is not found in the list of files.
  if { [file isdirectory [file dirname $Output]] } {
      if { [file exists $Output] } {
           exec rm $Output 
      }
  } else {
    puts "Can't create $Output. Directory does not exist."
    exit 1
  }

# See if the install directory exists. If so create a list of files. We have
# to find where cygwin is installed so we use the right find. Otherwise the
# first one in the path is the DOS one.

  if { [file isdirectory $SearchDir] } {
      cd $SearchDir
      set cygwin_install [exec cygpath -w /bin]
      set FilesInDir [exec $cygwin_install\\find . -type f -print]
  } else {
      puts "Directory $SearchDir does not exist"
      exit 1
  }

# We don't set the output until now so the file is not found in the list
  set out [open "$Output" w]

 
# Write the manifest file
# Header
  puts $out "<!-- Manifest for files delivered by the VDK group -->"
  puts $out "<flist>"
# Write the list of files in the release
  foreach File $FilesInDir {
      puts $out "\t<f p=\"[exec $cygwin_install\\cygpath -w $File]\" />"
  }
# End of the file
  puts $out "</flist>"
# Close the manifest
  close $out
