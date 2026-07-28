/**
 * @file vnl_list.h
 *
 * Standard generic doubly-linked list and associated functions
 *
 * @author Henry R
 * @date 2026-05-15
 */

#ifndef VANILLA_VNL_DS_LIST_H_
#define VANILLA_VNL_DS_LIST_H_

#include <core/vnl_status.h>
#include <core/vnl_types.h>

typedef struct VnlListNode {
    void *data;
    u32 size;

    struct VnlListNode *prev;
    struct VnlListNode *next;
} VnlListNode;

typedef struct VnlList {
    VnlListNode *head;
    VnlListNode *tail;
    u32 size;
} VnlList;

/**
 * @brief Macro to iterate over each node in a VnlList.
 * @param node_var The name of the VnlListNode* variable to define for
 * iteration.
 * @param list A pointer to the VnlList object.
 */
#define vnl_list_foreach(node_var, list)                                       \
    for (VnlListNode *node_var = (list)->head; node_var != NULL;               \
         node_var = node_var->next)

/**
 * @brief Create a VnlList object and allocates a head node.
 * @return The status of the operation.
 * @param list A pointer to the VnlList object
 */
VnlStatus vnl_list_create(VnlList *list);

/**
 * @brief Insert a node at the start of the list.
 * @param list A pointer to the list object.
 * @param data A pointer to the data to be stored.
 * @param size The size of the data in bytes.
 * @return A pointer to the list object. Mostly useful for chaining.
 */
VnlStatus vnl_list_pushfront_default(VnlList *list, const void *data, u32 size);

/**
 * @brief Insert a node containing a string at the start of the list.
 * @param list A pointer to the list object.
 * @param data A pointer to the data to be stored.
 * @return A pointer to the list object. Mostly useful for chaining.
 */
VnlStatus vnl_list_pushfront_str(VnlList *list, const char *str);

#define vnl_list_append(list, item)                                            \
    _Generic((item),                                                           \
        char *: vnl_list_pushback_str(list, (char *)(item)),                   \
        const char *: vnl_list_pushback_str(list, (const char *)(item)),       \
        default: vnl_list_pushback_default(list, &(item), sizeof(item)))

/**
 * @brief Insert a node at the end of the list.
 * @param list A pointer to the list object.
 * @param data A pointer to the data to be stored.
 * @param size The size of the data in bytes.
 * @return A pointer to the list object. Mostly useful for chaining.
 */
VnlStatus vnl_list_pushback_default(VnlList *list, const void *data, u32 size);

/**
 * @brief Insert a node containing a string at the end of the list.
 * @param list A pointer to the list object.
 * @param data A pointer to the data to be stored.
 * @return A pointer to the list object. Mostly useful for chaining.
 */
VnlStatus vnl_list_pushback_str(VnlList *list, const char *data);

/**
 * @brief Retrieves the element of the list in the corresponding index.
 * @param list A pointer to the list object.
 * @param index The index of the element to be retrieved.
 * @return A pointer to the data at that index.
 * @retval NULL If the element is not found.
 */
void *vnl_list_get_element_from_index(VnlList *list, u32 index);

/**
 * VnlListDestroyCallback is used alongside vnl_list_destroy to
 * deallocate complex structs being held inside a VnlList.
 */
typedef void (*VnlListDestroyCallback)(void *data);

/**
 * @brief Destroys a VnlList object and frees memory allocated by it.
 *
 * This function destroys a VnlList object and its nodes. If callback
 * is NULL, it will assume the data can be free without causing memory
 * leaks from shallow freeing. In case you are storing complex structures
 * with dynamically allocated memory inside the list object, you should
 * provide this function with a pointer to a destroy function that can
 * properly free the resources allocated in the node.
 *
 * @param list A pointer to the list object.
 * @param callback A pointer to a callback cleanup function.
 */
void vnl_list_destroy(VnlList *list, VnlListDestroyCallback destroy);

#endif