#include <stdio.h>
#include <stdlib.h>
#include "json.h"



int main(){

    char buffer[2560];
    scanf("%[^*]",buffer);
    remove_spaces(buffer);
    Object *root = parse(buffer);

    print_object(root,0);

    if(root != NULL){
        //print_object(root);
        free_object(root);
    }
    else{
        puts("Root was null");
    }


    //printf("%s\n",buffer);


    return 0;
}
