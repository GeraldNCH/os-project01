struct node
{
    char *filename;
    struct node *next;
};

struct node *push_node(struct node *head, char *filename);

void delete_list(struct node *head);

void print_list(struct node *head);