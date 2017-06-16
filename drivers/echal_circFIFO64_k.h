#include <stdint.h>

/**
 * Circular FIFO struct. Use push and pop to manipulate
 */
typedef struct
{
    uint16_t head; ///< Points where data will be written
    uint16_t tail; ///< Points where data will be read
    uint16_t free; ///< Shows how many words can be written
    uint16_t size; ///< Total buffer size
    uint8_t *data; ///< Pointer to data buffer
} circFIFO64k_t;

/**
 * Initiates circular FIFO buffer to be used
 */
uint8_t circFIFO_init(
        circFIFO64k_t *fifo,  ///< [out] pointer to circular FIFO
        uint8_t *buffer,  ///< [in] pointer to FIFO buffer
        uint16_t size  ///< [in] buffer size in words
);

/**
 * Adds the number of words to FIFO determined by length.
 *
 * Use circFIFO_size_free to check if data fits to buffer.
 */
uint16_t circFIFO_push(
        circFIFO64k_t *fifo,  ///< [in] pointer to circular FIFO
        const uint8_t *data,  ///< [in]
        uint16_t length ///< [in]
);

/**
 * Writes the number of words determined by length (or less if less is available)
 * to dest pointer and returns the word amount
 */
uint16_t circFIFO_pop(
        circFIFO64k_t *fifo,  ///< [in] pointer to circular FIFO
        uint8_t *dest,  ///< [out] pointer to array where data is put
        uint16_t length ///< [in] amount of words to be popped
);

/**
 * Writes the number of words determined by length (or less if less is available)
 * to dest pointer and returns the word amount,
 * whilst not changing FIFO head, tail and free values.
 */
uint16_t circFIFO_peek(
        circFIFO64k_t *fifo,  ///< [in] pointer to circular FIFO
        uint8_t *dest,  ///< [out] pointer to array where data is put
        uint16_t length ///< [in] amount of words to be peeked
);

/**
 * Returns the number of words that can be pushed to FIFO
 */
uint16_t circFIFO_size_free(
        circFIFO64k_t *fifo ///< [in] pointer to circular FIFO
);

/**
 * Returns the number of words in FIFO buffer
 */
uint16_t circFIFO_size_used(
        circFIFO64k_t *fifo ///< [in] pointer to circular FIFO
);

/**
 * Returns 1 if buffer is empty, 0 otherwise
 */
uint8_t circFIFO_isEmpty(
        circFIFO64k_t *fifo ///< [in] pointer to circular FIFO
);

/**
 * Returns 1 if buffer is full, 0 otherwise
 */
uint8_t circFIFO_isFull(
        circFIFO64k_t *fifo ///< [in] pointer to circular FIFO
);

/**
 * Empties FIFO buffer.
 */
void circFIFO_flush(
        circFIFO64k_t *fifo ///< [in] pointer to circular FIFO
);
