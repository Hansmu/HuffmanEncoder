#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "decode.h"
#include "list.h"

struct Node* getTreeBottom(char *encodedTree);
char* appendCharToString(char* string, char character);
struct Node* createNewCharacterNode(char nodeCharacter);

char* decodeText(char* string, struct Node* decodedTree) {
    char *decodedText = malloc(sizeof(char));
    decodedText[0] = '\0';
    struct Node* copyOfTreeForIterating = decodedTree;
    int i, isLeaf;

    for(i = 0; i < strlen(string); i++) {
        if (string[i] == '1') {
            copyOfTreeForIterating = copyOfTreeForIterating -> leftNode;
        } else if (string[i] == '0') {
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

struct Node* decodeTree(char* encodedTree) {
    struct Node* bottomNode = getTreeBottom(encodedTree);
    struct Node* treeTop = createNewNode();
    struct Node* oldTop;
    char* nextNodeInText = strstr(encodedTree, "10") + 5;
    int isNotPair = 0;

    treeTop -> leftNode = bottomNode;

    while (nextNodeInText[0] != '\0') {
        isNotPair = nextNodeInText[0] == '0';

        if (isNotPair) {
            treeTop -> rightNode = createNewCharacterNode(nextNodeInText[1]);
            oldTop = treeTop;
            treeTop = createNewNode();
            treeTop -> leftNode = oldTop;
            nextNodeInText = nextNodeInText + 2;
        } else {
            treeTop -> rightNode = createNodePair(
                    createNewCharacterNode(nextNodeInText[2]),
                    createNewCharacterNode(nextNodeInText[4])
            );

            oldTop = treeTop;
            treeTop = createNewNode();
            treeTop -> leftNode = oldTop;

            nextNodeInText = nextNodeInText + 5;
        }
    }

    return treeTop -> leftNode;
}

struct Node* getTreeBottom(char *encodedTree) {
    char *firstOccurrence = strstr(encodedTree, "10");

    struct Node* leftNode = createNewCharacterNode(firstOccurrence[2]);
    struct Node* rightNode = createNewCharacterNode(firstOccurrence[4]);
    struct Node* bottomPair = createNodePair(leftNode, rightNode);

    return bottomPair;
}

struct Node* createNewCharacterNode(char nodeCharacter) {
    struct Node* node = malloc(sizeof(*node));

    node -> leftNode = NULL;
    node -> rightNode = NULL;
    node -> character = nodeCharacter;
    node -> frequency = 0;

    return node;
}