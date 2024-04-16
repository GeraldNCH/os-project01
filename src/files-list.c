#include <stdio.h>
#include <stdlib.h>
#include "files-list.h"

struct node *push_node(struct node *head, char *filename)
{
    printf("Inserting filepath: %s\n", filename);
    struct node *new = (struct node *)malloc(sizeof(struct node));
    new->filename = filename;
    new->next = head;
    return new;
}

void delete_list(struct node *head)
{
    if (head == NULL)
    {
        return;
    }
    struct node *temp = head;
    while (head->next != NULL)
    {
        head = head->next;
        free(temp);
        temp = head;
    }
    free(temp);
    head = NULL, temp = NULL;
}

void print_list(struct node *head)
{
    while (head != NULL)
    {
        printf("Filepath: %s\n", head->filename);
        head = head->next;
    }
}