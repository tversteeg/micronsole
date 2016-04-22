#include <stdio.h>

#include <ccore/display.h>
#include <ccore/window.h>
#include <ccore/opengl.h>
#include <ccore/time.h>
#include <ccore/file.h>

/* these defines are both needed for the header
 * and source file. So if you split them remember
 * to copy them as well. */
#define MC_OUTPUT_TEXTURE_RGB
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

	struct mc_console con;
	mc_ccore_create(&con);
	mc_map(&con, "test", &mc_test_command);

	bool loop = true;
	while(loop){
		while(ccWindowEventPoll()){
			ccEvent event = ccWindowEventGet();
			if(event.type == CC_EVENT_WINDOW_QUIT){
				loop = false;
			}else{
				mc_ccore_handle_event(&con, event);
			}
		}

		ccTimeDelay(6);
	}

	mc_free(&con);
	ccFree();

	return 0;
}
