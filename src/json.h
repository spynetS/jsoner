#ifndef JSON_H_
#define JSON_H_

#define OBJECT      0
#define LIST        1
#define STRING      2
#define START_SIZE 10

typedef struct object{
    int type; // 0 object, 1 list
    char* key;
    char* value; // if the object is a propertie
    struct object **objects;
    int object_count;

}Object;

typedef struct token{
    char *token;
    char *value;
} Token;

Object *new_object(int type);
void add_object(Object* object, Object* element);
void free_object(Object *object);
void print_object(Object *object,int depth);

Object *parse(char *buffer);
void remove_spaces(char *buffer);

// sTACK

typedef struct node{
    void *data;
    struct node *next;
} Node;

typedef struct stack{
    Node *node;
    int count;
} Stack;

void *peek(Stack *stack);
void *pop (Stack *stack);
void  push(Stack *stack, void *data);


#endif // JSON_H_
