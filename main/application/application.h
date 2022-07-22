#include <cstddef>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include "utils.h"
#include "fb_gfx.h"
#include "../websocket/websocket.h"
#include "../http/app_mdns.h"
#include "../http/http_stream.h"
#include "../camera/app_camera_esp.h"

void captureTask(void *pvParameters);
void evaluationTask(void *pvParameters);
