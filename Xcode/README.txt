Pim build description for OSX
=============================

Description
	It is only possible to build Pim under OSX in Xcode (3.x is not 
	tested). The project produces Pim.framework, and moves it to 
	"/Library/Frameworks/".
	
Building
	Open Pim.xcodeproj in Xcode 4, build the project and you're done.
	Some errors may occur from the custom script that is moving
	some folders around in the produced Framework. If this happens,
	delete the "Build" and "bin" folders manually and build again.

Linking
	When linking, drag "/Library/Frameworks/Pim.framework" into your
	project. Also drag "Pim/Xcode/SDL2.framework" into your project.
	Build & run, you should be set.
