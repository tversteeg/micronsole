/*
	 Micronsole - v1.00 - public domain
	 no warrenty implied; use at your own risk.
	 authored from 2016 by Thomas Versteeg

DEFINES:
MC_OUTPUT_TEXTURE_[RGB, RGBA, BGR, BGRA] - render the output as a texture with the defined pixel format
MC_DYNAMIC_ARRAYS - dynamically grow the array size instead of using static sizes
MC_MAX_COMMANDS (n>0) - maximum number of commands that can be registered, only useable when MC_DYNAMIC_ARRAYS is not defined
MC_MAX_INPUT_LENGTH (n>0) - maximum length of the input string, only useable when MC_DYNAMIC_ARRAYS is not defined

LICENSE:
This software is dual-licensed to the public domain and under the following
license: you are granted a perpetual, irrevocable license to copy, modify,
publish and distribute this file as you see fit.
*/

#ifndef MC_H
#define MC_H

#if defined MC_OUTPUT_TEXTURE_RGB || defined MC_OUTPUT_RGBA || defined MC_OUTPUT_BGR || defined MC_OUTPUT_BGRA
#define MC_OUTPUT_TEXTURE
#endif

#ifndef MC_MAX_COMMANDS
#define MC_MAX_COMMANDS 128
#endif

#ifndef MC_MAX_INPUT_LENGTH
#define MC_MAX_INPUT_LENGTH 256
#endif

#include <stdbool.h>
#include <string.h>

#ifdef MC_OUTPUT_TEXTURE
struct {
#ifdef MC_OUTPUT_TEXTURE_RGB
	unsigned char r, g, b;
#elif defined MC_OUTPUT_TEXTURE_RGBA
	unsigned char r, g, b, a;
#elif defined MC_OUTPUT_TEXTURE_BGR
	unsigned char b, g, r;
#elif defined MC_OUTPUT_TEXTURE_BGRA
	unsigned char b, g, r, a;
#else
#error Undefined pixel format for texture output
#endif
} mc_pixel;
#endif

typedef struct mc_console _mc_console_t;
typedef void (*mc_cmd_ptr) (_mc_console_t *term, int argc, char **argv);

struct mc_console {
	char *outstr;
	unsigned outlen, outmaxlen;
	unsigned outwidth, outheight;
	bool outupdate;

#ifdef MC_DYNAMIC_ARRAYS
	char *instr;
#else
	char instr[MC_MAX_INPUT_LENGTH];
#endif
	unsigned inpos;

	bool insert;

#ifdef MC_DYNAMIC_ARRAYS
	mc_cmd_ptr *cmdfuncs;
	char *cmds;
#else
	mc_cmd_ptr cmdfuncs[MC_MAX_COMMANDS];
	char *cmds[MC_MAX_COMMANDS];
#endif
	unsigned ncommands;

#ifdef MC_OUTPUT_TEXTURE
	unsigned width, height;
	struct mc_pixel *pixels;
#endif
};

void mc_create(struct mc_console *con);
void mc_free(struct mc_console *con);

void mc_map(struct mc_console *con, const char *cmd, mc_cmd_ptr func);

#ifdef MC_OUTPUT_TEXTURE
void mc_set_texture_size(struct mc_console *con, unsigned width, unsigned height);
#endif

#ifdef MC_IMPLEMENTATION

void mc_create(struct mc_console *con)
{
	memset(con, 0, sizeof(struct mc_console));
	
#ifndef MC_DYNAMIC_ARRAYS
	memset(con->instr, 0, MC_MAX_INPUT_LENGTH);
	memset(con->cmdfuncs, 0, MC_MAX_COMMANDS * sizeof(mc_cmd_ptr));
	memset(con->cmds, 0, MC_MAX_COMMANDS);
#endif
}

#endif

#endif
