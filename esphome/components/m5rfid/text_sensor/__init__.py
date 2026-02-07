import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import CONF_ID

CODEOWNERS = ["@rubenbe"]
DEPENDENCIES = ["text_sensor"]
AUTO_LOAD = ["text_sensor"]

CONF_RX_PIN = "rx_pin"
CONF_TX_PIN = "tx_pin"

m5rfid_ns = cg.esphome_ns.namespace("m5rfid")
M5RFIDTextSensor = m5rfid_ns.class_(
    "M5RFIDTextSensor", text_sensor.TextSensor, cg.PollingComponent
)

CONFIG_SCHEMA = text_sensor.text_sensor_schema().extend(
    {
        cv.GenerateID(): cv.declare_id(M5RFIDTextSensor),
        cv.Optional(CONF_RX_PIN, default=32): cv.int_range(min=0, max=39),
        cv.Optional(CONF_TX_PIN, default=26): cv.int_range(min=0, max=39),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await text_sensor.register_text_sensor(var, config)
    cg.add(var.set_rx_pin(config[CONF_RX_PIN]))
    cg.add(var.set_tx_pin(config[CONF_TX_PIN]))
