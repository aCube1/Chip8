#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <stdbool.h>
#include <stdint.h>

int8_t audio_init(void); /* Create audio specification and open device. */
void audio_quit(void);	 /* Close audio device. */

/* Pause or unpause audio */
void audio_pause(bool pause);

#endif /* _AUDIO_H_ */
