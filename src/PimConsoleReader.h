#pragma once

#include <map>
#include <vector>


namespace Pim {
	/**
	 @class 	ConsoleReader
	 @availability Windows
	 
	 @brief		Reads the command line for input.
	 @details 	When the user inputs text to the command line window, the message
	 			is parsed by the ConsoleReader. If the @b first word is a match
	 			with any of the ConsoleListeners, those listeners are sent the
	 			full command.
	 */
	
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

	
	/**
	 @class 	ConsoleListener
	 @availability Windows. Non-functional on OSX.
	 @brief 	Inheritance from this class will alow CLI commands to be issued to you.
	 @details 	Console commands in Pim are issued to listeners of a specific command.
				 For instance, a Player-object can listen to the "player" command.
				 Any arguments following "player" in the command line will be issued to
				 the listeners, which must handle these messages individually.
	 
				 All ConsoleListener subclasses may listen for commands. GameNode (and all
				 it's subclasses) already inherit from this class.
	 
				 Any message beginning with "player" is issued to the player-object in it's
				 entirety.
	 */
	class ConsoleListener {
	public:
		virtual ~ConsoleListener() {
			ConsoleReader::RemoveListener(this);
		}
		virtual void HandleCommand(ConsoleCommand cmd) {
			// ...
		}
		void ListenCommand(const char *cmd) {
			ConsoleReader::AddListener(this, cmd);
		}
	};
#else
	class ConsoleListener {
	public:
		virtual ~ConsoleListener() { }
		virtual void HandleCommand(ConsoleCommand cmd) { }
		void ListenCommand(const char *cmd) { }
	};
#endif
}