#pragma once

#include "Stdafx.h"

namespace Pim
{
	class GameContol;

	class FontManager
	{
	public:
		static FontManager* getSingleton();

	private:
		friend class GameControl;

		static FontManager *singleton;

		static void instantiateSingleton();
		static void clearSingleton();

		void addFont(std::string);

		std::map<std::string, HFONT>	fonts;
	};
}