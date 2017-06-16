#pragma once
#include "echal_common.h"
#include <stdint.h>
/**
 * @file echal_gpio.h
 * ECHAL GPIO - general purpouse I/O module API
 *
 * Functions are not thread safe. Initialization function must be called before any other function.
 *
 * This module enables gpio write and read. Future versions of it will enable subscribing to changes in GPIO input (trigger lines).
 */

/**
 * Error codes that can be returned by GPIO module functions
 */
typedef enum {
    HAL_GPIO_PORT_OUT_OF_RANGE=64, ///< Port does not exist, must be small letter beginning from 'a'
    HAL_GPIO_PIN_OUT_OF_RANGE, ///< Pin does not exist, must be unsigned number less than number of pins
    HAL_GPIO_PIN_NOT_INITED, ///< Action can't be done as pin is not correctly set up
    HAL_GPIO_PIN_ALREADY_NOT_INITED = 192, ///< Action was not needed as pin wasn't inited (and still is not)
    HAL_GPIO_PIN_ALREADY_INITED, ///< Action was not needed as pin was inited (and still is)
} hal_gpio_errors_t;

typedef enum {
    PORT1,
    PORT2,
    PORT3,
    PORT4,
    PORT5,
    PORT6,
    PORT7,
    PORT8,
    PORT9,
    PORT10,
    PORTJ
} portNo_t;

typedef struct {
    portNo_t portNo;
    uint8_t pinNo;
} IO_t;

/**
 * States that a GPIO pin can be in
 */
typedef enum {
    HAL_GPIO_LOW,
    HAL_GPIO_HIGH
} hal_gpio_pin_state_t;

/**
 * Modes that GPIO pin can be configured to.
 */
typedef enum {
    HAL_GPIO_INPUT,
    HAL_GPIO_OUTPUT
} hal_gpio_pin_mode_t;

/**
 * GPIO pin output types
 */
typedef enum {
    HAL_GPIO_PUSH_PULL,
    HAL_GPIO_OPEN_DRAIN
} hal_gpio_pin_output_type_t;

/**
 * GPIO pin resistor configuration
 */
typedef enum {
    HAL_GPIO_NO_PULL,
    HAL_GPIO_PULL_UP,
    HAL_GPIO_PULL_DOWN
} hal_gpio_pin_pull_t;

/**
 * GPIO pin config struct
 */
typedef struct {
    hal_api_version_t hal_api_version;
    hal_gpio_pin_mode_t mode : 2;
    hal_gpio_pin_output_type_t output_type : 1;
    uint8_t speed : 8; ///< unsigned integer in MHz (approximate)
    hal_gpio_pin_pull_t pull : 2;
} hal_gpio_pin_cfg_t;

static const hal_gpio_pin_cfg_t hal_gpio_pin_cfg_default = {
    .hal_api_version = HAL_API_UNKNOWN,
    .mode = HAL_GPIO_OUTPUT,
    .output_type = HAL_GPIO_PUSH_PULL,
    .speed = 100,
    .pull = HAL_GPIO_NO_PULL
};

/**
 * Initializes a GPIO pin.
 * After that operation GPIO pin is ready to use according to supplied configuration table.
 *
 * Returns warning if pin initiated.
 */
hal_errors_t hal_gpio_pin_init(
    portNo_t port, ///< [in] portNo_t typedef to specify port (eg. 'PORT0')
    uint8_t pin, ///< [in] character (as unsigned integer) to specify pin (eg. 0)
    const hal_gpio_pin_cfg_t *cfg_table ///< [in] pointer to a struct containing configuration table
);

/**
 * Resets a GPIO pin.
 * After this operation pin is back to its reset values and can't be used before reinitialization.
 * Last reset automatically disables clock.
 *
 * Returns warning if pin not initiated.
 */
hal_errors_t hal_gpio_pin_reset(
    portNo_t port, ///< [in] portNo_t typedef to specify port (eg. 'PORT0')
    uint8_t pin ///< [in] character (as unsigned integer) to specify pin (eg. 0)
);

/**
 * Digital write to a GPIO pin.
 *
 * Returns error if pin not initiated.
 */
hal_errors_t hal_gpio_pin_write(
    portNo_t port, ///< [in] portNo_t typedef to specify port (eg. 'PORT0')
    uint8_t pin, ///< [in] character (as unsigned integer) to specify pin (eg. 0)
    hal_gpio_pin_state_t state ///< [in] to set pin into
);

/**
 * Digital read from a GPIO pin.
 *
 * Returns error if pin not initiated.
 */
hal_errors_t hal_gpio_pin_read(
    portNo_t port, ///< [in] portNo_t typedef to specify port (eg. 'PORT0')
    uint8_t pin, ///< [in] character (as unsigned integer) to specify pin (eg. 0)
    hal_gpio_pin_state_t *pin_state ///< [out]
);
