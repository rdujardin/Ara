/*
gpuTexture.h (part of Ara, https://github.com/rdujardin/Ara)

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

#ifndef GPU_TEXTURE_H
#define GPU_TEXTURE_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <bcm_host.h>

#include <opencv2/opencv.hpp>
#include <string>
#include <fstream>
#include <cstdio>

bool _checkGL();
#define checkGL() if(!_checkGL()) return false;

class GpuTexture {
	public:
		GpuTexture();
		~GpuTexture();

		bool createEmptyRGB(unsigned int width,unsigned int height);
		bool createRGB(unsigned int width,unsigned int height,const void* data);
		bool createFromBGRMat(cv::Mat& img);
		bool createFromTGAFile(std::string fileName);
		//bool saveToRGBMat(cv::Mat& img);

		GLuint id();
		unsigned int width();
		unsigned int height();
	private:
		unsigned int _width,_height;
		GLuint _id,_frameBufferId;
};

class GpuGlFbo {
	public:
		GpuGlFbo(GpuTexture* texture);
		~GpuGlFbo();

		void get(cv::Mat& out);

		GLuint id();
	private:
		GLuint _id;
		int _width,_height;
		GpuTexture* _texture;
		GLuint _depthBufferId;
};

#endif

