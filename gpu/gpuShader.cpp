/*
gpuShader.cpp (part of Ara, https://github.com/rdujardin/Ara)

Copyright (c) 2016, Raphaël Dujardin (rdujardin) & Jean Boehm (jboehm1)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "gpuShader.h"

using namespace std;

GpuShader::GpuShader(GLenum type,string source,bool fromFile) {
	_type=type;
	if(!fromFile) _source=source;
	else {
		ifstream file(source.c_str());
		if(!file) {
			throw "GpuShader : Shader file '"+source+"' could not be opened.";
			return;
		}
		string line;
		while(getline(file,line)) _source+=line+'\n';
		file.close();
	}
	_loaded=false;
}

GpuShader::~GpuShader() {
	if(_loaded) glDeleteShader(_id);
}

bool GpuShader::load() {
	_loaded=false;
	if(glIsShader(_id)==GL_TRUE) glDeleteShader(_id);
	_id=glCreateShader(_type);
	if(_id==0) {
		throw "GpuShader : Shader could not be created.";
		return false;
	}
	const GLchar* glSource=_source.c_str();
	glShaderSource(_id,1,&glSource,0);
	glCompileShader(_id);
	GLint compileStatus=0;
	glGetShaderiv(_id,GL_COMPILE_STATUS,&compileStatus);
	if(compileStatus!=GL_TRUE) {
		GLint errorSize=0;
		glGetShaderiv(_id,GL_INFO_LOG_LENGTH,&errorSize);
		char* error=new char[errorSize+1];
		glGetShaderInfoLog(_id,errorSize,&errorSize,error);
		error[errorSize]='\0';
		string strError(error);
		delete[] error;
		glDeleteShader(_id);
		throw "GpuShader : Shader got compiling errors : "+strError+".";
		return false;
	}
	else {
		_loaded=true;
		return true;
	}
}

GLenum GpuShader::type() { return _type; }
string GpuShader::source() { return _source; }
GLuint GpuShader::id() { return _id; }
bool GpuShader::loaded() { return _loaded; }


GpuProgram::GpuProgram(GpuShader& vertexShader,GpuShader& fragmentShader,vector<string> attribBindings) {
	_vertexShader=&vertexShader;
	_fragmentShader=&fragmentShader;
	_attribBindings=attribBindings;
	_loaded=false;
}

GpuProgram::~GpuProgram() {
	if(_loaded) glDeleteProgram(_id);
}

bool GpuProgram::load() {
	_loaded=false;
	if(glIsProgram(_id)==GL_TRUE) glDeleteProgram(_id);
	if(!_vertexShader->loaded()) {
		if(!_vertexShader->load()) return false;
	}
	if(!_fragmentShader->loaded()) {
		if(!_fragmentShader->load()) return false;
	}
	_id=glCreateProgram();
	glAttachShader(_id,_vertexShader->id());
	glAttachShader(_id,_fragmentShader->id());
	for(unsigned int i=0;i<_attribBindings.size();i++) {
		glBindAttribLocation(_id,i,_attribBindings.at(i).c_str());
	}
	glLinkProgram(_id);
	GLint linkStatus=0;
	glGetProgramiv(_id,GL_LINK_STATUS,&linkStatus);
	if(linkStatus!=GL_TRUE) {
		GLint errorSize=0;
		glGetProgramiv(_id,GL_INFO_LOG_LENGTH,&errorSize);
		char* error=new char[errorSize+1];
		glGetProgramInfoLog(_id,errorSize,&errorSize,error);
		error[errorSize]='\0';
		string strError(error);
		delete[] error;
		glDeleteProgram(_id);
		throw "GpuProgram : Program got linking errors : "+strError+".";
		return false;
	}
	else {
		_loaded=true;
		return true;
	}
}

void GpuProgram::enable() {
	if(_loaded) glUseProgram(_id);
}

void GpuProgram::disable() {
	glUseProgram(0);
}
		
GpuShader& GpuProgram::vertexShader() { return *_vertexShader; }
GpuShader& GpuProgram::fragmentShader() { return *_fragmentShader; }
GLuint GpuProgram::id() { return _id; }
bool GpuProgram::loaded() { return _loaded; }
vector<string> GpuProgram::attribBindings() { return _attribBindings; }

