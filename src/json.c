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

void print_object(Object *object, int depth){
    for(int i = 0; i < depth; i ++){
        printf("\t");
    }
    if(object->key != NULL)
        printf("%s:",object->key);
    if(object->type == OBJECT){
        puts("{");
    }else if(object->type == LIST){
        puts("[");
    }
    if(object->value != NULL)
        printf("%s\n",object->value);

    for(int i = 0; i < object->object_count; i ++){
        if(object->objects[i] != NULL){
            print_object(object->objects[i],depth+1);
        }
    }
    for(int i = 0; i < depth-1; i ++){
        printf("\t");
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
        printf("%s,", tokens[i]->token);
    }
    /* puts("----"); */
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

    int read_val = 0;
    for(int i =0 ; i < len; i ++){
        Token *t = src[i];
        printf("%s ",t->token);

        if(strcmp(t->token,"CHAR")==0){
            printf("found chars, ");
            //string literal before
            if(strcmp(src[i-1]->token,"SS")==0){
                //remove "
                free_token(token_buffer[tbc-1]);
                tbc--;

                int count = 0;
                char buf[100];
                while(strcmp(src[i]->token, "SS") !=0){
                    buf[count] = src[i]->value[0];
                    free_token(src[i]);
                    count++;
                    i++;
                }
                free_token(src[i]);

                buf[count] = '\0';
                char temp[count];
                sprintf(temp,"\"%s\"",buf); // add " to the value
                char token[] = "STR";
                if(strcmp(src[i+1]->token, "SEP")==0){
                    token[0] = 'K';
                    token[1] = 'E';
                    token[2] = 'Y';
                }
                token_buffer[tbc] = new_token(new_string(token), new_string(temp));
                tbc++;

            }
            else{
        int count = 0;
                char buf[100];
                while(!is_token(src[i]->token)){
                    buf[count] = src[i]->value[0];
                    free_token(src[i]);
                    count++;
                    i++;
                }
                //free_token(src[i]);
                i--;

                buf[count] = '\0';
                token_buffer[tbc] = new_token(new_string("STR"), new_string(buf));
                tbc++;
            }
        }
        else{
            printf("%s added at %d\n",t->token, tbc);
            token_buffer[tbc] = t;
            tbc++;
        }
        puts("");
    }
    puts("PARSE_VALUE ____ END _____");
    *target = realloc(*target, sizeof(Token*)*tbc);
    memcpy(*target, token_buffer, sizeof(Token*)*tbc);
    free(token_buffer);
    return tbc;
}

char *get_grammer(char *grammer[10][5], char *word){
    for(int i = 0 ; i < 6;i ++){
        int j = 0;
        while(grammer[i][j] != NULL){

            j++;
        }
        /* if(strcmp(grammer[i][1],word)==0){ */
        /*     printf("%s\n",grammer[i][0]); */
        /*     return grammer[i][0]; */
        /* } */
    }
    printf("NO FOUND\n");
    return NULL;
}

Object *parse(char *buffer){
    Token **tokens;
    int len = lex(&tokens,buffer);
    len = parse_values(&tokens,tokens,len);
    for(int i = 0; i < len; i ++){
        if(tokens[i] != NULL)
            printf("%s\n", tokens[i]->token);
        else
            printf("NULL\n");
    }
    puts("<----->");
    Stack *stack = malloc(sizeof(Stack));
    stack->count = 0;
    //the gramer list, the last string
    //is the strong to replace all the
    //string with
    char *seq[10][5] = {
        {"OS","OS"},
        {"LS","LS"},
        {"OC","OC"},
        {"LC","LC"},
        {"AV","STR","COM"},
        {"AV","STR"},
        {"AKV","KEY","SEP","STR","COM"},
        {"AKV","KEY","SEP","STR"},
        {"AKO","KEY","SEP","OS"},
        {"AKL","KEY","SEP","LS"},
    };

    char curr_key[100];
    char new[200];
    for(int i = 0; i < len; i ++){
        Token *t = tokens[i];
        // if token in grammer
        // check if next word maches gramer
        char got[4] = {'\0'};
        for(int j = 0 ; j < 8;j ++){
            int k = 0;
            while(seq[j][k+1] != NULL){
                if(i+k < len){
                    printf("%s,",tokens[i+k]->token);
                    if(strcmp(seq[j][k+1],tokens[i+k]->token) != 0){
                        printf(" last not eq %s %s ", seq[j][k+1], tokens[i+k]->token);
                        goto notfound;
                    }
                    k++;
                }else
                    goto notfound;
            }
            i+=k;
            strcpy(got,seq[j][0]);
            puts("addded");
            break;
        notfound:
            puts("not found");
            continue;
        }
        sprintf(new,"%s,%s",new,got);

    }
    puts(new);


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
