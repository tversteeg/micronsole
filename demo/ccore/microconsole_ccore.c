#include "microconsole_ccore.h"
#define MC_IMPLEMENTATION
#include "../../micronsole.h"

MC_API int mc_ccore_create(struct mc_console *con)
{
	if(!ccWindowExists()){
		return -1;
	}
	if(mc_create(con)){
		return -2;
	}

	ccRect s = ccWindowGetRect();
	if(mc_set_texture_size(con, s.width, s.height)){
		return -3;
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

#ifdef MC_CCORE_OPENGL
MC_API int mc_ccore_setup_texture(struct mc_console *con, GLuint tex)
{
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	return 0;
}

MC_API int mc_ccore_render_texture(struct mc_console *con, GLuint tex)
{
	glBindTexture(GL_TEXTURE_2D, tex);

#ifdef MC_OUTPUT_TEXTURE_RGB
	GLint format = GL_RGB;
#elif defined MC_OUTPUT_TEXTURE_RGBA
	GLint format = GL_RGBA;
#elif defined MC_OUTPUT_TEXTURE_BGR
	GLint format = GL_BGR;
#elif defined MC_OUTPUT_TEXTURE_BGRA
	GLint format = GL_BGRA;
#endif

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, con->width, con->height, 0, format, GL_UNSIGNED_BYTE, con->pixels);
	glBindTexture(GL_TEXTURE_2D, 0);

	return 0;
}
#endif // MC_CCORE_OPENGL
