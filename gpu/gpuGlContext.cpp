/*
gpuGlContext.cpp (part of Ara, https://github.com/rdujardin/Ara)

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

#include "gpuGlContext.h"

using namespace std;
using namespace cv;

bool _checkGL() {
	if(glGetError()!=0) {
		throw "OpenGL error";
		return false;
	}
	return true;
}

GpuGlContext::GpuGlContext(int width,int height) {
	bcm_host_init();
	DISPMANX_ELEMENT_HANDLE_T dispmanElement;
	DISPMANX_DISPLAY_HANDLE_T dispmanDisplay;
	DISPMANX_UPDATE_HANDLE_T dispmanUpdate;
	VC_RECT_T dstRect,srcRect;

	static const EGLint attributeList[]= {
		EGL_RED_SIZE,8,
		EGL_GREEN_SIZE,8,
		EGL_BLUE_SIZE,8,
		EGL_ALPHA_SIZE,8,
		EGL_SURFACE_TYPE,EGL_WINDOW_BIT,
		EGL_NONE
	};
	static const EGLint contextAttributes[]= {
		EGL_CONTEXT_CLIENT_VERSION,2,
		EGL_NONE
	};

	EGLConfig config;

	_display=eglGetDisplay(EGL_DEFAULT_DISPLAY);
	EGLBoolean result;
	EGLint numConfig;
	result=eglInitialize(_display,NULL,NULL);
	result=eglChooseConfig(_display,attributeList,&config,1,&numConfig);
	if(result==EGL_FALSE) {
		throw "GpuGlContext : init failed (eglChooseConfig).";
		return;
	}
	result=eglBindAPI(EGL_OPENGL_ES_API);
	if(result==EGL_FALSE) {
		throw "GpuGlContext : init failed (eglBindAPI).";
		return;
	}

	_context=eglCreateContext(_display,config,EGL_NO_CONTEXT,contextAttributes);
	if(_context==EGL_NO_CONTEXT) {
		throw "GpuGlContext : init failed (eglCreateContext).";
		return;
	}

	int32_t success=0;
	success=graphics_get_display_size(0,&_width,&_height);
	if(success<0) {
		throw "GpuGlContext : init failed (graphics_get_display_size).";
		return;
	}

	_width=width;
	_height=height;
	
	dstRect.x=0;
	dstRect.y=0;
	dstRect.width=640; //1
	dstRect.height=480; //1
	srcRect.x=0;
	srcRect.y=0;
	srcRect.width=_width; //<<16
	srcRect.height=_height;

	dispmanDisplay=vc_dispmanx_display_open(0);
	dispmanUpdate=vc_dispmanx_update_start(0);
	dispmanElement=vc_dispmanx_element_add(dispmanUpdate,dispmanDisplay,0,&dstRect,0,&srcRect,DISPMANX_PROTECTION_NONE,0,0,(DISPMANX_TRANSFORM_T) 0);

	_nativeWindow.element=dispmanElement;
	_nativeWindow.width=_width;
	_nativeWindow.height=_height;
	vc_dispmanx_update_submit_sync(dispmanUpdate);

	_surface=eglCreateWindowSurface(_display,config,&_nativeWindow,NULL);
	if(_surface==EGL_NO_SURFACE) {
		throw "GpuGlContext : init failed (eglCreateWindowSurface).";
		return;
	}

	result=eglMakeCurrent(_display,_surface,_surface,_context);
	if(result==EGL_FALSE) {
		throw "GpuGlContext : init failed (eglMakeCurrent).";
		return;
	}
}

void GpuGlContext::refresh() {
	eglSwapBuffers(_display,_surface);
}

//--------------------------------------
/*
GpuModule::GpuModule(std::string fragSrc,bool activated) {
	_activated=activated;
	_fragShader=new GpuShader(GL_FRAGMENT_SHADER,_fragIntro+fragSrc+_fragOutro);

	vector<std::string> gpuParams;
	gpuParams.push_back("vPosition");
	gpuParams.push_back("fTexPos");
	_program=new GpuProgram(_vertexShader,*_fragShader,gpuParams);
	try { _program->load(); }
	catch(string err) {
		throw err;
	}
}

GpuModule::~GpuModule() {
	_program->disable();
	delete _program;
	delete _fragShader;
}

void GpuModule::setActivated(bool activated) {
	_activated=activated;
}

GpuProgram* GpuModule::program() {
	return _program;
}

GpuContext::GpuContext() {
	output=NULL;
}

GpuContext::~GpuContext() {
	for(std::map<std::string,GpuModule*>::iterator it=_modules.begin();it!=_modules.end();++it) {
		delete it->second;
	}
	if(output) delete output;
}

void GpuContext::addModule(std::string name,GpuModule* module) {
	std::map<std::string,GpuModule*>::iterator it=_modules.find(name);	
	_modules[name]=module;
	if(it==_modules.end()) _modulesNames.push_back(name);
}

void GpuContext::setActivated(std::string name,bool activated) {
	std::map<std::string,GpuModule*>::iterator it=_modules.find(name);
	if(it!=_modules.end()) it->second->setActivated(activated);
}

GpuModule* GpuContext::module(std::string name) {
	std::map<std::string,GpuModule*>::iterator it=_modules.find(name);
	if(it!=_modules.end()) return it->second;
	else return NULL;
}

bool GpuContext::loop(cv::Mat& img) {
	return true;
}

/*
bool GpuContext::loop(cv::Mat& img) {
	std::cout << "caca" << std::endl;

	GpuTexture* inputTex=NULL;//=new GpuTexture();
		std::cout << "caca2" << std::endl;
	GpuTexture* outputTex=new GpuTexture();
		std::cout << "caca3" << std::endl;
	outputTex->createFromBGRMat(img);
		std::cout << "caca4" << std::endl;

//	outputTex->createEmptyRGB(img.cols,img.rows);
	GpuGlFbo* input=NULL;//new GpuGlFbo(inputTex);
		std::cout << "caca5" << std::endl;
	if(output) delete output;
	output=new GpuGlFbo(outputTex);
			std::cout << "caca6" << std::endl;
	float square_vertices[8]={-1.0,1.0,1.0,1.0,-1.0,-1.0,1.0,-1.0};

	std::cout << "a" << std::endl;
	for(std::vector<std::string>::iterator it=_modulesNames.begin();it!=_modulesNames.end();++it) {

		inputTex=outputTex;
		input=output;

		std::cout << "d" << std::endl;
		outputTex=new GpuTexture();
		outputTex->createEmptyRGB(img.cols,img.rows);
		output=new GpuGlFbo(outputTex);

		std::cout << "e" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER,output->id());

		glClearColor(0.0,0.0,0.0,1.0);
		glViewport(0,0,img.cols,img.rows);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			_modules[*it]->program()->enable();

				glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,square_vertices);
				glEnableVertexAttribArray(0);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D,inputTex->id());
				glUniform1i(glGetUniformLocation(_modules[*it]->program()->id(),"uTexture"),0);
					glDrawArrays(GL_TRIANGLE_STRIP,0,4);
				glDisableVertexAttribArray(0);

			_modules[*it]->program()->disable();
			//_glCtx.refresh();

		glBindFramebuffer(GL_FRAMEBUFFER,0);
	
		std::cout << "b" << std::endl;

		delete input;

		std::cout << "c" << std::endl;

		
	}

	std::cout << "d" << std::endl;
	output->get(img);
	
	return true;
}
*/

/*


bool GpuContext::loop(Mat& img) {
	if(_tex) { delete _tex; _tex=NULL; }
	_tex=new GpuTexture();
	_tex->createFromBGRMat(img);
	if(_glFbo) { delete _glFbo; _glFbo=NULL; }
	_glFbo=new GpuGlFbo(_tex);
	glClearColor(1.0,0.0,0.0,1.0);
	float square_vertices[8]={-1.0,1.0,1.0,1.0,-1.0,-1.0,1.0,-1.0};
	glViewport(0,0,img.cols,img.rows);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_prgm->enable();

		glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,square_vertices);
		glEnableVertexAttribArray(0);

		//glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,square_colors);
		//glEnableVertexAttribArray(1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,_tex->id());
		glUniform1i(glGetUniformLocation(_prgm->id(),"uTexture"),0);

			glDrawArrays(GL_TRIANGLE_STRIP,0,4);

		//glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);

	_prgm->disable();	
	_glFbo->get(img);
	_glCtx->refresh();
	return true;
}*/

