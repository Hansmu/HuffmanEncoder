#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "decode.h"
#include "list.h"

struct Node* getTreeBottom(char *bitString);
char convertBinaryStringFromFileToChar(char* bitString);
char* appendCharToString(char* string, char character);
struct Node* createNewCharacterNode(char nodeCharacter);

char* decodeText(char* bitsFromFile, struct Node* decodedTree) {
    char bit;
    char *decodedText = malloc(sizeof(char));
    decodedText[0] = '\0';
    struct Node* copyOfTreeForIterating = decodedTree;
    int isLeaf, i = 0;

    while (bitsFromFile[i] != '\0') {
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
        i++;
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

struct ReturnNodeAndLength decodeTree(char* encodedBits) {
    int numberOfNodes = 0;
    int i = 0;
    struct ReturnNodeAndLength returnElement;

    while(encodedBits[i] == '0') { i++; }
    numberOfNodes++;

    while(encodedBits[i] != '0') {
        numberOfNodes++, i++;
    }

    struct Node* bottomNode = getTreeBottom(encodedBits + i);
    numberOfNodes--;
    struct Node* treeTop = createNewNode();
    struct Node* oldTop;
    int isNotPair = 0;

    treeTop -> leftNode = bottomNode;

    while (numberOfNodes != 0) {
        i++;
        isNotPair = encodedBits[i] == '0';

        if (isNotPair) {
            treeTop -> rightNode = createNewCharacterNode(convertBinaryStringFromFileToChar(encodedBits));
            oldTop = treeTop;
            treeTop = createNewNode();
            treeTop -> leftNode = oldTop;
        } else {
            i++; //Move pointer forward by the 0, since a pair is 10
            struct Node *firstNode = createNewCharacterNode(convertBinaryStringFromFileToChar(encodedBits));
            i++;
            struct Node *secondNode = createNewCharacterNode(convertBinaryStringFromFileToChar(encodedBits));
            treeTop -> rightNode = createNodePair(firstNode, secondNode);

            oldTop = treeTop;
            treeTop = createNewNode();
            treeTop -> leftNode = oldTop;
        }

        numberOfNodes--;
    }

    returnElement.tree = treeTop -> leftNode;
    returnElement.length = i;
    return returnElement;
}

struct Node* getTreeBottom(char* bitString) {
    char firstChar, secondChar;
    int i = 0;

    firstChar = convertBinaryStringFromFileToChar(bitString);
    secondChar = convertBinaryStringFromFileToChar(bitString + 5);

    struct Node* leftNode = createNewCharacterNode(firstChar);
    struct Node* rightNode = createNewCharacterNode(secondChar);
    struct Node* bottomPair = createNodePair(leftNode, rightNode);

    return bottomPair;
}

char convertBinaryStringFromFileToChar(char* stringAsBits) {
    int i;
    char *charAsString = calloc(sizeof(char), 2);
    char *binaryRepresentationOfChar = calloc(sizeof(char), 5);

    for(i = 0; i < 4; i++) {
        charAsString[0] = stringAsBits[i];
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