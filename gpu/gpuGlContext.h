/*
gpuGlContext.h (part of Ara, https://github.com/rdujardin/Ara)

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

#ifndef GPU_GL_CONTEXT_H
#define GPU_GL_CONTEXT_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <bcm_host.h>
#include <opencv2/opencv.hpp>

#include <iostream>

#include "gpuShader.h"
#include "gpuTexture.h"

class GpuGlContext {
	public:
		GpuGlContext(int width,int height);

		void refresh();
	private:
		uint32_t _width,_height;
		EGLDisplay _display;
		EGLSurface _surface;
		EGLContext _context;
		EGL_DISPMANX_WINDOW_T _nativeWindow;
};
/*
class GpuModule {
	public:
		GpuModule();
		GpuModule(std::string fragSrc,bool activated=true);
		GpuModule(GpuModule const& module);
		void setActivated(bool activated);

		std::string fragSrc() const;
		bool activated() const;

		GpuModule& operator=(GpuModule const& module);
	private:
		std::string _fragSrc;
		bool _activated;
		GpuShader* _frag;
		GpuProgram* _prgm;
};
*/
/*static std::string _fragIntro="varying vec2 fTexPos;\n"
								"uniform sampler2D uTexture;\n"
								"void main() {\n";
								//"vec4 io=texture2D(uTexture,fTexPos);\n";
static std::string _fragOutro=//"gl_FragColor=texture2D(uTexture,fTexPos);\n"
								"}";


static GpuGlContext _glCtx(640,480);
static GpuShader _vertexShader(GL_VERTEX_SHADER,""
			"attribute vec2 vPosition;\n"
			"varying vec2 fTexPos;\n"
			"void main() {\n"
			"gl_Position=vec4(vPosition.x,vPosition.y,0.0,1.0);\n"
			"fTexPos=vec2((vPosition.x+1.0)/2.0,(vPosition.y+1.0)/2.0);\n"
			"}\n");
static float square_vertices[8]={-1.0,1.0,1.0,1.0,-1.0,-1.0,1.0,-1.0};
*//*
class GpuModule {
	public:
		GpuModule(std::string fragSrc,bool activated=true);
		~GpuModule();
		void setActivated(bool activated);

		GpuProgram* program();
	private:
		bool _activated;
		GpuShader* _fragShader;
		GpuProgram* _program;
};

class GpuContext {
	public:
		GpuContext();
		~GpuContext();

		void addModule(std::string name,GpuModule* module);
		void setActivated(std::string name,bool activated);

		bool loop(cv::Mat& img);

		GpuModule* module(std::string name);
	private:
		std::vector<std::string> _modulesNames;
		std::map<std::string,GpuModule*> _modules;	
		GpuGlFbo* output;
};

/*
class GpuContext {
	public:
		GpuContext();
		~GpuContext();

		bool loop(cv::Mat& img);

		bool addModule(std::string name,GpuModule* module);
		void setActivated(std::string name,bool activated);
	private:
		GpuGlContext* _glCtx;
		GpuTexture* _tex;
		GpuGlFbo* _glFbo;
		GpuShader* _vert;
		std::vector<std::string> _moduleNames;
		std::map<std::string,GpuModule*> _modules;

		bool updateProgram();
};*/



//Yann Lecun, réseaux profonds
#endif

