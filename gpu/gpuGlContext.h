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

