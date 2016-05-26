/*
gpuTexture.cpp (part of Ara, https://github.com/rdujardin/Ara)

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

#include "gpuTexture.h"

using namespace std;
using namespace cv;

GpuTexture::GpuTexture() {
	_width=0;
	_height=0;
	_id=0;
	_frameBufferId=0;
}

GpuTexture::~GpuTexture() {
	if(glIsTexture(_id)==GL_TRUE)
		glDeleteTextures(1,&_id);
}

bool GpuTexture::createEmptyRGB(unsigned int width,unsigned int height) {
	createRGB(width,height,NULL);
}

bool GpuTexture::createRGB(unsigned int width,unsigned int height,const void* data) {
	_width=width;
	_height=height;
	if(glIsTexture(_id)==GL_TRUE)
		glDeleteTextures(1,&_id);
	glGenTextures(1,&_id);
	checkGL();
	glBindTexture(GL_TEXTURE_2D,_id);
	checkGL();
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data);
	checkGL();
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	checkGL();
	glBindTexture(GL_TEXTURE_2D,0);
	return true;
}

bool GpuTexture::createFromBGRMat(Mat& img) {
	if(img.empty()) {
		throw "GpuTexture : Empty image.";
		return false;
	}
	flip(img,img,0);
	cvtColor(img,img,CV_BGR2RGB);
	glPixelStorei(GL_UNPACK_ALIGNMENT,(img.step&3)?1:4);
	checkGL();
	return createRGB(img.cols,img.rows,img.data);
}

bool GpuTexture::createFromTGAFile(string fileName) {
	char* buffer=NULL;
	FILE* f;
	unsigned char tgaheader[12];
	unsigned char attributes[6];
	unsigned int imagesize;

	f=fopen(fileName.c_str(),"rb");
	if(f==NULL) return false;

	if(fread(&tgaheader,sizeof(tgaheader),1,f)==0) {
		fclose(f);
		return false;
	}
	
	if(fread(attributes,sizeof(attributes),1,f)==0) {
		fclose(f);
		return false;
	}

	int width=attributes[1]*256+attributes[0];
	int height=attributes[3]*256+attributes[2];
	imagesize=attributes[4]/8*width*height;
	
	buffer=(char*) malloc(imagesize);
	if(buffer==NULL) {
		fclose(f);
		return false;
	}

	if(fread(buffer,1,imagesize,f)!=imagesize) {
		free(buffer);
		return false;
	}

	fclose(f);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	bool success=createRGB(width,height,buffer);
	free(buffer);
	
	return success;
}

/*bool GpuTexture::saveToRGBMat(Mat& img) {
	glPixelStorei(GL_PACK_ALIGNMENT,(img.step&3)?1:4);
	glPixelStorei(GL_PACK_ROW_LENGTH,img.step/img.elemSize());
	glReadPixels(0,0,
}*/

GLuint GpuTexture::id() { return _id; }
unsigned int GpuTexture::width() { return _width; }
unsigned int GpuTexture::height() { return _height; }

GpuGlFbo::GpuGlFbo(GpuTexture* texture) {
	_id=0;
	_width=texture->width();
	_height=texture->height();
	_texture=texture;
	_depthBufferId=0;

	if(glIsFramebuffer(_id)==GL_TRUE)
		glDeleteFramebuffers(1,&_id);
	glGenFramebuffers(1,&_id);
	glBindFramebuffer(GL_FRAMEBUFFER,_id);

		if(glIsRenderbuffer(_depthBufferId)==GL_TRUE)
			glDeleteRenderbuffers(1,&_depthBufferId);
		glGenRenderbuffers(1,&_depthBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER,_depthBufferId);
			glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,_width,_height);
		glBindRenderbuffer(GL_RENDERBUFFER,0);

		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,texture->id(),0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,_depthBufferId);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE) {
			glDeleteFramebuffers(1,&_id);
			glDeleteRenderbuffers(1,&_depthBufferId);
			throw "GpuGlFbo : created GL FBO is invalid.";
			return;
		}

	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

GpuGlFbo::~GpuGlFbo() {
	if(glIsFramebuffer(_id)==GL_TRUE)
		glDeleteFramebuffers(1,&_id);
	if(glIsRenderbuffer(_depthBufferId)==GL_TRUE)
		glDeleteRenderbuffers(1,&_depthBufferId);
	//delete _texture;
}

void GpuGlFbo::get(cv::Mat& out) {
	//glBindFramebuffer(GL_FRAMEBUFFER,_id);
	glBindRenderbuffer(GL_RENDERBUFFER,_texture->id());
	//glPixelStorei(GL_PACK_ALIGNMENT,(out.step&3)?1:4);
	//glPixelStorei(GL_PACK_IMAGE_HEIGHT,480);
	//cvtColor(out,out,CV_RGB2BGR);
	//flip(out,out,0);
	glPixelStorei(GL_PACK_ALIGNMENT,1);
	glReadPixels(0,0,_width,_height,GL_RGB,GL_UNSIGNED_BYTE,out.data);
	flip(out,out,0);
	cvtColor(out,out,CV_RGB2BGR);
	imwrite("output.png",out);
	glBindRenderbuffer(GL_RENDERBUFFER,0);
	//glBindFramebuffer(GL_FRAMEBUFFER,0);
}

GLuint GpuGlFbo::id() { return _id; }



