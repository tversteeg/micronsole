#include <ccTerm/ccTerm.h>

#include <stdio.h>
#include <string.h>

#define _CCT_PIXEL_FORMAT GL_RGBA
#define _CCT_PIXEL_TYPE GL_UNSIGNED_BYTE

typedef struct {
	unsigned char r, g, b, a;
} _cctPixel;

static void _cctCalcWidth(cctTerm *term)
{
	if(term->font == NULL){
		return;
	}
	term->outwidth = (term->width - 4) / term->font->gwidth;
	term->outheight = ((term->height - 4) / term->font->gheight - 1);
}

static void _cctSetSize(cctTerm *term, unsigned width, unsigned height)
{
	if(term->pixels){
		free(term->pixels);
		term->pixels = NULL;
	}
	term->pixels = (void*)malloc(width * height * sizeof(_cctPixel));

	term->width = width;
	term->height = height;

	_cctCalcWidth(term);
}

static void _cctRenderOut(cctTerm *term)
{
	size_t len = strlen(term->out);
	if(len == 0){
		return;
	}

	ccfFontConfiguration conf = {
		.x = 0, 
		.y = 0, 
		.width = term->width, 
		.wraptype = 0, 
		.color = {1.0, 1.0, 1.0}
	};

	unsigned x = 0;
	unsigned y = 0;
	for(unsigned i = 0; i < len; i++){
		conf.x = x * term->font->gwidth + 2;
		conf.y = y * term->font->gheight + 2;

		char c = term->out[i];
		if(c == '\n'){
			x = 0;
			y++;
			continue;
		}else if(c == '\t'){
			x += (4 - x % 4);
		}else if(c != ' '){
			int status = ccfGLTexBlitChar(term->font, term->out[i], &conf, term->width, term->height, _CCT_PIXEL_FORMAT, _CCT_PIXEL_TYPE, term->pixels);
			if(status < 0){
				fprintf(stderr, "ccfGLTexBlitChar failed with status code: %d\n", status);
				exit(1);
			}
		}

		x++;
		if(x == term->outwidth){
			x = 0;
			y++;
		}
	}
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
	term->font = NULL;

	term->outmaxlen = 16;
	term->out = (char*)calloc(term->outmaxlen, 1);
	term->outlen = 0;

	_cctSetSize(term, width, height);
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

	_cctCalcWidth(term);
}

void cctRender(cctTerm *term, GLuint texture)
{
	memset(term->pixels, 0, term->width * term->height * sizeof(_cctPixel));

	_cctRenderOut(term);

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
