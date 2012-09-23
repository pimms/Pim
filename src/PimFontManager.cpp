#include "Stdafx.h"
#include "PimFontManager.h"
#include "PimGameControl.h"
#include "PimException.h"

namespace Pim
{
	FontManager* FontManager::singleton = NULL;


	FontManager* FontManager::getSingleton()
	{
		return singleton;
	}

	void FontManager::instantiateSingleton()
	{
		PimAssert(!singleton, "Error: singleton already instantiated (font mgr)");

		singleton = new FontManager;
	}
	void FontManager::clearSingleton()
	{
		PimAssert(singleton, "Erorr: singleton already cleared (font mgr)");

		delete singleton;
		singleton = NULL;
	}


	void FontManager::addFont(std::string fname)
	{
		PimAssert(fonts.count(fname) == 0, fname.append(": font already added"));

		
	}
}