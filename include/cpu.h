#ifndef _CPU_H_
#define _CPU_H_

#include <stdint.h>

int8_t cpu_init(const char *filename);
int8_t cpu_emulate_cycle(void);

#endif /* _CPU_H_ */
