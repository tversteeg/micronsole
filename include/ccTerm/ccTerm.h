#pragma once

#include <stdint.h>
#include <stdarg.h>

#ifdef WINDOWS
#include <gl/GL.h>
#else
#include <GL/glew.h>
#endif

#include <ccFont/ccFont.h>
#include <ccore/event.h>

#define MAX_CMD_LEN 128

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _cctTerm cctTerm;
typedef void (*cctCmdptr) (cctTerm *term, int argc, char **argv);

struct _cctTerm {
	ccfFont *font;
	unsigned width, height;
	void *pixels;

	char *out;
	size_t outlen, outmaxlen;
	unsigned outwidth, outheight;
};

void cctCreate(cctTerm *term, unsigned width, unsigned height);
void cctFree(cctTerm *term);
void cctResize(cctTerm *term, unsigned width, unsigned height);
void cctSetFont(cctTerm *term, ccfFont *font);

void cctRender(cctTerm *term, GLuint texture);

void cctHandleEvent(cctTerm *term, ccEvent event);

void _cctPrintf(cctTerm *term, const char *text, ...) __attribute__((format(printf, 2, 3)));
#define cctPrintf(term, text, ...) _cctPrintf(term, text, ##__VA_ARGS__)

void cctMapCmd(cctTerm *term, const char *cmd, cctCmdptr cmdfunc);

#ifdef __cplusplus
}
#endif
