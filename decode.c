#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "decode.h"
#include "list.h"

struct Node* getTreeBottom(FILE *file);
char convertBinaryStringFromFileToChar(FILE *file);
char* appendCharToString(char* string, char character);
struct Node* createNewCharacterNode(char nodeCharacter);

char* decodeText(FILE *file, struct Node* decodedTree) {
    char bit;
    char *decodedText = malloc(sizeof(char));
    decodedText[0] = '\0';
    struct Node* copyOfTreeForIterating = decodedTree;
    int isLeaf;

    while (EOF != (bit = (char)fgetc(file))) {
        if (bit == '1') {
            copyOfTreeForIterating = copyOfTreeForIterating -> leftNode;
        } else if (bit == '0') {
            copyOfTreeForIterating = copyOfTreeForIterating -> rightNode;
        }

        isLeaf = copyOfTreeForIterating -> leftNode == NULL && copyOfTreeForIterating -> rightNode == NULL;
        if (isLeaf) {
            decodedText = appendCharToString(decodedText, copyOfTreeForIterating -> character);
            copyOfTreeForIterating = decodedTree;
        }
    }

    return decodedText;
}

char* appendCharToString(char* string, char character) {
    char* appendedString = malloc(sizeof(char) * (strlen(string) + 2));
    char* characterAsPointer = malloc(sizeof(char) * 2);

    characterAsPointer[0] = character;
    characterAsPointer[1] = '\0';

    strcpy(appendedString, string);
    strcat(appendedString, characterAsPointer);

    return appendedString;
}

struct Node* decodeTree(FILE *file) {
    int numberOfNodes = 0;

    while((char)fgetc(file) != '0') {
        numberOfNodes++;
    }

    struct Node* bottomNode = getTreeBottom(file);
    numberOfNodes--;
    struct Node* treeTop = createNewNode();
    struct Node* oldTop;
    int isNotPair = 0;

    treeTop -> leftNode = bottomNode;

    while (numberOfNodes != 0) {
        isNotPair = fgetc(file) == '0';

        if (isNotPair) {
            treeTop -> rightNode = createNewCharacterNode(convertBinaryStringFromFileToChar(file));
            oldTop = treeTop;
            treeTop = createNewNode();
            treeTop -> leftNode = oldTop;
        } else {
            fgetc(file); //Move pointer forward by the 0, since a pair is 10
            struct Node *firstNode = createNewCharacterNode(convertBinaryStringFromFileToChar(file));
            fgetc(file);
            struct Node *secondNode = createNewCharacterNode(convertBinaryStringFromFileToChar(file));
            treeTop -> rightNode = createNodePair(firstNode, secondNode);

            oldTop = treeTop;
            treeTop = createNewNode();
            treeTop -> leftNode = oldTop;
        }

        numberOfNodes--;
    }

    return treeTop -> leftNode;
}

struct Node* getTreeBottom(FILE *file) {
    char firstChar, secondChar;

    firstChar = convertBinaryStringFromFileToChar(file);
    fgetc(file);
    secondChar = convertBinaryStringFromFileToChar(file);

    struct Node* leftNode = createNewCharacterNode(firstChar);
    struct Node* rightNode = createNewCharacterNode(secondChar);
    struct Node* bottomPair = createNodePair(leftNode, rightNode);

    return bottomPair;
}

char convertBinaryStringFromFileToChar(FILE *file) {
    int i;
    char *charAsString = calloc(sizeof(char), 2);
    char *binaryRepresentationOfChar = calloc(sizeof(char), 9);

    for(i = 0; i < 8; i++) {
        charAsString[0] = (char)fgetc(file);
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