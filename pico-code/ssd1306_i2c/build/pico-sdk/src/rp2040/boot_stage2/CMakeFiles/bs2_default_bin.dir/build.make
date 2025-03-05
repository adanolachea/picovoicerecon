# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.30

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.30.4/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.30.4/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/adanolacheamoran/mycode/picovoicerecon/pico-code/ssd1306_i2c

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/adanolacheamoran/mycode/picovoicerecon/pico-code/ssd1306_i2c/build

# Utility rule file for bs2_default_bin.

# Include any custom commands dependencies for this target.
include pico-sdk/src/rp2040/boot_stage2/CMakeFiles/bs2_default_bin.dir/compiler_depend.make

# Include the progress variables for this target.
include pico-sdk/src/rp2040/boot_stage2/CMakeFiles/bs2_default_bin.dir/progress.make

pico-sdk/src/rp2040/boot_stage2/CMakeFiles/bs2_default_bin: pico-sdk/src/rp2040/boot_stage2/bs2_default.bin

pico-sdk/src/rp2040/boot_stage2/bs2_default.bin: pico-sdk/src/rp2040/boot_stage2/bs2_default.elf
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/Users/adanolacheamoran/mycode/picovoicerecon/pico-code/ssd1306_i2c/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating bs2_default.bin"
	cd /Users/adanolacheamoran/mycode/picovoicerecon/pico-code/ssd1306_i2c/build/pico-sdk/src/rp2040/boot_stage2 && /usr/local/bin/arm-none-eabi-objcopy -Obinary /Users/adanolacheamoran/mycode/picovoicerecon/pico-code/ssd1306_i2c/build/pico-sdk/src/rp2040/boot_stage2/bs2_default.elf /Users/adanolacheamoran/mycode/picovoicerecon/pico-code/ssd1306_i2c/build/pico-sdk/src/rp2040/boot_stage2/bs2_default.bin

bs2_default_bin: pico-sdk/src/rp2040/boot_stage2/CMakeFiles/bs2_default_bin
bs2_default_bin: pico-sdk/src/rp2040/boot_stage2/bs2_default.bin
bs2_default_bin: pico-sdk/src/rp2040/boot_stage2/CMakeFiles/bs2_default_bin.dir/build.make
.PHONY : bs2_default_bin

# Rule to build all files generated by this target.
pico-sdk/src/rp2040/boot_stage2/CMakeFiles/bs2_default_bin.dir/build: bs2_default_bin
.PHONY : pico-sdk/src/rp2040/boot_stage2/CMakeFiles/bs2_default_bin.dir/build

pico-sdk/src/rp2040/boot_stage2/CMakeFiles/bs2_default_bin.dir/clean:
	cd /Users/adanolacheamoran/mycode/picovoicerecon/pico-code/ssd1306_i2c/build/pico-sdk/src/rp2040/boot_stage2 && $(CMAKE_COMMAND) -P CMakeFiles/bs2_default_bin.dir/cmake_clean.cmake
.PHONY : pico-sdk/src/rp2040/boot_stage2/CMakeFiles/bs2_default_bin.dir/clean

pico-sdk/src/rp2040/boot_stage2/CMakeFiles/bs2_default_bin.dir/depend:
	cd /Users/adanolacheamoran/mycode/picovoicerecon/pico-code/ssd1306_i2c/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/adanolacheamoran/mycode/picovoicerecon/pico-code/ssd1306_i2c /Users/adanolacheamoran/pico-sdk/src/rp2040/boot_stage2 /Users/adanolacheamoran/mycode/picovoicerecon/pico-code/ssd1306_i2c/build /Users/adanolacheamoran/mycode/picovoicerecon/pico-code/ssd1306_i2c/build/pico-sdk/src/rp2040/boot_stage2 /Users/adanolacheamoran/mycode/picovoicerecon/pico-code/ssd1306_i2c/build/pico-sdk/src/rp2040/boot_stage2/CMakeFiles/bs2_default_bin.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : pico-sdk/src/rp2040/boot_stage2/CMakeFiles/bs2_default_bin.dir/depend

