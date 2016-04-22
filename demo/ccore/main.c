#include <stdio.h>

#define MC_IMPLEMENTATION
#define MC_OUTPUT_TEXTURE_RGB
#include "../../micronsole.h"

#include <ccore/display.h>
#include <ccore/window.h>
#include <ccore/opengl.h>
#include <ccore/time.h>
#include <ccore/file.h>

#define WIDTH 100
#define HEIGHT 100

void mc_test_command(struct mc_console *con, int argc, char **argv)
{
	printf("Command\n");
}

int main(void)
{
	ccDisplayInitialize();
	ccWindowCreate((ccRect){0, 0, WIDTH, HEIGHT}, "microconsole ccore demo", 0);

	struct mc_console con;
	mc_create(&con);
	mc_set_texture_size(&con, WIDTH, HEIGHT);
	mc_map(&con, "test", &mc_test_command);

	bool loop = true;
	while(loop){
		while(ccWindowEventPoll()){
			ccEvent event = ccWindowEventGet();
			switch(event.type){
				case CC_EVENT_WINDOW_QUIT:
					loop = false;
					break;
				case CC_EVENT_KEY_DOWN:
					if(event.keyCode == CC_KEY_ESCAPE){
						loop = false;
					}
					break;
				default: break;
			}
		}

		ccTimeDelay(6);
	}

	mc_free(&con);
	ccFree();

	return 0;
}
