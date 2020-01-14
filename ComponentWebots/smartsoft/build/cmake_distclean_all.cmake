# --------------------------------------------------------------------------
#
#  Copyright (C) 2013 Alex Lotz
#
#        lotz@fh-ulm.de
#
#        Alex Lotz
#        University of Applied Sciences
#        Prittwitzstr. 10
#        D-89075 Ulm
#        Germany
#
#  This file is part of the "SmartSoft Communication Library".
#  It provides standardized patterns for communication between
#  different components.
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# --------------------------------------------------------------------------

# Execute uninstall command if available and remove the corresponding files
IF(EXISTS "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/cmake_uninstall.cmake")
	MESSAGE(STATUS "Execute uninstall command")
	EXECUTE_PROCESS(COMMAND /usr/bin/cmake -P "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/cmake_uninstall.cmake")
	MESSAGE(STATUS "Remove cmake uninstall files")
	EXECUTE_PROCESS(COMMAND /usr/bin/cmake -E remove "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/cmake_uninstall.cmake")
	IF(EXISTS "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/install_manifest.txt")
		EXECUTE_PROCESS(
			COMMAND /usr/bin/cmake -E remove "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/install_manifest.txt"
		)
	ENDIF(EXISTS "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/install_manifest.txt")
ENDIF(EXISTS "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/cmake_uninstall.cmake")

# remove the default CMakeCache.txt file
IF(EXISTS "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/CMakeCache.txt")
	MESSAGE(STATUS "Remove CMakeCache.txt")
	EXECUTE_PROCESS(COMMAND /usr/bin/cmake -E remove "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/CMakeCache.txt")
ENDIF(EXISTS "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/CMakeCache.txt")

# remove the CMakeFiles directory
IF(EXISTS "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/CMakeFiles")
	MESSAGE(STATUS "Remove directory /CMakeFiles")
	EXECUTE_PROCESS(COMMAND /usr/bin/cmake -E remove_directory "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/CMakeFiles")
ENDIF(EXISTS "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/CMakeFiles")

# remove the default Makefile (only available when using make)
IF(EXISTS "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/Makefile")
	MESSAGE(STATUS "Remove Makefile")
	EXECUTE_PROCESS(COMMAND /usr/bin/cmake -E remove "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/Makefile")
ENDIF(EXISTS "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/Makefile")

# remove the default cmake_install.cmake
IF(EXISTS "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/cmake_install.cmake")
	MESSAGE(STATUS "Remove cmake_install.cmake")
	EXECUTE_PROCESS(COMMAND /usr/bin/cmake -E remove "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/cmake_install.cmake")
ENDIF(EXISTS "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/cmake_install.cmake")

# remove the default Makefile (only available when using make)
IF(EXISTS "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/install_manifest.txt")
	MESSAGE(STATUS "Remove install_manifest.txt")
	EXECUTE_PROCESS(COMMAND /usr/bin/cmake -E remove "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/install_manifest.txt")
ENDIF(EXISTS "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/install_manifest.txt")

# now check for the cmake_generated_files.txt which contains all other cmake generated (custom) files
# remove each file separatelly
IF(NOT EXISTS "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/cmake_generated_files.txt")
  MESSAGE("Cannot find file that traces cmake generated files: \"/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/cmake_generated_files.txt\"")
ELSE(NOT EXISTS "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/cmake_generated_files.txt")
  FILE(READ "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/cmake_generated_files.txt" files)
  STRING(REGEX REPLACE "\n" ";" files "${files}")
  # LIST(REVERSE files)
  FOREACH (file ${files})
    MESSAGE(STATUS "Remove \"${file}\"")
    IF (EXISTS "${file}")
      EXECUTE_PROCESS(
        COMMAND /usr/bin/cmake -E remove "${file}"
        OUTPUT_VARIABLE rm_out
        RESULT_VARIABLE rm_retval
      )
      IF (NOT ${rm_retval} EQUAL 0)
        MESSAGE(FATAL_ERROR "Problem when removing \"${file}\"")
      ENDIF (NOT ${rm_retval} EQUAL 0)
    ELSE (EXISTS "${file}")
      MESSAGE(STATUS "File \"${file}\" does not exist.")
    ENDIF (EXISTS "${file}")
  ENDFOREACH(file)

  EXECUTE_PROCESS(
    COMMAND /usr/bin/cmake -E remove "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/cmake_generated_files.txt"
    OUTPUT_VARIABLE rm_out
    RESULT_VARIABLE rm_retval
  )

  IF (NOT ${rm_retval} EQUAL 0)
    MESSAGE(FATAL_ERROR "Problem when removing \"/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/cmake_generated_files.txt\"")
  ENDIF (NOT ${rm_retval} EQUAL 0)

ENDIF(NOT EXISTS "/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/cmake_generated_files.txt")
