#ifndef MC_CCORE_H
#define MC_CCORE_H

#include "../../micronsole.h"

#include <ccore/window.h>
#include <ccore/event.h>

MC_API int mc_ccore_create(struct mc_console *con);
MC_API int mc_ccore_handle_event(struct mc_console *con, ccEvent event);

#endif
