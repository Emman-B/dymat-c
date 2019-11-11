#ifndef _DYMAT_H
#define _DYMAT_H

#include<stdlib.h>

// dynamic memory allocation tracker object
typedef struct dymat_obj* dymat;

// malloc which stores pointer to queue and returns it
// "Generic Pointer" is used as description if desc not defined
void* t_malloc(size_t sz);
void* td_malloc(char* desc, size_t sz);

// calloc which stores pointer to queue and returns it
// "Generic Pointer" is used as description if desc not defined
void* t_calloc(size_t number, size_t sz);
void* td_calloc(char* desc, size_t number, size_t sz);

int t_free(void* memptr);

#ifdef  _ENABLE_DYMAT_DEBUGGING
    #ifndef _DYMAT_H_DEBUG
    #define _DYMAT_H_DEBUG


    #endif
#endif

#endif