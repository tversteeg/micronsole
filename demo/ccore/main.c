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
	mc_ccore_create(&con);
	mc_ccore_setup_texture(&con, gltex);
	mc_map(&con, "test", &mc_test_command);

	bool loop = true;
	while(loop){
		while(ccWindowEventPoll()){
			ccEvent event = ccWindowEventGet();
			if(event.type == CC_EVENT_WINDOW_QUIT || (event.type == CC_EVENT_KEY_DOWN && event.keyCode == CC_KEY_ESCAPE)){
				loop = false;
			}else{
				mc_ccore_handle_event(&con, event);
			}
		}
		
		glClear(GL_COLOR_BUFFER_BIT);

		mc_ccore_render_texture(&con, gltex);

		ccGLBuffersSwap();

		ccTimeDelay(6);
	}

	mc_free(&con);
	ccFree();

	return 0;
}
