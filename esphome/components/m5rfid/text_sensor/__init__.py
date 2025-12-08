import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import CONF_ID

CODEOWNERS = ["@rubenbe"]
DEPENDENCIES = ["text_sensor"]
AUTO_LOAD = ["text_sensor"]

m5rfid_ns = cg.esphome_ns.namespace("m5rfid")
M5RFIDTextSensor = m5rfid_ns.class_(
    "M5RFIDTextSensor", text_sensor.TextSensor, cg.PollingComponent
)

CONFIG_SCHEMA = text_sensor.text_sensor_schema().extend(
    {
        cv.GenerateID(): cv.declare_id(M5RFIDTextSensor),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await text_sensor.register_text_sensor(var, config)
