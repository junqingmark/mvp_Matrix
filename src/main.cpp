#include <iostream>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <X11/Xlib.h>
#include <sys/types.h>
#include <utils.h>
#include <imageReader.h>
#include <cstdlib>

using namespace std;

EGLSurface eglSurface;
EGLDisplay eglDisplay;

GLfloat vVertices[] = {
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.92f, 0.0f,

		-1.0f, 1.0f, 0.0f,
		-1.0f, 0.92f, 0.0f,
		1.0f, 0.92f, 0.0f
};

GLfloat vCoords[] = {
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		0.0f,1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
};



GLuint LoadShader(const char *shaderSrc, GLenum type)
{
	GLuint shader;
	GLint compiled;

	// Create the shader object
	shader = glCreateShader(type);
	if(shader == 0)	
		return 0;

	// Load the shader source
	glShaderSource(shader, 1, &shaderSrc, NULL);
	// Compile the shader
	glCompileShader(shader);

	// Check the compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if(!compiled)
	{
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		if(infoLen > 1)	
		{
			char* infoLog = static_cast<char*>(malloc(sizeof(char) * infoLen));
			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			cout << "Error compiling shader: " << infoLog << endl;
			
			free(infoLog);
		}

	glDeleteShader(shader);
	return 0;
	}

	return shader;
}

int GLESInit()
{
	GLchar vShaderStr[] =
		"attribute vec3 vVertices;\n"
		"attribute vec2 vCoords;\n"
		"layout(location = 2) uniform mat4 mvp;\n"
		"varying vec2 aCoords;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(vVertices, 1.0);\n"
		"	aCoords = vCoords;\n"
		"}\n";


	GLchar fShaderStr[] = 
		"precision mediump float;\n"
		"uniform sampler2D s_texture;"
		"varying vec2 aCoords;\n"
		"void main()\n"
		"{\n"
		"	gl_FragColor = texture2D(s_texture, aCoords);\n"
		"}\n";

	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint programObject;
	GLint linked;

	// Load the vertex/fragment shaders
	vertexShader = LoadShader(vShaderStr, GL_VERTEX_SHADER);
	fragmentShader = LoadShader(fShaderStr, GL_FRAGMENT_SHADER);

	GLfloat matrix[16];
	init_matrix(matrix);
	translate_xyz(matrix, 0.0f, 0.0f, 0.0f);

	// Create the program object
	programObject = glCreateProgram();
	if(programObject == 0)
		return 0;
	glAttachShader(programObject, vertexShader);
	glAttachShader(programObject, fragmentShader);

	// Bind vPosition to attribute 0
	glBindAttribLocation(programObject, 0, "vVertices");
	glBindAttribLocation(programObject, 1, "vCoords");
	glUniformMatrix4fv(2, 1, GL_FALSE, matrix);

	// Link the program
	glLinkProgram(programObject);



	// Check the link status
	glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
	if(!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
		if(infoLen > 1)
		{
			char* infoLog = static_cast<char*>(malloc(sizeof(char) * infoLen));
			glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
			cout << "Error linking program: " << infoLog << endl;
			free(infoLog);
		}
		glDeleteProgram(programObject);
		return 0;
	}
	else
	{
		glUseProgram(programObject);
	}

	return 1;

}

EGLBoolean InitializeWindow(EGLNativeWindowType surface_id, EGLNativeDisplayType display_id)
{
	const EGLint configAttributes[] = 
	{
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_DEPTH_SIZE,24,
		EGL_NONE
	};

	const EGLint contextAttributes[]=
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	eglDisplay = eglGetDisplay(display_id);
	if (eglDisplay == EGL_NO_DISPLAY)
	{
		
		cout << "Fail to get the native display!" << endl;
		return EGL_FALSE;
	}

	EGLint major,minor;
	//EGLBoolean eglInitialize(EGLDisplay display, EGLint* majorVersion, EGLint* minorVersion);  
	if (!eglInitialize(eglDisplay,&major,&minor))
	{
	
		cout << "Fail to initialize the EGL !" << endl;
		return EGL_FALSE;
	}
	else
	{
		
		cout << "The version of EGL is: " << major <<" . " << minor <<endl;
	}

	EGLConfig config;
	EGLint ConfigNum;
	if (!eglChooseConfig(eglDisplay, configAttributes, &config, 1, &ConfigNum))
	{
		return EGL_FALSE;
	}

	//EGLSurface eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config,
	//			  EGLNativeWindowType win,
	//			  const EGLint *attrib_list);

	
	eglSurface = eglCreateWindowSurface(eglDisplay, config, surface_id, NULL);
	if (eglSurface == EGL_NO_SURFACE)
	{
		return EGL_FALSE;
	}

	//eglCreateContext(EGLDisplay dpy, EGLConfig config,
	//		    EGLContext share_context,
	//		    const EGLint *attrib_list);
	EGLContext context = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, contextAttributes);
	if (context == EGL_NO_CONTEXT)
	{
		return EGL_FALSE;
	}

	//eglMakeCurrent(EGLDisplay dpy, EGLSurface draw,
	//		  EGLSurface read, EGLContext ctx);
	if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, context))
	{
		return EGL_FALSE;
	}

	return EGL_TRUE;
}

void draw(int width, int height)
{
	glViewport(0, 0, width, height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, vCoords);
	glDrawArrays(GL_TRIANGLES, 0, 6);

}

int main(int argc, char* argv[])
{
	KeySym   escape;
	Display *x_display;
	imageReader* bmpReader = imageReader::createInstance();

	string fileName = "test.bmp";
	char* pixel = NULL;

	pixel = bmpReader->loadImage(fileName);
	unsigned int width = bmpReader->getImageWidth();
	unsigned int height = bmpReader->getImageHeight();
	int depth = bmpReader->getImageDepth();

	cout << "The size of the window is " << width << "*" << height << endl;
	cout << "The depth of the picture " << depth << endl;

	int window_width = 720;
	int window_height = 1280;
	
	
	x_display = XOpenDisplay(NULL);
	int screen = DefaultScreen(x_display);

	Window win = XCreateSimpleWindow(x_display, RootWindow(x_display, screen), 0, 0, window_width, window_height, 0, 0, WhitePixel(x_display,screen) );

	XSelectInput(x_display, win, ExposureMask | KeyPressMask | KeyReleaseMask);
	escape = XKeysymToKeycode(x_display, XK_Escape);

	XMapWindow(x_display,win);

	if (!InitializeWindow(win, x_display))
	{
		
		cout << "Intiallize the EGL window Fail!" << endl;
		return 1;
	}

	if(GLESInit() == 0)
	{
		cout << "Fail to Init GLES!" << endl;
	}

	GLuint textureID;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixel);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int quit = 0;
	XEvent event;

	while(!quit)
	{
		XNextEvent(x_display, &event);
		if(event.type == KeyPress)
		{
			
			if(event.xkey.keycode == escape)
			{
				quit = 1;	
			}
			
		}

		draw(width, height);
		eglSwapBuffers(eglDisplay, eglSurface);
	}

	delete pixel;

	return 1;
}