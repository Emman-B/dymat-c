#include "dymat.h"

//===========================================================//
// Structs
//===========================================================//
/** Struct: node_obj
 *  Fields: 
 *      void** memptr   - pointer to memory pointer
 *      char* desc      - description of pointer
 *      node_obj* next  - linked list connection to next node
 *      
 *      _DYMAT_MEMRPT fields
 *      int size        - size of the memory that is allocated
 *      int status      - status of memory:
 *                          0 = in use, 1 = free
 */ 
typedef struct node_obj
{
    void** memptr;
    char* desc;
    struct node_obj* next;

    #ifdef _DYMAT_MEMRPT
    int size;
    int status;
    #endif
};

// define "node" as a pointer to node_obj
typedef struct node_obj* node;

/** Struct: node_obj
 *  Fields: 
 *      node* pointerlist_head  - head of the linked list stack ADT
 */ 
typedef struct dymat_obj
{
    node pointerlist_head;
};

// define "dymat" as a pointer to dymat_obj
typedef struct dymat_obj* dymat;

// Primary Object for Dynamic Memory Allocation Tracking
dymat main_dymat = NULL;

//===========================================================//
//Private Methods
//===========================================================//
/* Function:    void init()
 * Parameters:  none
 * Description: initializes main_dymat
 */
void init()
{
    if (main_dymat == NULL)
    {
        main_dymat = malloc(sizeof(struct dymat_obj));
        main_dymat->pointerlist_head = NULL;
    }
}

/* Function:    node ctor_node()
 * Parameters:  void* memptr, char* desc, size_t sz
 * Description: constructor for node struct,
 *      also defines extra fields for _DYMAT_MEMRPT
 */
node ctor_node(void* memptr, char* desc, size_t sz)
{
    node newnode = malloc(sizeof(struct node_obj));
    newnode->memptr = &memptr;
    newnode->desc = desc;
    newnode->next = NULL;

    #ifdef _DYMAT_MEMRPT
    newnode->size = sz;
    newnode->status = 0;
    #endif

    return newnode;
}

/* Function:    void add_node()
 * Parameters:  node newnode
 * Description: Adds a node to dymat's list
 */
void add_node(node newnode)
{
    if (main_dymat->pointerlist_head == NULL)
    {
        main_dymat->pointerlist_head = newnode;
    }
    else
    {
        newnode->next = main_dymat->pointerlist_head;
        main_dymat->pointerlist_head = newnode;
    }
}


//===========================================================//
//Public Methods
//===========================================================//
/* Function:    void* td_malloc()
 * Parameters:  char* desc, size_t sz
 * Description: allocates memory with malloc to newmemory, and
 *      pointer to newmemory is stored into a node which is
 *      stored onto dymat's linked list
 *      This also takes in a description for the pointer.
 */
void* td_malloc(char* desc, size_t sz)
{
    init();
    // create node for newmemory
    void* newmemory = malloc(sz);
    node newnode = ctor_node(newmemory, desc, sz);

    // store node into dymat list
    add_node(newnode);
    
    return newmemory;
}

/* Function:    void* t_malloc()
 * Parameters:  size_t sz
 * Description: functions similarly to td_malloc(char*, size_t)
 *      except "Generic Pointer" is used as a description
 */
void* t_malloc(size_t sz)
{
    return td_malloc("Generic Pointer", sz);
}