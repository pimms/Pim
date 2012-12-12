#include "PimInternal.h"

#include "PimShaderManager.h"
#include "PimAssert.h"

namespace Pim
{
	GLint Shader::getUniformLocation(std::string name)
	{
		// Update the map to contain the uniform adresses.
		// This way they are only queried for from OpenGL once.
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
		listenCommand("shadermgr");

#ifdef _DEBUG
		std::cout <<"\nInstructions for altering shader uniforms via command line:\n";
		std::cout <<"\tThe following command will alter the 'factor' uniform in the\n";
		std::cout <<"\tshader 'myShader' to the value '0.5':\n";
		std::cout <<"\t\tshadermgr myShader uniform1f factor 0.5\n";
		std::cout <<"\tExample of setting a vec2 in the same shader:\n";
		std::cout <<"\t\tshadermgr myShader uniform2f position 13.2 10.0\n\n";
#endif
	}
	ShaderManager::~ShaderManager()
	{
		for (auto o_it=shaders.begin(); o_it!=shaders.end(); o_it++)
		{
			delete o_it->second;

			/*
			auto sub = o_it->second;
			for (auto i_it=sub.begin(); i_it!=sub.end(); i_it++)
			{
				delete i_it->second;
			}
			*/
		}
	}

	Shader* ShaderManager::addShaderFromFile(std::string fragFile, std::string vertFile, 
		std::string nm)
	{
		std::string fragString;
		std::string vertString;

		std::ifstream file;

		// Load the fragment shader
		file.open(fragFile, std::ios::in);
		PimAssert(file.is_open(), "Error: could not load fragment shader.");

		while (file.good())
		{
			std::string tmp;
			getline(file, tmp);
			fragString.append(tmp.append("\n"));
		}

		file.close();

		// Load the vertex shader
		file.open(vertFile, std::ios::in);
		PimAssert(file.is_open(), "Error: could not load vertex shader.");

		while (file.good())
		{
			std::string tmp;
			getline(file,tmp);
			vertString.append(tmp.append("\n"));
		}

		file.close();

		return addShader(fragString, vertString, nm);
	}
	Shader* ShaderManager::addShader(std::string fragString, std::string vertString, 
		std::string nm)
	{
		std::cout<<"\nCreating shader " <<nm <<"...\n";


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

		glShaderSource(shader->frag, 1, &fstr, &flen);
		glShaderSource(shader->vert, 1, &vstr, &vlen);

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

		delete[] ftmp;
		delete[] vtmp;

		if (!compile(shader->frag, "Fragment shader"))
		{
			delete shader;
			return NULL;
		}
		if (!compile(shader->vert, "Vertex shader"))
		{
			delete shader;
			return NULL;
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
			std::cout<<"Program linked successfully!\n\n";
		}
		else
		{
			std::cout<<"Failed to link shaders.\n\n";
			delete shader;
			return NULL;
		}

		if (singleton->shaders.count(nm))
			delete singleton->shaders[nm];

		singleton->shaders[nm] = shader;
		return shader;
	}

	Shader* ShaderManager::getShader(std::string name)
	{
		if (singleton->shaders.count(name))
			return singleton->shaders[name];
	}

	void ShaderManager::handleCommand(ConsoleCommand cmd)
	{
		if (cmd.size() > 4)
		{
			if (cmd[2].length() == 9)
			{
				char ty = cmd[2][8];
				int n = atoi(&cmd[2][7]);

				if (n + 4 != cmd.size() && n) return;

				Shader *s = getShader(cmd[1]);
				if (!s) return;

				if (ty == 'f' || ty == 'd')
				{
					float buf[4] = {0.f};
					for (int i=0; i<n; i++)
						buf[i] = atof(cmd[4+i].c_str());
					
					switch (n)
					{
					case 1:
						s->setUniform1f(cmd[3], buf[0]);
						break;
					case 2:
						s->setUniform2f(cmd[3], buf[0], buf[1]); 
						break;
					case 3:
						s->setUniform3f(cmd[3], buf[0], buf[1], buf[2]);
						break;
					case 4:
						s->setUniform4f(cmd[3], buf[0], buf[1], buf[2], buf[3]);
					}
				}
				else if (ty == 'i')
				{
					int buf[4] = {0.f};
					for (int i=0; i<n; i++)
						buf[i] = atoi(cmd[4+i].c_str());
					
					switch (n)
					{
					case 1:
						s->setUniform1i(cmd[3], buf[0]);
						break;
					case 2:
						s->setUniform2i(cmd[3], buf[0], buf[1]); 
						break;
					case 3:
						s->setUniform3i(cmd[3], buf[0], buf[1], buf[2]);
						break;
					case 4:
						s->setUniform4i(cmd[3], buf[0], buf[1], buf[2], buf[3]);
					}
				}
			}
		}
	}
}