#Pim v0.1 readme

Pim is a lightweight 2D game engine. There are many like it, etc..

PLEASE NOTE: Pim will only compile in debug mode currently.

#Setup

First of all, download the bin.zip file and place it somewhere you'll remember.

The following command will ease the linking and header paths, but is not required.
	$>setx PIM_HOME <dir_to_pim>

Open Visual Studio, and create a new Console Application. 

Add the following to your C++ header search path:
	$(PIM_HOME)\include

Obviously, replace $(PIM_HOME) with the path if you didn't setx it earlier.

Add the following library search path:
	$(PIM_HOME)\lib

And add the following libraries to your linker input:
	OpenGL32.lib
	glut32.lib
	pim_d.lib			// DEBUG VERSION - use pim.lib for release (which isn't working currently)
	
Some more settings need to be changed in order for the compilation to go as planned.

Go to Project properties -> configuration properties -> C,C++ -> Code Generation 
and change the Runtime Library to "Multi threaded debug (/MTd)"

#Tutorial

This is coming later. Take a look at the Pim_demo project for guidelines.
Note that only the source files for the project is attached. Just following the 
guidelines above and add the files into your project. Remember to put the image
in the working directory - pim_demo/debug/ - where your .exe file is built.