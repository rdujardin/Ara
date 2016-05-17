/*
gpu.cpp (part of Ara, https://github.com/rdujardin/Ara)

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

//g++ -o gpu gpu.cpp gpuGlContext.cpp gpuShader.cpp gpuTexture.cpp -lGLESv2 -lEGL -I/opt/vc/include -L/opt/vc/lib -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux -lbcm_host -lpthread `pkg-config --cflags --libs opencv`

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <cstdlib>
#include "gpuGlContext.h"
#include "gpuShader.h"
#include "gpuTexture.h"

using namespace cv;
using namespace std;

class GpuModule {
	unsigned int _width, _height;
	GpuTexture* _outTex;
	GpuGlFbo* _outFbo;
	GpuShader* _frag;
	GpuProgram* _prgm;

	std::vector<std::string> &strSplit(const std::string &s, char delim, std::vector<std::string> &elems) {
	    std::stringstream ss(s);
	    std::string item;
	    while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	    }
	    return elems;
	}


	std::vector<std::string> strSplit(const std::string &s, char delim) {
	    std::vector<std::string> elems;
	    strSplit(s, delim, elems);
	    return elems;
	}

	bool strStartsWith(const std::string &prefix,const std::string& str) {
		if(str.size()>=prefix.size()) return str.substr(0, prefix.size()) == prefix;
		else return false;
	}

	std::string preprocess_forLoop(std::vector<std::string>& src,std::vector<std::string>::iterator firstLine,std::map<std::string,int> iterators,std::vector<std::string>::iterator& closeLine) {
		std::vector<std::string>::iterator fend=firstLine+1;
		if(fend==src.end()) return ""; //syntax error
		int recursion=0;
		while(fend!=src.end()) {
			if(strStartsWith("}",*fend)) recursion--;
			else if(strStartsWith("for(",*fend)) recursion++;
			fend++;
			if(recursion==-1) break;
		}
		fend--;

		closeLine=fend;

		std::string out;
		std::vector<std::string> args;
		strSplit(*firstLine,';',args);
		out+=strSplit(args.front(),'(').back()+";\n";
		int index=atoi(strSplit(strSplit(args.front(),'(').back(),'=').back().c_str());
		while(index<=atoi(args.at(1).c_str())) {
			iterators[strSplit(strSplit(args.front(),'(').back(),'=').front()]=index;
			for(std::vector<std::string>::iterator ct=firstLine+1;ct!=closeLine;ct++) {
				if(strStartsWith("for(",*ct)) {
					std::vector<std::string>::iterator clcl=ct;
					out+=preprocess_forLoop(src,ct,iterators,clcl);
					ct=clcl;
				}
				else {
					//replace indexes
					size_t pos=0,close=0;
					std::string outct=*ct;
					do {
						pos=outct.find("$$(",0);
						close=outct.find(")$$",0);
						if(pos==std::string::npos || close==std::string::npos) break;
						std::string then=outct.substr(pos+3,close-pos-3);
						std::ostringstream oss;
						oss << iterators[then];
						outct.replace(pos,close-pos+3,oss.str());
					} while(pos!=std::string::npos);
					out+=outct+"\n";
				}
			}
			index++;
		}
		return out;
	}

	std::string preprocessFragSrc(std::string src) {
		std::map<std::string,int> iterators;
		std::string out="";
		std::vector<std::string> lines;
		strSplit(src,'\n',lines);
		std::vector<std::string>::iterator it=lines.begin();
		while(it!=lines.end()) {
			if(strStartsWith("for(",*it)) {
				std::vector<std::string>::iterator clcl;
				out+=preprocess_forLoop(lines,it,iterators,clcl);
				it=clcl+1;
			}
			else {
				out+=(*it)+"\n";
				it++;
			}
		}
		std::cout << "#PREPROCESSED :" << std::endl << out << std::endl << std::endl;
		return out;
	}
public:
	GpuModule(unsigned int width,unsigned int height,GpuShader* vert,std::string fragSrc) {
		_width=width;
		_height=height;
		_outTex=new GpuTexture();
		_outTex->createEmptyRGB(width,height); //img.cols, img.rows	
		//_outFbo=new GpuGlFbo(_outTex);
		_outFbo=NULL;
		vector<std::string> gpuParams;
		gpuParams.push_back("vPosition");
		gpuParams.push_back("fTexPos");
		//_frag=new GpuShader(GL_FRAGMENT_SHADER,std::string("varying vec2 fTexPos;\n uniform sampler2D uTexture;\n void main() {\n"+preprocessFragSrc(fragSrc)+"\n}\n"));
		_frag=new GpuShader(GL_FRAGMENT_SHADER,std::string("varying vec2 fTexPos;\n uniform sampler2D uTexture;\n void main() {\n"+fragSrc+"\n}\n"));
		_prgm=new GpuProgram(*vert,*_frag,gpuParams);
		try { _prgm->load(); }
		catch(std::string err) { throw; }
	}

	~GpuModule() {
		delete _prgm;
		delete _frag;
		if(_outFbo) delete _outFbo;
		delete _outTex;
	}

	void draw(GpuTexture* input) {
		if(_outFbo) delete _outFbo;
		_outFbo=new GpuGlFbo(_outTex);
		glBindFramebuffer(GL_FRAMEBUFFER,_outFbo->id());
		glClearColor(0.0,0.0,0.0,1.0);
		glViewport(0,0,_width,_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		float square_vertices[8]={-1.0,1.0,1.0,1.0,-1.0,-1.0,1.0,-1.0};
		_prgm->enable();
	
		glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,square_vertices);
		glEnableVertexAttribArray(0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,input->id());
		glUniform1i(glGetUniformLocation(_prgm->id(),"uTexture"),0);
			glDrawArrays(GL_TRIANGLE_STRIP,0,4);
		glDisableVertexAttribArray(0);

		_prgm->disable();
		//glBindFramebuffer(GL_FRAMEBUFFER,0);
	}

	void exportTo(cv::Mat& img) {
		if(_outFbo) _outFbo->get(img);
	}

	GpuTexture* out() { return _outTex; }
};

int main() {
	/*try {
	namedWindow("out",1);
	Mat img=imread("perroquet.jpg",CV_LOAD_IMAGE_COLOR);
	if(!img.data) return -1;
	imshow("out",img);
	cout << "1" << endl;
	GpuContext gpu;
	cout << "2" << endl;
	//GpuModule mod1("gl_FragColor=vec4(greaterThanEqual(texture2D(uTexture,fTexPos),vec4(0.5)));");
	GpuModule mod1("gl_FragColor=texture2D(uTexture,fTexPos);\n");
	cout << "3" << endl;
	gpu.addModule("threshold",&mod1);
	cout << "4" << endl;
	//if(!gpu.addModule(string("threshold"),GpuModule("texture2D(uTexture,fTexPos)=vec4(greaterThanEqual(texture2D(uTexture,fTexPos),vec4(0.5)));"))) return 1;
	while(true) {
		if(!gpu.loop(img)) return 1;
		cout << "5" << endl;
		imshow("out",img);
		if(waitKey(30)>=0) break;
	}
	}
	catch(const char* err) {
		cout << err << endl;
		return -1;
	}
	return 0;*/
	try {
	GpuGlContext* glCtx=new GpuGlContext(640,480);

	cout << "Begin" << endl;
	namedWindow("out",1);
	Mat img=imread("perroquet.jpg",CV_LOAD_IMAGE_COLOR);
	if(!img.data) return -1;
	cout << "Image loaded" << endl;
	
	//GpuContext gpu;
	cout << "Context created" << endl;
	//gpu.loop(img);

	std::cout << "Entering GPU op" << std::endl;
	GpuTexture* tex=new GpuTexture();
	tex->createFromBGRMat(img);
	std::cout << "Texture created" << std::endl;

	/*GpuTexture* outTex=new GpuTexture();
	outTex->createEmptyRGB(img.cols,img.rows);	
	GpuGlFbo* fbo=new GpuGlFbo(outTex);*/
	//std::cout << "FBO created" << std::endl;

	/*GpuShader frag(GL_FRAGMENT_SHADER,_fragIntro+"gl_FragColor=vec4(1.0,0.0,0.0,1.0);"+_fragOutro);
	std::cout << "Fragment shader created" << std::endl;
	vector<std::string> gpuParams;
	gpuParams.push_back("vPosition");
	gpuParams.push_back("fTexPos");
	GpuProgram prgm(_vertexShader,frag,gpuParams);
	std::cout << "Program created" << std::endl;
	try {prgm.load(); }
	catch(string err) {
		throw err;
	}
	std::cout << "Program loaded" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER,fbo.id());
	glClearColor(0.0,0.0,0.0,1.0);
	glViewport(0,0,img.cols,img.rows);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	std::cout << "Drawing init done" << std::endl;

	
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	std::cout << "Drawing done" << std::endl;
*/

	//std::vector<GpuProgram*> modules;

	GpuShader* vert=new GpuShader(GL_VERTEX_SHADER,""
			"attribute vec2 vPosition;\n"
			"varying vec2 fTexPos;\n"
			"void main() {\n"
			"gl_Position=vec4(vPosition.x,vPosition.y,0.0,1.0);\n"
			"fTexPos=vec2((vPosition.x+1.0)/2.0,(vPosition.y+1.0)/2.0);\n"
			"}\n");

	/*GpuShader* frag1=new GpuShader(GL_FRAGMENT_SHADER,""
			"varying vec2 fTexPos;\n"
			"uniform sampler2D uTexture;\n"
			"void main() {\n"
			"gl_FragColor=vec4(greaterThanEqual(texture2D(uTexture,fTexPos),vec4(0.5)));\n"
			"}\n");*/

	/*vector<std::string> gpuParams;
	gpuParams.push_back("vPosition");
	gpuParams.push_back("fTexPos");*/
	//GpuProgram* prgm=new GpuProgram(*vert,*frag,gpuParams);

	/*modules.push_back(new GpuProgram(*vert,*(new GpuShader(GL_FRAGMENT_SHADER,""
			"varying vec2 fTexPos;\n"
			"uniform sampler2D uTexture;\n"
			"void main() {\n"
			"gl_FragColor=vec4(greaterThanEqual(texture2D(uTexture,fTexPos),vec4(0.5)));\n"
			"}\n")),gpuParams));*/

	//try { prgm->load(); }
	/*try { for(std::vector<GpuProgram*>::iterator it=modules.begin();it!=modules.end();++it) { (*it)->load(); } }
	catch(string err) { throw err; }*/

	std::vector<GpuModule*> modules;
	modules.push_back(new GpuModule(img.cols,img.rows,vert,"gl_FragColor=vec4(greaterThanEqual(texture2D(uTexture,fTexPos),vec4(0.5)));"));
	modules.push_back(new GpuModule(img.cols,img.rows,vert,"gl_FragColor=vec4(texture2D(uTexture,fTexPos).x,0.0,0.0,1.0);"));
	/*modules.push_back(new GpuModule(img.cols,img.rows,vert,""
		"vec4 col=vec4(0);\n"
		"float total_added=0.0;\n"
		"for(int xoffset=-10;xoffset<=10;xoffset++) {\n"
			"for(int yoffset=-10;yoffset<=10;yoffset++) {\n"
				"vec2 offset=vec2(xoffset,yoffset);\n"
				"float prop=1.0/(offset.x*offset.x+offset.y*offset.y+1.0);\n"
				"total_added+=prop;\n"
				"col+=prop*texture2D(uTexture,fTexPos+offset);\n"
			"}\n"
		"}\n"
		"col/=total_added;\n"
		"gl_FragColor=clamp(col,vec4(0),vec4(1));"));*/
	/*modules.push_back(new GpuModule(img.cols,img.rows,vert,""
		"vec4 col=vec4(0);\n"
		"float totalAdded=0.0;\n"
		"int xoffset=-2;\n"
		"int yoffset=-2;\n"
		"for(xoffset=-2;xoffset<=2;xoffset++) { \n"
			/*"for(yoffset=-10;yoffset<=10;yoffset++) { \n"
				"vec2 offset=vec2(xoffset,yoffset);\n"
				"float prop=1.0/(offset.x*offset.x+offset.y*offset.y+1.0);\n"
				"totalAdded+=prop;\n"
				"col+=prop*texture2D(uTexture,fTexPos+offset);\n"
			"}\n"//*
		"}\n"
		//"col/=totalAdded;\n"
		"gl_FragColor=clamp(col,vec4(0),vec4(1));"));
			//"gl_FragColor=vec4(texture2D(uTexture,fTexPos).y,0.0,0.0,1.0);"));*/

	modules.push_back(new GpuModule(img.cols,img.rows,vert,""
		"if(fTexPos.x>0.5 || fTexPos.y<0.5) gl_FragColor=texture2D(uTexture,fTexPos);"));

	/*modules.push_back(new GpuModule(img.cols,img.rows,vert,""
		"vec4 col=vec4(0);\n"
		"float total_added=0.0;\n"
		"int xoffset;\n"
		"int yoffset;\n"
		"vec2 offset;\n"
		"float prop;\n"
		"for(xoffset=-1;1;xoffset++) {\n"
			"for(yoffset=-1;1;yoffset++) {\n"
				"offset=vec2($$(xoffset)$$,$$(yoffset)$$);\n"
				"prop=1.0/(offset.x*offset.x+offset.y*offset.y+1.0);\n"
				"total_added+=prop;\n"
				"col+=prop*texture2D(uTexture,vec2(fTexPos.x+offset.x/640.0,fTexPos.y+offset.y/480.0));\n"
			"}\n"
		"}\n"
		"col/=total_added;\n"
		"gl_FragColor=clamp(col,vec4(0),vec4(1));"));*/

	modules.push_back(new GpuModule(img.cols,img.rows,vert,""
		"vec4 col=vec4(0);\n"
		"float total_added=0.0;\n"
		//"int xoffset;\n"
		//"int yoffset;\n"
		"vec2 offset;\n"
		"float prop;\n"
		"for(int xoffset = -10; xoffset <= 10; xoffset++)\n"
		"{\n"
			"for(int yoffset=-10; yoffset<=10; yoffset++)\n"
			"{\n"
				"offset=vec2(xoffset,yoffset);\n"
				"prop=1.0/(offset.x*offset.x+offset.y*offset.y+1.0);\n"
				"total_added+=prop;\n"
				"col+=prop*texture2D(uTexture,vec2(fTexPos.x+offset.x/640.0,fTexPos.y+offset.y/480.0));\n"
			"}\n"
		"}\n"
		"col/=total_added;\n"
		"gl_FragColor=clamp(col,vec4(0),vec4(1));"));

	while(true) {
		/*glClearColor(0.0,0.0,0.0,1.0);
		glViewport(0,0,img.cols,img.rows);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/

		 //float square_vertices[8]={-1.0,1.0,1.0,1.0,-1.0,-1.0,1.0,-1.0};

		/*modules.front()->enable();
	
		glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,square_vertices);
		glEnableVertexAttribArray(0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,tex->id());
		glUniform1i(glGetUniformLocation(modules.front()->id(),"uTexture"),0);
			glDrawArrays(GL_TRIANGLE_STRIP,0,4);
		glDisableVertexAttribArray(0);

		modules.front()->disable();*/

		/*cout << "Draw 1" << endl;
		modules.front()->draw(tex);
		cout << "Draw 2" << endl;
		modules.back()->draw(modules.front()->out());
		cout << "Export" << endl;
		modules.front()->exportTo(img);*/

		GpuTexture* prev=tex;
		for(std::vector<GpuModule*>::iterator it=modules.begin();it!=modules.end();++it) {
			(*it)->draw(prev);
			prev=(*it)->out();
		}
		modules.back()->exportTo(img);

		glCtx->refresh(); //no need with one draw
		//fbo->get(img);
		imshow("out",img);
		if(waitKey(30)>=0) break;
	}

	cout << "Bye" << endl;

	for(std::vector<GpuModule*>::iterator it=modules.begin();it!=modules.end();++it) {
		delete *it;
	}
	//delete fbo;
	delete tex;
	delete vert;
	delete glCtx;
	}
	catch(char const* err) { cout << err << endl; }
	catch(std::string err) { cout << err << endl; }

	return 0;

}

