#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "json.h"

// if you tokenise this
// if we find
// string : string = property
// string : {

Token defined_tokens[] = {
    {"OS","{"},
    {"OC","}"},
    {"LS","["},
    {"LC","]"},
    {"SS","\""},
    {"SC","\""},
    {"SEP",":"},
    {"COM",","},
};
void syntax_error(){
    puts("JSON WRONGLY FORMATED");
    exit(1);
}

char* new_string(char *str){
    char *n = malloc(sizeof(char)*(strlen(str)+1));
    strcpy(n,str);
    return n;
}

void print_object(Object *object){
    if(object->type == OBJECT){
        puts("{");
    }else if(object->type == LIST){
        puts("[");
    }
    if(object->key != NULL)
        printf("%s",object->key);
    if(object->value != NULL)
        printf(":%s\n",object->value);

    for(int i = 0; i < object->object_count; i ++){
        if(object->objects[i] != NULL){
            if(object->type == LIST){
                printf("%d ",i);
            }
            print_object(object->objects[i]);
        }
    }
    if(object->type == OBJECT){
        puts("}");
    }else if(object->type == LIST){
        puts("]");
    }
}

// this function compiles json objects
//

Token *new_token(char *token, char *value){
    Token *new = malloc(sizeof(Token));
    new->token = token;
    new->value = value;
    return new;
}
char *getToken(char c){
    for(int i = 0; i < sizeof(defined_tokens)/sizeof(Token);i++){
        if(defined_tokens[i].value[0] == c){
            return defined_tokens[i].token;
        }
    }
    return NULL;
}
void free_token(Token *token){
    //printf("free %s\n",token->token);
    if(token->token != NULL) free(token->token);
    if(token->value != NULL) free(token->value);
    free(token);
}

int lex(Token ***target,char* buffer){
    Token **tokens = malloc(sizeof(Token*)*(strlen(buffer)+1));
    int tc = 0;
    int in_string = 0;
    for(int i = 0; i < strlen(buffer);i++){
        char c = buffer[i];

        char *found = getToken(c);
        if(found != NULL && strcmp(found,"SS") == 0)
            in_string = !in_string;
        if(found != NULL && ( !in_string || strcmp(found,"SS") == 0)){
            char *value = malloc(sizeof(char)*2);
            value[0] = c;
            value[1] = '\0';

            tokens[i] = new_token(new_string(found), value);
            tc++;
        }else{
            char *value = malloc(sizeof(char)*2);
            value[0] = c;
            value[1] = '\0';

            tokens[i] = new_token(new_string("CHAR"), value);
            tc++;
        }
    }

    for(int i = 0; i < tc; i ++){
        printf("%s\n", tokens[i]->token);
    }
    puts("----");
    *target = malloc(sizeof(Token*)*tc);
    memcpy(*target, tokens, sizeof(Token*)*tc);
    free(tokens);
    return tc;
}

int is_token(char *token){
    for(int i = 0; i < sizeof(defined_tokens)/sizeof(Token);i++){
        if(strcmp(defined_tokens[i].token,token)==0){
            return 1;
        }
    }
    return 0;
}

int parse_values(Token ***target, Token **src, int len){
    Token **token_buffer = malloc(sizeof(Token*)*len);
    int tbc = 0;

    for(int i =0 ; i < len; i ++){
        token_buffer[tbc] = src[i];
        tbc++;
        //find value
        if(!strcmp(src[i]->token,"SEP")){
    int count = 1;
    char buf[100];
            int reading_str = (strcmp(src[i+1]->token,"SS") == 0) ? 1 : 0;

            if(reading_str){
                free_token(src[i+count]);
                while( strcmp(src[i+count+1]->token,"SS") != 0 ){
                    buf[count-1] = src[i+count+1]->value[0];
                    free_token(src[i+count+1]);
                    count++;
                }
                i++;
                free_token(src[i+count]);

            }
            else{

                while(!is_token(src[i+count]->token)){
                    buf[count -1] = src[i+count]->value[0];
                    free_token(src[i+count]);
                    count++;
                }
                count --;
            }
            if(count > 1){
                buf[count-1] = '\0';
                token_buffer[tbc] = new_token(new_string("STR"),new_string(buf));
                tbc++;
                i += count;
            }
        }
        // find key
        else if(strcmp(src[i]->token,"SS") == 0){
            int count = 0;
            char buf[100];
            free_token(src[i]);
            tbc--;
            while( strcmp(src[i+count+1]->token,"SS") != 0 ){
                buf[count] = src[i+count+1]->value[0];
                free_token(src[i+count+1]);
                count++;
            }
            i++;
            free_token(src[i+count]);

            if(count > 1){
                buf[count] = '\0';

                token_buffer[tbc] = new_token(new_string("KEY"),new_string(buf));
                tbc++;
                i+= count;
            }
        }
    }
    *target = realloc(*target, sizeof(Token*)*tbc);
    memcpy(*target, token_buffer, sizeof(Token*)*tbc);
    free(token_buffer);
    return tbc;
}

Object *parse(char *buffer){
    Token **tokens;
    int len = lex(&tokens,buffer);
    len = parse_values(&tokens,tokens,len);
    for(int i = 0; i < len; i ++){
        printf("%s\n", tokens[i]->token);
    }
    puts("<----->");
    Stack *stack = malloc(sizeof(Stack));
    stack->count = 0;

    char curr_key[100];
    for(int i = 0; i < len; i ++){
        Token *t = tokens[i];
        if(strcmp(t->token,"OS")==0){
            puts("Open object");
            Object *n = new_object(OBJECT);
            push(stack,n);
        }
        if(strcmp(t->token,"OC")==0){
            puts("close object");
            if(stack->count > 1){
                Object *p = pop(stack);
                add_object(peek(stack), p);
            }else{
                goto end;
            }
        }
        if(strcmp(t->token,"LS")==0){
            puts("open list");
            Object *n = new_object(LIST);
            push(stack,n);
        }
        if(strcmp(t->token,"LC")==0){
            puts("close list");
            if(stack->count > 1){
                Object *p = pop(stack);
                add_object(peek(stack), p);
            }else{
                goto end;
            }
        }

        if(strcmp(t->token,"KEY")==0){
            if(strcmp(tokens[i+1]->token, "SEP") == 0){
                strcpy(curr_key, t->value);
                if( strcmp(tokens[i+2]->token, "STR") == 0 ){
                    Object *n = new_object(STRING);
                    n->key = new_string(curr_key);
                    n->value = new_string(tokens[i+2]->value);
                    add_object(peek(stack),n);
                }
                else if( strcmp(tokens[i+2]->token, "OS") == 0 ||
                         strcmp(tokens[i+2]->token, "LS") == 0 ){
                }
                else{
                    syntax_error();
                }
            }
            else{
                syntax_error();
            }
        }


    }


end:
    for(int i = 0; i < len; i ++){
        printf("%s,", tokens[i]->token);
        free_token(tokens[i]);
    }
    free(tokens);
    puts("|------|");

    Object *p = pop(stack);
    free(stack);
    return p;
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
    stack->count -= 1;

    stack->node = stack->node->next;
    free(node);
    return data;
}

void push(Stack *stack, void *data){
    Node *new = malloc(sizeof(Node));
    new->data = data;
    new->next = stack->node;
    stack->node = new;
    stack->count += 1;
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
{"users":[{"name":"alfred, allerller"},{"name":"ali","age":30}]}

*/
