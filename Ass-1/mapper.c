#include <stdio.h>
//#include <stdlib.h>

int main(int argc, char *argv[])
{
    //check for arguments
    if(argc < 2)
    {
        printf("Input fileName argument is missing\n");
        return -1;
    }    

    char word[100];
    FILE *fp = fopen(argv[1], "r");

    if(fp)
    {
        while(fscanf(fp, "%s", word) !=EOF)
            printf("(%s,1) \n",word);
    }
    else 
        perror("Not able to open the given input file");

    fclose(fp);
    return 0;
}