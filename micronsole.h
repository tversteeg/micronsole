/*
	 Micronsole - v1.00 - public domain
	 no warrenty implied; use at your own risk.
	 authored from 2016 by Thomas Versteeg

DEFINES:
MC_OUTPUT_TEXTURE_[RGB, RGBA, BGR, BGRA] - render the output as a texture with the defined pixel format
MC_DYNAMIC_ARRAYS - dynamically grow the array size instead of using static sizes
MC_MAX_COMMANDS (n>0) - maximum number of commands that can be registered, only useable when MC_DYNAMIC_ARRAYS is not defined
MC_MAX_COMMAND_LENGTH (n>0) - maximum string length of the command, only useable when MC_DYNAMIC_ARRAYS is not defined
MC_MAX_INPUT_LENGTH (n>0) - maximum length of the input string, only useable when MC_DYNAMIC_ARRAYS is not defined
MC_ASSERT - define the assert function, leave empty for no assertions

TODO:
UTF8 support
Option to replace stdlib

LICENSE:
This software is dual-licensed to the public domain and under the following
license: you are granted a perpetual, irrevocable license to copy, modify,
publish and distribute this file as you see fit.
*/

#ifndef MC_H
#define MC_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#if defined MC_OUTPUT_TEXTURE_RGB || defined MC_OUTPUT_RGBA || defined MC_OUTPUT_BGR || defined MC_OUTPUT_BGRA
#define MC_OUTPUT_TEXTURE
#endif

#ifndef MC_MAX_COMMANDS
#define MC_MAX_COMMANDS 128
#endif

#ifndef MC_MAX_INPUT_LENGTH
#define MC_MAX_INPUT_LENGTH 256
#endif

#ifndef MC_MAX_COMMAND_LENGTH
#define MC_MAX_COMMAND_LENGTH 64
#endif

#ifndef MC_ASSERT
#define MC_ASSERT(x) assert(x)
#endif

enum mc_keys {MC_KEY_LEFT, MC_KEY_RIGHT, MC_KEY_UP, MC_KEY_DOWN, MC_KEY_BACKSPACE};

#ifdef MC_OUTPUT_TEXTURE
struct mc_pixel {
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
};
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
	char **cmds;
#else
	mc_cmd_ptr cmdfuncs[MC_MAX_COMMANDS];
	char cmds[MC_MAX_COMMANDS][MC_MAX_COMMAND_LENGTH];
#endif
	unsigned ncmds;

#ifdef MC_OUTPUT_TEXTURE
	unsigned width, height;
	struct mc_pixel *pixels;
#endif
};

void mc_create(struct mc_console *con);
void mc_free(struct mc_console *con);

void mc_map(struct mc_console *con, const char *cmd, mc_cmd_ptr func);

void mc_input_key(struct mc_console *con, enum mc_keys key);
void mc_input_char(struct mc_console *con, char key);

#ifdef MC_OUTPUT_TEXTURE
void mc_set_texture_size(struct mc_console *con, unsigned width, unsigned height);
#endif

#ifdef MC_IMPLEMENTATION

void mc_create(struct mc_console *con)
{
	MC_ASSERT(con);
	memset(con, 0, sizeof(struct mc_console));

#ifdef MC_DYNAMIC_ARRAYS
	con->instr = (char*)calloc(1, sizeof(char));
	con->outstr = (char*)calloc(1, sizeof(char));
#else
	memset(con->instr, '\0', MC_MAX_INPUT_LENGTH * sizeof(char));
	memset(con->cmdfuncs, 0, MC_MAX_COMMANDS * sizeof(mc_cmd_ptr));
	for(int i = 0; i <  MC_MAX_COMMANDS; i++){
		memset(con->cmds[i], '\0', MC_MAX_COMMAND_LENGTH * sizeof(char));
	}
#endif
}

void mc_free(struct mc_console *con)
{
	MC_ASSERT(con);
#ifdef MC_DYNAMIC_ARRAYS
	free(con->instr);
	free(con->cmdfuncs);
	free(con->cmds);
#endif
}

void mc_map(struct mc_console *con, const char *cmd, mc_cmd_ptr func)
{
	MC_ASSERT(con);
#ifdef MC_DYNAMIC_ARRAYS
	con->cmdfuncs = (mc_cmd_ptr*)realloc(con->cmdfuncs, con->ncmds * sizeof(mc_cmd_ptr));
	con->cmds = (char**)realloc(con->cmds, con->ncmds * sizeof(char*));
#else
	MC_ASSERT(con->ncmds < MC_MAX_COMMANDS);
	if(con->ncmds == MC_MAX_COMMANDS){
		return;
	}
#endif

	con->cmdfuncs[con->ncmds] = func;
#ifdef MC_DYNAMIC_ARRAYS
	con->cmds[con->ncmds] = (char*)malloc(strlen(cmd));
#endif
	strcpy(con->cmds[con->ncmds], cmd);

	con->ncmds++;
}

void mc_input_key(struct mc_console *con, enum mc_keys key)
{
	MC_ASSERT(con);
	switch(key){
		case MC_KEY_LEFT:
			if(con->inpos > 0){
				con->inpos--;
			}
			break;
		case MC_KEY_RIGHT:
			if(con->instr[con->inpos] != '\0'){
				con->inpos++;
			}
			break;
		case MC_KEY_UP:
//TODO
			break;
		case MC_KEY_DOWN:
//TODO
			break;
		case MC_KEY_BACKSPACE:
			if(con->inpos > 0){
				con->inpos--;
				memmove(con->instr + con->inpos, con->instr + con->inpos + 1, strlen(con->instr) - con->inpos);
			}
			break;
	}
}

void mc_input_char(struct mc_console *con, char key)
{
	MC_ASSERT(con);
	if(key >= ' ' && key <= '~'){

	}

	switch(key){
		case '\t':

			break;
	}
}

#ifdef MC_OUTPUT_TEXTURE
void mc_set_texture_size(struct mc_console *con, unsigned width, unsigned height)
{
	MC_ASSERT(con);
	if(con->pixels){
		con->pixels = (struct mc_pixel*)realloc(con->pixels, width * height * sizeof(struct mc_pixel));
	}else{
		con->pixels = (struct mc_pixel*)malloc(width * height * sizeof(struct mc_pixel));
	}

	con->width = width;
	con->height = height;
}
#endif // MC_OUTPUT_TEXTURE

#endif // MC_IMPLEMENTATION

#endif // MC_H
