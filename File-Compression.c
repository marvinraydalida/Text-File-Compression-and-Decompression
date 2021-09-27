#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int charFreq[256], code[256];
int charCount, size = -1, ind = -1, notFound = 1, bitPos = 0, asciiBitCount, bitShift = 7, asciiBitRem, bitSpace = 8, fNameLen;
char charCode[256], tempry[50];
float initialsize,finalsize;
unsigned char bitStorage = '\0';
long int compressedFileLen;


typedef struct node{
    unsigned char ch;
    int freq;
    struct node *left;
    struct node *right;
}huffNode;

typedef struct queue{
    huffNode *pqNode;
    struct queue *next;
}priorityQueue;

priorityQueue *head;

void makeNull(){
    head = NULL;
}

priorityQueue *createNode(int i, int k, huffNode *l, huffNode *r){
    priorityQueue *newNode = (priorityQueue*)malloc(sizeof(priorityQueue));
    newNode -> pqNode = (huffNode*)malloc(sizeof(huffNode));
    newNode -> pqNode -> left = l;
    newNode -> pqNode -> right = r;
    newNode -> next = NULL;
    if(k){
        newNode -> pqNode -> ch = i;
        newNode -> pqNode -> freq = charFreq[i];
    }
    else{
        newNode -> pqNode -> ch = '\0';
        newNode -> pqNode -> freq = i;
    }
    return newNode;
}

void enqueue(priorityQueue* newNode){
    priorityQueue *p, *q;
    p = q = head;

    if(head == NULL){
        newNode -> next = head;
        head = newNode;
    }
    else{
        if(newNode -> pqNode -> freq < head -> pqNode -> freq ){
            newNode -> next = head;
            head = newNode;
        }
        else{
            while(p != NULL){
                q = p;
                p = p -> next;
                if(p != NULL){
                    if(newNode -> pqNode -> freq < p -> pqNode -> freq){
                        newNode -> next = p;
                        q -> next = newNode;
                        break;
                    }
                }
                else{
                    q -> next = newNode;
                    newNode -> next = p;
                }
            }
        }
    }
}

void dequeue(priorityQueue* node){
    if(node -> next != NULL)
        free(node -> next);
    free(node);
}

void createTree(priorityQueue* l, priorityQueue* r){
    priorityQueue *newNode, *temp = head;
    newNode = createNode((l -> pqNode -> freq) + (r -> pqNode -> freq), 0, l -> pqNode, r -> pqNode);
    if(r -> next != NULL)
        head = r -> next;
    else
        head = newNode;
    dequeue(temp);
    enqueue(newNode);
    if(head -> next != NULL)
        createTree(head, head -> next);
}

void traverse(huffNode* node){
    if (node -> left == NULL && node -> right == NULL){
        printf("\n%c\t: %d\t: ",node->ch, node -> freq);
        for(int i = 0; i <= ind; i++)
            printf("%d",code[i]);
        return;
    }
    ind++;
    code[ind] = 0;
    traverse(node->left);
    code[ind] = 1;
    traverse(node->right);
    ind--;
}

void fillCharFreqArray(){
    for(int i = 0; i < 256; i++)
        charFreq[i] = 0;
}
int main()
{
    priorityQueue *newNode;
    huffNode *huffManTree;
    char nameOfFile[50];
    fillCharFreqArray();
    makeNull();
    header();
    printf("Enter Text file: ");
    gets(nameOfFile);
    FILE *fp, *fp_out, *fp_freq;
    fp = fopen(nameOfFile,"r");
    if(fp == NULL){
        printf("file error");
        exit(0);
    }
    else{
        printf("\nText inside %s:\n\n",nameOfFile);
        while(!feof(fp)){
            charCount = fgetc(fp);
            charFreq[charCount]++;
            putchar(charCount);
            size++;
        }
        initialsize=size;
    }
    fclose(fp);
    if(size > 1){
        for(int i = 0; i < 256; i++){
            if(charFreq[i] > 0){
                newNode = createNode(i,1,NULL,NULL);
                enqueue(newNode);
            }
        }

        createTree(head, head -> next);
        huffManTree = head -> pqNode;
        free(head);
        printf("\n\nChar\tFreq.\tCode");
        traverse(huffManTree);
    }
    else{
        printf("\n\nstring length must be greater than 1.\n\n");
        exit(0);
    }


    fNameLen = strlen(nameOfFile);
    fNameLen -= 4;
    strncpy(tempry, nameOfFile,fNameLen);
    fp = fopen(nameOfFile,"r");
    encode(fp, huffManTree);
    fclose(fp);

    memset(tempry, '\0', sizeof(tempry));
    strncpy(tempry, nameOfFile,fNameLen);
    strcat(tempry,".tree");

    fp = fopen(tempry,"wb");
    bitStorage = '\0';
    saveTree(huffManTree,fp);
    fputc(bitStorage,fp);
    fclose(fp);

    memset(tempry, '\0', sizeof(tempry));
    strncpy(tempry, nameOfFile,fNameLen);
    strcat(tempry,".freq");

    fp_freq = fopen(tempry,"w");
    fprintf(fp,"%d",huffManTree -> freq);
    fclose(fp_freq);
    return 0;
}

void look(huffNode* node, unsigned char c){
    if (node -> left == NULL && node -> right == NULL ||node -> ch == c){
        if(node -> ch == c)
            notFound = 0;
        return;
    }
    ind++;
    charCode[ind] = 0;
    look(node->left, c);
    if(notFound){
        charCode[ind] = 1;
        look(node->right, c);
    }
    if(notFound)
        ind--;
}


void encode(FILE* fp_in, huffNode* node){
    FILE* encodedFile;
    strcat(tempry,".compressed");
    encodedFile = fopen(tempry,"wb");
    while(!feof(fp_in)){
        ind = -1;
        notFound = 1;
        charCount = fgetc(fp_in);
        look(node, charCount);
        for(int i = 0; i <= ind; i++){
            bitPos++;
            bitStorage = bitStorage | charCode[i];
            if(bitPos < 8)
                bitStorage = bitStorage << 1;
            if(bitPos == 8){
                fputc(bitStorage, encodedFile);
                bitPos = 0;
                bitStorage = '\0';

            }
        }
    }

    while(bitPos != 7){
        bitStorage = bitStorage << 1;
        bitPos++;
    }
    fputc(bitStorage, encodedFile);
    finalsize = compressedFileLen = ftell(encodedFile);
    printf("\n\nThe size of %s in bytes: %ld\n",tempry,compressedFileLen);
    printf("Compression Ratio: %.2f%%\n\n",((finalsize/initialsize)*100));
    fclose(encodedFile);
}

void saveTree(huffNode *node, FILE *tree){
    unsigned char temp;
    if(node -> left == NULL && node -> right == NULL){
        temp = 1;
        temp <<= bitShift;
        bitStorage |= temp;
        bitSpace--;
        asciiBitCount = 8 - bitShift;
        asciiBitRem = asciiBitCount;
        if(bitSpace > 0){
            temp = node ->ch;
            temp >>= asciiBitCount;
            bitStorage |= temp;
        }
        fputc(bitStorage,tree);
        bitStorage = '\0';
        bitShift = 7;
        bitSpace = 8;
        if(asciiBitRem > 0){
            temp = node -> ch;
            temp <<= 8 - asciiBitRem;
            bitStorage |= temp;
            bitSpace = 8 - asciiBitRem;
            bitShift = bitSpace - 1;
            if(asciiBitRem == 8){
                fputc(bitStorage,tree);
                bitStorage = '\0';
                bitShift = 7;
                bitSpace = 8;
            }
        }
        asciiBitCount = 8;
        asciiBitRem = 8;
        return;
    }
    if(node -> left -> ch == '\0'){
        temp = 0;
        temp <<= bitShift;
        bitStorage |= temp;
        bitShift--;
        bitSpace--;
        if(bitSpace == 0){
            fputc(bitStorage,tree);
            bitStorage = '\0';
            bitShift = 7;
            bitSpace = 8;
        }
    }
    saveTree(node -> left,tree);

    if(node -> right -> ch == '\0'){
        temp = 0;
        temp <<= bitShift;
        bitStorage |= temp;
        bitShift--;
        bitSpace--;
        if(bitSpace == 0){
            fputc(bitStorage,tree);
            bitStorage = '\0';
            bitShift = 7;
            bitSpace = 8;
        }
    }
    saveTree(node -> right,tree);
}

void header(){
    printf("%c",201);
    for(int i = 0; i < 51; i++)
        printf("%c",205);
    printf("%c\n",187);
    printf("%c          F I L E   C O M P R E S S I O N          %c\n",186,186);
    printf("%c                                                   %c\n",186,186);
    printf("%c    DALIDA, MARVIN RAY I.  HOD, WILLIAM CRIS E.    %c\n",186,186);
    printf("%c                                                   %c\n",186,186);
    printf("%c              BSCS - 2A (NON - STEM)               %c\n",186,186);
    printf("%c",200);
    for(int i = 0; i < 51; i++)
        printf("%c",205);
    printf("%c\n",188);
}
