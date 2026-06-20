#include "esphome/core/log.h"
#include "air_quality_sensor.h"
#include <chrono>

namespace esphome {
namespace air_quality_sensor {

static const char *TAG = "air_quality_sensor.sensor";

void AirQualitySensor::setup() {
	co2->resize(avg_);
	ch2o->resize(avg_);
	tvoc->resize(avg_);
	pm25->resize(avg_);
	pm10->resize(avg_);
	temp->resize(avg_);
	hum->resize(avg_);
}

void AirQualitySensor::update() {
}

void AirQualitySensor::loop() {
	// Scan for preamble 0x3C 0x02

	while (this->available() >= 17) {
		if (this->peek() != 0x3C) {
			this->read();  // discard non-preamble bytes
			continue;
		}
		ESP_LOGD(TAG, "Pre-amble found");

		// Peek ahead — read into a temp buffer to check B2
		uint8_t buf[17];
		// We need at least 17 bytes; read them
		for (int i = 0; i < 17; i++) {
			buf[i] = this->read();
		}

		// Verify preamble
		if (buf[0] != 0x3C || buf[1] != 0x02) {
		// Not a valid frame start; rewind isn't possible, just continue scanning
			continue;
		}
		ESP_LOGD(TAG,"Rec: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],buf[8],buf[9],buf[10],buf[11],buf[12],buf[13],buf[14],buf[15],buf[16]);
		
		// Verify checksum: sum of B1..B16 (bytes 0..15), compared to B17-B18
		uint16_t checksum = 0;
		for (int i = 0; i < 16; i++) {
			checksum += buf[i];
		}
		uint8_t received_check = buf[16];
		uint8_t cchecksum = ((uint8_t)(checksum & 0xFF));
		if (cchecksum != received_check) {
			ESP_LOGW(TAG, "Checksum mismatch: calculated 0x%02X, received 0x%02X",
				 cchecksum, received_check);
		continue;
		}
		
		// Parse values (big-endian 16-bit)
		if ( buf[2]+buf[3] != 0 ) {
			uint16_t co2meas  = ((uint16_t)buf[2]  << 8) | buf[3];
			uint16_t co2avg = co2->push(co2meas);
			this->co2_->publish_state(co2avg);			
		}
		if ( buf[4]+buf[5] != 0 ) {
			uint16_t ch2omeas = ((uint16_t)buf[4]  << 8) | buf[5];
			uint16_t ch2oavg = ch2o->push(ch2omeas);
			this->ch2o_->publish_state(ch2oavg);	
		}
		
		if ( buf[6]+buf[7] != 0 ) {
			uint16_t tvocmeas = ((uint16_t)buf[6]  << 8) | buf[7];
			uint16_t tvocavg = tvoc->push(tvocmeas);
			this->tvoc_->publish_state(tvocavg);
		}
		if ( buf[8]+buf[9] != 0 ) {
			uint16_t pm25meas = ((uint16_t)buf[8]  << 8) | buf[9];
			uint16_t pm25avg = pm25->push(pm25meas);
			this->pm25_->publish_state(pm25avg);
		}
		if ( buf[10]+buf[11] != 0 ) {
			uint16_t pm10meas = ((uint16_t)buf[10] << 8) | buf[11];
			uint16_t pm10avg = pm10->push(pm10meas);
			this->pm10_->publish_state(pm10avg);
		}
		if ( buf[12]+buf[13]+buf[14]+buf[15] != 0 ) {
			uint16_t tempmeas = (int8_t)buf[12]*100 + (uint8_t)buf[13];
			uint16_t tempavg = temp->push(tempmeas);
			this->temp_->publish_state(tempavg / 100.0f);
			
			uint16_t hummeas = (uint16_t)buf[14]*100 + buf[15];
			uint16_t humavg = hum->push(hummeas);
			this->hum_->publish_state(humavg / 100.0f);
		}

		ESP_LOGD("air_quality", "CO2=%uppm CH2O=%u TVOC=%u PM2.5=%u PM10=%u T=%.1f°C H=%.1f%%",
               co2->average(), ch2o->average(), tvoc->average(), pm25->average(), pm10->average(), temp->average() / 100.0f, hum->average()/100.0f);
	}
}

void AirQualitySensor::dump_config(){
    ESP_LOGCONFIG(TAG, "Air Quality sensor");
}

}  // namespace air_quality_sensor
}  // namespace esphome