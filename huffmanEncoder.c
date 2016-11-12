#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "list.h"
#include "decode.h"

char* getFileContents(FILE *file);
struct Node* createEncodingTree(char *text);
void removeCharacterFromString(char* str, char c);
struct Node* getLowestFrequencyNode(char *string);
char* encodeText(char *text, struct ListElement* list);
char* appendStringToString(char* base, char* appended);
int getLetterCountFromString(char *string, char letter);
void decodeTextFromFile(char* fileName, char* outputFileName);
void encodeTextFromFile(char* fileName, char* outputFileName);
char* find(struct Node* node, struct ListElement *list, char* path);
struct ListElement* createListOfLeafValues(struct Node* encodingTree);
struct Node* findLowestFrequencyLetterAndRemoveFromString(char *string);
struct Node* getLowestFrequencyNodeOrPair(char* text, int currentNodeFrequency, struct Node* smallestFrequencyNode, struct Node* secondSmallestFrequencyNode);

int main(int argc, char *argv[]) {
    argc = 3;
    argv[1] = "-d";
    argv[2] = "C:\\Users\\Hans\\Documents\\Programming\\C\\huffmanEncoder\\encoded.txt";
    argv[3] = "-o";
    argv[4] = "C:\\Users\\Hans\\Documents\\Programming\\C\\huffmanEncoder\\decoded.txt";

    if (argc == 3) {
        int isEncoding = strcmp(argv[1], "-e") == 0 || strcmp(argv[3], "-e") == 0;
        int isDecoding = strcmp(argv[1], "-d") == 0 || strcmp(argv[3], "-d") == 0;
        char* outputFileName = strcmp(argv[1], "-o") == 0 ? argv[2] : argv[4];
        char* fileName = strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "-d") == 0 ? argv[2] : argv[4];

        if (isEncoding) {
            encodeTextFromFile(fileName, outputFileName);
        } else if (isDecoding) {
            decodeTextFromFile(fileName, outputFileName);
        }
    }

    return 0;
}

void encodeTextFromFile(char* fileName, char* outputFileName) {
    FILE *file = fopen(fileName, "r");
    FILE *outputFile = fopen(outputFileName, "w+");
    char* fileContents = getFileContents(file);
    char* contentsCopy = malloc(sizeof(char) * (strlen(fileContents) + 1));
    strcpy(contentsCopy, fileContents);
    struct Node* encodingTree = createEncodingTree(fileContents);
    struct ListElement* list = createListOfLeafValues(encodingTree);
    char* encodedText = encodeText(contentsCopy, list);
    char* encodedTree = getEncodedTree(list);
    fputs(appendStringToString(encodedTree, encodedText), outputFile);
    fflush(outputFile);
    fclose(outputFile);
}

void decodeTextFromFile(char* fileName, char* outputFileName) {
    FILE *file = fopen(fileName, "r");
    FILE *outputFile = fopen(outputFileName, "w");
    struct Node* decodedTree = decodeTree(file);
    char* decodedText = decodeText(file, decodedTree);
    fputs(decodedText, outputFile);
    fflush(outputFile);
    fclose(outputFile);
}

char* getFileContents(FILE *file) {
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *fileContents = malloc(fileSize + 1);
    fread(fileContents, fileSize, 1, file);
    fileContents[fileSize] = '\0';
    fclose(file);

    return fileContents;
}

struct ListElement* createListOfLeafValues(struct Node* encodingTree) {
    struct ListElement *list = malloc(sizeof(*list));
    list -> combination = NULL;
    list -> character = malloc(sizeof(char) * 2);
    list -> character[1] = '\0';
    list -> nextElement = NULL;

    find(encodingTree, list, "");
    return list;
}

char* encodeText(char *text, struct ListElement* list) {
    int i;
    char* encodedText = malloc(sizeof(char));
    encodedText[0] = '\0';

    for( i= 0; i < strlen(text); i++) {
        char letter = text[i];
        char* encodedLetter = findCharacterCodeInList(list, letter);
        encodedText = appendStringToString(encodedText, encodedLetter);
    }

    return encodedText;
}

char* appendStringToString(char* base, char* appended) {
    char *appendedString = malloc(sizeof(char) * (strlen(base) + strlen(appended) + 1));
    strcpy(appendedString, base);
    strcat(appendedString, appended);
    free(base);
    free(appended);
    return appendedString;
}

char* find(struct Node* node, struct ListElement *list, char* path) {
    char* currentPath;
    char* newPathLeft = malloc(strlen(path) + 1);
    char* newPathRight = malloc(strlen(path) + 1);
    strcpy(newPathLeft, path);
    strcpy(newPathRight, path);

    if (node == NULL) {
        return NULL;
    }
    if (node -> leftNode == NULL && node -> rightNode == NULL) {
        pushUniqueLetterAndPathToList(list, node -> character, path);
    }
    if(NULL != (currentPath=find(node -> leftNode, list, strcat(newPathLeft, "1")))) {
        return currentPath;
    }
    if(NULL!=(currentPath=find(node -> rightNode, list, strcat(newPathRight, "0")))) {
        return currentPath;
    }
    return NULL;
}

struct Node* createEncodingTree(char *text) {
    struct Node* smallestFrequencyNode, *secondSmallestFrequencyNode, *currentNode, *newParent;
    currentNode = createNewNode();

    while(strlen(text)) {
        smallestFrequencyNode = findLowestFrequencyLetterAndRemoveFromString(text);

        if (currentNode -> leftNode == NULL) {
            if (strlen(text)) {
                secondSmallestFrequencyNode = findLowestFrequencyLetterAndRemoveFromString(text);
                currentNode -> leftNode = createNodePair(smallestFrequencyNode, secondSmallestFrequencyNode);
            } else {
                currentNode -> leftNode = smallestFrequencyNode;
            }
            currentNode -> frequency += currentNode -> leftNode -> frequency;
        } else {
            int combinedFrequency, currentNodeFrequency = currentNode -> frequency;

            if (strlen(text)) {
                secondSmallestFrequencyNode = getLowestFrequencyNode(text);
                currentNode -> rightNode = getLowestFrequencyNodeOrPair(text, currentNodeFrequency, smallestFrequencyNode, secondSmallestFrequencyNode);
            } else {
                currentNode -> rightNode = smallestFrequencyNode;
            }

            combinedFrequency = currentNode -> leftNode -> frequency + currentNode -> rightNode -> frequency;
            currentNode -> frequency = combinedFrequency;

            if (strlen(text)) {
                newParent = createNewNode();
                newParent -> leftNode = currentNode;
                newParent -> frequency = currentNode -> frequency;
                currentNode = newParent;
            }
        }
    }

    return currentNode;
}

struct Node* createNewNode() {
    struct Node* node = malloc(sizeof(*node));

    node -> frequency = 0;
    node -> character = '\0';
    node -> leftNode = NULL;
    node -> rightNode = NULL;

    return node;
}

struct Node* getLowestFrequencyNodeOrPair(char* text, int currentNodeFrequency, struct Node* smallestFrequencyNode, struct Node* secondSmallestFrequencyNode) {
    int smallestFrequency = smallestFrequencyNode -> frequency;
    int secondSmallestFrequency = secondSmallestFrequencyNode -> frequency;

    if (smallestFrequency <= currentNodeFrequency && secondSmallestFrequency <= currentNodeFrequency) {
        removeCharacterFromString(text, secondSmallestFrequencyNode -> character);
        return createNodePair(smallestFrequencyNode, secondSmallestFrequencyNode);
    } else if (smallestFrequency <= currentNodeFrequency) {
        return smallestFrequencyNode;
    } else {
        removeCharacterFromString(text, secondSmallestFrequencyNode -> character);
        return secondSmallestFrequencyNode;
    }
}

struct Node* createNodePair(struct Node* leftNode, struct Node* rightNode) {
    struct Node* newNode = malloc(sizeof(*newNode));
    int combinedFrequency = leftNode -> frequency + rightNode -> frequency;

    newNode -> frequency = combinedFrequency;
    newNode -> character = '\0';
    newNode -> leftNode = leftNode;
    newNode -> rightNode = rightNode;

    return newNode;
}

void removeCharacterFromString(char* str, char c) {
    char *pr = str, *pw = str;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != c);
    }
    *pw = '\0';
}

int getLetterCountFromString(char *string, char letter) {
    int letterToCompareTo, count = 0;

    for (letterToCompareTo = 0; letterToCompareTo < strlen(string); letterToCompareTo++) {
        if (letter == string[letterToCompareTo]) {
            count++;
        }
    }

    return count;
}

struct Node* findLowestFrequencyLetterAndRemoveFromString(char *string) {
    struct Node* lowestFrequencyNode = getLowestFrequencyNode(string);
    removeCharacterFromString(string, lowestFrequencyNode -> character);
    return lowestFrequencyNode;
}

struct Node* getLowestFrequencyNode(char *string) {
    char letter;
    int letterToRead, count;
    struct Node* lowestFrequencyNode = createNewNode();

    for (letterToRead = 0; letterToRead < strlen(string); letterToRead++) {
        letter = string[letterToRead];
        count = getLetterCountFromString(string, letter);

        if (count < lowestFrequencyNode->frequency || !lowestFrequencyNode->frequency) {
            lowestFrequencyNode->frequency = count;
            lowestFrequencyNode->character = letter;
        }
    }

    return lowestFrequencyNode;
}