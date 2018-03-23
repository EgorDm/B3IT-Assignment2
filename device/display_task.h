//
// Created by egordm on 23-3-2018.
//

#ifndef B3ITASSIGNMENT2_DISPLAY_TASK_H
#define B3ITASSIGNMENT2_DISPLAY_TASK_H


#include <SSD1306Wire.h>
#include "tasks.h"

#define FRAME_COUNT 4
class DisplayRoutine: public internals::RoutineTask {
private:
    WaterPlantRoutine *plant_routine;
    SSD1306Wire display;
    OLEDDisplayUi ui;
    FrameCallback frames[FRAME_COUNT];
    int ticks;
    unsigned char frame_i;
public:
    explicit DisplayRoutine(WaterPlantRoutine *plant_routine);

    bool execute() override;
};

#endif //B3ITASSIGNMENT2_DISPLAY_TASK_H
