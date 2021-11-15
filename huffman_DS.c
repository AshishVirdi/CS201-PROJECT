//                      CS-201 PROJECT
//   Compression and decompression of files like .bmp(uncompressed image) ,.yuv(raw data no header needed) - for videos etc
//    By Huffman encoding and decoding.
// SUBMISSION BY -> ASHISH VIRDI(2020CSB1077) && AVNISH KUMAR(2020CSB1078)

#include <stdio.h>
#include <stdlib.h> //used for malloc
#define INT_MAX 1000000000 // value used in cases to represent an impossible outcome.


/***************************************************************************/
//DECLARING HUFFMAN TREE NODE STRUCTURE AND FUNTCION TO ALLOCATE IT MEMORY//
//    huffman_node structure is used as min priority queue structure    // 
/***************************************************************************/
typedef struct Huffman_node // Declaring structure of Huffman_node and typedefining it to Huff_Node.
{
    char data;
    int frequency;
    struct Huffman_node* left;
    struct Huffman_node* right;
    struct Huffman_node* next;
} Huff_Node;

Huff_Node* newHuff_Node(char data, int count)    // allocating memory to new huffman node.
{
    Huff_Node* temp = (Huff_Node*)malloc(sizeof(Huff_Node));
    temp->data = data;
    temp->frequency = count;
    Huff_Node* left = NULL;
    Huff_Node* right = NULL;
    Huff_Node* next = NULL;
    return temp;
}

Huff_Node *pq=NULL;                     // root/head node of priority queue. 
Huff_Node *huff_root = NULL;            // root node of huffman tree.
long int buffer_capacity = INT_MAX;     // stores length of input file.
long int encoded_length = 0;            // stores length of encoded file.


/************************************************************/
// READING SOURCE FILE AND WRITING TARGET FILE //
/************************************************************/
char *read_FILE(const char *filename){       // read buffer from input file

    FILE *input = NULL;
    if((input = fopen(filename,"rb")) == NULL){
       printf("Error! opening file");
       // Program exits if the file pointer returns NULL.
       exit(1);
    }
    fseek(input, 0, SEEK_END);                 // go to the end of file.
    buffer_capacity = ftell(input);            // find the position of file pointer to determine the length of file.
    rewind(input);
    char *input_buffer = (char*)malloc(buffer_capacity);  //allocate memory to the buffer equal to length of file.

    if(input_buffer==NULL) { 
        printf("Error! Cannot allocate memory to the buffer\n");
        exit(1);
    }
    if(fread(input_buffer, 1, buffer_capacity, input) != buffer_capacity) { // reading whole file into the input buffer.
        printf("Error! Cannot read the file\n");
        exit(1);
    }
    fclose(input);
    return input_buffer;
};


void write_FILE(char* buffer,const char *Targetfile){     // writes buffer to target/output file
    FILE *output = NULL;
    if ((output = fopen(Targetfile,"wb")) == NULL){
       printf("Error! opening file");
       // Program exits if the file pointer returns NULL.
       exit(1);
    }
    fwrite(buffer,1,sizeof(buffer),output);  // writing the output/encoded buffer into the target file. 
    encoded_length = ftell(output);
    free(buffer);
    fclose(output);
    printf("FILE CREATED SUCCESSFULLY!!!");
};



/************************************************************/
// MIN PRIORITY QUEUE REPRESENTATION USING LINKED LIST //
// IN INCREASING ORDER OF PRIORITY/FREQUENCY //
/************************************************************/
Huff_Node* PQ_MIN(Huff_Node** head){ return (*head); }    // used to get the minimum node.

void PQ_REMOVE(Huff_Node** head)                      // used to remove the minimum node.
{
    Huff_Node* temp = *head;
    (*head) = (*head)->next;
    temp->next = NULL;
}

void PQ_INSERT(Huff_Node** head, Huff_Node** node)   //used to insert a node into min priority queue.
{
    if((*head)==NULL){
        (*head) = (*node);
    }
    Huff_Node* begin = (*head);
    if ((*head)->frequency > (*node)->frequency) {
        (*node)->next = *head;
        (*head) = (*node);
    }
    else {
        while (begin->next != NULL && begin->next->frequency < (*node)->frequency)
        {
            begin = begin->next;
        }
        (*node)->next = begin->next;
        begin->next = (*node);
    }
}

int PQ_isEMPTY(Huff_Node** head){ return (*head) == NULL; }   // checks if priority queue is empty.

int PQ_size(Huff_Node** head){              // used to count the number of characters present in out priority queue.
    int size = 0;
    Huff_Node* begin = (*head);
    while (begin != NULL)
    {
        size++;
        begin = begin->next;
    }
    return size;
}

void printPQ(Huff_Node* head){                   // used for printing priority queue.
    printf("PRINTING PRIORITY QUEUE\n");
    while(head != NULL){
        printf("%c : %d\n", head->data, head->frequency);
        head = head->next;
    }
}

/****************************************************************************/
// BUILDING HUFFMAN TREE USING THE PRIORITY QUEUES AND UPDATING THE FREQUECNY
/****************************************************************************/
void  Build_Huffmantree(Huff_Node** head,Huff_Node** root){  // function to build huffman tree by choosing 2 minimum nodes and adding them into new parent node.
      Huff_Node* newhead;
      Huff_Node* min1=NULL,*min2=NULL;
      int n = PQ_size(&pq);
      printf("size of priority queue -> %d\n",n);
      printf("\n\n");
      for(int i = 1 ;i <= n-2; i++)    // n-1 comparisons are required in total to form huffman tree from min priority queue.
      {
      min1 = PQ_MIN(head);
      PQ_REMOVE(head);
      min2 = PQ_MIN(head);
      PQ_REMOVE(head);
      newhead = newHuff_Node('$',min1->frequency + min2->frequency);
      newhead->left = min1;
      newhead->right = min2;
      PQ_INSERT(&pq,&newhead);
      printPQ(pq);
      }
      min1 = PQ_MIN(head);
      PQ_REMOVE(head);
      min2 = PQ_MIN(head);
      PQ_REMOVE(head);
      (*root) = newHuff_Node('$',min1->frequency + min2->frequency); //root node is set for the huffman tree.
      (*root)->left = min1;
      (*root)->right = min2;
      printf("ROOT NODE FOR HUFFMAN TREE IS :\n");
      printf("%c : %d\n",(*root)->data ,(*root)->frequency);
}

/********************************************************************/
// Counter - to count frequency of unique characters present in file//
/********************************************************************/
void freq_counter(char* input_buffer,int *ascii,int *char_order);


/********************************************************************/
// ENCODING SOURCE BUFFER TO .TXT //
/********************************************************************/
void addto_encoding_buffer(int encoding[], int depth);

void form_encodings(Huff_Node *root,int encoding[], int depth);


/********************************************************************/
// DECODING .TXT TO TARGET FILE //
/********************************************************************/
void decoding(char* encoded_buffer);


/********************************************************************/
// ENCODING SOURCE BUFFER TO .TXT AND DECODING .TXT TO TARGET FILE //
/********************************************************************/
void ENCODE(char* input_buffer,const char *Encodedfile){
    int ascii[255]={0};                  //stores frequency of ascii characters.
    int char_order[255]={-1};            //stores the order of appearance of characters.
    freq_counter(input_buffer,ascii,char_order);  //function to count the frequency of characters in increasing order of appearance.

    Build_Huffmantree(&pq,&huff_root);  // Building Huffman tree and assigning huff_root as its root node.  
    Huff_Node *temp = huff_root;
    int encoding[buffer_capacity+1];
    int depth = 0;
    //form_encodings(huff_root,encoding, level);
    // write_FILE(encoded_buffer,Encodedfile);
    // printf("Compression Rate is %d\n", (buffer_capacity/encoded_length)*100);
};


// void DECODE(char* encoded_buffer,const char *Targetfile){
//     decoding(encoded_buffer);
//     write_FILE(decoded_buffer,Targetfile);
// };


/************************************************************/
// Main function -> for inputting user defined inputs //
/************************************************************/
int main()
{
    char filename[30];
    printf("Kindly Enter the name of the file you wish to encode/compress.. \n");
    scanf(" %s",filename);       // input file

    char Encodedfile[30];
    printf("Kindly Enter the destination path/name of encoded file \n");
    scanf(" %s", Encodedfile);     // encoded file name.

    char *input_buffer = read_FILE(filename);
    printf("BUFFER READ FROM FILE IS GIVEN AS : ");
    for(int i=0;i<buffer_capacity;i++){         // Displaying input read from given file.
      printf("%c",input_buffer[i]);
    }
    printf("\n");

    ENCODE(input_buffer,Encodedfile);  // inside encode we call write_FILE(encoded_buffer,Encodedfile);

    char Targetfile[30];
    printf("Kindly Enter the output file path/name \n");
    scanf(" %s",Targetfile);        // target file

    // char *encoded_buffer = read_FILE(Encodedfile);
    // DECODE(encoded_buffer,Targetfile);  inside decode we call write_FILE(decoded_buffer,Targetfile);

    return 0;
}
