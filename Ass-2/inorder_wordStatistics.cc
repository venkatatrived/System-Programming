#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <fstream>
#include <iostream>        
#include <queue>

#define buf_size 10
using namespace std;
struct wordNode
{
    string word;
    int count;
    struct wordNode* next;
};

struct wordNode* mapperbuffer[buf_size];
struct wordNode* reducerbuffer[buf_size];
struct wordNode* summarizerbuffer[buf_size];

void printWords(struct wordNode* head);
void concatWrdCnt(struct wordNode* head);
void concatLtrCnt(struct wordNode* head);
struct wordNode* newNode(string s);
void addWord(struct wordNode** head, string s);
void mapUpdtCnt(struct wordNode* head);
void wwordCnt(struct wordNode* head);

int maxbuf_size=buf_size;

pthread_mutex_t mapper_mutex    = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  mappool_full_cond   = PTHREAD_COND_INITIALIZER;
pthread_cond_t  mappool_empty_cond   = PTHREAD_COND_INITIALIZER;
int map_flag=0, mapbuf_sizefilled=0, mappool_in =0, mappool_out=0;

pthread_mutex_t reducer_mutex   = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  redpool_full_cond   = PTHREAD_COND_INITIALIZER;
pthread_cond_t  redpool_empty_cond   = PTHREAD_COND_INITIALIZER;
int red_flag=0, redbuf_sizefilled=0, redpool_out=0, redpool_in=0;

pthread_mutex_t wordcnt_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  sumpool_full_cond   = PTHREAD_COND_INITIALIZER;
pthread_cond_t  sumpool_empty_cond   = PTHREAD_COND_INITIALIZER;
int wordcnt_flag=0, sumbuf_sizefilled=0, sumpool_out=0, sumpool_in=0;

void *mapperPool_updtrt(void *fileName)
{
    string * filename;
    filename = (string*)fileName;
    string word;
    char prevChar = '\0'; 
    struct wordNode* head = NULL;
    ifstream fp;
    fp.open(*filename);
    if(fp.is_open())
    {
        while(!fp.eof())
        {
            fp >> word;
            //cout << word << endl;
            if(prevChar != '\0' && prevChar != word[0])
            {
                //cout << "reached here 1" << endl;
                pthread_mutex_lock( &mapper_mutex );
                while(mapbuf_sizefilled >= maxbuf_size)
                    pthread_cond_wait(&mappool_full_cond, &mapper_mutex);
                mapperbuffer[mappool_in] = head;
                mappool_in = (mappool_in +1)%maxbuf_size;
                mapbuf_sizefilled++;
                pthread_cond_signal(&mappool_empty_cond);
                pthread_mutex_unlock( &mapper_mutex );
                head = newNode(word);
                prevChar=word[0];
            }
            else
            {
                //cout << "reached here 2"<< endl;
                addWord(&head,word);
                prevChar=word[0];
            }
        }
        if(head != NULL)
        {
            pthread_mutex_lock( &mapper_mutex );
            while(mapbuf_sizefilled >= maxbuf_size)
                pthread_cond_wait(&mappool_full_cond, &mapper_mutex);
            mapperbuffer[mappool_in] = head;
            mappool_in = (mappool_in +1)%maxbuf_size;
            mapbuf_sizefilled++;
            map_flag = 1;
            pthread_cond_signal(&mappool_empty_cond);
            pthread_mutex_unlock( &mapper_mutex );
        }
    }
    fp.close();
    pthread_exit(NULL);
}

void *mappert(void *arg)
{
    int i = *((int *)arg);
    struct wordNode* node;
    while(1)
    {
        pthread_mutex_lock( &mapper_mutex );
        while(mapbuf_sizefilled == 0 && map_flag == 0)
        {
            pthread_cond_wait(&mappool_empty_cond, &mapper_mutex);         
        }
        if(mapbuf_sizefilled > 0 )
        {
            //cout << "reached inside mapool"<<endl;
            node = mapperbuffer[mappool_out];
            mappool_out = (mappool_out + 1)%maxbuf_size;
            mapbuf_sizefilled--;
        

            pthread_mutex_lock( &reducer_mutex );
            while(redbuf_sizefilled >= maxbuf_size)
                pthread_cond_wait(&redpool_full_cond, &reducer_mutex);
            //cout << "reached after mutex condition of reducer inside mapper"<<endl;
            mapUpdtCnt(node);
            reducerbuffer[redpool_in] = node;
            redpool_in = (redpool_in +1)%maxbuf_size;
            redbuf_sizefilled++;
            pthread_cond_signal(&redpool_empty_cond);
            pthread_mutex_unlock( &reducer_mutex );

            pthread_cond_signal(&mappool_full_cond);
            pthread_mutex_unlock( &mapper_mutex );
            //cout << i << endl;
            //printWords(node);
        }
        else if (map_flag == 1 )
        {
            map_flag = 2;
            //cout << "Mapper Exit" << endl;
            pthread_cond_broadcast(&mappool_empty_cond);
            pthread_mutex_unlock( &mapper_mutex );

            pthread_mutex_lock(&reducer_mutex);
            red_flag = 1;
            pthread_cond_signal(&redpool_empty_cond);
            pthread_mutex_unlock(&reducer_mutex);

            break;
        }
        else if(map_flag > 1)
        {
            pthread_mutex_unlock( &mapper_mutex );
            break;
        }
        else
            pthread_mutex_unlock( &mapper_mutex );

        sleep(1);
    }
    pthread_exit(NULL);
}

void* reducert(void *arg)
{
    int i = *((int *)arg);
    struct wordNode* node;
    //cout << i << endl;
    while(1)
    {
        pthread_mutex_lock( &reducer_mutex );
        while(redbuf_sizefilled == 0 && red_flag == 0)
        {
            pthread_cond_wait(&redpool_empty_cond, &reducer_mutex);         
        }
        if(redbuf_sizefilled > 0 )
        {
            //cout << "reached inside reducer"<<endl;
            node = reducerbuffer[redpool_out];
            concatWrdCnt(node);
            redpool_out = (redpool_out + 1)%maxbuf_size;
            redbuf_sizefilled--;

            pthread_mutex_lock( &wordcnt_mutex );
            while(sumbuf_sizefilled >= maxbuf_size)
                pthread_cond_wait(&sumpool_full_cond, &wordcnt_mutex);
            summarizerbuffer[sumpool_in] = node;
            //printWords(node);
            sumpool_in = (sumpool_in +1)%maxbuf_size;
            sumbuf_sizefilled++;
            pthread_cond_signal(&sumpool_empty_cond);
            pthread_mutex_unlock( &wordcnt_mutex );

            pthread_cond_signal(&redpool_full_cond);
            pthread_mutex_unlock( &reducer_mutex );
            //cout << i << endl;
            
        }
        else if (red_flag == 1 )
        {
            red_flag = 2;
            //cout << "Reducer Exit" << endl;
            pthread_cond_broadcast(&redpool_empty_cond);
            pthread_mutex_unlock( &reducer_mutex );

            pthread_mutex_lock(&wordcnt_mutex);
            wordcnt_flag=1;
            pthread_cond_signal(&sumpool_empty_cond);
            pthread_mutex_unlock(&wordcnt_mutex);

            break;
        }
        else if(red_flag > 1)
        {
            //cout << "Mapper Exiting" << endl;
            pthread_mutex_unlock( &reducer_mutex );
            break;
        }
        else 
            pthread_mutex_unlock( &reducer_mutex );
        sleep(1);
    }
    pthread_exit(NULL);
}

void* wordCountt(void *arg)
{
    struct wordNode* node;
    //cout <<"word Count thread " << endl;
    ifstream wordcnt("wordCount.txt");
    if(wordcnt)
    {
        system("rm -rf wordCount.txt");
        wordcnt.close();
    }

    while(1)
    {
        pthread_mutex_lock( &wordcnt_mutex );
        while( sumbuf_sizefilled == 0 && wordcnt_flag == 0)
        {
            pthread_cond_wait(&sumpool_empty_cond, &wordcnt_mutex);          
        }
        if(sumbuf_sizefilled > 0)
        {
            node = summarizerbuffer[sumpool_out];
            wwordCnt(node);
            sumpool_out = (sumpool_out + 1)%maxbuf_size;
            sumbuf_sizefilled--;
            pthread_cond_signal(&sumpool_full_cond);
            pthread_mutex_unlock( &wordcnt_mutex );
        }
        else if(wordcnt_flag == 1)
        {
            pthread_mutex_unlock( &wordcnt_mutex );
            break;
        }
        else
            pthread_mutex_unlock( &wordcnt_mutex );
    }
    pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
    if(argc != 4)
    {
        cout << "Some of the arguments are missing\n";
        return -1;
    } 

    string fileName = argv[1];
    pthread_t mapper[atoi(argv[2])];
    pthread_t reducer[atoi(argv[3])];
    pthread_t mapperpool_uptr, word_count, letter_count;
    int mpool_updtr, wrdcnt;
    int i =0, rc;

    // Checking if input file is accessible or not
    ifstream fp(argv[1]);
    if(!fp)
    {
        cout << "Unable to open input file" << endl;
        fp.close();
        exit(-1);
    }
    fp.close();

    mpool_updtr = pthread_create(&mapperpool_uptr, NULL, mapperPool_updtrt, &fileName); 

    if (mpool_updtr)
    {
        cout << "Error:unable to create Mapper pool update thread," << mpool_updtr << endl;
        exit(-1);
    }
    else
        cout << "Mapper pool Updater thread started" <<endl;

    for(i=0 ; i < atoi(argv[2]); i++ )
    {
        rc = pthread_create(&mapper[i], NULL, mappert, &i);
        if (rc)
        {
            cout << "Error:unable to create Mapper thread," << rc << endl;
            exit(-1);
        }
        else
            cout << "Mapper thread "<<(i+1)<< " started"<<endl;

    } 

    for(i=0 ; i < atoi(argv[3]); i++ )
    {
        rc = pthread_create(&reducer[i], NULL, reducert, &i);
        if (rc)
        {
            cout << "Error:unable to create Reducer thread," << rc << endl;
            exit(-1);
        }
        else 
            cout << "Reducer thread "<<(i+1)<< " started"<<endl;
    }

    wrdcnt = pthread_create(&word_count, NULL, wordCountt, NULL);
    if (wrdcnt)
    {
        cout << "Error:unable to create Word Count Writer update thread," << wrdcnt<< endl;
        exit(-1);
    }
    else
        cout << "Word Count Writer thread started" <<endl;

    pthread_exit(NULL);
}


void concatWrdCnt(struct wordNode* head)
{
    struct wordNode *node1 = head, *node2, *temp=NULL;
    while(node1 != NULL)
    {
        node2 = node1;
        while(node2->next != NULL)
        {
            temp = node2->next;
            if(node1->word == node2->next->word)
            {
                node1->count = node1->count+ node2->next->count;
                node2->next = node2->next->next;
                delete(temp); 
            }
            else 
                node2 = node2->next;
        }
        node1 = node1->next;
    }
}

void concatLtrCnt(struct wordNode* head)
{
    struct wordNode *node1 = head, *node2, *temp=NULL;
    node1->word = string(1,node1->word[0]);
    node2 = node1->next;
    node1->next = NULL;
    while(node2 != NULL)
    {
        node1->count += node2->count;
        temp = node2;
        node2 = node2->next;
        delete(temp);
    }
    printWords(node1);
}

struct wordNode* newNode(string s)
{
    struct wordNode* node = new wordNode();
    if(node == NULL)
        fprintf(stderr, "Unable to allocate memory for new node\n"); //exit(-1);
    node->word = s;
    node->count = 0;
    node->next = NULL;    
    return node;  
}


void addWord(struct wordNode** head, string s)
{
    if(*head == NULL)
    {
        //cout << "reached findWord"<<endl;
        struct wordNode* temp = new wordNode();
        temp->word = s;
        *head = temp;
        //cout << head->word << endl;
        return;  
    }
    struct wordNode* temp = *head;
    while(temp->next != NULL)
        temp = temp->next;
    temp->next= newNode(s);
}

void mapUpdtCnt(struct wordNode* head)
{
    if(head == NULL)
     {
        return;
     }
    head->count =1;
    //cout << "("<<head->word <<","<< head->count<<")"<< endl;
    mapUpdtCnt(head->next);
}

void wwordCnt(struct wordNode* head)
{
    if(head == NULL)
        return;

    ofstream fp;
    fp.open("wordCount.txt", ios_base::app);
    struct wordNode* current = head;
    while( current != NULL) 
    {
        fp << "("<<current->word <<","<< current->count<<")"<< endl;
        //cout << "("<<current->word <<","<< current->count<<")"<< endl;
        current = current->next;
    }
    fp.close();
}

void printWords(struct wordNode* head)
{
    if(head == NULL)
     {
        return;
     }
    cout << "hi ("<<head->word <<","<< head->count<<")"<< endl;
    printWords(head->next);
}