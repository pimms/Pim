#include "PimInternal.h"

#include "PimShaderManager.h"
#include "PimAssert.h"

namespace Pim {
	/*
	=====================
	Shader::Shader
	=====================
	*/
	Shader::Shader() {
		vert	= -1;
		frag	= -1;
		program = -1;
	}

	/*
	=====================
	Shader::~Shader
	=====================
	*/
	Shader::~Shader() {
		if (vert > 0) {
			glDeleteShader(vert);
		}
		if (frag > 0) {
			glDeleteShader(frag);
		}
		if (program > 0) {
			glDeleteProgram(program);
		}
	}
	
	/*
	=====================
	Shader::EnableShader
	=====================
	*/
	void Shader::EnableShader() const {
		glUseProgram(program);
	}
	
	/*
	=====================
	Shader::DisableShader
	=====================
	*/
	void Shader::DisableShader() const {
		glUseProgram(0);
	}

	/*
	=====================
	Shader::GetProgram
	=====================
	*/
	GLint Shader::GetProgram() const {
		return program;
	}

	/*
	=====================
	Shader::GetUniformLocation
	=====================
	*/
	GLint Shader::GetUniformLocation(const string name) {
		// Update the map to contain the uniform adresses.
		// This way they are only queried for from OpenGL once.
		if (!uniform.count(name)) {
			GLint loc = glGetUniformLocation(program, name.c_str());
			cout<<"Location of " <<name <<": " <<loc <<"\n";
			uniform[name] = loc;
		}

		return uniform[name];
	}
	
	/*
	=====================
	Shader::SetUniform1i
	=====================
	*/
	void Shader::SetUniform1i(string n, GLint a) {
		GLint arr[] = {a};
		SetUniformXi(n, 1, arr);
	}

	/*
	=====================
	Shader::SetUniform2i
	=====================
	*/
	void Shader::SetUniform2i(string n, GLint a, GLint b) {
		GLint arr[] = {a,b};
		SetUniformXi(n, 2, arr);
	}

	/*
	=====================
	Shader::SetUniform3i
	=====================
	*/
	void Shader::SetUniform3i(string n, GLint a, GLint b, GLint c) {
		GLint arr[] = {a,b,c};
		SetUniformXi(n,3,arr);
	}

	/*
	=====================
	Shader::SetUniform4i
	=====================
	*/
	void Shader::SetUniform4i(string n, GLint a, GLint b, GLint c, GLint d) {
		GLint arr[] = {a,b,c,d};
		SetUniformXi(n,4,arr);
	}

	/*
	=====================
	Shader::SetUniform1f
	=====================
	*/
	void Shader::SetUniform1f(string n, GLfloat a) {
		GLfloat arr[] = {a};
		SetUniformXf(n,1,arr);
	}

	/*
	=====================
	Shader::SetUniform2f
	=====================
	*/
	void Shader::SetUniform2f(string n, GLfloat a, GLfloat b) {
		GLfloat arr[] = {a,b};
		SetUniformXf(n,2,arr);
	}

	/*
	=====================
	Shader::SetUniform3f
	=====================
	*/
	void Shader::SetUniform3f(string n, GLfloat a, GLfloat b, GLfloat c) {
		GLfloat arr[] = {a,b,c};
		SetUniformXf(n,3,arr);
	}

	/*
	=====================
	Shader::SetUniform4f
	=====================
	*/
	void Shader::SetUniform4f(string n, GLfloat a, GLfloat b, GLfloat c, GLfloat d) {
		GLfloat arr[] = {a,b,c,d};
		SetUniformXf(n,4,arr);
	}

	/*
	=====================
	Shader::SetUniformXi
	=====================
	*/
	void Shader::SetUniformXi(string name, GLint len, GLint arr[]) {
		glUseProgram(program);

		switch (len) {
		case 1:
			glUniform1i(GetUniformLocation(name), arr[0]);
			break;
		case 2:
			glUniform2i(GetUniformLocation(name), arr[0], arr[1]);
			break;
		case 3:
			glUniform3i(GetUniformLocation(name), arr[0], arr[1], arr[2]);
			break;
		case 4:
			glUniform4i(GetUniformLocation(name), arr[0], arr[1], arr[2], arr[3]);
			break;
		}

		glUseProgram(0);
	}
	
	/*
	=====================
	Shader::SetUniformXf
	=====================
	*/
	void Shader::SetUniformXf(string name, GLint len, GLfloat arr[]) {
		glUseProgram(program);

		switch (len) {
		case 1:
			glUniform1f(GetUniformLocation(name), arr[0]);
			break;
		case 2:
			glUniform2f(GetUniformLocation(name), arr[0], arr[1]);
			break;
		case 3:
			glUniform3f(GetUniformLocation(name), arr[0], arr[1], arr[2]);
			break;
		case 4:
			glUniform4f(GetUniformLocation(name), arr[0], arr[1], arr[2], arr[3]);
			break;
		}

		glUseProgram(0);
	}


	/* Static ShaderManager methods */
	ShaderManager* ShaderManager::singleton = NULL;

	/*
	=====================
	ShaderManager::GetSingleton
	=====================
	*/
	ShaderManager* ShaderManager::GetSingleton() {
		return singleton;
	}

	/*
	=====================
	ShaderManager::InstantiateSingleton
	=====================
	*/
	void ShaderManager::InstantiateSingleton() {
		PimAssert(singleton == NULL, "Error: ShaderManager singleton is already set.");
		singleton = new ShaderManager;
	}

	/*
	=====================
	ShaderManager::ClearSingleton
	=====================
	*/
	void ShaderManager::ClearSingleton() {
		if (singleton) {
			delete singleton;
		}
	}

	/*
	=====================
	ShaderManager::AddShaderFromFile
	=====================
	*/
	Shader* ShaderManager::AddShaderFromFile(string fragFile, string vertFile, string nm) {
		string fragString;
		string vertString;

		ifstream file;

		// Load the fragment shader
		file.open(fragFile, ios::in);
		PimAssert(file.is_open(), "Error: could not load fragment shader.");

		while (file.good()) {
			string tmp;
			getline(file, tmp);
			fragString.append(tmp.append("\n"));
		}

		file.close();

		// Load the vertex shader
		file.open(vertFile, ios::in);
		PimAssert(file.is_open(), "Error: could not load vertex shader.");

		while (file.good()) {
			string tmp;
			getline(file,tmp);
			vertString.append(tmp.append("\n"));
		}

		file.close();

		return AddShader(fragString, vertString, nm);
	}

	/*
	=====================
	ShaderManager::AddShader
	=====================
	*/
	Shader* ShaderManager::AddShader(string fragString, string vertString, string nm) {
		printf("\nCreating shader %s...\n", nm.c_str());

		Shader *shader = new Shader;
		shader->frag = glCreateShader(GL_FRAGMENT_SHADER);
		shader->vert = glCreateShader(GL_VERTEX_SHADER);

		// Prepare compilation
		GLint flen = (GLint)fragString.size();
		GLint vlen = (GLint)vertString.size();
		GLcharARB *ftmp = new GLcharARB[flen+1];
		GLcharARB *vtmp = new GLcharARB[vlen+1];
		copy(fragString.begin(), fragString.end(), ftmp);
		copy(vertString.begin(), vertString.end(), vtmp);
		ftmp[flen] = '\0';
		vtmp[vlen] = '\0';

		const GLcharARB *fstr = ftmp;
		const GLcharARB *vstr = vtmp;

		glShaderSource(shader->frag, 1, &fstr, &flen);
		glShaderSource(shader->vert, 1, &vstr, &vlen);
		
		// Compile the shaders (lambda ftw)
		auto Compile_l = [](GLuint shader, string str) -> bool {
			GLint status;

#ifdef WIN32
			glCompileShaderARB(shader);
			glGetObjectParameterivARB(shader, GL_COMPILE_STATUS, &status);
#elif defined __APPLE__
			glCompileShader(shader);
			glGetObjectParameterivARB(&shader, GL_COMPILE_STATUS, &status);
#endif
			if (!status) {
				GLint blen=0, slen=0;

				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &blen);
				if (blen > 0) {
					GLchar *log = (GLchar*)malloc(blen);
					
#ifdef WIN32
					glGetInfoLogARB(shader, blen, &slen, log);
#elif defined __APPLE__
					glGetInfoLogARB(&shader, blen, &slen, log);
#endif
					printf("Compile log:\n%s\n", log);
					free(log);
				}

				return false;
			} else {				
				printf("%s: Compiled successfully!\n", str.c_str());
				return true;
			}
		};

		delete[] ftmp;
		delete[] vtmp;

		if (!Compile_l(shader->frag, "Fragment shader")) {
			delete shader;
			return NULL;
		}
		if (!Compile_l(shader->vert, "Vertex shader")) {
			delete shader;
			return NULL;
		}

		// Create the program and link that fucka'
		shader->program = glCreateProgram();

		glAttachShader(shader->program, shader->frag);
		glAttachShader(shader->program, shader->vert);
		
		glLinkProgram(shader->program);
		glValidateProgram(shader->program);

		GLint status;
		glGetProgramiv(shader->program, GL_LINK_STATUS, &status);
		if (status) {
			printf("Program linked successfully!\n\n");
		} else {
			printf("WARNING: Failed to link vertex and fragment shader \"%s\"!\n",
				   nm.c_str());
			
			char logBuf[1024];
			int len;
			glGetProgramInfoLog(shader->program, 1024, &len, logBuf);
			printf("Program Info Log:\n%s\n", logBuf);
			
			delete shader;
			return NULL;
		}

		if (singleton->shaders.count(nm)) {
			delete singleton->shaders[nm];
		}

		singleton->shaders[nm] = shader;
		return shader;
	}

	/*
	=====================
	ShaderManager::GetShader
	=====================
	*/
	Shader* ShaderManager::GetShader(string name) {
		if (singleton->shaders.count(name)) {
			return singleton->shaders[name];
		}

		return NULL;
	}

	/*
	==================
	ShaderManager::RemoveShader
	==================
	*/
	bool ShaderManager::RemoveShader(string name) {
		if (singleton->shaders.count(name)) {
			return RemoveShader(singleton->shaders[name]);
		}

		return false;
	}

	/*
	==================
	ShaderManager::RemoveShader
	==================
	*/
	bool ShaderManager::RemoveShader(Shader *shader) {
		map<string, Shader*> *shaders = &singleton->shaders;

		map<string, Shader*>::iterator it = shaders->begin();
		for (; it != shaders->end(); it++) {
			if (it->second == shader) {
				delete it->second;
				shaders->erase(it);

				return true;
			}
		}

		return false;
	}


	/*
	=====================
	ShaderManager::ShaderManager
	=====================
	*/
	ShaderManager::ShaderManager() {
		ListenCommand("shadermgr");

		#ifdef _DEBUG
		cout <<"\nInstructions for altering shader uniforms via command line:\n";
		cout <<"\tThe following command will alter the 'factor' uniform in the\n";
		cout <<"\tshader 'myShader' to the value '0.5':\n";
		cout <<"\t\tshadermgr myShader uniform1f factor 0.5\n";
		cout <<"\tExample of setting a vec2 in the same shader:\n";
		cout <<"\t\tshadermgr myShader uniform2f position 13.2 10.0\n\n";
		#endif /* _DEBUG */
	}

	/*
	=====================
	ShaderManager::~ShaderManager
	=====================
	*/
	ShaderManager::~ShaderManager() {
		ClearShaders();
	}

	/*
	=====================
	ShaderManager::ClearShaders
	=====================
	*/
	void ShaderManager::ClearShaders() {
		for (auto o_it=shaders.begin(); o_it!=shaders.end(); o_it++) {
			delete o_it->second;
		}

		shaders.clear();
	}

	/*
	=====================
	ShaderManager::HandleCommand
	 
	Todo: Comment this unholy spawn.
	=====================
	*/
	void ShaderManager::HandleCommand(ConsoleCommand cmd) {
		if (cmd.size() > 4) {
			if (cmd[2].length() == 9) {
				char ty = cmd[2][8];
				int n = atoi(&cmd[2][7]);

				if (n + 4 != cmd.size() && n) {
					return;
				}

				Shader *s = GetShader(cmd[1]);
				if (!s) {
					return;
				}

				if (ty == 'f' || ty == 'd') {
					// Handle float and double parametrs

					float buf[4] = {0.f};

					for (int i=0; i<n; i++) {
						buf[i] = (float)atof(cmd[4+i].c_str());
					}

					switch (n) {
						case 1:
							s->SetUniform1f(cmd[3], buf[0]);
							break;
						case 2:
							s->SetUniform2f(cmd[3], buf[0], buf[1]);
							break;
						case 3:
							s->SetUniform3f(cmd[3], buf[0], buf[1], buf[2]);
							break;
						case 4:
							s->SetUniform4f(cmd[3], buf[0], buf[1], buf[2], buf[3]);
					}
				} else if (ty == 'i') {
					// Handle int parameters 

					int buf[4] = {0};

					for (int i=0; i<n; i++) {
						buf[i] = atoi(cmd[4+i].c_str());
					}

					switch (n) {
						case 1:
							s->SetUniform1i(cmd[3], buf[0]);
							break;
						case 2:
							s->SetUniform2i(cmd[3], buf[0], buf[1]);
							break;
						case 3:
							s->SetUniform3i(cmd[3], buf[0], buf[1], buf[2]);
							break;
						case 4:
							s->SetUniform4i(cmd[3], buf[0], buf[1], buf[2], buf[3]);
					}
				}
			}
		}
	}
}