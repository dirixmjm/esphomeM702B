#pragma once

#include <numeric>
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "running_average.hpp"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace air_quality_sensor {

class AirQualitySensor : public sensor::Sensor, public PollingComponent, public uart::UARTDevice {
 public:
    void set_co2(sensor::Sensor *co2) { co2_ = co2; }
    void set_ch2o(sensor::Sensor *ch2o) { ch2o_ = ch2o; }
    void set_tvoc(sensor::Sensor *tvoc) { tvoc_ = tvoc; }
    void set_pm25(sensor::Sensor *pm25) { pm25_ = pm25; }
    void set_pm10(sensor::Sensor *pm10) { pm10_ = pm10; }
    void set_temp(sensor::Sensor *temp) { temp_ = temp; }	
	void set_hum(sensor::Sensor *hum) { hum_ = hum; }
	void set_avg(int avg) { this->avg_ = avg; }
 
	void setup() override;
	void update() override;
	void loop() override;
	void dump_config() override;
	
 protected:

    sensor::Sensor *co2_;
    sensor::Sensor *ch2o_;
    sensor::Sensor *tvoc_;
    sensor::Sensor *pm25_;
    sensor::Sensor *pm10_;
    sensor::Sensor *temp_;
    sensor::Sensor *hum_;
	int avg_{1};	
	RunningAverage<uint16_t> *co2 = new RunningAverage<uint16_t>(1);
	RunningAverage<uint16_t> *ch2o = new RunningAverage<uint16_t>(1);
	RunningAverage<uint16_t> *tvoc = new RunningAverage<uint16_t>(1);
	RunningAverage<uint16_t> *pm25 = new RunningAverage<uint16_t>(1);
	RunningAverage<uint16_t> *pm10 = new RunningAverage<uint16_t>(1);
	RunningAverage<uint16_t> *temp = new RunningAverage<uint16_t>(1);
	RunningAverage<uint16_t> *hum = new RunningAverage<uint16_t>(1);
};

}  // namespace air_quality_sensor
}  // namespace esphome