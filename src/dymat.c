#include "dymat.h"


#define TABLE_SIZE 499  // prime number
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

/** Struct: dymat_obj
 *  Fields: 
 *      node* table                 - array of linked lists for hash table
 *  Extra Description: To add nodes to table, the value of the pointer is used.
 *      The pointer value is used as the seed for srand().
 */ 
typedef struct dymat_obj
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
 *      also sets all nodes in table to NULL
 */
void init()
{
    if (main_dymat == NULL)
    {
        main_dymat = malloc(sizeof(struct dymat_obj));
        main_dymat->table = calloc(TABLE_SIZE, sizeof(struct node_obj));
        for (int i = 0; i < TABLE_SIZE; i++)
        {
            main_dymat->table[i] = NULL;
        }
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
    unsigned long int index = (unsigned long int) *newnode->memptr;
    if (main_dymat->table[index] == NULL)
    {
        main_dymat->table[index] = newnode;
    }
    else
    {
        // TODO: complete this (and optimize it)
        // verify that the memptr doesn't already exist in the table; if so, replace it
        // if the first node in the list of size 1 has the same memptr value as newnode
        if (main_dymat->table[index]->next == NULL && *(main_dymat->table[index]->memptr) == *(newnode->memptr))
        {
            node toreplace = main_dymat->table[index];
            main_dymat->table[index] = newnode;
            free(toreplace);
            toreplace = NULL;
        }
        // the list is not of size one
        else
        {
            // if the first thing is what needs to be replaced...
            if ( *(main_dymat->table[index]->memptr) == *(newnode->memptr))
            {
                node toreplace = main_dymat->table[index];
                newnode->next = main_dymat->table[index]->next;
                main_dymat->table[index] = newnode;
                free(toreplace);
                toreplace = NULL;
            }
            // otherwise start checking the rest of the list
            else
            {
                node prev = main_dymat->table[index];
                node current = prev->next;
                while (current != NULL)
                {
                    // if the current node is what needs to be replaced
                    if ( *(current->memptr) == *(newnode->memptr) )
                    {
                        newnode->next = current->next;
                        prev->next = newnode;
                        free(current);
                        current = NULL;
                        return;
                    }
                    prev = prev->next;
                    current = current->next;
                }
            }
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
    unsigned long int index = (unsigned long int) *deletenode->memptr;
    // TODO: finish this
}

/* Function:    int node_equals()
 * Parameters:  node nodeA, node nodeB
 * Description: Returns 1 if *(nodeA->memptr) is equal to *(nodeB->memptr). In other words, returns 1
 *      if two nodes happen to be pointing to the same memory. This means that the description can
 *      differ but it will still return 1.
 */

int node_equals(node nodeA, node nodeB)
{
    return *(nodeA->memptr) == *(nodeB->memptr);
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