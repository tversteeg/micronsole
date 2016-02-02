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

#define WIDTH 400
#define HEIGHT 50

#define TEST_FORMAT GL_RGB
#define TEST_TYPE GL_UNSIGNED_BYTE

typedef struct {
	unsigned char r, g, b;
} pixel_t;

typedef struct {
	unsigned int width, height;
	pixel_t *pixels;
} texture_t;

GLuint gltex;
texture_t tex;
ccfFont *font;

void renderTexture(texture_t tex)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, gltex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex.width, tex.height, 0, TEST_FORMAT, TEST_TYPE, tex.pixels);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(1.0f, 1.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void blitText(ccfFont *font, const char *text, int x, int y)
{
	ccfFontConfiguration conf = {.x = x, .y = y, .width = WIDTH, .wraptype = 0, .color = {1.0, 1.0, 0.0}};
	int status = ccfGLTexBlitText(font, text, &conf, tex.width, tex.height, TEST_FORMAT, TEST_TYPE, (void*)tex.pixels);
	if(status < 0){
		fprintf(stderr, "ccfGLTexBlitText failed with status code: %d\n", status);
		exit(1);
	}
}

ccfFont* loadFont(const char *file)
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

	ccfFont *binfont = (ccfFont*)malloc(sizeof(ccfFont));
	if(ccfBinToFont(binfont, bin, flen) == -1){
		fprintf(stderr, "Binary font failed: invalid version\n");
		exit(1);
	}

	return binfont;
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

	tex.width = WIDTH;
	tex.height = HEIGHT;
	tex.pixels = (pixel_t*)malloc(tex.width * tex.height * sizeof(pixel_t));
	font = loadFont("font.ccf");

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
		}

		renderTexture(tex);

		blitText(font, "Bla", 0, 0);

		ccGLBuffersSwap();

		ccTimeDelay(6);
	}

	ccFree();
	return 0;
}
