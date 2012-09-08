#include "Stdafx.h"

#include "PimShaderManager.h"
#include "Pim.h"

namespace Pim
{
	GLint Shader::getUniformLocation(std::string name)
	{
		if (!uniform.count(name))
		{
			GLint loc = glGetUniformLocation(program, name.c_str());
			std::cout<<"Location of " <<name <<": " <<loc <<"\n";
			uniform[name] = loc;
		}
				
		return uniform[name];
	}

	/*
	GLint Shader::getAttribLocation(std::string name)
	{
		if (!attrib.count(name))
		{
			GLint loc = glGetAttribLocation(program, name.c_str());
			std::cout<<"Location of " <<name <<": " <<loc <<"\n";
			attrib[name] = loc;
		}
		return attrib[name];
	}
	*/

	void Shader::setUniformXi(std::string name, GLint len, GLint arr[])
	{
		glUseProgram(program);

		switch (len)
		{
			case 1:
				glUniform1i(getUniformLocation(name), arr[0]);
				break;
			case 2:
				glUniform2i(getUniformLocation(name), arr[0], arr[1]);
				break;
			case 3:
				glUniform3i(getUniformLocation(name), arr[0], arr[1], arr[2]);
				break;
			case 4:
				glUniform4i(getUniformLocation(name), arr[0], arr[1], arr[2], arr[3]);
				break;
		}

		glUseProgram(0);
	}
	void Shader::setUniformXf(std::string name, GLint len, GLfloat arr[])
	{
		glUseProgram(program);

		switch (len)
		{
			case 1:
				glUniform1f(getUniformLocation(name), arr[0]);
				break;
			case 2:
				glUniform2f(getUniformLocation(name), arr[0], arr[1]);
				break;
			case 3:
				glUniform3f(getUniformLocation(name), arr[0], arr[1], arr[2]);
				break;
			case 4:
				glUniform4f(getUniformLocation(name), arr[0], arr[1], arr[2], arr[3]);
				break;
		}

		glUseProgram(0);
	}

	/*
	void Shader::setAttribXi(std::string name, GLint len, GLint arr[])
	{
		glUseProgram(program);

		switch (len)
		{
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
		}

		glUseProgram(0);
	}
	void Shader::setAttribXf(std::string name, GLint len, GLfloat arr[])
	{
		glUseProgram(program);

		switch (len)
		{
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
		}

		glUseProgram(0);
	}
	*/


	ShaderManager* ShaderManager::singleton = NULL;

	void ShaderManager::instantiateSingleton()
	{
		PimAssert(singleton == NULL, "Error: ShaderManager singleton is already set.");
		singleton = new ShaderManager;
	}
	void ShaderManager::clearSingleton()
	{
		if (singleton)
			delete singleton;
	}

	ShaderManager::ShaderManager()
	{
		defaultQuality = Quality::MED;
	}
	ShaderManager::~ShaderManager()
	{
		for (auto o_it=shaders.begin(); o_it!=shaders.end(); o_it++)
		{
			auto sub = o_it->second;
			for (auto i_it=sub.begin(); i_it!=sub.end(); i_it++)
			{
				delete i_it->second;
			}
		}
	}

	bool ShaderManager::addShaderFromFile(std::string fragFile, std::string vertFile, 
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
			getline(file, tmp);
			fragString.append(tmp.append("\n"));
		}

		file.close();

		// Load the vertex shader
		file.open(vertFile, std::ios::in);
		PimAssert(file, "Error: could not load vertex shader.");

		while (file.good())
		{
			std::string tmp;
			getline(file,tmp);
			vertString.append(tmp.append("\n"));
		}

		file.close();

		return addShader(fragString, vertString, nm, q);
	}
	bool ShaderManager::addShader(std::string fragString, std::string vertString, 
		std::string nm, Quality::Quality q)
	{
		std::cout<<"Loading shader " <<nm <<"...\n";


		Shader *shader = new Shader;
		shader->frag = glCreateShader(GL_FRAGMENT_SHADER);
		shader->vert = glCreateShader(GL_VERTEX_SHADER);

		// Prepare compilation
		int flen = fragString.size();
		int vlen = vertString.size();
		GLcharARB *ftmp = new GLcharARB[flen+1];
		GLcharARB *vtmp = new GLcharARB[vlen+1];
		std::copy(fragString.begin(), fragString.end(), ftmp);
		std::copy(vertString.begin(), vertString.end(), vtmp);
		ftmp[flen] = '\0';
		vtmp[vlen] = '\0';

		const GLcharARB *fstr = ftmp;
		const GLcharARB *vstr = vtmp;

		glShaderSourceARB(shader->frag, 1, &fstr, &flen);
		glShaderSourceARB(shader->vert, 1, &vstr, &vlen);

		// Compile the shaders (lambdas ftw)
		auto compile =  [](GLuint shader, std::string str) -> bool
		{
			GLint status;

			glCompileShaderARB(shader);
			glGetObjectParameterivARB(shader, GL_COMPILE_STATUS, &status);
			if (!status)
			{
				GLint blen=0, slen=0;

				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &blen);
				if (blen > 0)
				{
					GLchar *log = (GLchar*)malloc(blen);
					glGetInfoLogARB(shader, blen, &slen, log);
					std::cout<<str <<" compilation log:\n" <<log <<"\n";
					free(log);
				}

				return false;
			}
			else
			{
				std::cout<<str <<" compiled successfully!\n";
				return true;
			}
		};

		delete ftmp;
		delete vtmp;

		if (!compile(shader->frag, "Fragment shader"))
		{
			delete shader;
			return false;
		}
		if (!compile(shader->vert, "Vertex shader"))
		{
			delete shader;
			return false;
		}

		// Create the program and link that fucka'
		shader->program = glCreateProgram();

		glAttachShader(shader->program, shader->frag);
		glAttachShader(shader->program, shader->vert);

		glLinkProgram(shader->program);

		GLint status;
		glGetProgramiv(shader->program, GL_LINK_STATUS, &status);
		if (status)
		{
			std::cout<<"Program linked successfully!\n";
		}
		else
		{
			std::cout<<"Failed to link shaders.\n";
			delete shader;
			return false;
		}

		singleton->shaders[nm][q] = shader;

		return true;
	}

	void ShaderManager::setDefaultQuality(Quality::Quality q)
	{
		PimAssert(q >= 0 && q >= 2, "Error: bad quality parameter");
		singleton->defaultQuality = q;
	}

	Shader* ShaderManager::getShader(std::string name)
	{
		return getShader(name, singleton->defaultQuality);
	}
	Shader* ShaderManager::getShader(std::string name, Quality::Quality q)
	{
		if (singleton->shaders.count(name))
		{
			if (singleton->shaders[name].count(q))
			{
				return singleton->shaders[name][q];
			}
			
			for (int i=0; i<3; i++)
			{
				if (i == q) continue;
				
				if (singleton->shaders[name].count((Quality::Quality)i))
				{
					std::cout<<"WARNING: Quality " <<q <<" for shader \""
							 <<name <<"\" not found. Using quality " <<i <<"\n";
					return singleton->shaders[name][(Quality::Quality)i];
				}
			}
		}

		std::cout<<"ERROR: Shader \"" <<name <<"\" not found for any qualities.\n";
		return NULL;
	}
}