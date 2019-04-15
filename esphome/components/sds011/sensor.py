from esphome.components import sensor, uart
from esphome.components.uart import UARTComponent
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import (CONF_ID, CONF_NAME, CONF_PM_10_0, CONF_PM_2_5, CONF_RX_ONLY,
                           CONF_UART_ID, CONF_UPDATE_INTERVAL, UNIT_MICROGRAMS_PER_CUBIC_METER,
                           ICON_CHEMICAL_WEAPON)


DEPENDENCIES = ['uart']

sds011_ns = cg.esphome_ns.namespace('sds011')
SDS011Component = sds011_ns.class_('SDS011Component', uart.UARTDevice, cg.Component)


def validate_sds011_rx_mode(value):
    if CONF_UPDATE_INTERVAL in value and not value.get(CONF_RX_ONLY):
        update_interval = value[CONF_UPDATE_INTERVAL]
        if update_interval.total_minutes > 30:
            raise cv.Invalid("Maximum update interval is 30min")
    elif value.get(CONF_RX_ONLY) and CONF_UPDATE_INTERVAL in value:
        # update_interval does not affect anything in rx-only mode, let's warn user about
        # that
        raise cv.Invalid("update_interval has no effect in rx_only mode. Please remove it.",
                         path=['update_interval'])
    return value


CONFIG_SCHEMA = cv.All(cv.Schema({
    cv.GenerateID(): cv.declare_variable_id(SDS011Component),

    cv.Optional(CONF_PM_2_5):
        cv.nameable(sensor.sensor_schema(UNIT_MICROGRAMS_PER_CUBIC_METER, ICON_CHEMICAL_WEAPON, 1)),
    cv.Optional(CONF_PM_10_0):
        cv.nameable(sensor.sensor_schema(UNIT_MICROGRAMS_PER_CUBIC_METER, ICON_CHEMICAL_WEAPON, 1)),

    cv.Optional(CONF_RX_ONLY, default=False): cv.boolean,
    cv.Optional(CONF_UPDATE_INTERVAL, default='0min'): cv.positive_time_period_minutes,
}).extend(cv.COMPONENT_SCHEMA).extend(uart.UART_DEVICE_SCHEMA), validate_sds011_rx_mode)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield uart.register_uart_device(var, config)

    cg.add(var.set_update_interval_min(config[CONF_UPDATE_INTERVAL]))
    cg.add(var.set_rx_mode_only(config[CONF_RX_ONLY]))

    if CONF_PM_2_5 in config:
        sens = yield sensor.new_sensor(config[CONF_PM_2_5])
        cg.add(var.set_pm_2_5_sensor(sens))

    if CONF_PM_10_0 in config:
        sens = yield sensor.new_sensor(config[CONF_PM_10_0])
        cg.add(var.set_pm_10_0_sensor(sens))

