#include "dymat.h"


const unsigned int TABLE_SIZE = 499;  // prime number
//===========================================================//
// Structs
//===========================================================//
/** Struct: node_obj
 *  Fields: 
 *      void** memptr   - pointer to memory pointer
 *      char* desc      - description of pointer
 *      node_obj* next  - linked list connection to next node
 *      
 *      DYMAT_MEMRPT fields
 *      int size        - size of the memory that is allocated
 *      int status      - status of memory:
 *                          0 = in use, 1 = free
 */ 
struct node_obj
{
    void* memptr;
    char* desc;
    struct node_obj* next;

    #ifdef DYMAT_MEMRPT
    int size;
    int status;
    #endif
};

// define "node" as a pointer to node_obj
typedef struct node_obj* node;

/** Struct: dymat_obj
 *  Fields: 
 *      node* table                 - array of linked lists for hash table
 *  Extra Description: To add nodes to table, the value of the pointer is used.
 *      The pointer value is used as the seed for srand().
 */ 
struct dymat_obj
{
    node* table;    // size is defined by TABLE_SIZE
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
 * Description: initializes main_dymat and table
 */
void init()
{
    if (main_dymat == NULL)
    {
        main_dymat = calloc(1, sizeof(struct dymat_obj));
        main_dymat->table = calloc(TABLE_SIZE, sizeof(struct node_obj));
    }
}

/* Function:    node ctor_node()
 * Parameters:  void* memptr, char* desc, size_t sz
 * Description: constructor for node struct,
 *      also defines extra fields for DYMAT_MEMRPT
 */
node ctor_node(void* memptr, char* desc, size_t sz)
{
    node newnode = calloc(1, sizeof(struct node_obj));
    newnode->memptr = memptr;
    newnode->desc = desc;
    newnode->next = NULL;

    #ifdef DYMAT_MEMRPT
    newnode->size = sz;
    newnode->status = 0;
    #endif

    return newnode;
}

/* Function:    int dtor_node()
 * Parameters:  node nodetodestruct
 * Description: destroys node, frees memory associated with node
 *      returns 1 if nodetodestruct is NULL (no destruction done), 0 otherwise
 */
int dtor_node(node nodetodestruct)
{
    if (nodetodestruct == NULL)
    {
        return 1;
    }
    else
    {
        if (nodetodestruct->memptr != NULL)
        {
            free(nodetodestruct->memptr);
            nodetodestruct->memptr = NULL;
        }
        free(nodetodestruct);
        nodetodestruct = NULL;
        return 0;
    }
    
}

/* Function:    int node_equals()
 * Parameters:  node nodeA, node nodeB
 * Description: Returns 1 if *(nodeA->memptr) is equal to *(nodeB->memptr). In other words, returns 1
 *      if two nodes happen to be pointing to the same memory. This means that the description can
 *      differ but it will still return 1.
 */

int node_equals(node nodeA, node nodeB)
{
    return nodeA->memptr == nodeB->memptr;
}

/* Function:    void add_node()
 * Parameters:  node newnode
 * Description: Adds a node to dymat's table by using the memptr value (the actual number) to calculate
 *      an index for the table. It adds the node to the front of the list.
 *      If the node is being added to list of size 1 or greater, it will iterate through
 *      the list to see if there is another node with the same memptr, that is, 
 *      the dereferenced newnode memptr is equal to another node's dereferenced memptr.
 *      If found, the new node will replace the old one.
 *      The replacement process will not free the memptr, only the node associated with it.
 */
void add_node(node newnode)
{
    unsigned long int index = (unsigned long int) newnode->memptr;
    index %= TABLE_SIZE;
    // Add case 1: The list is NULL at index
    if (main_dymat->table[index] == NULL)
    {
        main_dymat->table[index] = newnode;
    }
    // Add case 2: The list is not empty
    else
    {
        // verify that the memptr doesn't already exist in the table; if so, replace it
        // if the first node in the list of size 1 has the same memptr value as newnode

        // Add case 2a: the first node is what needs replacing
        node prevnode = main_dymat->table[index];
        if (node_equals(prevnode, newnode))
        {
            node toreplace = prevnode;
            newnode->next = main_dymat->table[index]->next;
            main_dymat->table[index] = newnode;
            free(toreplace);
        }
        else
        {
            // Add case 2b: the node that may need replacing is elsewhere
            node current = prevnode->next;
            while (current != NULL)
            {
                if (node_equals(current, newnode))
                {
                    node toreplace = current;
                    newnode->next = current->next;
                    prevnode->next = newnode;
                    free(toreplace);
                }
                prevnode = prevnode->next;
                current = current->next;
            }

            //Add case 3: there is no node that needs replacing
            newnode->next = main_dymat->table[index];
            main_dymat->table[index] = newnode;
        }
    }
}

/* Function:    void delete_node()
 * Parameters:  node deletenode
 * Description: Removes a node from dymat's table by using memptr value (the actual number) to calculate 
 *      the index for the table. If the list (at the index) has size greater than 1, it will iterate
 *      through the list, comparing memptr values until it finds the correct one.
 */
void remove_node(node deletenode)
{
    unsigned long int index = (unsigned long int) deletenode->memptr;
    index %= TABLE_SIZE;
    // Error 1: node at index does not exist
    if (main_dymat->table[index] == NULL)
    {
        fprintf(stderr, "[ERROR] remove_node called on node not on list\n");
    }
    // if else statement is reached, list at index exists
    else
    {
        node current = main_dymat->table[index];
        if (node_equals(deletenode, current))
        {
            main_dymat->table[index] = current->next;
            dtor_node(current);
        }
        else
        {
            node previous = current;
            current = current->next;
            while (current != NULL)
            {
                if (node_equals(deletenode, current))
                {
                    previous->next = current->next;
                    dtor_node(current);
                    break;
                }
                previous = previous->next;
                current = current->next;
            }
        }
    }
}
/* Function:    void freeall()
 * Parameters:  none
 * Description: When called, all nodes in main_dymat is freed.
 *      This does not free main_dymat or its table.
 */
void freeall()
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        while (main_dymat->table[i] != NULL)
        {
            remove_node(main_dymat->table[i]);
        }
    }
}

/* Function:    void destroy_dymatobj()
 * Parameters:  none
 * Description: When called, all nodes in main_dymat is freed and
 *      the main_dymat and main_dymat->table are freed. This should
 *      be called on exit of the program.
 */
void destroy_dymatobj()
{
    if (main_dymat == NULL)
    {
        return;
    }
    freeall();

    free(main_dymat->table);
    main_dymat->table = NULL;

    free(main_dymat);
    main_dymat = NULL;
}

/* Function:    int is_null()
 * Parameters:  void* memptr
 * Description: Checks memptr if it exists in the table. If it does
 *      exist, it returns 1; else it returns 0.
 */
int is_null(void* memptr)
{
    unsigned long int index = (unsigned long int) memptr;
    index %= TABLE_SIZE;

    node current = main_dymat->table[index];
    while (current != NULL)
    {
        if (current->memptr == memptr)
        {
            return 1;
        }
        current = current->next;
    }
    return 0;
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

/* Function:    void* td_calloc()
 * Parameters:  char* desc, size_t number, size_t sz
 * Description: allocates memory with calloc to newmemory, and
 *      pointer to newmemory is stored into a node which is
 *      stored onto dymat's linked list
 *      This also takes in a description for the pointer.
 */
void* td_calloc(char* desc, size_t number, size_t sz)
{
    init();
    // create node for newmemory
    void* newmemory = calloc(number, sz);
    node newnode = ctor_node(newmemory, desc, number * sz);

    // store node into dymat list
    add_node(newnode);
    
    return newmemory;
}

/* Function:    void* t_calloc()
 * Parameters:  size_t number, size_t sz
 * Description: functions similarly to td_calloc(char*, size_t, size_t)
 *      except "Generic Pointer" is used as a description
 */
void* t_calloc(size_t number, size_t sz)
{
    return td_calloc("Generic Pointer", number, sz);
}



//===========================================================//
//Debug Methods (accessible if DYMAT_ENABLE_DEBUG is defined)
//===========================================================//
void** get_memptr (node n)
{
    return n->memptr;
}
char* get_desc (node n)
{
    return n->desc;
}
node get_next(node n)
{
    return n->next;
}
#ifdef DYMAT_MEMRPT
    int get_size (node n)
    {
        return n->size;
    }
    int get_status (node n)
    {
        return n->status;
    }
#endif