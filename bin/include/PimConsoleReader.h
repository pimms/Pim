#pragma once

#include <map>
#include <vector>

/*
	Console commands in Pim are issued to listeners of a specific command.
	For instance, a Player-object can listen to the "player" command.
	Any arguments following "player" in the command line will be issued to
	the listeners, which must handle these messages individually.

	All ConsoleListener subclasses may listen for commands. GameNode (and all 
	it's subclasses) already inherit this class.

	Example of listening:
	
	Listen for "player" commands:
		player->listenCommand("player");

	Any message beginning with "player" is issued to the player-object in it's
	entirety. Note that it's stupidly important that the command BEGINS with the
	listened keyword.
*/

namespace Pim
{
	// Forward declarations
	class GameControl;
	class ConsoleListener;
	
	// The ConsoleReader class
	class ConsoleReader
	{
	public:
		// Called from the gameloop
		static void dispatch();

		static ConsoleReader* getSingleton() { return singleton; }
		static void addListener(ConsoleListener*, const char *cmd);
		static void removeListener(ConsoleListener*);

	private:
		friend class GameControl;

		ConsoleReader();
		~ConsoleReader();

		// In order to multithread in an object, it must be called from a static method.
		static void beginLoop(void*);

		// The main loop our console thread will work in.
		void consoleLoop();

		// When a command has been read, it must be prepared for dispatchment
		void prepareCommand(const char *cmd);

		void dispatchCommand();

		// Begin instantiates the singleton, and starts the loop in a new thread.
		static void begin();

		// Shutdown tells the thread to quit, and waits for it to die.
		static void shutDown();
		
		// Our thread handle
		HANDLE					thread;

		// Our mutex handle
		HANDLE					mutex;

		// Flagged to true when "shutDown()" is called
		bool					quit;

		// The listener objects, ordered by the commands to which they listen.
		std::map<std::string, std::vector<ConsoleListener*>> listeners;

		// Once a command has been prepared, the main thread needs to 
		// dispatch it in order to prevent funky behaviour.
		std::vector<std::string>	command;

		// The singleton object.
		static ConsoleReader	*singleton;
	};


	// The ConsoleListener class
	class ConsoleListener
	{
	public: 
		virtual ~ConsoleListener()	
		{
			ConsoleReader::removeListener(this);
		}

		virtual void handleCommand(std::vector<std::string> *cmd)	
		{
			// ...
		}

		void listenCommand(const char *cmd)
		{
			ConsoleReader::addListener(this, cmd);
		}
	};
}