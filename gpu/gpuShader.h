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

