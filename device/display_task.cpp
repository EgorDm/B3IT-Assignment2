//
// Created by egordm on 23-3-2018.
//

#include <OLEDDisplay.h>
#include <OLEDDisplayUi.h>
#include "display_task.h"
#include "images.hpp"


void frame_sensor_data_1(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y) {
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->setFont(ArialMT_Plain_10);

    display->drawStringMaxWidth(x + 64, y + 5, 128, "Sensor Data 1/2");
    display->drawStringMaxWidth(x + 64, y + 16, 128, SENSOR_TXT("Temperature: ", sensor_data.temparature, "C"));
    display->drawStringMaxWidth(x + 64, y + 27, 128, SENSOR_TXT("Humidity: ", sensor_data.humidity, "%"));
    display->drawStringMaxWidth(x + 64, y + 38, 128, SENSOR_TXT("Pressure: ", sensor_data.pressure, "Pa"));
}

void frame_sensor_data_2(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y) {
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->setFont(ArialMT_Plain_10);

    display->drawStringMaxWidth(x + 64, y + 5, 128, "Sensor Data 2/2");
    display->drawStringMaxWidth(x + 64, y + 16, 128, SENSOR_TXT("Light: ", sensor_data.light, ""));
    display->drawStringMaxWidth(x + 64, y + 27, 128, SENSOR_TXT("Moisture: ", sensor_data.soil_moisture, ""));
}

void frame_state(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y) {
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->setFont(ArialMT_Plain_10);

    String lw_text = String("");
    const auto water_diff = millis() - sensor_data.last_watered;
    if(water_diff < 1000*60*2) lw_text += (water_diff/1000) + String("s");
    else if(water_diff < 1000*60*60*2) lw_text += (water_diff/1000/60) + String("m");
    else lw_text += (water_diff/1000/60/60) + String("h");

    display->drawStringMaxWidth(x + 64, y + 5, 128, "State");
    display->drawStringMaxWidth(x + 64, y + 16, 128, SENSOR_TXT("Last watering: ", lw_text, " ago"));
    display->drawStringMaxWidth(x + 64, y + 27, 128, SENSOR_TXT("Mode: ", config.automatic_mode ? "Automatic" : "Manual", ""));
}

void frame_watering(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y) {
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->setFont(ArialMT_Plain_10);

    display->drawStringMaxWidth(x + 64, y + 5, 128, "Watering the plants");
    display->drawProgressBar(x, y + 32, 120, 10, (uint8_t)(((float)sensor_data.water_ticks / WATER_PLANT_DURATION) * 100));
}


DisplayRoutine::DisplayRoutine(WaterPlantRoutine *plant_routine)
        : RoutineTask(DISPLAY_INTERVAL), plant_routine(plant_routine), display(OLED_ADDRESS, WIRE_SDA, WIRE_SCK),
          ui(&display), frames{&frame_sensor_data_1, &frame_sensor_data_2, &frame_state, &frame_watering}, ticks(0),
          frame_i(0) {

    ui.setTargetFPS(OLED_FPS);
    ui.setActiveSymbol(ACTIVE_SYMBOL);
    ui.setInactiveSymbol(INACTIVE_SYMBOL);
    ui.setIndicatorPosition(BOTTOM);
    ui.setIndicatorDirection(LEFT_RIGHT);
    ui.setFrameAnimation(SLIDE_LEFT);

    ui.setFrames(frames, FRAME_COUNT);
    ui.disableAutoTransition();
    ui.init();

    display.flipScreenVertically();
}

bool DisplayRoutine::execute() {
    ++ticks;
    if(sensor_data.water_ticks < WATER_PLANT_DURATION) {
        if(frame_i != FRAME_COUNT - 1) {
            ui.transitionToFrame(FRAME_COUNT - 1);
            frame_i = FRAME_COUNT - 1;
        }
    } else {
        if(ticks >= DISPLAY_FRAME_SLIDE_TICKS || frame_i == FRAME_COUNT - 1) {
            if(++frame_i > FRAME_COUNT - 2) frame_i = 0;
            ui.transitionToFrame(frame_i);
            ticks = 0;
        }
    }

    ui.update();
    return false;
}
