Pim setup tutorial
==================

Getting, preparing and setting up Pim is quite painless if you got some clue of what you're doing.


## Important notice ##
 
Currently, you are only able to link to the Pim binaries if you're using Visual
Studio 2012. If you wish to use Pim with a previous version of Pim, you have to
build Pim yourself from source. All the dependencies are included in the "src"
directory, and it shouldn't be too painful. The instructions for building Pim
from source is lower in this document.


## 1. Downloading Pim ##

Click the "ZIP" button located somewhere above - or clone the repository via git. 
Put the entire folder somewhere easily locatable. If you're cloning from github, 
the default github-path will ensure that when you clone future updates, all your
project use the updated header files and binaries.


## 2. Setting the PIM_HOME variable ##

Start off by opening the command line (open cmd.exe), and type the following command:
>setx PIM_HOME  <path\to\Pim>

The path must be the parent folder of the "include" and the "lib" folders, which by default
is located in the "bin" directory. $(PIM_HOME)\include should be the folder of the header files.


## 3. Creating a new Pim project ##

You could either use the provided "pim_template.zip" file to use a project template, or you
could create a project from existing files. If you're using the template, you can stop reading.
Otherwise, stay a while and listen.

Create a new Win32 console application. That's it for now. Let's get linkin'.


## 4. Linking to Pim ##

In the top left corner, select "All Configurations" from the Configuration selection menu.

Open the project properties. Under the "C/C++"->"General" tab, add the following path
to the "Additional include directories"-field:

>$(PIM_HOME)\include;

Great. Now head to the "Linker"->"General" tab, and add the following path to the
"Additional library directories"-field:

>$(PIM_HOME)\lib;

Fantastic! Now set the Configuration selection menu to "Debug". 

Under the "Linker"->"Input" tab, add the following to the "Additional
dependencies":

>OpenGL32.lib;Pim_d.lib;

Change the Configuration selection to "Release", and add the following under the 
"Linger"->"Input" tab:

>OpenGL32.lib;Pim.lib;

And you're good to go!


## 5. Your first project ##

The following snippet will quickly throw an image up to the screen:

>#include "Pim.h"			// Include the Pim library
>#include <math.h>			// need cosf and sinf
>
>class MyPimLayer : public Pim::Layer 					// Our Layer-class
>{
>	void update(float dt)								// Called every frame if you listen to the frame
>	{
>		time += dt;
>
>		mySprite->position = Pim::Vec2( 				// Move sprite in circle
>			400.f + 100*cosf(time),
>			300.f + 100*sinf(time)
>			);
>	}
>	void loadResources()								// Use this method to instantiate your objects
>	{
>		time = 0.f;
>		
>		mySprite = new Pim::Sprite("image.png");		// Only PNG's are supported
>		mySprite->position = Pim::Vec2(400.f, 300.f);
>
>		listenFrame();									// Schedule self for updates (each frame)
>	}
>
>	float time;
>	Pim::Sprite *mySprite;
>};
>
>int main()
>{
>		// The game controller is the main object of the Pim library
>	Pim::GameControl *gc = new Pim::GameControl;	
>		
>	Pim::WinStyle::CreationData cd(		// The window describer
>		"My Pim App", 					// Window title
>		600, 4000, 						// Window resolution
>		Pim::WinStyle::WINDOWED 		// Window style
>		);
>
>	cd.forcedAspectRatio = true;					// Force AR. Does not stretch image, creates black borders
>	cd.aspectRatio = 4.f / 3.f;						// 4:3 aspect ratio
>	cd.coordinateSystem = Pim::Vec2(1.f, 1.f);		// Sets the coordinate of the top right corner. (0,0) is always bottom left.
>
>	gc->go(new MyPimLayer, cd);			// Hand control over to Pim. Send it the CreationData and an instance of MyPimLayer.
>	delete gc;							// Delete the game controller when the game quits.
>
>	return 0;
>}