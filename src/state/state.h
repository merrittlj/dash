/*
 * filename:	state.h
 * date:	04.17.24
 * author:	Lucas Merritt/merrittlj
 * description:	finite state machine logic
 */

#ifndef STATE_H
#define STATE_H
#include <stdint.h>

#include "common.h"


struct state {
	func_ptr action;  /* What action to take when transitioning in. */
	func_ptr transition;  /* What action to take when transitioning out. */
	uint8_t next;  /* Next state. */

	char _padding[3];
};


struct fsm {
	const struct state *table;  /* State lookup table. */
	uint8_t cur;  /* Current state index. */
	
	char _padding[3];
};

/* Set appropriate variables in struct and start first state's action. */
extern void fsm_init(struct fsm *f, const struct state *table, uint8_t state);
extern void fsm_action(struct fsm *f);  /* Run current state action. */
/* Transition out of current state, go to next state, start state action. */
extern void fsm_next(struct fsm *f);

#endif
