//
//  Copyright (C) 2023  Nick Gasson
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef _PSL_FSM_H
#define _PSL_FSM_H

#include "prim.h"

typedef struct _fsm_edge fsm_edge_t;
typedef struct _fsm_state fsm_state_t;

typedef enum {
   EDGE_NEXT, EDGE_EPSILON
} edge_kind_t;

typedef struct _fsm_edge {
   fsm_edge_t  *next;
   fsm_state_t *dest;
   edge_kind_t  kind;
   psl_node_t   guard;
} fsm_edge_t;

typedef struct _fsm_state {
   unsigned     id;
   fsm_state_t *next;
   fsm_edge_t  *edges;
   psl_node_t   test;
   bool         initial;
   bool         accept;
} fsm_state_t;

typedef struct {
   fsm_state_t  *states;
   fsm_state_t **tail;
   unsigned      next_id;
} psl_fsm_t;

psl_fsm_t *psl_fsm_new(psl_node_t p);
void psl_fsm_free(psl_fsm_t *fsm);
void psl_fsm_dump(psl_fsm_t *fsm, const char *fname);
unsigned psl_fsm_size(psl_fsm_t *fsm);

#endif  // _PSL_FSM_H
