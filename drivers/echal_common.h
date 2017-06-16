#pragma once
/**
 * @file echal_common.h
 * ECHAL common definitions for all api modules
 */

/**
 * ECHAL API versions that must be specified in each initialization call.
 * Calls should specify the api version that developer used at the time of writing the code.
 * API version is visible from the branch name and in the header of api documentation.
 */
typedef enum {
    HAL_API_UNKNOWN, ///< Default value, should always be changed
    HAL_API_VERSION_1
} hal_api_version_t;

/**
 * Common error and warning definitions across all modules.
 * API convention (in README) has more information about how this works.
 */
typedef enum{
    //errors
    HAL_OK, ///< All function calls must check if this code is set
    HAL_UNKNOWN_ERROR, ///< Fallback error if no more information can be returned
    HAL_NOT_IMPLEMENTED, ///< Both incomplete implementation and lacking hardware support
    HAL_HW_NOT_SUPPORTED, ///< This action on this subject is not supported by HW
    HAL_BUSY,  ///< Action not possible because suject in busy
    HAL_IN_USE, ///< Action not possible because subject is in use (eg system clock can't be disabled)
    HAL_OUT_OF_RANGE, ///< Value not in range that is supported by hw
    HAL_OUT_OF_MEMORY, ///< Out of memory
    HAL_NO_CALLBACKS_FOR_THIS_EVENT, ///< No callbacks for this event exists
    HAL_CALLBACK_NOT_FOUND, ///< Did not find the specified callback
    //warnings
    HAL_UNKNOWN_WARNING=128, ///< Fallback warning if no more information can be returned
}hal_errors_t;
