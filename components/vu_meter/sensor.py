import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, speaker
from esphome.const import CONF_ID

vu_meter_ns = cg.esphome_ns.namespace("vu_meter")
VuMeter = vu_meter_ns.class_(
    "VuMeter", cg.Component, speaker.Speaker, sensor.Sensor
)

DEPENDENCIES = ["speaker"]
CONF_SOURCE = "source"

CONFIG_SCHEMA = sensor.sensor_schema(
    VuMeter,
).extend(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(VuMeter),
        cv.Required(CONF_SOURCE): cv.use_id(speaker.Speaker),
        cv.Optional("update_interval_ms", default=100): cv.positive_int,
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)
    await speaker.register_speaker(var, config)

    source = await cg.get_variable(config[CONF_SOURCE])
    cg.add(var.set_source(source))
    cg.add(var.set_update_interval(config["update_interval_ms"]))