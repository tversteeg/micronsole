#include <stdio.h>

#include <ccore/display.h>
#include <ccore/window.h>
#include <ccore/opengl.h>
#include <ccore/time.h>
#include <ccore/file.h>

/* these defines are both needed for the header
 * and source file. So if you split them remember
 * to copy them as well. */
#define MC_CCORE_OPENGL
#define MC_OUTPUT_TEXTURE_BGR
#include "microconsole_ccore.h"
#include "microconsole_ccore.c"

#define WIDTH 600
#define HEIGHT 400

#define EXIT_ON_E(x) {\
	int e; \
	if((e = x) != 0){ \
		fprintf(stderr, "Error on line %d:\n\t" #x "; -> %d\n", __LINE__, e); \
		exit(1); \
	} \
}

void mc_test_command(struct mc_console *con, int argc, char **argv)
{
	printf("Command\n");
}

int main(void)
{
	ccDisplayInitialize();
	ccWindowCreate((ccRect){0, 0, WIDTH, HEIGHT}, "microconsole ccore demo", 0);

	ccGLContextBind();
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint gltex;
	glGenTextures(1, &gltex);

	struct mc_console con;
	EXIT_ON_E(mc_ccore_create(&con));
	EXIT_ON_E(mc_ccore_setup_texture(&con, gltex));
	EXIT_ON_E(mc_map(&con, "test", &mc_test_command));

	bool loop = true;
	while(loop){
		while(ccWindowEventPoll()){
			ccEvent event = ccWindowEventGet();
			if(event.type == CC_EVENT_WINDOW_QUIT || (event.type == CC_EVENT_KEY_DOWN && event.keyCode == CC_KEY_ESCAPE)){
				loop = false;
			}else{
				EXIT_ON_E(mc_ccore_handle_event(&con, event));
			}
		}
		
		glClear(GL_COLOR_BUFFER_BIT);

		EXIT_ON_E(mc_blit_glyph_default(&con, 5, 5, 'a'));
		EXIT_ON_E(mc_blit_glyph_default(&con, 20, 5, 'b'));
		EXIT_ON_E(mc_ccore_render_texture(&con, gltex));
		
		glBindTexture(GL_TEXTURE_2D, gltex);
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

		ccGLBuffersSwap();

		ccTimeDelay(6);
	}

	mc_free(&con);
	ccFree();

	return 0;
}
