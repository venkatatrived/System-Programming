#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

//Structure of a (key,value) pair node
struct keyNode
{
    char *key;
    int value;
    struct keyNode* next;
};


//Function to create a newNode
struct keyNode* newNode(char* key, int value)
{
    struct keyNode* node = (struct keyNode*)malloc(sizeof(struct keyNode));
    if(node == NULL)
        fprintf(stderr, "Unable to allocate memory for new node\n"); //exit(-1);
    node->key = strdup(key);
    node->value = value;
    node->next = NULL;    
    return node;  
}

//Update or Add keyNode to the list
void updateoraddkey(struct keyNode** head, char* key, int value)
{
    if(*head == NULL)
    {
        struct keyNode* temp = newNode(key,value);
        *head = temp;
        return;  
    }
    else if(strcmp((*head)->key, key) == 0)
    {
        (*head)->value = ((*head)->value)+value;
        return;
    }
    else if(strcmp((*head)->key, key) != 0 && (*head)->next == NULL)
    {
        (*head)->next = newNode(key, value);
        return;   
    }
    else
    {
       updateoraddkey(&((*head)->next), key, value);
    }
}

// Delete the list sent in the input
void deleteList(struct keyNode** head)
{
   struct keyNode* current = *head;
   struct keyNode* next;
 
   while (current != NULL) 
   {
       next = current->next;
       free(current);
       current = next;
   }
   
   *head = NULL;
}


//Prints all the key value Pairs in the list provided as input.
void printkeyvalues(struct keyNode* head)
{
    if(head == NULL)
        return;
    printf("(%s,%d)\n",head->key, head->value);
    printkeyvalues(head->next);
}

// Main Start point of the execution 
int main()
{
    char* inLine=NULL, *key=NULL, *cnt=NULL ;
    char prevChar='\0';
    int value, wordL, countL;
    struct keyNode* head = NULL;
    size_t size;
    while(getline(&inLine, &size, stdin) != -1)
    {
        // Check to see the first Letter of a word
        if((prevChar != '\0') && (prevChar != inLine[1]))
        {
            printkeyvalues(head);
            deleteList(&head);
        }

        // start bracket(SB) Position
        char *sB = strchr(inLine,'(');
        if( sB == NULL)
            return -1;

        // Comma Postion 
        char *comma = strstr(inLine,",");
        if(comma == NULL)
            return -1;
        // End bracket(Eb) Position
        char *eB = strchr(inLine,')');
        if(eB == NULL)
            return -1;

        // Length of key and value
        wordL = (int)(comma-sB);
        countL = (int)(eB-comma);

        // Extraction of key and value value.
        key = (char*)malloc(wordL * sizeof(char) ); 
        char *cnt = (char*)malloc(countL * sizeof(char) ); 
        strncpy(key,++sB,wordL-1);
        strncpy(cnt,++comma,countL-1);
        key[wordL-1] = '\0';
        cnt[countL-1]= '\0';

        //Conversion of cnt to integer and calling node update/add function
        value = atoi(cnt);
        updateoraddkey(&head,key,value);

        //Store the previous key first character.
        prevChar=inLine[1];
    }

    //Print Final set of (key,value) pairs        
    printkeyvalues(head);
    deleteList(&head);
}



