#ifndef GUI_H
#define GUI_H

#ifdef __cplusplus
extern "C" {
#endif
#include "custom.h"
#include "gui_guider.h"
#include "events_init.h"


void task_gui(void *arg);
void task_gui_calibrate(void *arg);

#ifdef __cplusplus
}
#endif
#endif /*End of "Content enable"*/