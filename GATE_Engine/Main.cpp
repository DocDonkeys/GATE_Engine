#include <stdlib.h>
#include "Application.h"
#include "Globals.h"

// Old school memory leak detector (mmgr)
#ifdef _DEBUG
	//#define TEST_MEMORY_MANAGER
	#include "libs/mmgr/mmgr.h"
#endif

////VS Memory Leak Detector
//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>

// We need to include this here because SDL overwrites main()
#include "libs/SDL/include/SDL.h"
#pragma comment( lib, "libs/SDL/libx86/SDL2.lib" )
#pragma comment( lib, "libs/SDL/libx86/SDL2main.lib" )

//Profiler
#include "libs/Brofiler/Brofiler.h"
#pragma comment( lib, "libs/Brofiler/ProfilerCore32.lib" )

enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

int main(int argc, char ** argv)
{
	LOG("Starting game '%s'...", TITLE);

	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;
	Application* App = NULL;

	while (state != MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_CREATION:

			LOG("-------------- Application Creation --------------");
			App = new Application();
			state = MAIN_START;
			break;

		case MAIN_START:

			LOG("-------------- Application Init --------------");
			if (App->Init() == false)
			{
				LOG("Application Init exits with ERROR");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
				LOG("-------------- Application Update --------------");
			}

			break;

		case MAIN_UPDATE:
		{
			BROFILER_FRAME("MainLoop");
			int update_return = App->Update();

			if (update_return == UPDATE_ERROR)
			{
				LOG("Application Update exits with ERROR");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
			break;

		case MAIN_FINISH:

			LOG("-------------- Application CleanUp --------------");
			if (App->CleanUp() == false)
			{
				LOG("Application CleanUp exits with ERROR");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}
	}

	delete App;
	LOG("Exiting '%s'...\n", TITLE);

	//mmgr Mem Leak Detector
#ifdef _DEBUG
	int leaks = MAX(0, m_getMemoryStatistics().totalAllocUnitCount - 23);
	LOG("With %d memory leaks!\n", (leaks > 0) ? leaks : 0);
	SDL_assert(leaks <= 0);
#endif

	//VS Mem Leak Detector
	//_CrtDumpMemoryLeaks();	

	return main_return;
}