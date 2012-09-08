#include "Stdafx.h"

#include "PimShaderManager.h"
#include "Pim.h"

namespace Pim
{
	ShaderManager::ShaderManager()
	{
		defaultQuality = Quality::MED;
	}
	ShaderManager::~ShaderManager()
	{
	}

	void ShaderManager::addShaderFromFile(std::string fragFile, std::string vertFile, 
		std::string nm, Quality::Quality q)
	{
		std::string fragString;
		std::string vertString;

		std::ifstream file;
		int len;

		// Load the fragment shader
		file.open(fragFile, std::ios::in);
		PimAssert(file, "Error: could not load fragment shader.");

		while (file.good())
		{
			std::string tmp;
			file >> tmp;
			fragString.append(tmp);
		}

		file.close();

		// Load the vertex shader
		file.open(vertFile, std::ios::in);
		PimAssert(file, "Error: could not load vertex shader.");

		while (file.good())
		{
			std::string tmp;
			file >> tmp;
			vertString.append(tmp);
		}

		file.close();

		addShader(fragString, vertString, nm, q);
	}
	void ShaderManager::addShader(std::string fragString, std::string vertString, 
		std::string nm, Quality::Quality q)
	{
		Shader shader;
		
	}
}