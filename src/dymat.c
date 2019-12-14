#include "dymat.h"


const unsigned int TABLE_SIZE = 499;  // prime number
//===========================================================//
// Structs
//===========================================================//
/** Struct: node_obj
 *  Fields: 
 *      void** memptr   - pointer to memory pointer
 *      node_obj* next  - linked list connection to next node
 */ 
struct node_obj
{
    void* memptr;
    struct node_obj* next;
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
 * Parameters:  void* memptr, size_t sz
 * Description: constructor for node struct.
 */
node ctor_node(void* memptr, size_t sz)
{
    node newnode = calloc(1, sizeof(struct node_obj));
    newnode->memptr = memptr;
    newnode->next = NULL;

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
 *      if two nodes happen to be pointing to the same memory.
 */

int node_equals(node nodeA, node nodeB)
{
    return nodeA->memptr == nodeB->memptr;
}

/* Function:    unsigned long int make_index()
 * Parameters:  void* memptr
 * Description: Turns memptr into an index restricted to TABLE_SIZE.
 */
unsigned long int make_index(void* memptr)
{
    return ((unsigned long int) memptr) % TABLE_SIZE;
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
    unsigned long int index = make_index(newnode->memptr);

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
    unsigned long int index = make_index(deletenode->memptr);
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

/* Function:    node find_in_table()
 * Parameters:  void* memptr
 * Description: Returns a node that is currently in the table using memptr
 *      as a lookup value. Returns NULL if no such memptr exists.
 */
node find_in_table(void* memptr)
{
    unsigned long int index = make_index(memptr);
    node current = main_dymat->table[index];
    while (current != NULL)
    {
        if (current->memptr == memptr)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

//===========================================================//
//Public Methods
//===========================================================//
/* Function:    void* t_malloc()
 * Parameters:  size_t sz
 * Description: allocates memory with malloc to newmemory, and
 *      pointer to newmemory is stored into a node which is
 *      stored onto dymat's linked list
 */
void* t_malloc(size_t sz)
{
    init();
    // create node for newmemory
    void* newmemory = malloc(sz);
    node newnode = ctor_node(newmemory, sz);

    // store node into dymat list
    add_node(newnode);
    
    return newmemory;
}

/* Function:    void* t_calloc()
 * Parameters:  size_t number, size_t sz
 * Description: allocates memory with calloc to newmemory, and
 *      pointer to newmemory is stored into a node which is
 *      stored onto dymat's linked list
 */
void* t_calloc(size_t number, size_t sz)
{
    init();
    // create node for newmemory
    void* newmemory = calloc(number, sz);
    node newnode = ctor_node(newmemory, number * sz);

    // store node into dymat list
    add_node(newnode);
    
    return newmemory;
}

/* Function:    int t_free()
 * Parameters:  void* memptr
 * Description: Frees memory at a given memptr. Returns 0 if free is done,
 *      1 if free is not done.
 */
int t_free(void* memptr)
{
    node tofree = find_in_table(memptr);
    if (tofree != NULL)
    {
        remove_node(tofree);
        return 0;
    }
    else
    {
        return 1;
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
    if (find_in_table(memptr) != NULL)
    {
        return 1;
    }
    return 0;
}