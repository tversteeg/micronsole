#include <ccTerm/ccTerm.h>

#include <stdio.h>
#include <string.h>

#define _CCT_PIXEL_FORMAT GL_RGBA
#define _CCT_PIXEL_TYPE GL_UNSIGNED_BYTE

typedef struct {
	unsigned char r, g, b, a;
} _cctPixel;

static void _cctSetSize(cctTerm *term, unsigned width, unsigned height)
{
	if(term->pixels){
		free(term->pixels);
		term->pixels = NULL;
	}
	term->pixels = (void*)malloc(width * height * sizeof(_cctPixel));

	term->width = width;
	term->height = height;
}

static void __cctPrintf(cctTerm *term, const char *text)
{
	size_t len = strlen(text);

	size_t total = term->outlen + len;
	while(total > term->outmaxlen){
		term->outmaxlen *= 2;

		//TODO make safe
		term->out = (char*)realloc(term->out, term->outmaxlen);
	}

	strcpy(term->out + term->outlen, text);
	term->outlen = total;
}

void cctCreate(cctTerm *term, unsigned width, unsigned height)
{
	_cctSetSize(term, width, height);

	term->outmaxlen = 16;
	term->out = (char*)calloc(term->outmaxlen, 1);
	term->outlen = 0;
}

void cctFree(cctTerm *term)
{
	free(term->out);
}

void cctResize(cctTerm *term, unsigned width, unsigned height)
{
	_cctSetSize(term, width, height);
}

void cctSetFont(cctTerm *term, ccfFont *font)
{
	term->font = font;
}

void cctRender(cctTerm *term, GLuint texture)
{
	memset(term->pixels, 0, term->width * term->height * sizeof(_cctPixel));

	ccfFontConfiguration conf = {.x = 0, .y = 0, .width = term->width, .wraptype = 0, .color = {1.0, 1.0, 0.0}};
	int status = ccfGLTexBlitText(term->font, term->out, &conf, term->width, term->height, _CCT_PIXEL_FORMAT, _CCT_PIXEL_TYPE, term->pixels);
	if(status < 0){
		fprintf(stderr, "ccfGLTexBlitText failed with status code: %d\n", status);
		exit(1);
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, term->width, term->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, term->pixels);

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

void cctHandleEvent(cctTerm *term, ccEvent event)
{
	if(event.type != CC_EVENT_KEY_DOWN){
		return;
	}
}

void _cctPrintf(cctTerm *term, const char *text, ...)
{
	va_list arg;

	va_start(arg, text);
	char buf[256];
	vsprintf(buf, text, arg);
	__cctPrintf(term, buf);
	va_end(arg);
}
