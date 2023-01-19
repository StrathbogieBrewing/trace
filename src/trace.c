#include <avr/interrupt.h>
#include <avr/io.h>

#include "cobsm.h"
#include "trace.h"

#define TRACE_BUFFER_BITS (6)
#define TRACE_BUFFER_SIZE (1 << TRACE_BUFFER_BITS)
#define TRACE_BUFFER_MASK (TRACE_BUFFER_SIZE - 1)

static volatile uint8_t trace_data[TRACE_BUFFER_SIZE] __attribute__((aligned(TRACE_BUFFER_SIZE)));
static volatile uint8_t trace_head = 0;
static volatile uint8_t trace_tail = 0;
static volatile uint8_t trace_read_tail = 0;

ISR(SPI_STC_vect) {
    if (trace_head != trace_tail) {
        SPDR = trace_data[trace_tail];
        trace_data[trace_tail] = SPDR;
        trace_tail += 1;
        trace_tail &= TRACE_BUFFER_MASK;
    } else {
        // SPCR &= ~(1 << SPIE);
        SPDR = 0x00;
    }
}

void trace_init(void) {
    DDRB |= (1 << PORTB2) | (1 << PORTB3) | (1 << PORTB5);
    PORTB |= (1 << PORTB3);
    SPCR = (1 << MSTR) | (1 << SPE) | (1 << SPR0) | (1 << SPR1) | (1 << CPHA) | (0 << CPOL);
    SPCR |= (1 << SPIE);
    SPDR = 0x00;
}

void trace_write(uint8_t data[], uint8_t size) {
    size = cobsm_encode(data, size);
    cli();

    for (uint8_t index = 0; index < size; index++) {
        trace_data[trace_head] = data[index];
        trace_head += 1;
        trace_head &= TRACE_BUFFER_MASK;
        data[index] = trace_data[trace_head];
    }

    trace_data[trace_head] = 0x00;
    trace_head += 1;
    trace_head &= TRACE_BUFFER_MASK;

    SPDR = 0x01;
    SPCR |= (1 << SPIE);
    sei();
}

uint8_t trace_read(uint8_t data[], uint8_t size) {
    cli();
    uint8_t index = 0;
    while ((trace_read_tail != trace_tail) && (index < size)) {
        data[index] = trace_data[trace_read_tail];
        index += 1;
        trace_read_tail += 1;
        trace_read_tail &= TRACE_BUFFER_MASK;
    }
    sei();
    return index;
}
