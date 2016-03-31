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

