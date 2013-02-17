#ifdef WIN32

#include "PimInternal.h"

#include <thread>
#include <conio.h>

#include "PimConsoleReader.h"
#include "PimGameControl.h"
#include "PimAssert.h"

namespace Pim {
ConsoleReader* ConsoleReader::singleton = NULL;

	/*
	=====================
	ConsoleReader::ConsoleReader
	=====================
	*/
	ConsoleReader::ConsoleReader() {
		DWORD threadID;

		quit = false;
		thread = CreateThread(
					 0,
					 0,
					 (LPTHREAD_START_ROUTINE)BeginLoop,
					 this,
					 0,
					 &threadID
				 );
		if (!threadID) {
			cout<<"ERROR: Failed to thread ConsoleReader!\n";
		}

		mutex = CreateMutex(NULL, false, "cmdmtx");
	}

	/*
	=====================
	ConsoleReader::~ConsoleReader
	=====================
	*/
	ConsoleReader::~ConsoleReader() {
		CloseHandle(thread);
		CloseHandle(mutex);
	}

	/*
	=====================
	ConsoleReader::BeginLoop
	=====================
	*/
	void ConsoleReader::BeginLoop(void *param) {
		ConsoleReader *reader = static_cast<ConsoleReader*>(param);
		reader->ConsoleLoop();
	}

	/*
	=====================
	ConsoleReader::ConsoleLoop
	=====================
	*/
	void ConsoleReader::ConsoleLoop() {
		while (!quit) {
			if (_kbhit()) {
				char input[80];
				cin.getline(input,80);

				DWORD ret = WaitForSingleObject(mutex, INFINITE);
				if (ret == WAIT_OBJECT_0) {
					PrepareCommand(input);
				}

				ReleaseMutex(mutex);
			}
			Sleep(100);
		}
	}

	/*
	=====================
	ConsoleReader::PrepareCommand
	=====================
	*/
	void ConsoleReader::PrepareCommand(const char *cmd) {
		// Split the command into individual words
		int begin = 0;
		int end = 0;
		for (unsigned i=0; i<strlen(cmd)+1; i++) {
			if (cmd[i] == ' ' || cmd[i] == '\0') {
				if (begin != end) {
					char *sub = new char[end-begin+1];
					memcpy(sub, &cmd[begin], end-begin);
					sub[end-begin] = '\0';
					command.push_back(sub);
					delete sub;
				}
				begin = ++end;
			} else {
				end++;
			}
		}
	}

	/*
	=====================
	ConsoleReader::Dispatch
	=====================
	*/
	void ConsoleReader::Dispatch() {
		if (singleton) {
			/*	Waiting for the mutex to be ready seems to only slow things down.	*/
			/*	Uncomment at own will.												*/

			//DWORD ret = WaitForSingleObject(singleton->mutex, 0);
			//if (ret == WAIT_OBJECT_0) {
				singleton->DispatchCommand();
			//}
			//ReleaseMutex(singleton->mutex);
		}
	}

	/*
	=====================
	ConsoleReader::DispatchCommand
	=====================
	*/
	void ConsoleReader::DispatchCommand() {
		if (command.size()) {
			for (unsigned int i=0; i<listeners[command[0]].size(); i++) {
				for each (ConsoleListener *l in listeners[command[0]]) {
					l->HandleCommand(command);
				}
			}

			command.clear();
		}
	}

	/*
	=====================
	ConsoleReader::AddListener
	=====================
	*/
	void ConsoleReader::AddListener(ConsoleListener *lst, const char *cmd) {
		if (singleton) {
			singleton->listeners[cmd].push_back(lst);
		}
	}

	/*
	=====================
	ConsoleReader::RemoveListener
	=====================
	*/
	void ConsoleReader::RemoveListener(ConsoleListener *lst) {
		if (singleton) {
			for (auto it=singleton->listeners.begin();
				it != singleton->listeners.end(); it++) {
				for (unsigned int i=0; i<it->second.size(); i++) {
					if (it->second[i] == lst) {
						it->second.erase(it->second.begin() + i);
					}
				}
			}
		}
	}

	/*
	=====================
	ConsoleReader::Begin
	=====================
	*/
	void ConsoleReader::Begin() {
		cout<<"\nIMPORTANT NOTICE:\n"
				 <<"If you try to close the window and nothing happens, focus the\n"
				 <<"console window and press enter. The window should now close.\n\n";
		PimAssert(singleton == NULL, "Error: ConsoleReader already instantiated!");

		singleton = new ConsoleReader;
	}

	/*
	=====================
	ConsoleReader::ShutDown
	=====================
	*/
	void ConsoleReader::ShutDown() {
		singleton->quit = true;

		WaitForSingleObject(singleton->thread, INFINITE);

		delete singleton;
	}
}

#endif /* WIN32 */
