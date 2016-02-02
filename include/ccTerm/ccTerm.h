#pragma once

#ifdef WINDOWS
#include <gl/GL.h>
#else
#include <GL/glew.h>
#endif

#include <stdint.h>

#include <ccFont/ccFont.h>
#include <ccore/event.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _cctTerm_t cctTerm_t;
typedef void (*cctCmdptr_t) (cctTerm_t *con, int argc, char **argv);

struct _cctTerm_t {
	font_t *font;
	texture_t tex;
	char *buf, cmdstr[MAX_CMD_LEN], **cmdnames;
	cmdptr_t *cmdfs;
	bool active;
	unsigned int bufmaxsize, buflen, buflines, bufmaxlines, cmdstrlen, cmds;
};

void cctInit(cctTerm_t *con, unsigned int width, unsigned int height);
void cctResize(cctTerm_t *con, unsigned int width, unsigned int height);
void cctSetFont(cctTerm_t *con, font_t *font);

void cctRender(cctTerm_t *con, texture_t *target);

void cctHandleEvent(cctTerm_t *con, ccEvent event);

void _cctPrintf(cctTerm_t *con, const char *text, ...) __attribute__((format(printf, 2, 3)));
#define cctPrintf(con, text, ...) _cctPrintf(con, text, ##__VA_ARGS__)

void cctMapCmd(cctTerm_t *con, const char *cmd, cctCmdptr_t cmdfunc);

#ifdef __cplusplus
}
#endif
