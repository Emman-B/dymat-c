#ifndef _DYMAT_H
#define _DYMAT_H

#include<stdlib.h>

// dynamic memory allocation tracker object
typedef struct dymat_obj* dymat;
typedef struct node_obj* node;      // identify node

// malloc which stores pointer to queue and returns it
// "Generic Pointer" is used as description if desc not defined
void* t_malloc(size_t sz);
void* td_malloc(char* desc, size_t sz);

// calloc which stores pointer to queue and returns it
// "Generic Pointer" is used as description if desc not defined
void* t_calloc(size_t number, size_t sz);
void* td_calloc(char* desc, size_t number, size_t sz);

int t_free(void* memptr);

// functions for debugging table/node functions
#if defined(_DYMAT_ENABLE_DEBUG)
    void ctor_node(void* memptr, char* desc, size_t sz);
    int node_equals(node nodeA, node nodeB);
    void add_node(node newnode);
    void remove_node(node deletenode);
#endif

#endif