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

#define CCT_MAX_CMD_LEN 128
#define CCT_MAX_CMDS 128

#define CCT_BLINK_INTERVAL 20

#define CCT_MIRROR_STDOUT

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _cctTerm cctTerm;
typedef void (*cctCmdptr) (cctTerm *term, int argc, char **argv);

typedef struct {
	unsigned char r, g, b;
} cctColor;

typedef struct {
	char c;
	cctColor fg, bg;
} cctGlyph;

struct _cctTerm {
	ccfFont *font;
	unsigned width, height;
	void *pixels;

	char *outstr;
	cctGlyph *outg;
	size_t outlen, outmaxlen;
	unsigned outwidth, outheight;
	bool outupdate;

	char in[CCT_MAX_CMD_LEN];
	size_t inpos;

	int blink;
	bool insert;

	cctCmdptr cmdfuncs[CCT_MAX_CMDS];
	char *cmds[CCT_MAX_CMDS];
	size_t ncmds;
};

void cctCreate(cctTerm *term, unsigned width, unsigned height);
void cctFree(cctTerm *term);
void cctResize(cctTerm *term, unsigned width, unsigned height);
void cctSetFont(cctTerm *term, ccfFont *font);

void cctSetInsert(cctTerm *term, bool mode);

void cctRender(cctTerm *term, GLuint texture);

void cctHandleEvent(cctTerm *term, ccEvent event);

void _cctPrintf(cctTerm *term, const char *text, ...) __attribute__((format(printf, 2, 3)));
#define cctPrintf(term, text, ...) _cctPrintf(term, text, ##__VA_ARGS__)

void cctMapCmd(cctTerm *term, const char *cmd, cctCmdptr cmdfunc);

#ifdef __cplusplus
}
#endif
