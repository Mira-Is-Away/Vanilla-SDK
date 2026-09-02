#include "vnl_ds/vnl_ilist.h"

void vnl_ilist_init(VnlIList *list) {
    if (!list)
        return;
    list->head  = NULL;
    list->tail  = NULL;
    list->count = 0;
}

void vnl_ilist_push_back(VnlIList *list, VnlIListLink *link) {
    link->next = NULL;
    link->prev = list->tail;

    if (list->tail) {
        list->tail->next = link;
    } else {
        list->head = link;
    }

    list->tail = link;
    list->count++;
}

void vnl_ilist_push_front(VnlIList *list, VnlIListLink *link) {
    link->prev = NULL;
    link->next = list->head;

    if (list->head) {
        list->head->prev = link;
    } else {
        list->tail = link;
    }

    list->head = link;
    list->count++;
}

VnlIListLink *vnl_ilist_remove(VnlIList *list, VnlIListLink *link) {
    if (link->prev) {
        link->prev->next = link->next;
    } else {
        list->head = link->next;
    }

    if (link->next) {
        link->next->prev = link->prev;
    } else {
        list->tail = link->prev;
    }

    link->next = NULL;
    link->prev = NULL;
    list->count--;

    return link;
}

VnlIListLink *vnl_ilist_pop_back(VnlIList *list) {
    if (!list->tail)
        return NULL;
    return vnl_ilist_remove(list, list->tail);
}

VnlIListLink *vnl_ilist_pop_front(VnlIList *list) {
    if (!list->head)
        return NULL;
    return vnl_ilist_remove(list, list->head);
}
