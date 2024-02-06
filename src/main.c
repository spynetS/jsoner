#include <stdio.h>
#include <stdlib.h>
#include "json.h"



int main(){

    char buffer[256];
    scanf("%[^*]",buffer);
    remove_spaces(buffer);
    Object *root = parse(buffer);

    if(root != NULL){
        print_object(root);
    }
    else{
        puts("Root was null");
    }

    free_object(root);

    printf("%s\n",buffer);


    return 0;
}
