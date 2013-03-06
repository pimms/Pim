#pragma once

#include "PimInternal.h"
#include "PimConsoleReader.h"

namespace Pim {
	class GameControl;
	class ShaderManager;
	
	/**
	 @class 		Shader
	 @brief 		Wrapper around an OpenGL-shader.
	 @details 		This class provides an interface for quick modification
	 				of uniforms and binding of the shader.
	 */
	class Shader {
	private:
		friend class ShaderManager;

	public:
								Shader();
								~Shader();
		void 					EnableShader() const;
		void 					DisableShader() const;
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
	
	
	/**
	 @class 		ShaderManager
	 @brief 		Manager of the shaders.
	 @details 		All shaders must be created via the ShaderManager singleton.
	 				You can provide either two files, or two strings. 

	 				When adding a shader you must also provide a unique name,
	 				which allows you to retreive the shader easily by name
					from anywhere in your game.
	 
	 				See the description of HandleCommand for how to modify
	 				uniforms in a shader at run-time.
	 */
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
	
	/**
	 @fn 			ShaderManager::HandleCommand
	 @availability 	Windows
	 @brief 		Overriden ConsoleListener method. Modifies uniforms from CLI.
	 @details 		The ShaderManager singleton listens to the command @e shadermgr.
	 				In order to modify a uniform 'uni' in a shader named 'myshader', 
	 				enter the following command into the command window:
	 					shadermgr myshader uniform1i uni 1
	 				
	 				Note that the @e uniformXT statement can be replaced by:
	 					uniform(1-2-3-4)(i-f)
	 
	 				Note that the amount of parameters provided must match the
	 				X-term in @e uniformXT. This is an invalid command:
	 					shadermgr myshader uniform4i uni 1
	 */
	
	/**
	 @fn 			ShaderManager::ClearShaders
	 @brief 		All shaders are deleted. Use with moderately extreme caution.
	 */
}