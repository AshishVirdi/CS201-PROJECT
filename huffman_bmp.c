//                      CS-201 PROJECT
//   Compression and decompression of files like .bmp(uncompressed image).
//    By Huffman encoding and decoding.
// SUBMISSION BY -> ASHISH VIRDI(2020CSB1077) && AVNISH KUMAR(2020CSB1078)

#include <stdio.h>
#include <stdlib.h> //used for malloc
#include <string.h>  //for using dynamic sized strings/unsigned char arrays
#include <dirent.h>  // used to read files inside our uncompressed bmp images folder.
#define INT_MAX 1000000000 // value used in cases to represent an impossible outcome.


/***************************************************************************/
//DECLARING HUFFMAN TREE NODE STRUCTURE AND FUNTCION TO ALLOCATE IT MEMORY //
//    huffman_node structure is used as min priority queue structure       //
/***************************************************************************/
typedef struct Huffman_node             // Declaring structure of Huffman_node and typedefining it to Huff_Node.
{
    unsigned int data;
    struct Huffman_node* left;
    struct Huffman_node* right;
    struct Huffman_node* parent;
} Huff_Node;


Huff_Node* newHuff_Node(unsigned int data,unsigned int freq)    // allocating memory to new huffman node.
{
    Huff_Node* temp = (Huff_Node*)malloc(sizeof(Huff_Node));
    temp->data = data;
    Huff_Node* left = NULL;
    Huff_Node* right = NULL;
    Huff_Node* parent = NULL;
    return temp;
}

void swapnodes(Huff_Node** node1, Huff_Node** node2)
{
    Huff_Node* temp = (*node1);
    (*node1) = (*node2);
    (*node2) = temp;
}

void swap(unsigned int *a,unsigned int *b)
{
    unsigned int temp = (*a);
    (*a) = (*b);
    (*b) = temp;
}

unsigned int inputbitlength=0;
/************************************************************/
//           BMP HEADER STRUCTURE                           //
/************************************************************/
struct BITMAP_header{
  char name[2];
  unsigned int size;
  int garbage;
  unsigned int image_offset;
};

struct DIB_header{
    unsigned int header_size;
    unsigned int width;
    unsigned int height;
    unsigned short int colorplanes;
    unsigned short int bitsperpixel;
    unsigned int compression;
    unsigned int image_size;
    unsigned int temp[4];
};

/*************************************************************************/
// READING SOURCE BMP FILE AND DISPLAYING ITS HEADER CONTENTS/ATTRIBUTES //
/*************************************************************************/
void read_header(const char *filename){       // read and display header from input file

    FILE *input = fopen(filename,"rb");
    if(input == NULL){
       printf("Error! opening file");
       // Program exits if the file pointer returns NULL.
       exit(1);
    }
    fseek(input, 0, SEEK_END);                 // go to the end of file.
    inputbitlength = ftell(input);            // find the position of file pointer to determine the length of file.
    printf("Total input bits length of file -> %d\n", inputbitlength);
    rewind(input);

    struct BITMAP_header header;
    struct DIB_header dibheader;

    fread(header.name,2,1,input);
    fread(&header.size,3*sizeof(int),1,input);

    printf("First two characters:%c%c\n",header.name[0],header.name[1]);
    printf("Size:%d\n",header.size);
    printf("Offset:%d\n",header.image_offset);
    fread(&dibheader,sizeof(struct DIB_header),1,input);
    printf("Header size:%d\nWidth:%d\nHeight:%d\nColor planes:%d\nBits per pixel:%d\nCompression:%d\nImage size:%d\n\n",
    dibheader.header_size,dibheader.width,dibheader.height,dibheader.colorplanes,dibheader.bitsperpixel,dibheader.compression,dibheader.image_size);
    fclose(input);
};


/********************************************************************/
// Counter - to count frequency of unique characters present in file//
/********************************************************************/
void freq_counter(FILE* fpr, unsigned int* ascii){

    unsigned long int begin = ftell(fpr);
    int character;
    while(fpr){
        character = fgetc(fpr);  // fgetc returns negative value when EOF is reached.
        if(character < 0 || character > 255) { break; }
        ascii[character]++;
    }
    rewind(fpr);
}

/****************************************************************************/
// BUILDING HUFFMAN TREE USING THE PRIORITY QUEUES AND UPDATING THE FREQUECNY
/****************************************************************************/
Huff_Node *Build_Huffmantree(unsigned int* ascii, Huff_Node* leafnodes) {

    unsigned int freq[256] = {0};
    Huff_Node *node[256];
    Huff_Node helper[256];           // acts a auxiliary array to store the parents after every last two minimum leafnodes are extracted. //
                                     //   2 min nodes can be extracted at maximum 256-1 times as 256 will only require 255 comparisons.   //

    for (int i = 0; i < 256; i++)
    {
        freq[i] = ascii[i];
        leafnodes[i].data = i;
        leafnodes[i].left = NULL;
        leafnodes[i].right = NULL;
        leafnodes[i].parent = NULL;
        node[i] = &leafnodes[i];
    }

    for (int i = 0; i < 256; i++)
    {
        helper[i].data = i;
        helper[i].left = NULL;
        helper[i].right = NULL;
        helper[i].parent = NULL;
    }

    unsigned int i, j;
    unsigned int min_idx;
    for (i = 0; i < 255; i++) {
        min_idx = i;
        for (j = i + 1; j < 256; j++)
        {
            if(freq[j] < freq[min_idx]){min_idx = j;}
        }
        swap(&freq[min_idx], &freq[i]);
        swapnodes(&node[min_idx],&node[i]);
        }

    int chars = 0;
    while(chars < 255)
    {
        Huff_Node *parent = &helper[chars];
        int i = chars+1, j = chars;

        parent->left = node[j];
        parent->right = node[i];
        node[j]->parent = parent;
        node[i]->parent = parent;
        node[i] = parent;
        freq[i] += freq[j];

        while(i<255) // works like enqueue in min heap/ priority queue.
        {
                if (freq[i] > freq[i+1])
                {
                    swapnodes(&node[i],&node[i+1]);
                    swap(&freq[i],&freq[i+1]);
                }
                i++;
        }
        chars++;
    }
    node[255]->parent = NULL;
    return node[255];
}


/********************************************************************/
// ENCODING SOURCE BUFFER TO .TXT/.bmp                              //
/********************************************************************/
void form_encodings(Huff_Node *head,unsigned char* encodings,int *height)
{
    // ENCODING IS DONE FROM BOTTOM TO TOP/ROOT.
    // encodings array stores the path of current character form the root node.
        while(head->parent!=NULL)
        {
            if(head == head->parent->left) {
                encodings[(*height)] = 0;
            }
            else if (head == head->parent->right) {
                encodings[(*height)] = 1;
            }
            head = head->parent;
            (*height)++;
        }
}

void write_encodings(FILE* fpw,unsigned char* encodings,int height,unsigned int *buffer,int *buffersize)
{
    /**************************************************************************
    OUR TARGET IS TO WRITE OUR ENCODING OBTAINED FROM form_encodings()
    FOR A CHARATER READ FROM THE FILE.
    SUPPOSE ENCODING FOR A CHARACTER IS ENCODING[] = 1010, HEIGHT = 4
    WE NEED TO WRITE ITS ASCII i.e. '�' INTO THE ENCODED FILE
    INITIAL BUFFER - 00000000
    BUFFERSIZE - 0
    TO WRITE THE FIRST BIT OF ENCODING[] WE NEED TO PERFORM OR OPERATION -
    00000000 | 00000001          // WE NEED TO LEFT SHIFT ENCODING[HEIGHT-1]
    BUFFER - 00000001
    BUFFERSIZE - 1
    00000001 | 00000000
    BUFFER - 00000001
    BUFFERSIZE - 2
    00000001 | 00000100
    BUFFER - 00000101
    BUFFERSIZE - 3
    00000101 | 00000000
    BUFFER - 00000101
    BUFFERSIZE - 4

    WE WILL WRITE THE BUFFER INTO THE FILE WHEN THE BUFER SIZE BECOMES 8 BIT
    WE CAN WRTIE IT INTO THE FILE AS AN UNSIGNED CHARACTER (0-255).

    **************************************************************************/
        for(int i = height-1; i >= 0; i--)     // SINCE WE ARE FORMING THE ENCODINGS FROM BOTTOM TO TOP OF TREE WE NEED TO READ FROM TOP TO BOTTOM
        {
            (*buffer) |= encodings[i] << (*buffersize);
            (*buffersize)++;

            if((*buffersize) == 8){    // WHEN SIZE OF BUFFER EQUALS TO 8 WE WRITE THE BUFFER AS UNSIGNED CHAR (0-255) INTO THE FILE.
                fputc((*buffer), fpw);
                (*buffersize) = 0;     // AFTER A BUFFER IS WRITTEN INTO FILE WE RESET THE BUFFER AS 00000000.
                (*buffer) = 0;         // BUFFER SIZE IS ALSO SET TO 0.
            }
        }
}


void encoding_buffer(FILE* fpr, FILE* fpw, Huff_Node* leafnodes,unsigned int* offbits) {

    int check_char;    // getc returns int so we need an integer to store the values.
    int buffersize=0;
    unsigned int character,buffer = 0;

    Huff_Node *temp;
    unsigned char encodings[256];  // to store the encodings of each ascii.
/***************************************************************************/
// encodings array will never exceed [256] as 256 is the maximum length of tree
//                 where every character has frequency 1.
/***************************************************************************/

    check_char = fgetc(fpr);

    while(!(check_char<0)){    //since EOF is negative
        character = check_char;

        temp = &leafnodes[character];
        int height = 0;       // height at leaf node is taken as 0.

        form_encodings(temp,encodings,&height); //forms encoding for the character in encoding[] and stores the respective height needed.

        write_encodings(fpw,encodings,height,&buffer,&buffersize); //forms encoding[] as 8 bits characters and write the character into the file.
        check_char = fgetc(fpr);
    }

    printf("END OF FILE IS REACHED, WRITING THE LAST CHARACTER AND COMPUTING ITS OFFBITS \n");
     // the last stored buffer is written into the file.
    fputc(buffer, fpw);
    buffer = 0;
    (*offbits) = 8 - buffersize;
    // we need to find the padding/offbits/missing bits so that we can ignore them in the decoding of last character.
}



/********************************************************************/
// DECODING .TXT\.bmp TO TARGET FILE //
/********************************************************************/
void decoding_buffer(FILE* fpr, FILE* fpw,Huff_Node* head, unsigned int offbits) {

    unsigned long int begin = ftell(fpr);
    fseek(fpr, 0, SEEK_END);
    unsigned long int end = ftell(fpr);
    fseek(fpr, begin, SEEK_SET);
    int length = end - begin;    // length gives the length of encoded characters

    unsigned char buffer = 0;
    int buffersize = 0;
    unsigned char bit;              // bit is used to extract bits from the buffer and traverse the huffman tree according to the bits recieved.

    Huff_Node *temp;
    while(length > 0 || buffersize > 0){
        temp = head;
        while(temp->left || temp->right)  // While node is not a root node
        {
            if(buffersize == 0)      // if buffersize is 0 then we read a new character for the file as set the buffersize as 8.
            {
                if(length <= 0){ return; }   // special case where all characters are read and we reach end of file.

                buffer = fgetc(fpr);
                if(length == 1) {            //  special case where we reach the last offbit char and we adjust the padding for it by using buffersize as 8 - offbits.
                    buffersize = 8 - offbits;
                    if(buffersize == 0){ return; }
                }
                else{ buffersize = 8;}

            length--;    // we keep decrementing the length of file left to read.
            }

       /** Taking buffer character and read its bits to get the path on huffman tree **/
            bit = buffer & 1;    // we get the lsb of buffer.
            buffer >>= 1;        // we right shift the buffer for reading its next bit.
            buffersize--;        // buffer size is decreased in order to read the next bit from the file.
            if(bit == 0)   // 0 for left
            {
                temp = temp->left;
            }
            else           // 1 for right
            {
               temp = temp->right;
            }
        }
        fputc(temp->data, fpw);  // print the data at the root node.
    }
}


/********************************************************************/
// ENCODING SOURCE BUFFER TO .TXT AND DECODING .TXT TO TARGET FILE //
/********************************************************************/
void ENCODE(const char* Inputfile, const char* Encodedfile, const char* compression_ratios) {

    FILE *fpr = fopen(Inputfile, "rb");
    if(fpr == NULL){
       printf("Error! opening input file");
       // Program exits if the file pointer returns NULL.
       exit(1);
    }

    unsigned int ascii[256]={0};    //stores frequency of ascii characters.
    Huff_Node leafnodes[256];

    /** Count the frequencies of each ascii character into to ascii array.**/
    freq_counter(fpr, ascii);

    /** Build the huffman tree using the ascii array.**/
    Huff_Node *head = Build_Huffmantree(ascii,leafnodes);
    rewind(fpr);

    /** write the frequencies of each ascii character in order form ascii[0] to ascii[255] into to the encoded file.**/
    FILE *fpw = fopen(Encodedfile, "wb");              // fpw is at 0
    if(fpw == NULL){
       printf("Error in creating file");
       // Program exits if the file pointer returns NULL.
       exit(1);
    }
    for(int i = 0; i < 256; i++)                      // writing the frequency of ascii charaters into the encoded file.
    {
        fwrite(ascii + i, sizeof(unsigned int), 1, fpw);
    }
    unsigned long int offbitset = ftell(fpw);                            // fpw will be at 256*4 = 1024

    /** we write a dummy variable that acts as place holder for offbits**/
    unsigned int temp;                                 // dummy variable to act as temporary placeholder for our file.
    fwrite(&temp, sizeof(unsigned int), 1, fpw);

    unsigned int offbits;
    /** we encode all the unsigned ascii characters present in the input file by the help of huffman tree. **/
    encoding_buffer(fpr, fpw, leafnodes ,&offbits);

    unsigned int encodedbitlength = ftell(fpw);
    printf("Encoded bits length -> %d \n",encodedbitlength);
    /** we seek the pointer to offbitset which denotes the location where offbits need to be written. **/
    fseek(fpw, offbitset, SEEK_SET);
    fwrite(&offbits, sizeof(unsigned int), 1, fpw);

    FILE *fptext = fopen(compression_ratios, "a");
    if(fptext == NULL){
       printf("Error! opening input file");
       // Program exits if the file pointer returns NULL.
       exit(1);
    }

    float i = inputbitlength;
    float e = encodedbitlength;
    float ratio = i/e;

    printf("*******************************************\n");
    printf("       COMPRESSION RATE -> %f \n", ratio);
    printf("*******************************************\n");

    fprintf(fptext,"******************************************************\n");
    fprintf(fptext,"%s  - COMPRESSION RATE -> %f \n",Inputfile,ratio);
    fprintf(fptext,"******************************************************\n");

    fclose(fptext);
    fclose(fpw);
    fclose(fpr);
}

void DECODE(const char* Encodedfile, const char* Targetfile) {

    FILE *fpr = fopen(Encodedfile, "rb");
    if(fpr == NULL){
       printf("Error! opening Encoded file");
       // Program exits if the file pointer returns NULL.
       exit(1);
    }
    unsigned int ascii[256]={0};   // ascii array will store frequency of each ascii character(0-255).
    Huff_Node leafnodes[256];

    /** read the frequencies into to ascii array that stores the frequency of each ascii character.**/
    unsigned int freq;
    for(int i = 0; i < 256; i++){
        fread(&freq, sizeof(unsigned int), 1, fpr);
        ascii[i] = freq;
    }

    //     256 * 4  = 1024 characters are read.

    /** read the offbits which will help us in determinimg the last character.**/
    unsigned int offbits;
    fread(&offbits, sizeof(unsigned int), 1, fpr);

    //     1024 + 4 characters are read.

    /** Build the huffman tree using the ascii array.**/
    Huff_Node *head = Build_Huffmantree(ascii,leafnodes);

    FILE *fpw = fopen(Targetfile, "wb");
    if(fpw == NULL){
       printf("Error in creating file");
       // Program exits if the file pointer returns NULL.
       exit(1);
    }

    /** Decode the file using the huffman tree created.**/
    decoding_buffer(fpr, fpw, head, offbits);

    fclose(fpw);
    fclose(fpr);
}


/************************************************************/
// Main function -> for inputting user defined inputs //
/************************************************************/
int main()
{
    DIR* dir = NULL;

    char Inputdirectory[40];
    printf("Kindly Enter the path/directory of Input files you wish to encode/compress.. \n");
    scanf(" %s",Inputdirectory);       // input path/directory of uncompressed BMP files.

    dir = opendir(Inputdirectory);
    if(dir == NULL){
        printf("File Directory cannot be opened \n");
        return 0;
    }

    printf("File Directory is opened \n");

    char outputdirectory[40] = "";
    printf("Kindly Enter the path/directory of files where you wish to store output files \n");
    scanf(" %s",outputdirectory);       // input path/directory of uncompressed BMP files.

    char filename[40] = "";
    char outfile[40] = "";
    char outfile2[40] = "";
    char outfile3[40] = "";
    char temp[40] = "";

    struct dirent *entity;
    while((entity = readdir(dir)) != NULL)
        {

            if (entity->d_type == DT_REG)
            {
            strcat(filename, Inputdirectory);
            strcat(filename, "/");
            strcat(outfile, outputdirectory);
            strcat(outfile, "/encoded");
            strcat(outfile2, outputdirectory);
            strcat(outfile2, "/decode");
            strcat(outfile3, outputdirectory);
            strcat(outfile3, "/compression.txt");
            strcat(filename, entity->d_name);
            printf("%s\n",filename);
            printf("\nHEADER CONTENTS READ FROM FILE ARE GIVEN AS : \n");
            read_header(filename);        // Displaying source bmp header read from each file given in directory one by one.  (HELPFUL IN CHECKING IF INPUT FILE IS A BMP FILE.)

            strcat(outfile, entity->d_name);
            printf("Header Read successfully!!\n\n");

            printf("%s - %s \n",filename,outfile);
            ENCODE(filename,outfile,outfile3);        // inside encode we call encoding_buffer() which writes the encodings to Encodedfile;
            printf("File -> %s encoded successfully!!\n",filename);

            strcat(outfile2, entity->d_name);
            DECODE(outfile,outfile2);         //inside decode we call decoding_buffer() which writes the decodings to target file;
            printf("File -> %s decoded successfully!!\n",outfile);

            strcpy(filename,temp);
            strcpy(outfile,temp);
            strcpy(outfile2,temp);
            strcpy(outfile3, temp);

            }
        }
        closedir(dir);
    return 0;
}
