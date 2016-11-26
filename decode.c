#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "decode.h"
#include "list.h"

struct Node* getTreeBottom(FILE *tempBitsFile);
struct Node* createNewCharacterNode(char nodeCharacter);
char convertBinaryStringFromFileToChar(FILE *tempFileBits);

void decodeText(FILE* outputFile, FILE *tempBitsFile, struct Node* decodedTree) {
    struct Node* copyOfTreeForIterating = decodedTree;
    char letter;
    int isLeaf;

    do {
        letter = (char)fgetc(tempBitsFile);
        if (feof(tempBitsFile)) {
            break;
        }

        if (letter == '1') {
            copyOfTreeForIterating = copyOfTreeForIterating -> leftNode;
        } else if (letter == '0') {
            copyOfTreeForIterating = copyOfTreeForIterating -> rightNode;
        }

        isLeaf = copyOfTreeForIterating -> leftNode == NULL && copyOfTreeForIterating -> rightNode == NULL;
        if (isLeaf) {
            fputc(copyOfTreeForIterating -> character, outputFile);
            copyOfTreeForIterating = decodedTree;
        }
    }while(1);
}

struct Node* decodeTree(FILE *tempBitsFile) {
    int numberOfNodes = 0;
    int i = 0;
    struct NodeAndContent returnElement;
    rewind(tempBitsFile);

    while(fgetc(tempBitsFile) == '0') {
        //Read past the 0's
    }
    numberOfNodes++;
    while(fgetc(tempBitsFile) != '0') {
        numberOfNodes++;
    }

    struct Node* bottomNode = getTreeBottom(tempBitsFile);
    numberOfNodes--;
    struct Node* treeTop = createNewNode();
    struct Node* oldTop;
    int isNotPair = 0;

    treeTop -> leftNode = bottomNode;

    while (numberOfNodes != 0) {
        isNotPair = fgetc(tempBitsFile) == '0';

        if (isNotPair) {
            treeTop -> rightNode = createNewCharacterNode(convertBinaryStringFromFileToChar(tempBitsFile));
            oldTop = treeTop;
            treeTop = createNewNode();
            treeTop -> leftNode = oldTop;
        } else {
            fgetc(tempBitsFile); //Move pointer forward by the 0, since a pair is 10
            struct Node *firstNode = createNewCharacterNode(convertBinaryStringFromFileToChar(tempBitsFile));
            fgetc(tempBitsFile);
            struct Node *secondNode = createNewCharacterNode(convertBinaryStringFromFileToChar(tempBitsFile));
            treeTop -> rightNode = createNodePair(firstNode, secondNode);

            oldTop = treeTop;
            treeTop = createNewNode();
            treeTop -> leftNode = oldTop;
        }

        numberOfNodes--;
    }


    return treeTop -> leftNode;
}

struct Node* getTreeBottom(FILE *tempBitsFile) {
    char firstChar, secondChar;

    firstChar = convertBinaryStringFromFileToChar(tempBitsFile);
    fgetc(tempBitsFile); //Pass the zero
    secondChar = convertBinaryStringFromFileToChar(tempBitsFile);

    struct Node* leftNode = createNewCharacterNode(firstChar);
    struct Node* rightNode = createNewCharacterNode(secondChar);
    struct Node* bottomPair = createNodePair(leftNode, rightNode);

    return bottomPair;
}

char convertBinaryStringFromFileToChar(FILE *tempFileBits) {
    int i;
    char *charAsString = calloc(sizeof(char), 2);
    char *binaryRepresentationOfChar = calloc(sizeof(char), 9);

    for(i = 0; i < 8; i++) {
        charAsString[0] = (char)fgetc(tempFileBits);
        strcat(binaryRepresentationOfChar, charAsString);
    }

    return (char)strtol(binaryRepresentationOfChar, NULL, 2);
}

struct Node* createNewCharacterNode(char nodeCharacter) {
    struct Node* node = malloc(sizeof(*node));

    node -> leftNode = NULL;
    node -> rightNode = NULL;
    node -> character = nodeCharacter;
    node -> frequency = 0;

    return node;
}