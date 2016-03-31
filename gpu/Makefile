all:
	g++ -o gpu gpu.cpp gpuGlContext.cpp gpuShader.cpp gpuTexture.cpp -lGLESv2 -lEGL -I/opt/vc/include -L/opt/vc/lib -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux -lbcm_host -lpthread `pkg-config --cflags --libs opencv`
