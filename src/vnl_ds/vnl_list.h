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

#include <mira/vnl_status.h>
#include <mira/vnl_types.h>

/**
 * @struct VnlListNode
 * @brief Node in a doubly-linked list containing payload data.
 */
typedef struct VnlListNode {
    void *data;
    u32   size;

    struct VnlListNode *prev;
    struct VnlListNode *next;
} VnlListNode;

/**
 * @struct VnlList
 * @brief Head and tail management structure for a doubly-linked list.
 */
typedef struct VnlList {
    VnlListNode *head;
    VnlListNode *tail;
    u32          size;
} VnlList;

/**
 * @brief Macro to iterate over each node in a VnlList.
 * @param node_var The name of the VnlListNode* variable to define for
 * iteration.
 * @param list A pointer to the VnlList object.
 */
#define vnl_list_foreach(node_var, list)                                       \
    for (VnlListNode *node_var = (list)->head; node_var != NULL;               \
         node_var              = node_var->next)

/**
 * @brief Create a VnlList object and allocates a head node.
 * @param[out] list A pointer to the VnlList object to initialise.
 * @retval VNL_SUCCESS If creation was successful.
 */
VnlStatus vnl_list_create(VnlList *list);

/**
 * @brief Insert a node at the start of the list.
 * @param[in,out] list A pointer to the list object.
 * @param[in] data A pointer to the data to be stored.
 * @param[in] size The size of the data in bytes.
 * @retval VNL_SUCCESS If the node was inserted successfully.
 */
VnlStatus vnl_list_pushfront_default(VnlList *list, const void *data, u32 size);

/**
 * @brief Insert a node containing a string at the start of the list.
 * @param[in,out] list A pointer to the list object.
 * @param[in] str The null-terminated string to be stored.
 * @retval VNL_SUCCESS If the string was inserted successfully.
 */
VnlStatus vnl_list_pushfront_str(VnlList *list, const char *str);

/**
 * @brief Type-generic macro to append an item to the end of a list.
 * @param list Pointer to the VnlList object.
 * @param item The value or string to append.
 */
#define vnl_list_append(list, item)                                            \
    _Generic((item),                                                           \
        char *: vnl_list_pushback_str(list, (char *)(item)),                   \
        const char *: vnl_list_pushback_str(list, (const char *)(item)),       \
        default: vnl_list_pushback_default(list, &(item), sizeof(item)))

/**
 * @brief Insert a node at the end of the list.
 * @param[in,out] list A pointer to the list object.
 * @param[in] data A pointer to the data to be stored.
 * @param[in] size The size of the data in bytes.
 * @retval VNL_SUCCESS If the node was inserted successfully.
 */
VnlStatus vnl_list_pushback_default(VnlList *list, const void *data, u32 size);

/**
 * @brief Insert a node containing a string at the end of the list.
 * @param[in,out] list A pointer to the list object.
 * @param[in] data The null-terminated string to be stored.
 * @retval VNL_SUCCESS If the string was inserted successfully.
 */
VnlStatus vnl_list_pushback_str(VnlList *list, const char *data);

/**
 * @brief Retrieves the element of the list in the corresponding index.
 * @param[in] list A pointer to the list object.
 * @param[in] index The index of the element to be retrieved.
 * @return A pointer to the data at that index.
 * @retval NULL If the element is not found.
 */
void *vnl_list_get_element_from_index(VnlList *list, u32 index);

/**
 * @typedef VnlListDestroyCallback
 * @brief Callback function used alongside vnl_list_destroy to
 * deallocate complex structs being held inside a VnlList.
 * @param[in] data Pointer to the node data to deallocate.
 */
typedef void (*VnlListDestroyCallback)(void *data);

/**
 * @brief Destroys a VnlList object and frees memory allocated by it.
 *
 * This function destroys a VnlList object and its nodes. If destroy
 * is NULL, it will assume the data can be freed without causing memory
 * leaks from shallow freeing. In case you are storing complex structures
 * with dynamically allocated memory inside the list object, you should
 * provide this function with a pointer to a destroy function that can
 * properly free the resources allocated in the node.
 *
 * @param[in,out] list A pointer to the list object.
 * @param[in] destroy A pointer to a callback cleanup function (or NULL).
 */
void vnl_list_destroy(VnlList *list, VnlListDestroyCallback destroy);

#endif