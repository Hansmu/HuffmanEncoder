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
    char *decodedText = malloc(sizeof(char));
    decodedText[0] = '\0';
    struct Node* copyOfTreeForIterating = decodedTree;
    int isLeaf, i = 0;

    while (bitsFromFile[i] != '\0') {
        if (bitsFromFile[i] == '1') {
            copyOfTreeForIterating = copyOfTreeForIterating -> leftNode;
        } else if (bitsFromFile[i] == '0') {
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
    char* appendedString = calloc(strlen(string) + 2, sizeof(char));
    char* characterAsPointer = malloc(sizeof(char) * 2);

    characterAsPointer[0] = character;
    characterAsPointer[1] = '\0';

    strcpy(appendedString, string);
    strcat(appendedString, characterAsPointer);

    return appendedString;
}

struct NodeAndContent decodeTree(char* encodedBits) {
    int numberOfNodes = 0;
    int i = 0;
    struct NodeAndContent returnElement;

    while(encodedBits[i] == '0') { i++; }

    while(encodedBits[i] != '0') {
        numberOfNodes++, i++;
    }
    i++; //Move past the first zero
    struct Node* bottomNode = getTreeBottom(encodedBits + i);
    i+=17; //Move past the first node pair
    numberOfNodes--;
    struct Node* treeTop = createNewNode();
    struct Node* oldTop;
    int isNotPair = 0;

    treeTop -> leftNode = bottomNode;

    while (numberOfNodes != 0) {
        isNotPair = encodedBits[i] == '0';
        i++;

        if (isNotPair) {
            treeTop -> rightNode = createNewCharacterNode(convertBinaryStringFromFileToChar(encodedBits + i));
            i += 8; //Move by byte
            oldTop = treeTop;
            treeTop = createNewNode();
            treeTop -> leftNode = oldTop;
        } else {
            i++; //Move pointer forward by the 0, since a pair is 10
            struct Node *firstNode = createNewCharacterNode(convertBinaryStringFromFileToChar(encodedBits + i));
            i += 8; //Move by byte
            i++;
            struct Node *secondNode = createNewCharacterNode(convertBinaryStringFromFileToChar(encodedBits + i));
            i += 8;
            treeTop -> rightNode = createNodePair(firstNode, secondNode);

            oldTop = treeTop;
            treeTop = createNewNode();
            treeTop -> leftNode = oldTop;
        }

        numberOfNodes--;
    }

    returnElement.node = treeTop -> leftNode;
    returnElement.content = encodedBits + i;
    return returnElement;
}

struct Node* getTreeBottom(char* bitString) {
    char firstChar, secondChar;

    firstChar = convertBinaryStringFromFileToChar(bitString);
    secondChar = convertBinaryStringFromFileToChar(bitString + 9);

    struct Node* leftNode = createNewCharacterNode(firstChar);
    struct Node* rightNode = createNewCharacterNode(secondChar);
    struct Node* bottomPair = createNodePair(leftNode, rightNode);

    return bottomPair;
}

char convertBinaryStringFromFileToChar(char* stringAsBits) {
    int i;
    char *charAsString = calloc(sizeof(char), 2);
    char *binaryRepresentationOfChar = calloc(sizeof(char), 9);

    for(i = 0; i < 8; i++) {
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