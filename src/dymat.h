#ifndef DYMAT_H
#define DYMAT_H

#include<stdlib.h>
#include<stdio.h>

// dynamic memory allocation tracker object
typedef struct dymat_obj* dymat;
typedef struct node_obj* node;      // primarily for debugging, not intended to be accessed

// malloc which stores pointer to queue and returns it
void* t_malloc(size_t sz);

// calloc which stores pointer to queue and returns it
void* t_calloc(size_t number, size_t sz);

// frees a specific pointer
int t_free(void* memptr);

// frees all allocated memory (except for main_dymat)
void freeall();
// frees everything (including main_dymat)
void destroy_dymatobj();

// verifies if a pointer is not null (by searching through table using pointer)
int is_null(void* memptr);

#endif