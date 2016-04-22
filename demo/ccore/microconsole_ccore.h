#ifndef MC_CCORE_H
#define MC_CCORE_H

#include <ccore/window.h>
#include <ccore/event.h>

#ifdef MC_CCORE_OPENGL
#ifdef WINDOWS
#include <gl/GL.h>
#else
#include <GL/glew.h>
#endif

#include <ccore/opengl.h>
#endif // MC_CCORE_OPENGL

#include "../../micronsole.h"

MC_API int mc_ccore_create(struct mc_console *con);
MC_API int mc_ccore_handle_event(struct mc_console *con, ccEvent event);

#ifdef MC_CCORE_OPENGL
MC_API int mc_ccore_setup_texture(struct mc_console *con, GLuint tex);
MC_API int mc_ccore_render_texture(struct mc_console *con, GLuint tex);
#endif

#endif
