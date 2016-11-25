#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "list.h"
#include "decode.h"

int letterFrequency[256];

struct Node* createEncodingTree();
char *decodeBitsFromFile(FILE *file);
void createLetterStatistics(FILE *file);
char* createStringPadding(int paddingSize);
struct Node* getLowestFrequencyNode();
int encodeTextAndWriteToTempFile(FILE *file, FILE *temp, struct ListElement* list);
char* appendStringToString(char* base, char* appended);
int decodeTextFromFile(char* fileName, char* outputFileName);
int encodeTextFromFile(char* fileName, char* outputFileName);
char* find(struct Node* node, struct ListElement *list, char* path);
struct ListElement* createListOfLeafValues(struct Node* encodingTree);
struct Node* findLowestFrequencyLetterAndRemoveFromStatistics();
void writeBitsAsCharsToFile(char* padding, char* encodedTree, FILE* output, FILE *tempContent);
struct Node* getLowestFrequencyNodeOrPair(int currentNodeFrequency, struct Node* smallestFrequencyNode, struct Node* secondSmallestFrequencyNode);

int main(int argc, char *argv[]) {
    int isEncoding;
    int isDecoding;
    char* outputFileName;
    char* fileName;

    argc = 3;
    if (argc == 4) {
        argv[1] = "-d";
        argv[2] = "C:\\Users\\Hans\\Documents\\Programming\\C\\huffmanEncoder\\output.txt";
        argv[3] = "C:\\Users\\Hans\\Documents\\Programming\\C\\huffmanEncoder\\decoded.txt";
    } else if (argc = 3) {
        argv[1] = "-d";
        argv[1] = "C:\\Users\\Hans\\Documents\\Programming\\C\\huffmanEncoder\\input.txt";
        argv[2] = "C:\\Users\\Hans\\Documents\\Programming\\C\\huffmanEncoder\\output.txt";
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
    FILE *tempText = fopen("temp.txt", "w+");

    if (file == NULL) {
        return 1;
    }

    createLetterStatistics(file);
    struct Node* encodingTree = createEncodingTree();
    struct ListElement* list = createListOfLeafValues(encodingTree);
    int contentLength = encodeTextAndWriteToTempFile(file, tempText, list);
    char* encodedTree = getEncodedTree(list);
    int paddingNeeded = (contentLength + strlen(encodedTree)) % 8;
    int paddingSize = 8 - (paddingNeeded == 0 ? 8 : paddingNeeded);
    char* padding = createStringPadding(paddingSize);
    fflush(tempText);
    writeBitsAsCharsToFile(padding, encodedTree, outputFile, tempText);
    fclose(tempText);
    fclose(tempText);
    return 0;
}

void createLetterStatistics(FILE *file) {
    int letter, i;

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
    }while(1);
}

void writeBitsAsCharsToFile(char* padding, char* encodedTree, FILE* output, FILE *tempContent) {
    FILE *tempFullContent = fopen("tempTogether.txt", "w+");
    char letter, *eightBits;
    rewind(tempContent);

    fputs(padding, tempFullContent);
    fputs(encodedTree, tempFullContent);


    do {
        letter = fgetc(tempContent);
        if (letter == EOF) {
            break;
        } else {
            fputc(letter, tempFullContent);
        }
    }while(1);

    rewind(tempFullContent);

    while(!feof(tempFullContent)) {
        eightBits = calloc(9, sizeof(char));
        fgets(eightBits, 9, tempFullContent);
        letter = (char)strtol(eightBits, 0, 2);
        fputc(letter, output);
        fflush(output);
        free(eightBits);
    }

    fclose(tempFullContent);
    remove("tempTogether.txt");
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

struct ListElement* createListOfLeafValues(struct Node* encodingTree) {
    struct ListElement *list = malloc(sizeof(*list));
    list -> combination = NULL;
    list -> character = malloc(sizeof(char) * 2);
    list -> character[1] = '\0';
    list -> nextElement = NULL;

    find(encodingTree, list, "");
    return list;
}

int encodeTextAndWriteToTempFile(FILE* file, FILE *temp, struct ListElement* list) {
    int length = 0;
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