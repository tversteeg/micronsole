#include <ccTerm/ccTerm.h>

#include <ccFont/ccFont.h>
#include <ccore/display.h>
#include <ccore/window.h>
#include <ccore/opengl.h>
#include <ccore/time.h>
#include <ccore/file.h>

#include <stdio.h>

#ifdef WINDOWS
#include <gl/GL.h>
#else
#include <GL/glew.h>
#endif

#define WIDTH 500
#define HEIGHT 400

GLuint gltex;
ccfFont font;
cctTerm term;

void loadFont(const char *file)
{
	unsigned flen = ccFileInfoGet(file).size;

	FILE *fp = fopen(file, "rb");
	if(!fp){
		fprintf(stderr, "Can not open file: %s\n", file);
		exit(1);
	}

	uint8_t *bin = (uint8_t*)malloc(flen);
	fread(bin, 1, flen, fp);

	fclose(fp);

	if(ccfBinToFont(&font, bin, flen) == -1){
		fprintf(stderr, "Binary font failed: invalid version\n");
		exit(1);
	}
}

int main(int argc, char **argv)
{
	ccDisplayInitialize();

	ccWindowCreate((ccRect){0, 0, WIDTH, HEIGHT}, "ccFont test", 0);
	ccWindowMouseSetCursor(CC_CURSOR_NONE);

	ccGLContextBind();

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glBindTexture(GL_TEXTURE_2D, 0);
	glGenTextures(1, &gltex);
	glBindTexture(GL_TEXTURE_2D, gltex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	loadFont("font.ccf");

	cctCreate(&term, WIDTH, HEIGHT);
	cctSetFont(&term, &font);

	cctPrintf(&term, "Hello");

	bool loop = true;
	while(loop){
		while(ccWindowEventPoll()){
			ccEvent event = ccWindowEventGet();
			switch(event.type){
				case CC_EVENT_WINDOW_QUIT:
					loop = false;
					break;
				case CC_EVENT_KEY_DOWN:
					if(event.keyCode == CC_KEY_ESCAPE){
						loop = false;
					}
					break;
				default: break;
			}
			cctHandleEvent(&term, event);
		}

		glClear(GL_COLOR_BUFFER_BIT);

		cctRender(&term, gltex);

		ccGLBuffersSwap();

		ccTimeDelay(6);
	}

	ccFree();
	return 0;
}
