#include "weather.h"
#include "temperature_humidity.h"
#include "light.h"
#include "display.h"
#include "util/delay.h"
#include "dht11.h"

#define NUM_READINGS 36

void weather_init(){
    
    dht11_init();
    light_init();
    display_init();
}

void updateWeather(float TempHumidLight[]){
    float temperatureSum = 0;
    float humiditySum = 0;
    uint16_t lightSum = 0;
    TempHumid tempandhumid;
    double second = 5000;
    for (int i = 0; i < NUM_READINGS; i++) {
        tempandhumid = temperature_humidity_get_combined_values();
        temperatureSum += tempandhumid.temp;
        humiditySum += tempandhumid.humid;
        _delay_ms(50);
        lightSum += light_read();
        _delay_ms(50);
        display_int(i);
        _delay_ms(second);
    }

    float temperatureAvg = temperatureSum / NUM_READINGS;
    float lightAvg = lightSum / NUM_READINGS;
    float humidAvg = humiditySum / NUM_READINGS;

    TempHumidLight[0] = temperatureAvg;
    TempHumidLight[1] = humidAvg;
    TempHumidLight[2] = lightAvg;


    
    display_int((int)temperatureAvg);
    _delay_ms(second);
    display_int((int)humidAvg);
    _delay_ms(second);
    display_int((int)lightAvg);
    _delay_ms(second);
}
