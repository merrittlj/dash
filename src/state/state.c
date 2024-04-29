/*
 * filename:	state.c
 * date:	04.17.24
 * author:	Lucas Merritt/merrittlj
 * description:	implementation for state.h
 */

#include <stdint.h>

#include "state.h"


void fsm_init(struct fsm *f, const struct state *table, uint8_t state)
{
	f->table = table;
	f->cur = state;

	f->table[f->cur].action();
}

void fsm_action(struct fsm *f)
{
	f->table[f->cur].action();
}

void fsm_next(struct fsm *f)
{
	f->table[f->cur].transition();
	f->cur = f->table[f->cur].next;
	f->table[f->cur].action();
}


