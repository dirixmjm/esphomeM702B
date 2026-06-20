import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor
from esphome.const import (
   CONF_ID, 
   ICON_EMPTY,
   ICON_PERCENT,
   ICON_THERMOMETER,
   ICON_AIR_FILTER,
   UNIT_EMPTY, 
   UNIT_CELSIUS, 
   UNIT_PERCENT, 
   UNIT_PARTS_PER_MILLION
)

DEPENDENCIES = ["uart"]

air_quality_sensor_ns = cg.esphome_ns.namespace("air_quality_sensor")
AirQualitySensor = air_quality_sensor_ns.class_(
    "AirQualitySensor", cg.Component, uart.UARTDevice
)

CONF_CO2 = "CO2"
CONF_CH2O = "CH2O"
CONF_TVOC = "TVOC"
CONF_PM25 = "PM25"
CONF_PM10 = "PM10"
CONF_TEMP = "temperature"
CONF_HUM = "humidity"
CONF_AVG = "average"


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(AirQualitySensor),
        cv.Optional(CONF_AVG): cv.int_range(1, 255),
        cv.Optional(CONF_CO2): sensor.sensor_schema(
            unit_of_measurement=UNIT_PARTS_PER_MILLION, icon=ICON_EMPTY, accuracy_decimals=0
        ),
        cv.Optional(CONF_CH2O): sensor.sensor_schema(
            unit_of_measurement="µg/m³", icon=ICON_EMPTY, accuracy_decimals=0
        ),
        cv.Optional(CONF_TVOC): sensor.sensor_schema(
            unit_of_measurement="µg/m³", icon=ICON_EMPTY, accuracy_decimals=0
        ),
        cv.Optional(CONF_PM25): sensor.sensor_schema(
            unit_of_measurement="µg/m³", icon=ICON_AIR_FILTER, accuracy_decimals=0
        ),
        cv.Optional(CONF_PM10): sensor.sensor_schema(
            unit_of_measurement="µg/m³", icon=ICON_AIR_FILTER, accuracy_decimals=0
        ),
        cv.Optional(CONF_TEMP): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS, icon=ICON_THERMOMETER, accuracy_decimals=2
        ),
        cv.Optional(CONF_HUM): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT, icon=ICON_PERCENT, accuracy_decimals=2
        ),
    }
).extend(uart.UART_DEVICE_SCHEMA).extend(cv.COMPONENT_SCHEMA)
#.extend(cv.polling_component_schema("60s"))
async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    
    if avg := config.get(CONF_AVG):
        cg.add(var.set_avg(avg))
    if co2_config := config.get(CONF_CO2):
        sens = await sensor.new_sensor(co2_config)
        cg.add(var.set_co2(sens))

    if sensor_2_config := config.get(CONF_CH2O):
        sens = await sensor.new_sensor(sensor_2_config)
        cg.add(var.set_ch2o(sens))

    if sensor_3_config := config.get(CONF_TVOC):
        sens = await sensor.new_sensor(sensor_3_config)
        cg.add(var.set_tvoc(sens))
        
    if sensor_4_config := config.get(CONF_PM25):
        sens = await sensor.new_sensor(sensor_4_config)
        cg.add(var.set_pm25(sens))

    if sensor_5_config := config.get(CONF_PM10):
        sens = await sensor.new_sensor(sensor_5_config)
        cg.add(var.set_pm10(sens))

    if sensor_6_config := config.get(CONF_TEMP):
        sens = await sensor.new_sensor(sensor_6_config)
        cg.add(var.set_temp(sens))
        
    if sensor_7_config := config.get(CONF_HUM):
        sens = await sensor.new_sensor(sensor_7_config)
        cg.add(var.set_hum(sens))