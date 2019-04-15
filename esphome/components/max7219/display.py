import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display, spi
from esphome.const import CONF_ID, CONF_INTENSITY, CONF_LAMBDA, CONF_NUM_CHIPS, CONF_UPDATE_INTERVAL

DEPENDENCIES = ['spi']

max7219_ns = cg.esphome_ns.namespace('max7219')
MAX7219Component = max7219_ns.class_('MAX7219Component', cg.PollingComponent, spi.SPIDevice)
MAX7219ComponentRef = MAX7219Component.operator('ref')

CONFIG_SCHEMA = display.BASIC_DISPLAY_SCHEMA.extend({
    cv.GenerateID(): cv.declare_variable_id(MAX7219Component),

    cv.Optional(CONF_UPDATE_INTERVAL, default='1s'): cv.update_interval,
    cv.Optional(CONF_NUM_CHIPS, default=1): cv.All(cv.uint8_t, cv.Range(min=1)),
    cv.Optional(CONF_INTENSITY, default=15): cv.All(cv.uint8_t, cv.Range(min=0, max=15)),
}).extend(cv.COMPONENT_SCHEMA).extend(spi.SPI_DEVICE_SCHEMA)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], config[CONF_UPDATE_INTERVAL])
    yield cg.register_component(var, config)
    yield spi.register_spi_device(var, config)
    yield display.register_display(var, config)

    cg.add(var.set_num_chips(config[CONF_NUM_CHIPS]))
    cg.add(var.set_intensity(config[CONF_INTENSITY]))

    if CONF_LAMBDA in config:
        lambda_ = yield cg.process_lambda(config[CONF_LAMBDA], [(MAX7219ComponentRef, 'it')],
                                          return_type=cg.void)
        cg.add(var.set_writer(lambda_))
