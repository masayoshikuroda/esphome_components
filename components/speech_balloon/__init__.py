import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import color, display, font
from esphome.const import CONF_ID

DEPENDENCIES = ["display"]
MULTI_CONF = True

speech_balloon_ns = cg.esphome_ns.namespace("speech_balloon")
SpeechBalloon = speech_balloon_ns.class_(
    "SpeechBalloon",
    cg.Component,
)

CONF_LEFT = "left"
CONF_TOP = "top"
CONF_RIGHT = "right"
CONF_BOTTOM = "bottom"
CONF_TAIL_X = "tail_x"
CONF_TAIL_Y = "tail_y"
CONF_FONT = "font"
CONF_FOREGROUND_COLOR = "foreground_color"
CONF_BACKGROUND_COLOR = "background_color"
CONF_MESSAGE = "message"
CONF_SCROLL_SPEED = "scroll_speed"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(SpeechBalloon),
        cv.Required(CONF_FONT): cv.use_id(font.Font),
        cv.Optional(CONF_LEFT, default=-1): cv.int_,
        cv.Optional(CONF_TOP, default=-1): cv.int_,
        cv.Optional(CONF_RIGHT, default=-1): cv.int_,
        cv.Optional(CONF_BOTTOM, default=-1): cv.int_,
        cv.Optional(CONF_TAIL_X, default=-1): cv.int_,
        cv.Optional(CONF_TAIL_Y, default=-1): cv.int_,
        cv.Optional(CONF_FOREGROUND_COLOR): cv.use_id(color.ColorStruct),
        cv.Optional(CONF_BACKGROUND_COLOR): cv.use_id(color.ColorStruct),
        cv.Optional(CONF_MESSAGE, default=""): cv.string,
        cv.Optional(CONF_SCROLL_SPEED, default=5): cv.positive_int,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    font_var = await cg.get_variable(config[CONF_FONT])
    cg.add(var.set_font(font_var))

    if CONF_FOREGROUND_COLOR in config:
        foreground_color_var = await cg.get_variable(config[CONF_FOREGROUND_COLOR])
        cg.add(var.set_foreground_color(foreground_color_var))
    else:
        cg.add(var.set_foreground_color(cg.ArrayInitializer(0, 0, 0, 0)))

    if CONF_BACKGROUND_COLOR in config:
        background_color_var = await cg.get_variable(config[CONF_BACKGROUND_COLOR])
        cg.add(var.set_background_color(background_color_var))
    else:
        cg.add(var.set_background_color(cg.ArrayInitializer(255, 255, 255, 0)))

    cg.add(var.set_left(config[CONF_LEFT]))
    cg.add(var.set_top(config[CONF_TOP]))
    cg.add(var.set_tail_x(config[CONF_TAIL_X]))
    cg.add(var.set_tail_y(config[CONF_TAIL_Y]))
    cg.add(var.set_right(config[CONF_RIGHT]))
    cg.add(var.set_bottom(config[CONF_BOTTOM]))
    cg.add(var.set_message(config[CONF_MESSAGE]))
    cg.add(var.set_scroll_speed(config[CONF_SCROLL_SPEED]))