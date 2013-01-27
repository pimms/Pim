#pragma once

#include "PimInternal.h"
#include "PimConsoleReader.h"

namespace Pim {
	class GameControl;
	class ShaderManager;

	class Shader {
	private:
		friend class ShaderManager;

	public:
								Shader();
								~Shader();
		GLint					GetProgram() const;
		GLint					GetUniformLocation(const string name);
		void					SetUniform1i(string n, GLint a);
		void					SetUniform2i(string n, GLint a, GLint b);
		void					SetUniform3i(string n, GLint a, GLint b, GLint c);
		void					SetUniform4i(string n, GLint a, GLint b, GLint c, GLint d);
		void					SetUniform1f(string n, GLfloat a);
		void					SetUniform2f(string n, GLfloat a, GLfloat b);
		void					SetUniform3f(string n, GLfloat a, GLfloat b, GLfloat c);
		void					SetUniform4f(string n, GLfloat a, GLfloat b, GLfloat c, GLfloat d);

	private:
		GLint					vert;
		GLint					frag;
		GLint					program;
		map<string,GLint>		uniform;

		void					SetUniformXi(string, GLint, GLint[]);
		void					SetUniformXf(string, GLint, GLfloat[]);
	};

	class ShaderManager : public ConsoleListener {
	private:
		friend class GameControl;

	public:
		static ShaderManager*	GetSingleton();
		static Shader			*AddShaderFromFile(string fragFile,string vertFile,string name);
		static Shader			*AddShader(string fragString,string vertString,string name);
		static Shader*			GetShader(string name);
		void					HandleCommand(ConsoleCommand cmd);
		void					ClearShaders();

	private:
		map<string, Shader*>	shaders;
		static ShaderManager*	singleton;

								ShaderManager();
								~ShaderManager();
		static void				InstantiateSingleton();
		static void				ClearSingleton();
	};
}