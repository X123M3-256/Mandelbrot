#include <stdio.h>
#include <stdlib.h>

#include<stdio.h>
#include<math.h>
#include<SDL/SDL.h>
#include<SDL/SDL_opengl.h>
#include<GL/gl.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 640
#define ASPECT_RATIO ((float)SCREEN_WIDTH/SCREEN_HEIGHT)
int running=1;

double mousex=0;
double mousey=0;
int click=0;
int rightclick=0;

int lastmilliseconds=0;
int curmilliseconds=0;
double deltaT;

float translatex=0.0,translatey=0.0,scale=1.0;

struct
{
GLuint Shader;
GLuint ColorMap;
GLuint ColorMapLocation;
GLuint StartLocation;
GLuint RangeLocation;
}Resources;

typedef struct
{
double real,imaginary;
}complex_t;

void UpdateTimer()
{
lastmilliseconds=curmilliseconds;
curmilliseconds=SDL_GetTicks();
deltaT=(double)(curmilliseconds-lastmilliseconds)/1000;printf("FPS: %f\n",1/deltaT);
}
void ProcessEvents()
{
SDL_Event Event;
    while(SDL_PollEvent(&Event))
    {
        switch(Event.type)
        {
        case SDL_QUIT:
        running=0;
        break;
        case SDL_KEYDOWN:
        //running=false;
        break;
        case SDL_MOUSEMOTION:
        mousex=(double)Event.motion.x/SCREEN_WIDTH;
        mousey=(double)Event.motion.y/SCREEN_HEIGHT;
        //printf("Mouse: %f,%f\n",mousex,mousey);
        break;
        case SDL_MOUSEBUTTONDOWN:
            if(Event.button.button==SDL_BUTTON_LEFT)click=1;
            else if(Event.button.button==SDL_BUTTON_RIGHT)rightclick=1;
        break;
        case SDL_MOUSEBUTTONUP:
            if(Event.button.button==SDL_BUTTON_LEFT)click=0;
            else if(Event.button.button==SDL_BUTTON_RIGHT)rightclick=0;
        break;
        }
    }
}

char* ReadFileString(char* filename)
{
FILE* file=fopen(filename,"r");
fseek(file,0,SEEK_END);
int length=ftell(file);
char* string=malloc(length+1);
fseek(file,0,SEEK_SET);
fread(string,1,length,file);
string[length]=0;
fclose(file);
return string;
}

void CreateShaders()
{
GLuint vertexshader=glCreateShader(GL_VERTEX_SHADER);
GLuint fragmentshader=glCreateShader(GL_FRAGMENT_SHADER);
Resources.Shader=glCreateProgram();
glAttachShader(Resources.Shader,vertexshader);
glAttachShader(Resources.Shader,fragmentshader);
char* vertsrc=ReadFileString("vertexshader.glsl");
char* fragsrc=ReadFileString("fragmentshader.glsl");
glShaderSource(vertexshader,1,&vertsrc,NULL);
glShaderSource(fragmentshader,1,&fragsrc,NULL);
glCompileShader(vertexshader);
glCompileShader(fragmentshader);
glLinkProgram(Resources.Shader);
free(vertsrc);
free(fragsrc);

Resources.ColorMapLocation=glGetUniformLocation(Resources.Shader,"colormap");
Resources.StartLocation=glGetUniformLocation(Resources.Shader,"ds_start");
Resources.RangeLocation=glGetUniformLocation(Resources.Shader,"ds_range");

char error[1024];
glGetShaderInfoLog(fragmentshader,1023,NULL,error);
printf("version %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));
printf("error %s\n",error);
}

#define TEXTURE_SIZE 15
void CreateTexture()
{
//Create noise
float texData[TEXTURE_SIZE*3]={
0.0,0.0,0.0, 0.0,0.0,1.0, 0.0,1.0,1.0, 0.0,1.0,0.0,  1.0,1.0,0.0,  1.0,0.0,0.0,  1.0,0.0,1.0,  1.0,1.0,1.0,
1.0,0.0,1.0, 1.0,0.0,0.0, 1.0,1.0,0.0, 0.0,1.0,0.0,  0.0,1.0,1.0,  0.0,0.0,1.0,  0.0,0.0,0.0
};
glGenTextures(1,&(Resources.ColorMap));
glBindTexture(GL_TEXTURE_1D,Resources.ColorMap);
glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
glTexImage1D(GL_TEXTURE_1D,0,GL_RGB8,TEXTURE_SIZE,0,GL_RGB,GL_FLOAT,texData);
}

void RenderScreen(complex_t start,complex_t range)
{
glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
glScalef(scale,scale,1.0);
glTranslatef(translatex,translatey,0);


glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE0,Resources.ColorMap);

glUseProgram(Resources.Shader);
glUniform1i(Resources.ColorMapLocation,0);
float ds_complex[4];
ds_complex[0]=(float)start.real;
ds_complex[1]=start.real-(double)ds_complex[0];
ds_complex[2]=(float)start.imaginary;
ds_complex[3]=start.imaginary-(double)ds_complex[2];
glUniform4fv(Resources.StartLocation,1,ds_complex);
ds_complex[0]=(float)range.real;
ds_complex[1]=range.real-(double)ds_complex[0];
ds_complex[2]=(float)range.imaginary;
ds_complex[3]=range.imaginary-(double)ds_complex[2];
glUniform4fv(Resources.RangeLocation,1,ds_complex);





glBegin(GL_QUADS);
glVertex2d(0.0,0.0);
glVertex2d(1.0,0.0);
glVertex2d(1.0,1.0);
glVertex2d(0.0,1.0);
glEnd();
SDL_GL_SwapBuffers();
glFinish();
}

SDL_Surface* Init()
{
    if(SDL_Init(SDL_INIT_EVERYTHING)<0)return NULL;
atexit(SDL_Quit);
SDL_Surface* surface=SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,32,SDL_HWSURFACE|SDL_OPENGL);
SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
CreateShaders();
CreateTexture();
return surface;
}



int main(int argc,char* argv[])
{
complex_t centre,half_range;
centre.real=0.0;
centre.imaginary=0.0;
half_range.real=2.0;
half_range.imaginary=2.0;

Init();
    while(running)
    {
    UpdateTimer();
    ProcessEvents();

    centre.real+=(mousex-0.5)*deltaT*half_range.real;
    centre.imaginary-=(mousey-0.5)*deltaT*half_range.imaginary;
    if(click)
    {
    half_range.real/=1.05;
    half_range.imaginary/=1.05;
    }
    else if(rightclick)
    {
    half_range.real*=1.05;
    half_range.imaginary*=1.05;
    }
    complex_t start,range;
    start.real=centre.real-half_range.real;
    start.imaginary=centre.imaginary-half_range.imaginary;
    range.real=2.0*half_range.real;
    range.imaginary=2.0*half_range.imaginary;
    RenderScreen(start,range);
    }
return 0;
}

