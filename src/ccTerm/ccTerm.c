#include <ccTerm/ccTerm.h>

#include <stdio.h>
#include <string.h>

#define _CCT_PIXEL_FORMAT GL_RGBA
#define _CCT_PIXEL_TYPE GL_UNSIGNED_BYTE

bool shiftdown = false;

typedef struct {
	unsigned char r, g, b, a;
} _cctPixel;

static int _cctSplitStr(const char *str, char ***res, const char *delim)
{
	int count = 0;
	int max = 10;

	*res = (char**)malloc(max * sizeof(char*));

	const char *e = str;

	if(e) do{
		const char *s = e;
		e = strpbrk(s, delim);

		if(count > max){
			max *= 2;
			*res = (char**)realloc(*res, max * sizeof(char*));
		}

		if(e){
			(*res)[count++] = strndup(s, e - s);
		}else{
			(*res)[count++] = strdup(s);
		}
	}while(e && *(++e));

	if(count > max){
		*res = (char**)realloc(*res, ++max * sizeof(char*));
	}

	(*res)[count + 1] = 0;

	return count;
}

static void _cctCalcWidth(cctTerm *term)
{
	if(term->font == NULL){
		return;
	}
	term->outwidth = (term->width - 4) / term->font->gwidth;
	term->outheight = (term->height - 4) / term->font->gheight - 1;

	if(term->outg){
		free(term->outg);
		term->outg = NULL;
	}
	term->outg = (cctGlyph*)calloc(term->outwidth * term->outheight, sizeof(cctGlyph));
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

static void _cctRenderIn(cctTerm *term)
{	
	ccfFontConfiguration conf = {
		.x = 2, 
		.y = term->height - (term->font->gheight + 2), 
		.width = term->width, 
		.wraptype = 0, 
		.color = {1.0, 1.0, 1.0, 1.0}
	};

	ccfGLTexBlitChar(term->font, '>', &conf, term->width, term->height, _CCT_PIXEL_FORMAT, _CCT_PIXEL_TYPE, term->pixels);

	ccfGLTexBlitText(term->font, term->in, &conf, term->width, term->height, _CCT_PIXEL_FORMAT, _CCT_PIXEL_TYPE, term->pixels);
}

static void _cctUpdateGlyphs(cctTerm *term)
{
	size_t len = strlen(term->outstr);
	if(len == 0){
		return;
	}

	unsigned x = 0;
	unsigned y = 0;
	cctColor fg = {255, 255, 255};
	cctColor bg = {0};
	for(unsigned i = 0; i < len; i++){
		if(x == term->outwidth){
			x = 0;
			y++;
		}

		char c = term->outstr[i];
		if(c == '\n'){
			x = 0;
			y++;
			continue;
		}else if(c == '\t'){
			x += (4 - x % 4);
		}else if(c == '\a'){
			if(term->outstr[++i] != '['){
				continue;
			}

			int j = i;
			char colc;
			do{
				colc = term->outstr[++j];
			}while(colc != 'm' && colc);

			char colstr[j - i];
			strncpy(colstr, term->outstr + i + 1, j - i - 1);
			colstr[j - i - 1] = '\0';

			char **colors = NULL;
			int nchannels = _cctSplitStr(colstr, &colors, ";");
			if(nchannels == 3){
				fg.r = atoi(colors[0]);
				fg.g = atoi(colors[1]);
				fg.b = atoi(colors[2]);
			}else if(nchannels == 1){
				if(atoi(colors[0]) == 0){
					fg.r = fg.g = fg.b = 255;
				}
			}

			i = j;
			x--;

			free(colors);
		}else if(c != ' '){
			cctGlyph *g = term->outg + x + y * term->outwidth;
			g->c = c;
			g->fg = fg;
			g->bg = bg;
		}

		x++;
	}
}

static void _cctRenderOut(cctTerm *term)
{
	if(term->outupdate){
		term->outupdate = false;
		_cctUpdateGlyphs(term);
	}

	ccfFontConfiguration conf = {
		.width = term->width, 
		.wraptype = 0, 
		.color = {1.0f, 1.0f, 1.0f}
	};

	for(unsigned y = 0; y < term->outheight; y++){
		for(unsigned x = 0; x < term->outwidth; x++){
			cctGlyph g = term->outg[x + y * term->outwidth];
			if(g.c == 0){
				continue;
			}
			conf.x = x * term->font->gwidth;
			conf.y = y * term->font->gheight;
			conf.color[0] = g.fg.r / 255.0f;
			conf.color[1] = g.fg.g / 255.0f;
			conf.color[2] = g.fg.b / 255.0f;
			ccfGLTexBlitChar(term->font, g.c, &conf, term->width, term->height, _CCT_PIXEL_FORMAT, _CCT_PIXEL_TYPE, term->pixels);
		}
	}
}

static void _cctRenderBlink(cctTerm *term)
{
	if(term->blink > CCT_BLINK_INTERVAL){
		term->blink = -CCT_BLINK_INTERVAL;
	}
	term->blink++;

	if(term->blink > 0){
		return;
	}

	unsigned startx = 2 + (term->inpos + 1) * term->font->gwidth;
	unsigned starty = term->height - (term->font->gheight + 2);

	for(int y = 0; y < term->font->gheight; y++){
		for(int x = 0; x < term->font->gwidth; x++){
			int i = x + startx + (y + starty) * term->width;
			unsigned char *pix = term->pixels + i * sizeof(_cctPixel);
			pix[0] = pix[1] = pix[2] = pix[3] = 128;
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
		term->outstr = (char*)realloc(term->outstr, term->outmaxlen);
	}

	strcpy(term->outstr + term->outlen, text);
	term->outlen = total;

	term->outupdate = true;
}

static void _cctParseInput(cctTerm *term)
{
	char **argv = NULL;
	int argc = _cctSplitStr(term->in, &argv, " ");
	size_t namelen = strlen(argv[0]);

	int command = -1;
	for(int i = 0; i < term->ncmds; i++){
		if(strncmp(term->cmds[i], argv[0], namelen) == 0){
			command = i;
		}
	}

	if(command < 0){
		cctPrintf(term, "Command \"%s\" not found!\n", argv[0]);
	}else{
		term->cmdfuncs[command](term, argc, argv);
	}

	free(argv);
}

static void _cctAutoComplete(cctTerm *term)
{
	size_t inlen = strlen(term->in);

	int command = -1;
	size_t newlen = 0;
	bool addspace = true;
	for(int i = 0; i < term->ncmds; i++){
		if(strncmp(term->cmds[i], term->in, inlen) != 0){
			continue;
		}
		if(command < 0){
			command = i;
			newlen = strlen(term->cmds[i]);
		}else{
			char *c1 = term->cmds[i];
			char *c2 = term->cmds[command];
			int min = 0;
			while(*c1 == *c2){
				c1++;
				c2++;
				min++;
			}
			if(min < newlen){
				newlen = min;
			}
			addspace = false;
		}
	}

	if(command < 0){
		return;	
	}

	strncpy(term->in, term->cmds[command], newlen);

	term->inpos = newlen;


	// If Tab is pressed when the closest match is already found, show the possible options
	if(newlen == inlen){	
		cctPrintf(term, ">%s\n", term->in);
		for(int i = 0; i < term->ncmds; i++){
			if(i == command || strncmp(term->cmds[i], term->cmds[command], newlen) != 0){
				continue;
			}
			cctPrintf(term, "%s\n", term->cmds[i]);
		}
	}else if(addspace){
		term->in[newlen] = ' ';
		term->in[++newlen] = '\0';
		term->inpos++;
	}
}

void cctCreate(cctTerm *term, unsigned width, unsigned height)
{
	term->font = NULL;

	term->outmaxlen = 16;
	term->outstr = (char*)calloc(term->outmaxlen, 1);
	term->outg = NULL;
	term->outlen = 0;
	term->outupdate = false;

	term->insert = false;

	term->ncmds = 0;

	memset(term->in, 0, CCT_MAX_CMD_LEN);

	_cctSetSize(term, width, height);
}

void cctFree(cctTerm *term)
{
	free(term->outstr);
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

void cctSetInsert(cctTerm *term, bool insert)
{
	term->insert = insert;
}

void cctRender(cctTerm *term, GLuint texture)
{
	memset(term->pixels, 0, term->width * term->height * sizeof(_cctPixel));

	_cctRenderOut(term);
	_cctRenderIn(term);
	_cctRenderBlink(term);

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
	if(event.type == CC_EVENT_KEY_UP && event.keyCode == CC_KEY_LSHIFT){
		shiftdown = false;
	}
	if(event.type != CC_EVENT_KEY_DOWN){
		return;
	}

	if(event.keyCode == CC_KEY_LSHIFT){
		shiftdown = true;
	}else if(event.keyCode == CC_KEY_LEFT){
		if(term->inpos > 0){
			term->inpos--;
		}
	}else if(event.keyCode == CC_KEY_RIGHT){
		if(term->in[term->inpos] != '\0'){
			term->inpos++;
		}
	}else if(event.keyCode == CC_KEY_BACKSPACE){
		if(term->inpos > 0){
			term->inpos--;
			memmove(term->in + term->inpos, term->in + term->inpos + 1, strlen(term->in) - term->inpos);
		}
	}else if(event.keyCode == CC_KEY_RETURN){
		_cctParseInput(term);
		term->inpos = 0;
		term->in[0] = '\0';
	}else if(event.keyCode == CC_KEY_TAB){
		bool haswhite = false;
		for(char *c = term->in; *c && c; c++){
			if(*c == ' ' || *c == '\t'){
				haswhite = true;
				break;
			}
		}

		if(!haswhite){
			_cctAutoComplete(term);
		}
	}else{
		char c = ccEventKeyToChar(event.keyCode);
		if(c != 0){
			if(shiftdown){
				c += 'A' - 'a';
			}
			if(term->insert){
				term->in[term->inpos] = c;
				if(++term->inpos == strlen(term->in) - 1){
					term->in[term->inpos] = '\0';
				}
			}else{
				size_t len = strlen(term->in);
				if(term->inpos == len){
					term->in[term->inpos] = c;
					term->in[++term->inpos] = '\0';
				}else if(len < CCT_MAX_CMD_LEN){
					memmove(term->in + term->inpos + 1, term->in + term->inpos, len - term->inpos);
					term->in[++term->inpos] = c;
				}
			}
		}
	}
}

void _cctPrintf(cctTerm *term, const char *text, ...)
{
	va_list arg;

	va_start(arg, text);
	char buf[256];
	vsprintf(buf, text, arg);
#ifdef CCT_MIRROR_STDOUT
	printf(buf);
#endif
	__cctPrintf(term, buf);
	va_end(arg);
}

void cctMapCmd(cctTerm *term, const char *cmd, cctCmdptr cmdfunc)
{
	term->cmdfuncs[term->ncmds] = cmdfunc;
	term->cmds[term->ncmds] = (char*)malloc(strlen(cmd));
	strcpy(term->cmds[term->ncmds], cmd);
	term->ncmds++;
}
