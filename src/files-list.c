#include <stdio.h>
#include <stdlib.h>
#include "files-list.h"

struct files_list
{
    char *filename;
    struct files_list *next;
};

node create_node(char *filename)
{
    node temp;
    temp = (node)malloc(sizeof(struct files_list));
    temp->filename = filename;
    temp->next = NULL;
    return temp;
}

node add_node(node head, node new_node)
{
    node temp = head;
    if (head == NULL)
    {
        head = new_node;
    }
    else
    {
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_node;
    }
    return head;
}

node delete_list(node head)
{
    if (head == NULL)
    {
        return head;
    }
    node temp = head;
    while (head->next != NULL)
    {
        head = head->next;
        free(temp);
        temp = head;
    }
    free(head);
    head = NULL, temp = NULL;
    return head;
}

void print_list(node head)
{
    node temp = head;
    while (temp->next != NULL)
    {
        printf("Filename: %s\n", temp->filename);
        temp = temp->next;
    }
    printf("Filename: %s\n", temp->filename);
}