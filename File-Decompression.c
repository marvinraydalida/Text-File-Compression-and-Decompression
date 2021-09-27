#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int shiftLeft = 0, asciiBitRem, fNameLen, totalCharFrequency, scanned = 0, readBinary;
unsigned char bitStorage = '\0', asciiBitStorage = '\0',  tempBitStorage = '\0', temp;
char tempry[50], nameOfFile[50], option = '\0';

typedef struct node{
    unsigned char ch;
    int leafNode;
    struct node *left;
    struct node *right;
}huffNode;

int main()
{
    huffNode *huffManTree = (huffNode*)malloc(sizeof(huffNode));
    huffNode *newNode;
    header();
    printf("Enter (.)compressed file: ");
    gets(nameOfFile);
    fNameLen = strlen(nameOfFile);
    fNameLen -= 11;
    strncpy(tempry, nameOfFile, fNameLen);
    strcat(tempry,".tree");
    FILE *fp, *fp_compressed, *fp_out, *fp_freq;
    fp = fopen(tempry,"rb");
    if(fp == NULL){
        printf("The (.)tree file is not found");
        exit(0);
    }
    else{
        memset(tempry, '\0', sizeof(tempry));
        strncpy(tempry, nameOfFile, fNameLen);
        strcat(tempry,".freq");
        fp_freq = fopen(tempry,"r");
        if(fp_freq == NULL){
            printf("The (.)freq file is not found");
            exit(0);
        }
        else
            fscanf(fp_freq,"%d",&totalCharFrequency);

        fclose(fp_freq);
        readBinary = fgetc(fp);
        bitStorage = readBinary;
        reCreateTree(huffManTree, fp);
    }
    fclose(fp);

    fp_compressed = fopen(nameOfFile,"rb");
    memset(tempry, '\0', sizeof(tempry));
    fNameLen = strlen(nameOfFile);
    fNameLen -= 11;
    strncpy(tempry, nameOfFile,fNameLen);
    strcat(tempry,".txt");

    fp_out = fopen(tempry,"w");


    if(fp_compressed == NULL){
        printf("The (.)compressed file is not found.");
        exit(0);
    }
    else{
        printf("\nDecoded message:\n\n");
        newNode = huffManTree;
        while(!feof(fp_compressed)){
            readBinary = fgetc(fp_compressed);
            bitStorage = readBinary;
            shiftLeft = 0;

            while(shiftLeft < 8){
                temp = bitStorage;
                temp <<= shiftLeft;
                temp >>= 7;
                shiftLeft++;

                if(temp == 0)
                    newNode = newNode -> left;

                else if(temp == 1)
                    newNode = newNode -> right;

                if(newNode -> leafNode == 1){
                    putchar(newNode ->ch);
                    fputc(newNode -> ch, fp_out);
                    newNode = huffManTree;
                    scanned++;
                }

                if(scanned == totalCharFrequency)
                    break;
            }
            if(scanned == totalCharFrequency)
                break;
        }
    }
    fclose(fp_compressed);
    fclose(fp_out);
    printf("\n\nDecoded message is saved in %s\n",tempry);
    printf("\nDo you want to delete %s? [Y/N]: ",nameOfFile);
    while(option != 'y' && option != 'Y' && option != 'n' && option != 'N'){
        option = getchar();
        if(option == 'y' || option == 'Y'){
            deleteFiles();
            break;
        }
    }
    printf("\nExiting program. . .\n");
    free(huffManTree);
    return 0;
}


void deleteFiles(){
    int del = remove(nameOfFile);
    memset(tempry, '\0', sizeof(tempry));
    fNameLen = strlen(nameOfFile);
    fNameLen -= 11;
    strncpy(tempry, nameOfFile,fNameLen);
    strcat(tempry,".tree");
    del = remove(tempry);
    memset(tempry, '\0', sizeof(tempry));
    strncpy(tempry, nameOfFile,fNameLen);
    strcat(tempry,".freq");
    del = remove(tempry);
    printf("Files successfully deleted.\n");
}

void reCreateTree(huffNode *node, FILE* fp){
    huffNode *newNode;
    if(temp == 1){
        if(shiftLeft < 8){
            asciiBitStorage |= bitStorage << shiftLeft;
            asciiBitRem = shiftLeft;
        }
        else{
            readBinary = fgetc(fp);
            asciiBitStorage = readBinary;
            asciiBitRem = 0;
        }

        if(asciiBitRem > 0){
            readBinary = fgetc(fp);
            bitStorage = readBinary;
            tempBitStorage = bitStorage;
            tempBitStorage >>= 8-shiftLeft;
            asciiBitStorage |=  tempBitStorage;
        }
        node -> ch = asciiBitStorage;
        node -> leafNode = 1;
        asciiBitStorage = '\0';
        return;
    }


    if(shiftLeft < 8)
        temp = bitStorage;
    else{
        readBinary = fgetc(fp);
        bitStorage = readBinary;
        temp = bitStorage;
        shiftLeft = 0;
    }
    temp <<= shiftLeft;
    temp >>= 7;
    shiftLeft++;

    newNode = (huffNode*)malloc(sizeof(huffNode));
    newNode -> ch = '\0';
    newNode -> leafNode = 0;
    node -> left = newNode;
    reCreateTree(node -> left, fp);

    if(shiftLeft < 8)
        temp = bitStorage;
    else{
        readBinary = fgetc(fp);
        bitStorage = readBinary;
        temp = bitStorage;
        shiftLeft = 0;
    }
    temp <<= shiftLeft;
    temp >>= 7;
    shiftLeft++;

    newNode = (huffNode*)malloc(sizeof(huffNode));
    newNode -> ch = '\0';
    newNode -> leafNode = 0;
    node -> right = newNode;
    reCreateTree(node -> right, fp);

}


void header(){
    printf("%c",201);
    for(int i = 0; i < 51; i++)
        printf("%c",205);
    printf("%c\n",187);
    printf("%c        F I L E   D E C O M P R E S S I O N        %c\n",186,186);
    printf("%c                                                   %c\n",186,186);
    printf("%c    DALIDA, MARVIN RAY I.  HOD, WILLIAM CRIS E.    %c\n",186,186);
    printf("%c                                                   %c\n",186,186);
    printf("%c              BSCS - 2A (NON - STEM)               %c\n",186,186);
    printf("%c",200);
    for(int i = 0; i < 51; i++)
        printf("%c",205);
    printf("%c\n",188);
}
