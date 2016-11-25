#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "list.h"
#include "decode.h"

int letterFrequency[256];

char* getFileContents(FILE *file);
char *decodeBitsFromFile(FILE *file);
void createLetterStatistics(char *text);
char* createStringPadding(int paddingSize);
struct Node* createEncodingTree(char *text);
void removeCharacterFromString(char* str, char c);
struct Node* getLowestFrequencyNode(char *string);
char* encodeText(char *text, struct ListElement* list);
char* appendStringToString(char* base, char* appended);
int getLetterCountFromString(char *string, char letter);
void writeBitsAsCharsToFile(char* bitString, FILE* output);
int decodeTextFromFile(char* fileName, char* outputFileName);
int encodeTextFromFile(char* fileName, char* outputFileName);
char* find(struct Node* node, struct ListElement *list, char* path);
struct ListElement* createListOfLeafValues(struct Node* encodingTree);
struct Node* findLowestFrequencyLetterAndRemoveFromString(char *string);
struct Node* getLowestFrequencyNodeOrPair(char* text, int currentNodeFrequency, struct Node* smallestFrequencyNode, struct Node* secondSmallestFrequencyNode);

int main(int argc, char *argv[]) {
    int isEncoding;
    int isDecoding;
    char* outputFileName;
    char* fileName;

    int i = 0;
    for (i = 0; i < 256; i++) {
        letterFrequency[i] = 0;
    }

    if (argc == 5) {
        isEncoding = strcmp(argv[1], "-e") == 0 || strcmp(argv[3], "-e") == 0;
        isDecoding = strcmp(argv[1], "-d") == 0 || strcmp(argv[3], "-d") == 0;
        outputFileName = strcmp(argv[1], "-o") == 0 ? argv[2] : argv[4];
        fileName = strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "-d") == 0 ? argv[2] : argv[4];

        if (isEncoding) {
            printf("Encoding.\n");
            return encodeTextFromFile(fileName, outputFileName);
        } else if (isDecoding) {
            printf("Decoding.\n");
            return decodeTextFromFile(fileName, outputFileName);
        }
    } else if (argc == 3) {
        printf("Encoding.\n");
        fileName = argv[1];
        outputFileName = argv[2];
        return encodeTextFromFile(fileName, outputFileName);
    } else if (argc == 4) {
        isDecoding = strcmp(argv[1], "-d") == 0;
        if (isDecoding) {
            printf("Decoding.\n");
            fileName = argv[2];
            outputFileName = argv[3];
            return decodeTextFromFile(fileName, outputFileName);
        }
    }

    return 1;
}

int encodeTextFromFile(char* fileName, char* outputFileName) {
    FILE *file = fopen(fileName, "rb");
    FILE *outputFile = fopen(outputFileName, "w+");
    if (file == NULL) {
        return 1;
    }
    char* fileContents = getFileContents(file);
    char* contentsCopy = malloc(sizeof(char) * (strlen(fileContents) + 1));
    strcpy(contentsCopy, fileContents);
    createLetterStatistics(fileContents);
    struct Node* encodingTree = createEncodingTree(fileContents);
    struct ListElement* list = createListOfLeafValues(encodingTree);
    char* encodedText = encodeText(contentsCopy, list);
    char* encodedTree = getEncodedTree(list);
    char* encodedContent = appendStringToString(encodedTree, encodedText);
    int paddingNeeded = strlen(encodedContent) % 8;
    int paddingSize = 8 - (paddingNeeded == 0 ? 8 : paddingNeeded);
    char* padding = createStringPadding(paddingSize);
    char* encodedContentWithPadding = appendStringToString(padding, encodedContent);
    writeBitsAsCharsToFile(encodedContentWithPadding, outputFile);

    return 0;
}

void createLetterStatistics(char *text) {
    int i = 0;
    while(text[i] != '\0') {
        letterFrequency[(unsigned char) text[i]]++;
        i++;
    }
}

void writeBitsAsCharsToFile(char* bitString, FILE* output) {
    int i;
    char letter;
    char *eightBits;
    for(i = 0; i < strlen(bitString); i+=8) {
        eightBits = malloc(sizeof(char) * 9);
        strncpy(eightBits, bitString + i, 8);
        letter = (char)strtol(eightBits, 0, 2);
        fputc(letter, output);
        fflush(output);
        free(eightBits);
    }

    fclose(output);
}

char* createStringPadding(int paddingSize) {
    int i;
    char* padding = calloc(paddingSize + 1, sizeof(char));

    for(i = 0; i < paddingSize; i++) {
        strcat(padding, "0");
    }

    return padding;
}

int decodeTextFromFile(char* fileName, char* outputFileName) {
    FILE *file = fopen(fileName, "r");
    FILE *outputFile = fopen(outputFileName, "w");
    if (file == NULL) {
        return 1;
    }
    char* bitsFromFile = decodeBitsFromFile(file);
    struct NodeAndContent decodedTreeAndContent = decodeTree(bitsFromFile);
    struct Node* decodedTree = decodedTreeAndContent.node;
    char* fileContent = decodedTreeAndContent.content;
    char* decodedText = decodeText(fileContent, decodedTree);
    fputs(decodedText, outputFile);
    fflush(outputFile);
    fclose(outputFile);

    return 0;
}

char *decodeBitsFromFile(FILE *file) {
    char* bitsFromFile = malloc(sizeof(char));
    bitsFromFile[0] = '\0';
    char letter;

    do {
        letter = fgetc(file);
        if( feof(file) ) {
            break ;
        }
        bitsFromFile = appendStringToString(bitsFromFile, convertCharToBitString(letter));
    }while(1);

    return bitsFromFile;
}

char* getFileContents(FILE *file) {
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *fileContents = calloc(fileSize + 1, sizeof(char));
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
    //free(base);
    //free(appended);
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

int hasFrequenciesLeft() {
    int i;
    for(i = 0; i < 256; i++) {
        if (letterFrequency[i] != 0) {
            return 1;
        }
    }

    return 0;
}

struct Node* createEncodingTree(char *text) {
    struct Node* smallestFrequencyNode, *secondSmallestFrequencyNode, *currentNode, *newParent;
    currentNode = createNewNode();

    while(hasFrequenciesLeft()) {
        smallestFrequencyNode = findLowestFrequencyLetterAndRemoveFromString(text);

        if (currentNode -> leftNode == NULL) {
            if (hasFrequenciesLeft()) {
                secondSmallestFrequencyNode = findLowestFrequencyLetterAndRemoveFromString(text);
                currentNode -> leftNode = createNodePair(smallestFrequencyNode, secondSmallestFrequencyNode);
            } else {
                currentNode -> leftNode = smallestFrequencyNode;
            }
            currentNode -> frequency += currentNode -> leftNode -> frequency;
        } else {
            int combinedFrequency, currentNodeFrequency = currentNode -> frequency;

            if (hasFrequenciesLeft()) {
                secondSmallestFrequencyNode = getLowestFrequencyNode(text);
                currentNode -> rightNode = getLowestFrequencyNodeOrPair(text, currentNodeFrequency, smallestFrequencyNode, secondSmallestFrequencyNode);
            } else {
                currentNode -> rightNode = smallestFrequencyNode;
            }

            combinedFrequency = currentNode -> leftNode -> frequency + currentNode -> rightNode -> frequency;
            currentNode -> frequency = combinedFrequency;

            if (hasFrequenciesLeft()) {
                newParent = createNewNode();
                newParent -> leftNode = currentNode;
                newParent -> frequency = currentNode -> frequency;
                currentNode = newParent;
            }
        }
    }

    if (currentNode -> rightNode == NULL) {
        return currentNode -> leftNode;
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
        letterFrequency[(unsigned char)secondSmallestFrequencyNode -> character] = 0;
        return createNodePair(smallestFrequencyNode, secondSmallestFrequencyNode);
    } else if (smallestFrequency <= currentNodeFrequency) {
        return smallestFrequencyNode;
    } else {
        letterFrequency[(unsigned char)secondSmallestFrequencyNode -> character] = 0;
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

struct Node* findLowestFrequencyLetterAndRemoveFromString(char *string) {
    struct Node* lowestFrequencyNode = getLowestFrequencyNode(string);
    return lowestFrequencyNode;
}

struct Node* getLowestFrequencyNode(char *string) {
    int index, minFrequency = 0;
    struct Node* lowestFrequencyNode = createNewNode();

    for(index = 0; index < 256; index++) {
        if (minFrequency == 0 && letterFrequency[index] != 0) {
            minFrequency = letterFrequency[index];
        } else if (letterFrequency[index] != 0 && minFrequency > letterFrequency[index]) {
            minFrequency = letterFrequency[index];
        }
    }

    if (minFrequency != 0) {
        for(index = 0; index < 256; index++) {
            if (letterFrequency[index] == minFrequency) {
                lowestFrequencyNode -> frequency = minFrequency;
                lowestFrequencyNode -> character = (char)index;
                letterFrequency[index] = 0;

                return lowestFrequencyNode;
            }
        }
    }

    return lowestFrequencyNode;
}