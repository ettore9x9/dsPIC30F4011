/*
 * File: my_circular_buffer_lib.c
 * Authors: Carlone Matteo, Maragliano Matteo, Musumeci Mattia, Sani Ettore
 *
 * Created on 8 novembre 2022, 11.55
 */


#include "xc.h"
#include "my_circular_buffer_lib.h"

int cb_length()
{
    return SIZE;
}

void cb_init(volatile circular_buffer *cb)
{
    // Init the circular buffer
    cb->count = 0;
    cb->head = 0;
    cb->tail = 0;
}

int cb_push_back(volatile circular_buffer *cb, char item)
{
    // Add an element to the circular buffer
    if(cb->count == SIZE)
        return -1; // no space to write
    
    // Store the new item
    cb->container[cb->head] = item;
    // Update the buffer head
    cb->head = (cb->head+1) % SIZE;
    cb->count++;
    return 0;
}

int cb_pop_front(volatile circular_buffer *cb, char* item)
{
    // Read an element from the circular buffer
    if(cb->count == 0)
        return 0; // no things to be read
    
    // Pop the item from the buffer tail
    *item = cb->container[cb->tail];
    // Update the tale
    cb->tail = (cb->tail+1) % SIZE;
    cb->count--;
    return 1;
}