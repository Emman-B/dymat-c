# DYnamic Memory Allocation Tracker (DYMAT)
This will assist with tracking memory that has been dynamically allocated. It defines 6 functions,
3 of which intend to replace existing functions. Each pointer that is allocated memory is stored
into a hashtable, in which the pointer value is used as the key.

## C Standard
The code was written with c99 in mind.

## Usage
Incorporate dymat.c and dymat.h into an existing project written in C. This will allow you to make
use of 6 functions: t_malloc(), t_calloc(), t_free(), freeall(), destroy_dymatobj(), is_null().

t_malloc(), t_calloc(), and t_free() take the same parameters as their non-t_ counterparts and 
functions similarly to it. The only difference is it interacts with a built-in table data structure
that handles tracking of the pointers.

freeall() frees all the existing memory pointers without destroying the table. This can result in
memory leaks if destroy_dymatobj() is not called, but it is useful if the table only needs to be cleared.

destroy_dymatobj() frees all existing memory pointers and frees the table. This will clear all memory leaks.

is_null(void*) is used in place of the standard of setting pointers to NULL if they are freed. This function
checks the table if the memory pointer provided exists on the table.