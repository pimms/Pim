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



namespace Pim {
	class ConsoleListener;
	class GameControl;

	typedef vector<string>& ConsoleCommand;

#ifdef WIN32
	class ConsoleReader {
	private:
		friend class GameControl;

	public:
		static void								Dispatch();
		static ConsoleReader*					GetSingleton() { return singleton; }
		static void								AddListener(ConsoleListener*, 
															const char *cmd);
		static void								RemoveListener(ConsoleListener*);

	private:
		HANDLE									thread;
		HANDLE									mutex;
		bool									quit;
		map<string, vector<ConsoleListener*>>	listeners;
		vector<string>							command;
		static ConsoleReader					*singleton;

												ConsoleReader();
												~ConsoleReader();
		static void								BeginLoop(void*);
		void									ConsoleLoop();
		void									PrepareCommand(const char *cmd);
		void									DispatchCommand();
		static void								Begin();
		static void								ShutDown();
	};
#endif /* WIN32 */
}


namespace Pim {
    class ConsoleListener {
	public:
#ifdef WIN32
		virtual ~ConsoleListener() {
			ConsoleReader::RemoveListener(this);
		}
		virtual void HandleCommand(ConsoleCommand cmd) {
			// ...
		}
		void ListenCommand(const char *cmd) {
			ConsoleReader::AddListener(this, cmd);
		}
#else 
        virtual ~ConsoleListener() { }
        virtual void HandleCommand(ConsoleCommand cmd) { }
        void ListenCommand(const char *cmd) { }
#endif
	};
}


