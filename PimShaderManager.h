#pragma once

#include "Stdafx.h"

namespace Pim
{
	struct Shader
	{
		// The vertex program ID
		GLuint vert;

		// The fragment program ID
		GLuint frag;
	};

	class ShaderManager
	{
	public:
		ShaderManager();
		~ShaderManager();

		void addShaderFromFile(std::string fragFile, std::string vertFile,
			std::string name, Quality::Quality quality);
		void addShader(std::string fragString, std::string vertString, 
			std::string name, Quality::Quality quality);

		void setDefaultQuality(Quality::Quality q);

	private:
		std::map<std::string, std::map<Quality::Quality, Shader>> shaders;

		Quality::Quality		defaultQuality;
	};
}