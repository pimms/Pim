#pragma once

#include "Stdafx.h"

namespace Pim
{
	// Forward declarations
	class GameControl;
	class ShaderManager;

	class Shader
	{
	public:
		Shader()
		{
			vert = -1;
			frag = -1;
			program = -1;
		}
		~Shader()
		{
			if (vert > 0)		glDeleteShader(vert);
			if (frag > 0)		glDeleteShader(frag);
			if (program > 0)	glDeleteProgram(program);
		}
		GLint getProgram()
		{ 
			return program; 
		}
		GLint getUniformLocation(std::string name);
		//GLint getAttribLocation(std::string name);


		// BOILERPLATE UNTIL PRIVATE MEMBERS
		void setUniform1i(std::string n, GLint a)	
				{ GLint arr[] = {a}; setUniformXi(n, 1, arr); }
		void setUniform2i(std::string n, GLint a, GLint b)
				{ GLint arr[] = {a,b}; setUniformXi(n, 2, arr); }
		void setUniform3i(std::string n, GLint a, GLint b, GLint c)
				{ GLint arr[] = {a,b,c}; setUniformXi(n,3,arr); }
		void setUniform4i(std::string n, GLint a, GLint b, GLint c, GLint d)
				{ GLint arr[] = {a,b,c,d}; setUniformXi(n,4,arr); }

		void setUniform1f(std::string n, GLfloat a)
				{ GLfloat arr[] = {a}; setUniformXf(n,1,arr); }
		void setUniform2f(std::string n, GLfloat a, GLfloat b)
				{ GLfloat arr[] = {a,b}; setUniformXf(n,2,arr); }
		void setUniform3f(std::string n, GLfloat a, GLfloat b, GLfloat c)
				{ GLfloat arr[] = {a,b,c}; setUniformXf(n,3,arr); }
		void setUniform4f(std::string n, GLfloat a, GLfloat b, GLfloat c, GLfloat d)
				{ GLfloat arr[] = {a,b,c,d}; setUniformXf(n,4,arr); }

		/* ATTRIBUTES DID NOT WORK AS
		void setAttrib1i(std::string n, GLint a)	
				{ GLint arr[] = {a}; setAttribXi(n, 1, arr); }
		void setAttrib2i(std::string n, GLint a, GLint b)
				{ GLint arr[] = {a,b}; setAttribXi(n, 2, arr); }
		void setAttrib3i(std::string n, GLint a, GLint b, GLint c)
				{ GLint arr[] = {a,b,c}; setAttribXi(n,3,arr); }
		void setAttrib4i(std::string n, GLint a, GLint b, GLint c, GLint d)
				{ GLint arr[] = {a,b,c,d}; setAttribXi(n,4,arr); }

		void setAttrib1f(std::string n, GLfloat a)
				{ GLfloat arr[] = {a}; setAttribXf(n,1,arr); }
		void setAttrib2f(std::string n, GLfloat a, GLfloat b)
				{ GLfloat arr[] = {a,b}; setAttribXf(n,2,arr);}
		void setAttrib3f(std::string n, GLfloat a, GLfloat b, GLfloat c)
				{ GLfloat arr[] = {a,b,c}; setAttribXf(n,3,arr); }
		void setAttrib4f(std::string n, GLfloat a, GLfloat b, GLfloat c, GLfloat d)
				{ GLfloat arr[] = {a,b,c,d}; setAttribXf(n,4,arr); }
		*/
	private:
		friend class ShaderManager;

		void setUniformXi(std::string, GLint, GLint[]);
		void setUniformXf(std::string, GLint, GLfloat[]);
		//void setAttribXi(std::string, GLint, GLint[]);
		//void setAttribXf(std::string, GLint, GLfloat[]);

		// The vertex program ID
		GLint vert;

		// The fragment program ID
		GLint frag;

		// The program containing the vert and frag shaders
		GLint program;

		std::map<std::string,GLint>	uniform; 
		//std::map<std::string,GLint>	attrib;
	};

	class ShaderManager
	{
	public:
		static ShaderManager* getSingleton()
				{ return singleton; }

		static Shader* addShaderFromFile(std::string fragFile,std::string vertFile,std::string name);
		static Shader* addShader(std::string fragString,std::string vertString,std::string name);
		
		static Shader* getShader(std::string name);

	private:
		friend class GameControl;

		static void instantiateSingleton();
		static void clearSingleton();

		ShaderManager();
		~ShaderManager();

		std::map<std::string, Shader*> shaders;

		static ShaderManager *singleton;
	};
}