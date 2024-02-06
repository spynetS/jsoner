#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "json.h"

// if you tokenise this
// if we find
// string : string = property
// string : {


void print_object(Object *object){
    if(object->key != NULL)
        printf("Key %s",object->key);
    if(object->value != NULL)
        printf(" > %s",object->value);
    puts("");

    for(int i = 0; i < object->object_count; i ++){
        if(object->objects[i] != NULL){
            if(object->type == LIST){
                printf("%d ",i);
            }
            print_object(object->objects[i]);
        }
    }
}

Object *parse(char *buffer){
    char oc = '{';
    char cc = '}';
    char ol = '[';
    char cl = ']';
    Stack *stack = malloc(sizeof(Stack));
    int reading_key = 0;
    char keybuffer[100];

    int reading_value = 0;
    char valuebuffer[100];
    int reading_list = 0;

    for(int i = 0; i < strlen(buffer); i ++){
        char c = buffer[i];
        if(c == ol){
            reading_list = 1;
            Object *new = new_object(LIST);
            printf("keyu %s\n", keybuffer);
            if(strlen(keybuffer) > 0){
               char* key = malloc(sizeof(char)*(strlen(keybuffer)+1));
               strcpy(key,keybuffer);
               new->key = key;
               memset(keybuffer,'\0',sizeof(keybuffer));

            }
            reading_value = 1;
            push(stack,new);
            continue;
        }
        if(c == cl){
            reading_list = 0;
            Object *poped = pop(stack);
            if(peek(stack) != NULL){
                add_object(peek(stack), poped);
            }
            else {
                free(stack);
                return poped;
            }
        }
        if(c == oc){
            reading_list = 0;
            Object *new = new_object(OBJECT);
            if(strlen(keybuffer) > 0){
               char* key = malloc(sizeof(char)*(strlen(keybuffer)+1));
               strcpy(key,keybuffer);
               new->key = key;
               memset(keybuffer,'\0',sizeof(keybuffer));

            }
            reading_value = 0;
            push(stack,new);
        }
        if(c == cc){
            Object *poped = pop(stack);
            if(peek(stack) != NULL){
                add_object(peek(stack), poped);
            }
            else {
                free(stack);
                return poped;
            }
        }
        if(!reading_list){
            // if we find a " and our key
            // is empty we should now read key
            if(c == '"' && strlen(keybuffer) == 0 && !reading_value) reading_key = 1;
            // if we see " and we have read key
            // we should read value
            else if(c == ':' && strlen(keybuffer) > 0 && reading_value == 0 ){
                reading_value = 1;
                reading_key = 0;
                continue;
            }
            // if we see " and we are reading value
            // we should stop reading and add
            else if((c == ',' || c==cc || c == cl) && reading_value == 1){
                Object *string = new_object(STRING);

                char *tmp = malloc(sizeof(char) * (strlen(valuebuffer)+1));
                strcpy(tmp, valuebuffer);
                sprintf(tmp,"%s",tmp); // add the "
                string->value = tmp;

                char *key = malloc(sizeof(char) * (strlen(keybuffer)+1));
                strcpy(key,keybuffer);
                string->key = key;

                add_object(((Object*)peek(stack)), string);

                printf("new P %s\n",((Object*)peek(stack))->objects[0]->key);

                // reset our buffers
                memset(keybuffer,'\0',sizeof(keybuffer));
                memset(valuebuffer,'\0',sizeof(valuebuffer));
                reading_value = 0;
                reading_key = 0;
            }

            if(reading_key){
                // add char to string keybuffer
                sprintf(keybuffer,"%s%c",keybuffer,c);
            }
        }
        else{
            if((c == ',' || c==cc || c == cl) && reading_value == 1){
                Object *string = new_object(STRING);
                char *tmp = malloc(sizeof(char) * (strlen(valuebuffer)+1));
                strcpy(tmp, valuebuffer);
                sprintf(tmp,"%s",tmp); // add the "
                string->value = tmp;

                add_object(((Object*)peek(stack)), string);

                printf("new P %s\n",((Object*)peek(stack))->objects[0]->key);
                // reset our buffers
                memset(valuebuffer,'\0',sizeof(valuebuffer));
                continue;
            }

        }
        if(reading_value){
            // add char to string keybuffer
            sprintf(valuebuffer,"%s%c",valuebuffer,c);
        }
        //log
        printf("read rk %d, rv %d, %c\n", reading_key, reading_value,c);
    }
    free(stack);
    return pop(stack);
}


Object *new_object(int type){
    Object *new = malloc(sizeof(Object));
    new->type = type;
    new->key = NULL;
    new->value = NULL;
    new->object_count = 0;
    new->objects = malloc(sizeof(Object*)*START_SIZE);
    return new;
}
void add_object(Object* object, Object* element){
    object->objects[object->object_count] = element;
    object->object_count += 1;
}

void free_object(Object *object){
    for (int i = 0 ; i < object->object_count; i++) {
        if(object->objects[i] != NULL){
            free_object(object->objects[i]);
        }
    }
    if(object->key != NULL){
        free(object->key);
    }
    if(object->value != NULL){
        free(object->value);
    }
    free(object->objects);
    free(object);
    object = NULL;// dont think this matters
}

void remove_spaces(char *buffer){
    char *buffer_no_space = malloc(sizeof(char)*(strlen(buffer)+1));
    int count = 0;
    int in_string = 0;
    for(int i = 0; i < strlen(buffer); i ++){
        if(buffer[i] == '"') in_string = !in_string;

        if(in_string || (!in_string && buffer[i] != '\n' &&
            buffer[i] != ' ')){
            buffer_no_space[count] = buffer[i];
            count ++;
        }

    }
    strcpy(buffer,buffer_no_space);
    free(buffer_no_space);
}

void *peek(Stack *stack){
    Node *node = stack->node;
    if(node != NULL)
        return node->data;
    else return NULL;
}

void *pop(Stack *stack){
    Node *node = stack->node;
    if(node == NULL) return NULL;

    void *data = node->data;

    stack->node = stack->node->next;
    free(node);
    return data;
}

void push(Stack *stack, void *data){
    Node *new = malloc(sizeof(Node));
    new->data = data;
    new->next = stack->node;
    stack->node = new;
}

/*

    "users":[
        {
            "name": "Alfred Roos",
            "age": 10
        },
        {
            "name": "Alfred",
            "age": 10
        }
    ]

*/
