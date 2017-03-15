# winmoc
moc.exe wrapper

Compares last modified dates of input and output files, and executes moc.exe.

## Build
Build on VS2015 with Release x64 configuration.

## Usage
$ winmoc.exe [input_filename] -o [output_filename]

Only this format is accepted and other options are not supported.

## Requirements
Path to Qt5.7 is hard-coded in winmoc.cpp (C:\\Qt\\5.7\\msvc2015_64\\bin\\moc.exe). Change it accordingly.