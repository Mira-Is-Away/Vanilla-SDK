#include "vnl_ds/vnl_list.h"

#include <core/vnl_status.h>
#include <core/vnl_types.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <vnl_ds/vnl_list.h>
#ifndef NDEBUG
#define MIRA_CLARITY_DEBUG
#endif
#include <mira/clarity.h>

/*
Henry: this file is a great candidate for usage of a traceback feature in
Clarity
*/

static VnlListNode *vnl_list_create_node(const void *data, u32 size) {
    VnlListNode *node = (VnlListNode *)CLARITY_MALLOC(sizeof(VnlListNode));
    if (!node)
        return NULL;

    if (data && size > 0) {
        node->data = CLARITY_MALLOC(size);
        if (!node->data) {
            CLARITY_FREE(node);
            return NULL;
        }
        memcpy(node->data, data, size);
    } else {
        node->data = NULL;
    }

    node->size = size;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

VnlStatus vnl_list_create(VnlList *list) {
    if (!list) {
        CLARITY_LOG_WARN(
            "Attempted to initialise VnlList object from NULL pointer.");
        return VNL_FAILURE;
    }

    list->head = NULL;
    list->tail = list->head;
    list->size = 0;

    return VNL_SUCCESS;
}

VnlStatus vnl_list_pushfront_default(VnlList *list, const void *data,
                                     u32 size) {
    if (!list) {
        CLARITY_LOG_WARN("Attemped to add item to NULL list.");
        return VNL_FAILURE;
    }

    VnlListNode *node = vnl_list_create_node(data, size);
    if (node == NULL)
        return VNL_FAILURE;

    VnlListNode *old_head = list->head;

    if (old_head == NULL) {
        list->head = node;
        list->tail = list->head;
    } else {
        old_head->prev = node;
        node->next     = old_head;
        list->head     = node;
    }

    list->size++;

    return VNL_SUCCESS;
}

VnlStatus vnl_list_pushfront_str(VnlList *list, const char *str) {
    if (!list || !str) {
        CLARITY_LOG_WARN("Attempted to add NULL string or to a NULL list.");
        return VNL_FAILURE;
    }

    return vnl_list_pushfront_default(list, (const void *)str,
                                      (u32)(strlen(str) + 1));
}

VnlStatus vnl_list_pushback_default(VnlList *list, const void *data, u32 size) {
    if (!list) {
        CLARITY_LOG_WARN("Attemped to add item to NULL list.");
        return VNL_FAILURE;
    }

    VnlListNode *node = vnl_list_create_node(data, size);
    if (!node)
        return VNL_FAILURE;

    VnlListNode *old_tail = list->tail;

    if (old_tail == NULL) {
        list->head = node;
        list->tail = list->head;
    } else {
        old_tail->next = node;
        node->prev     = old_tail;
        list->tail     = node;
    }

    list->size++;

    return VNL_SUCCESS;
}

VnlStatus vnl_list_pushback_str(VnlList *list, const char *str) {
    if (!list || !str) {
        CLARITY_LOG_WARN("Attempted to add NULL string or to a NULL list.");
        return VNL_FAILURE;
    }

    return vnl_list_pushback_default(list, (const void *)str,
                                     (u32)(strlen(str) + 1));
}

void *vnl_list_get_element_from_index(VnlList *list, u32 index) {
    VnlListNode *cursor = list->head;
    for (u32 i = 0; i <= index; i++) {
        if (cursor == NULL)
            return NULL;

        if (i == index)
            return cursor->data;

        cursor = cursor->next;
    }

    return NULL;
}

void vnl_list_destroy(VnlList *list, VnlListDestroyCallback destroy) {
    if (list == NULL) {
        CLARITY_LOG_WARN("Attempted to destroy NULL VnlList.");
        return;
    }

    VnlListNode *cursor = list->tail;
    while (cursor != NULL) {
        VnlListNode *next = cursor->prev;

        if (destroy && cursor->data) {
            destroy(cursor->data);
        } else {
            CLARITY_FREE(cursor->data);
        }

        CLARITY_FREE(cursor);
        cursor = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}