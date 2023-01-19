#ifndef TRACE__H
#define TRACE__H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void trace_init(void);
void trace_write(uint8_t data[], uint8_t size);
uint8_t trace_read(uint8_t data[], uint8_t size);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* TRACE__H */
