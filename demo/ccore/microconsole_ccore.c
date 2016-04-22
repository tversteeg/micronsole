#include "microconsole_ccore.h"
#define MC_IMPLEMENTATION
#include "../../micronsole.h"

MC_API int mc_ccore_create(struct mc_console *con)
{
	if(!ccWindowExists()){
		return -1;
	}
	if(!mc_create(con)){
		return -1;
	}

	ccRect s = ccWindowGetRect();
	if(!mc_set_texture_size(con, s.width, s.height)){
		return -1;
	}

	return 0;
}

MC_API int mc_ccore_handle_event(struct mc_console *con, ccEvent event)
{
	switch(event.type){
		case CC_EVENT_KEY_DOWN:
			switch(event.keyCode){
				case CC_KEY_LEFT:
					mc_input_key(con, MC_KEY_LEFT);
					break;
				case CC_KEY_RIGHT:
					mc_input_key(con, MC_KEY_RIGHT);
					break;
				case CC_KEY_UP:
					mc_input_key(con, MC_KEY_UP);
					break;
				case CC_KEY_DOWN:
					mc_input_key(con, MC_KEY_DOWN);
					break;
				case CC_KEY_BACKSPACE:
					mc_input_key(con, MC_KEY_BACKSPACE);
					break;
				default:
					mc_input_char(con, event.keyCode);
					break;
			}
			break;
		default: break;
	}

	return 0;
}
