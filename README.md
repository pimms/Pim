PIM Alpha 0.9
=============

Pim is a 2D engine for Windows, with OSX and Linux support tagging along at some point in the future.
This engine is as much a learning project as a work in progress, and should be treated as such. If
you're looking for a game engine for a professional game, you should look elsewhere.

The structure of PIM is very similar to that of Cocos2D, wherein all game objects are nodes in a tree
with a Scene-object at the top of the hierarchy. See the documentation for more details.


Documentation
=============

Doxygen generated documents can be found at http://jstien.com/pimdoc. Tutorials and examples are few
and rare, but if you read about GameControl, Scene, Layer and Sprite you should know enough to get 
started. Any inquiries regarding poor documentation and/or questions regarding the engine may be 
directed to joakim.stien@gmail.com. I will happily aid you as best I can.


Contributions
=============

If you wish to contribute and develop for PIM, fork the repo and check out the issue-list for stuff
to do.


Your first project
==================

### Windows

For the time being, Visual Studio 2012 is required to link / build PIM.

Download PIM into any folder. Open up a command prompt, and run the following command. Notice the
"/bin" at the end.
	setx PIM_HOME dir/to/PIM/bin

If you have your new PIM-project open, close Visual Studio 2012 and try again,


# Create and configure new project

Open your project properties. In the top-left corner, set the 'Configuration'-tab to "All".

Create a new empty Win32 project. Add to 'Linker -> General -> Additional Library Directories':
		$(PIM_HOME)\lib\$(Configuration);

Add to 'Linker -> Input -> Additional Dependencies':
		PIM.lib;OpenGL32.lib;

Add to 'C/C++ -> General -> Additional Include Directories':
		$(PIM_HOME)\include;

Set 'C/C++ -> Code Generation -> Runtime Library' to:
		'Multithreaded Debug (MTd)'       for debug builds
		'Multithreaded (MT)'              for release builds

You should be set.


# Slap in some code

If you created an empty project, add a new cpp-file. Add the following code to it. If you get
any errors, go over the previous steps and ensure you've followed them correctly.

	#include "Pim.h"

	using namespace Pim;

	class TestLayer : public Layer {
	public:
		void LoadResources() {
			// Load sprites etc. here

			/*
			Sprite *sprite = new Sprite("image.png");
			AddChild(sprite);			// Layers need sprites
			*/
		}
	};

	class TestScene : public Scene {
		void LoadResources() {
			AddLayer(new TestLayer);	// Scenes need layers
		}
	};

	// As PIM uses SDL, it's crucial that you include both
	// parameters in main.
	int main(int argc, char *argv[]) {
		WinStyle::CreationData cd("PIM!", 800, 600, WinStyle::WINDOWED);
		
		GameControl gameControl;
		gameControl.Go(new TestScene, cd);

		return 0;
	}



### OSX

The OSX-version of PIM is currently too early in development for anyone to bother. Alpha version 1.0
will have full OSX support and premade templates for Win and OSX.

Should you absolutely insist, remove all files in the "PLUNDER"-directory and add the code from the 
last step of the Windows-setup tutorial. 


Credits
=======

The Pim engine is built upon, and would not be possible without the use of the following libraries.
A million thanks go out to all the contributors and developers of these libraries.


### OpenGL 

http://www.opengl.org/

License:
http://www.sgi.com/products/software/opengl/license.html


### SDL2.0 (6940)

http://www.libsdl.org

Liscense: 
http://www.libsdl.org/license.php


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

### TinyXML

http://www.grinninglizard.com/tinyxml/index.html

License:
Zlib (http://www.grinninglizard.com/tinyxmldocs/index.html)



License
=======

Pim is distributed freely under the BSD license. 


Copyright (c) 2012-2013, Joakim Nordstrand Stien
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
   Powered by PIM
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
