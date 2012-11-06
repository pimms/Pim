0.5 changelog
==============

The following enum's accessibility has been altered:
>		Text::TextAlignment  has been changed to  Label::TextAlignment
>
>		Key::KeyCode  has been changed to  KeyEvent::KeyCode
>
>		Mouse::MouseButton  has been changed to  MouseEvent::MouseButton
>
>		Layer::loadResources() is now called 100% automatically 
>		when adding a layer to a scene.

The following classes have been added:
	
>		Pim::Button
>			A button class. It requires sprites for the different states it
>			can have (normal, hovered, pressed, deactivated), and is quite
>			versatile.

>		Pim::Slider
>			The Slider class utilizes a Pim::Button as the slider handle, and
>			as such it requires the same state-sprites as the Button-class. In
>			addition to this, it requires a background sprite and two vectors.

>		Pim::Scene
>			The new top-level node. Scene-objects are primarily layer-handlers,
>			and it is aimed to provide a more intuitive interface for dynamic
>			layer changing and hierarchy logic. The Scene objects in themselves
>			does absolutely nothing. The Scene class has a "loadResources()" method
>			you should utilize to load your resources. It is called automatically 
>			by the game controller when the Scene is set as the main scene. 

The engine hierarchy has been altered slightly. GameControl can no longer
work when only passed a layer - it must now be passed an instance of the
new class "Pim::Scene". This should make the engine more flexible and 
intuitive when it comes to game logic, layer hierarchy and HUD / UI.
Note that only one scene can be active at any given time.

The setup tutorial in the bottom section of this document has been altered
to reflect all new changes.


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
>setx PIM_HOME  [somedirectory]\Pim\bin

The path must be the parent folder of the "include" and the "lib" folders, which by default
is located in the "bin" directory. $(PIM_HOME)\include should be the folder of the header files.


## 3. Creating a new Pim project ##

You could either use the provided "pim_template.zip" file to use a project template, or you
could create a project from existing files. If you're using the template, you can stop reading.
Otherwise, stay a while and listen.

Create a new Win32 console application. That's it for now. Let's get linkin'.


## 4. Linking to Pim ##

Open up the project property-page by right clicking on the project name and select "properties".

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
"Linekr"->"Input" tab:

>OpenGL32.lib;Pim.lib;

And you're ALMOST good to go!

Finally under the project properties, navigate yourself to "C/C++"->"Code Generation".
Under the "Runtime Library" pane, set the value to be "/MTd" for the Debug Configuration,
and "/MT" for the Release Configuration. If you're getting spammed with error messages,
that is probably the reason.


## 5. Your first project ##

The following snippet will quickly throw an image up to the screen:

		#include "Pim.h"            // Include the Pim library  
		#include <math.h>           // need cosf and sinf

		class MyPimLayer : public Pim::Layer                    // Our Layer-class
		{
			void update(float dt)                               // Called every frame if you listen to the frame
			{
				time += dt;

				mySprite->scale = Pim::Vec2(
					1.f + cosf(time),
					1.f + sinf(time)
				);
			}
			void loadResources()                                // Use this method to instantiate your objects
			{
				time = 0.f;

				mySprite = new Pim::Sprite("image.png");        // Only PNG's are supported
				mySprite->position = Pim::Vec2(300.f, 200.f);
				addChild(mySprite);

				listenFrame();                                  // Schedule self for updates (each frame)
			}

			float time;
			Pim::Sprite *mySprite;
		};

		class MyScene : public Pim::Scene                       // Our Scene-class
		{
			void loadResources()                                // Called by the GameControl-object
			{
				MyPimLayer *layer = new MyPimLayer;             // Instantiate a layer
				addLayer(layer);                                // Add it to the scene. This calls "loadResources()" on the layer.
			}
		};

		int main()
		{
			// The game controller is the main object of the Pim library
			Pim::GameControl *gc = new Pim::GameControl;    

			Pim::WinStyle::CreationData cd(     // The window describer
				"Pim Application",              // Window title
				600, 400,                       // Window resolution
				Pim::WinStyle::WINDOWED         // Window style
				);

			cd.forcedAspectRatio = true;        // Force AR. Does not stretch image, creates black borders
			cd.aspectRatio = 3.f / 2.f;         // 4:3 aspect ratio

			gc->go(new MyScene, cd);            // Hand control over to Pim. Send it the CreationData and an instance of MyScene.
			delete gc;                          // Delete the game controller when the game quits.

			return 0;
		}



Credits
=======

The Pim engine is built upon, and would not be possible without the use of the following libraries.
A million thanks go out to all the contributors and developers of these libraries.


### OpenGL 

http://www.opengl.org/

License:
http://www.sgi.com/products/software/opengl/license.html


### FreeType 2.1.10

http://www.freetype.org

License:
http://www.freetype.org/FTL.TXT
Attached


### Ogg Vorbis

www.vorbis.com

License:
http://www.xiph.org/licenses/bsd/
Attached


### GLEW 1.9.0

http://glew.sourceforge.net/

License:
Attached


### libpng

http://www.libpng.org

License:
http://www.libpng.org/pub/png/src/libpng-LICENSE.txt
Attached


### zlib

http://www.zlib.net

License:
http://www.zlib.net/zlib_license.html


License
=======

Pim is distributed freely under the BSD license. 


Copyright (c) 2012, Joakim Nordstrand Stien
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by Joakim Stien.
4. Neither the name of the copyright holder nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY JOAKIM NORDSTRAND STIEN ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL JOAKIM NORDSTRAND STIEN BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
