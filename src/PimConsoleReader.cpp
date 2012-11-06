#include "PimInternal.h"
#include <thread>
#include <conio.h>

#include "PimConsoleReader.h"
#include "PimGameControl.h"
#include "PimException.h"

namespace Pim
{
	ConsoleReader* ConsoleReader::singleton = NULL;

	void ConsoleReader::beginLoop(void *param)
	{
		ConsoleReader *reader = static_cast<ConsoleReader*>(param);
		reader->consoleLoop();
	}
	void ConsoleReader::consoleLoop()
	{
		while (!quit)
		{
			if (kbhit())
			{
				char input[80];
				std::cin.getline(input,80);

				DWORD ret = WaitForSingleObject(mutex, INFINITE);
				if (ret == WAIT_OBJECT_0)
					prepareCommand(input);
				ReleaseMutex(mutex);
			}
		}
	}
	void ConsoleReader::prepareCommand(const char *cmd)
	{
		// Split the command into individual words
		int b=0, e=0;
		for (int i=0; i<strlen(cmd)+1; i++)
		{
			if (cmd[i] == ' ' || cmd[i] == '\0')
			{
				if (b != e)
				{
					char *sub = new char[e-b+1];
					memcpy(sub, &cmd[b], e-b);
					sub[e-b] = '\0';
					command.push_back(sub);
					delete sub;
				}
				b = ++e;
			}
			else
			{
				e++;
			}
		}
	}

	void ConsoleReader::dispatch()
	{
		if (singleton)
		{
			/*	Not particularly thread-safe. No issues detected thus far.			*/
			/*	Waiting for the mutex to be ready seems to only slow things down.	*/
			/*	Uncomment at own will.												*/

			//DWORD ret = WaitForSingleObject(singleton->mutex, 0);
			//if (ret == WAIT_OBJECT_0)
				singleton->dispatchCommand();
			//ReleaseMutex(singleton->mutex);
		}
	}
	void ConsoleReader::dispatchCommand()
	{
		if (command.size())
		{
			// Notify shit
			for (unsigned int i=0; i<listeners[command[0]].size(); i++)
			{
				for each (ConsoleListener *l in listeners[command[0]])
				{
					l->handleCommand(&command);
				}
			}

			command.clear();
		}
	}

	void ConsoleReader::addListener(ConsoleListener *lst, const char *cmd)
	{
		singleton->listeners[cmd].push_back(lst);
	}
	void ConsoleReader::removeListener(ConsoleListener *lst)
	{
		for (	auto it=singleton->listeners.begin(); 
				it != singleton->listeners.end(); it++)
		{
			for (unsigned int i=0; i<it->second.size(); i++)
			{
				if (it->second[i] == lst)
					it->second.erase(it->second.begin() + i);
			}
		}
	}

	ConsoleReader::ConsoleReader()
	{
		DWORD threadID;

		quit = false;
		thread = CreateThread(
					0, 
					0, 
					(LPTHREAD_START_ROUTINE)beginLoop, 
					this, 
					0, 
					&threadID
					);
		if (!threadID)
		{
			std::cout<<"ERROR: Failed to thread ConsoleReader!\n";	
		}

		mutex = CreateMutex(NULL, false, "cmdmtx");
	}
	ConsoleReader::~ConsoleReader()
	{
		CloseHandle(thread);
		CloseHandle(mutex);
	}

	void ConsoleReader::begin()
	{
		std::cout<<"\nIMPORTANT NOTICE:\n"
				 <<"If you try to close the window and nothing happens, focus the\n"
				 <<"console window and press enter. The window should now close.\n\n";
		PimAssert(singleton == NULL, "Error: ConsoleReader already instantiated!");
		
		singleton = new ConsoleReader;
	}
	void ConsoleReader::shutDown()
	{
		singleton->quit = true;

		WaitForSingleObject(singleton->thread, INFINITE);
		
		delete singleton;
	}
}