#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "list.h"
#include "decode.h"

int letterFrequency[256];

struct Node* createEncodingTree();
struct Node* getLowestFrequencyNode();
unsigned long createLetterStatistics(FILE *file);
char* createStringPadding(int paddingSize);
void decodeBitsFromFileAndWriteToTemp(FILE *file, FILE *tempBitsFile);
int decodeTextFromFile(char* fileName, char* outputFileName);
int encodeTextFromFile(char* fileName, char* outputFileName);
struct Node* findLowestFrequencyLetterAndRemoveFromStatistics();
char* find(struct Node* node, struct ListElement *list, char* path);
void copyFileContentToFile(FILE *sourceFile, FILE *destinationFile);
struct ListElement* createListOfLeafValues(struct Node* encodingTree);
unsigned long encodeTextAndWriteToTempFile(FILE *file, FILE *temp, struct ListElement* list);
void writeBitsAsCharsToFile(char* padding, FILE *tempHeader, FILE* output, FILE *tempContent);
struct Node* getLowestFrequencyNodeOrPair(int currentNodeFrequency, struct Node* smallestFrequencyNode, struct Node* secondSmallestFrequencyNode);

int main(int argc, char *argv[]) {
    int isEncoding;
    int isDecoding;
    char* outputFileName;
    char* fileName;

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
    FILE *outputFile = fopen(outputFileName, "wb");
    FILE *tempText = fopen("temp.txt", "w+");
    FILE *tempHeader = fopen("tempHeader.txt", "w+");

    if (file == NULL) {
        return 1;
    }

    unsigned long initialFileSize = createLetterStatistics(file);
    struct Node* encodingTree = createEncodingTree();
    struct ListElement* list = createListOfLeafValues(encodingTree);
    unsigned long contentLength = encodeTextAndWriteToTempFile(file, tempText, list);
    unsigned long headerLength = getEncodedTree(list, tempHeader);
    int paddingNeeded = (contentLength + headerLength) % 8;
    int paddingSize = 8 - (paddingNeeded == 0 ? 8 : paddingNeeded);
    char* padding = createStringPadding(paddingSize);
    fflush(tempText);
    fflush(tempHeader);

    if (initialFileSize > ((contentLength + headerLength + paddingSize) / 8)) {
        writeBitsAsCharsToFile(padding, tempHeader, outputFile, tempText);
    } else {
        fclose(outputFile);
        remove(outputFileName);
    }

    fclose(tempText);
    fclose(tempHeader);
    remove("temp.txt");
    remove("tempHeader.txt");

    return 0;
}

unsigned long createLetterStatistics(FILE *file) {
    int letter, i;
    unsigned long size = 0;

    for (i = 0; i < 256; i++) {
        letterFrequency[i] = 0;
    }

    do {
        letter = fgetc(file);
        if(feof(file))
        {
            break ;
        }
        letterFrequency[letter]++;
        size++;
    }while(1);

    return size;
}

void writeBitsAsCharsToFile(char* padding, FILE *tempHeader, FILE* output, FILE *tempContent) {
    FILE *tempFullContent = fopen("tempTogether.txt", "wb+");
    char letter, *eightBits;

    fputs(padding, tempFullContent);
    copyFileContentToFile(tempHeader, tempFullContent);
    copyFileContentToFile(tempContent, tempFullContent);
    fflush(tempFullContent);

    rewind(tempFullContent);

    while(!feof(tempFullContent) && !ferror(tempFullContent)) {
        eightBits = calloc(9, sizeof(char));
        fgets(eightBits, 9, tempFullContent);
        letter = (char)strtol(eightBits, 0, 2);

        if (strcmp(eightBits, "") != 0) {
            fputc(letter, output);
        }

        fflush(output);
        free(eightBits);
    }

    fclose(tempFullContent);
    fflush(output);
    remove("tempTogether.txt");
    fclose(output);
}

void copyFileContentToFile(FILE *sourceFile, FILE *destinationFile) {
    char letter;
    rewind(sourceFile);

    do {
        letter = (char)fgetc(sourceFile);
        if (letter == EOF) {
            break;
        } else {
            fputc(letter, destinationFile);
        }
    }while(1);
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
    FILE *file = fopen(fileName, "rb");
    FILE *outputFile = fopen(outputFileName, "wb");
    FILE *pureBitsFile = fopen("tempBits.txt", "wb+");

    if (file == NULL) {
        return 1;
    }
    decodeBitsFromFileAndWriteToTemp(file, pureBitsFile);
    fflush(pureBitsFile);
    struct Node* decodedTree = decodeTree(pureBitsFile);
    decodeText(outputFile, pureBitsFile, decodedTree);
    fflush(outputFile);
    fclose(outputFile);
    fclose(pureBitsFile);
    remove("tempBits.txt");

    return 0;
}

void decodeBitsFromFileAndWriteToTemp(FILE *file, FILE *tempBitsFile) {
    char letter;

    do {
        letter = (char)fgetc(file);

        if(feof(file)) {
            break ;
        }

        fputs(convertCharToBitString(letter), tempBitsFile);
        fflush(tempBitsFile);
    }while(1);
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

unsigned long encodeTextAndWriteToTempFile(FILE* file, FILE *temp, struct ListElement* list) {
    unsigned long length = 0;
    rewind(file);

    do{
        unsigned char letter = (unsigned char)fgetc(file);
        if (feof(file)) {
            break;
        }
        char* encodedLetter = findCharacterCodeInList(list, letter);
        fputs(encodedLetter, temp);
        length += strlen(encodedLetter);
    }while(1);

    return length;
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

struct Node* createEncodingTree() {
    struct Node* smallestFrequencyNode, *secondSmallestFrequencyNode, *currentNode, *newParent;
    currentNode = createNewNode();

    while(hasFrequenciesLeft()) {
        smallestFrequencyNode = findLowestFrequencyLetterAndRemoveFromStatistics();

        if (currentNode -> leftNode == NULL) {
            if (hasFrequenciesLeft()) {
                secondSmallestFrequencyNode = findLowestFrequencyLetterAndRemoveFromStatistics();
                currentNode -> leftNode = createNodePair(smallestFrequencyNode, secondSmallestFrequencyNode);
            } else {
                currentNode -> leftNode = smallestFrequencyNode;
            }
            currentNode -> frequency += currentNode -> leftNode -> frequency;
        } else {
            int combinedFrequency, currentNodeFrequency = currentNode -> frequency;

            if (hasFrequenciesLeft()) {
                secondSmallestFrequencyNode = getLowestFrequencyNode();
                currentNode -> rightNode = getLowestFrequencyNodeOrPair(currentNodeFrequency, smallestFrequencyNode, secondSmallestFrequencyNode);
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

struct Node* getLowestFrequencyNodeOrPair(int currentNodeFrequency, struct Node* smallestFrequencyNode, struct Node* secondSmallestFrequencyNode) {
    int smallestFrequency = smallestFrequencyNode -> frequency;
    int secondSmallestFrequency = secondSmallestFrequencyNode -> frequency;

    if (smallestFrequency <= currentNodeFrequency && secondSmallestFrequency <= currentNodeFrequency) {
        letterFrequency[(unsigned char)secondSmallestFrequencyNode -> character] = 0;
        return createNodePair(smallestFrequencyNode, secondSmallestFrequencyNode);
    } else {
        return smallestFrequencyNode;
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

struct Node* findLowestFrequencyLetterAndRemoveFromStatistics() {
    struct Node* lowestFrequencyNode = getLowestFrequencyNode();
    letterFrequency[(unsigned char)lowestFrequencyNode -> character] = 0;
    return lowestFrequencyNode;
}

struct Node* getLowestFrequencyNode() {
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

                return lowestFrequencyNode;
            }
        }
    }

    return lowestFrequencyNode;
}