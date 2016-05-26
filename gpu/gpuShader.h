/*
gpuShader.h (part of Ara, https://github.com/rdujardin/Ara)

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

#ifndef GPU_SHADER_H
#define GPU_SHADER_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <bcm_host.h>

#include <vector>
#include <string>
#include <fstream>

class GpuShader {
	public:
		GpuShader(GLenum type,std::string source,bool fromFile=false);
		~GpuShader();

		bool load();

		GLenum type();
		std::string source();
		GLuint id();
		bool loaded();
	private:
		GLenum _type;
		std::string _source;
		GLuint _id;
		bool _loaded;
};

class GpuProgram {
	public:
		GpuProgram(GpuShader& vertexShader,GpuShader& fragmentShader,std::vector<std::string> attribBindings);
		~GpuProgram();

		bool load();
		void enable();
		void disable();

		GpuShader& vertexShader();
		GpuShader& fragmentShader();
		GLuint id();
		bool loaded();
		std::vector<std::string> attribBindings();
	private:
		GpuShader* _vertexShader;
		GpuShader* _fragmentShader;
		GLuint _id;
		bool _loaded;
		std::vector<std::string> _attribBindings;
};

#endif

