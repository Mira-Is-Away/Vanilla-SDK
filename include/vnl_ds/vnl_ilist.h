/**
 * @file vnl_ilist.h
 *
 * Intrusive doubly-linked list and associated functions and macros
 *
 * @author Henry R
 * @date 2026-02-16
 */

#ifndef VANILLA_VNL_DS_ILIST_H_
#define VANILLA_VNL_DS_ILIST_H_

#include "misc/vnl_types.h"
#include <stddef.h>

/**
 * @struct VnlIListLink
 * @brief Intrusive link to be embedded in data structures.
 */
typedef struct VnlIListLink {
    struct VnlIListLink *next;
    struct VnlIListLink *prev;
} VnlIListLink;

/**
 * @struct VnlIList
 * @brief Head of an intrusive doubly linked list.
 */
typedef struct VnlIList {
    VnlIListLink *head;
    VnlIListLink *tail;
    u32 count;
} VnlIList;

/**
 * @brief Retrieves the parent structure from a list link pointer.
 * @param ptr Pointer to the VnlIListLink.
 * @param type The type of the parent structure.
 * @param member The name of the VnlIListLink member within the parent
 * structure.
 */
#define vnl_ilist_container_of(ptr, type, member)                              \
    ((type *)((char *)(ptr) - offsetof(type, member)))

/**
 * @brief Simple iteration over the list links.
 * @param ptr A valid pointer to the desired object's type.
 * @param list Pointer to the list link.
 */
#define vnl_ilist_for_each(ptr, list)                                          \
    for (ptr = (list)->head; ptr != NULL; ptr = ptr->next)

/**
 * @brief Iteration safe against removal of the current link.
 * @param ptr A valid pointer to the desired object's type.
 * @param next Must be another valid pointer of the desired object type. Used
 * for internal safety safeguards. param list Pointer to the list link.
 */
#define vnl_ilist_for_each_safe(ptr, next, list)                               \
    for (ptr = (list)->head, next = (ptr ? ptr->next : NULL); ptr != NULL;     \
         ptr = next, next = (ptr ? ptr->next : NULL))

/**
 * @brief Iterates over the list and provides the parent structure pointer
 * directly.
 * @param ptr A valid pointer to the desired object's type.
 * @param type The type of the parent structure.
 * @param list Pointer to the list link.
 * @param member The name of the struct field that holds the pointers to the
 * nodes on the list.
 */
#define vnl_ilist_for_each_entry(ptr, type, list, member)                      \
    for (ptr = ((list)->head                                                   \
                    ? vnl_ilist_container_of((list)->head, type, member)       \
                    : NULL);                                                   \
         ptr != NULL;                                                          \
         ptr = (ptr->member.next                                               \
                    ? vnl_ilist_container_of(ptr->member.next, type, member)   \
                    : NULL))

/**
 * @brief Iterates over the list entries safely (allows removal).
 * @param ptr A valid pointer of the desired object's type.
 * @param next A pointer of the same type as ptr. The macro will use it to
 * iterate over the list. It doesn't need to be initialised.
 * @param type The type of the parent structure.
 * @param list A pointer to the list link.
 * @param member The name of the struct field that holds the pointers to the
 * nodes on the list.
 */

/* you're not expected to understand this */
#define vnl_ilist_for_each_entry_safe(ptr, next, type, list, member)           \
    for (ptr = ((list)->head                                                   \
                    ? vnl_ilist_container_of((list)->head, type, member)       \
                    : NULL),                                                   \
        next = ((ptr && ptr->member.next)                                      \
                    ? vnl_ilist_container_of(ptr->member.next, type, member)   \
                    : NULL);                                                   \
         ptr != NULL; ptr = next,                                              \
        next = ((ptr && ptr->member.next)                                      \
                    ? vnl_ilist_container_of(ptr->member.next, type, member)   \
                    : NULL))

/**
 * @brief Initializes a list head.
 * @param list Pointer to the list to initialize.
 */
void vnl_ilist_init(VnlIList *list);

/**
 * @brief Pushes a link to the end of the list.
 * @param list Pointer to the list.
 * @param link Pointer to the link to insert.
 */
void vnl_ilist_push_back(VnlIList *list, VnlIListLink *link);

/**
 * @brief Pushes a link to the front of the list.
 * @param list Pointer to the list.
 * @param link Pointer to the link to insert.
 */
void vnl_ilist_push_front(VnlIList *list, VnlIListLink *link);

/**
 * @brief Removes a specific link from the list.
 * @param list Pointer to the list.
 * @param link Pointer to the link to remove.
 * @return The removed link pointer.
 */
VnlIListLink *vnl_ilist_remove(VnlIList *list, VnlIListLink *link);

/**
 * @brief Pops a link from the end of the list.
 * @param list Pointer to the list.
 * @return The removed link pointer, or NULL if empty.
 */
VnlIListLink *vnl_ilist_pop_back(VnlIList *list);

/**
 * @brief Pops a link from the front of the list.
 * @param list Pointer to the list.
 * @return The removed link pointer, or NULL if empty.
 */
VnlIListLink *vnl_ilist_pop_front(VnlIList *list);

#endif
