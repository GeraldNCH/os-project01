struct files_list;

typedef struct files_list *node;

node create_node(char *filename);

node add_node(node head, node new_node);

node delete_list(node head);

void print_list(node head);