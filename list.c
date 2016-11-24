#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "list.h"

/**
 * SWAPPED OUT ENCODED NODE FOR LISTELEMENT STRUCT
 * CHARACTER = ENCODEDNODE
 * COMBINATION = PARENTSEQUENCE
 * **/

char* getParentFromCombination(char *combination);
char *convertAllLettersToBinary(char *encodedTree);
int getNumberOfUnconvertedLetters(char *encodedTree);
char* setMemoryForConvertedString(char *encodedTree);
int isLetterInList(struct ListElement* list, char letter);
char* createEncodedTreeStringFromTreeList(struct ListElement *list);
struct ListElement findLongestPathInListAndRemove(struct ListElement *list);
char* createEncodedPairOfNodes(char* leftNodeCombination, char* rightNodeCombination);
struct ListElement findNeighbourPathElementInListAndRemove(char* sequence, struct ListElement *list);
struct ListElement* removeElementFromList(struct ListElement *list, struct ListElement *elementToRemove);

char *getEncodedTree(struct ListElement *list) {
    char *encodedTree = createEncodedTreeStringFromTreeList(list);
    return convertAllLettersToBinary(encodedTree);;
}

char *convertAllLettersToBinary(char *encodedTree) {
    int i = 0;
    char *characterAsString = malloc(sizeof(char) * 2);
    char *binaryTree = setMemoryForConvertedString(encodedTree);
    characterAsString[1] = '\0';
    for(i = 0; i < strlen(encodedTree); i++) {
        if (encodedTree[i] != '1' && encodedTree[i] != '0') {
            strcat(binaryTree, convertCharToBitString(encodedTree[i]));
        } else {
            characterAsString[0] = encodedTree[i];
            strcat(binaryTree, characterAsString);
        }
    }

    return binaryTree;
}

char* setMemoryForConvertedString(char *encodedTree) {
    int charAmount = strlen(encodedTree) + 1;
    int numberOfLetters = getNumberOfUnconvertedLetters(encodedTree);
    charAmount -= numberOfLetters;
    charAmount = charAmount +  8 * numberOfLetters;
    char *newString = (char*)calloc(charAmount, sizeof(char));
    return newString;
}

int getNumberOfUnconvertedLetters(char *encodedTree) {
    int i = 0;
    int letterCount = 0;
    for(i = 0; i < strlen(encodedTree); i++) {
        if (encodedTree[i] != '1' && encodedTree[i] != '0') {
            letterCount++;
        }
    }

    return letterCount;
}

char* convertCharToBitString(char character) {
    char *output = malloc(sizeof(char) * 9);
    int i;
    for(i = 0; i < 8; i++) {
        output[7-i] = (character & (1 << i)) ? '1' : '0';
    }
    output[8] = '\0';
    return output;
}

char* createEncodedTreeStringFromTreeList(struct ListElement *list) {
    char *encodedTree;
    struct ListElement *firstNode, *nextElementInList;
    struct ListElement firstElement, secondElement;

    while (list) {
        firstNode = malloc(sizeof(*firstNode));

        firstElement = findLongestPathInListAndRemove(list);
        list = firstElement.nextElement;
        secondElement = findNeighbourPathElementInListAndRemove(firstElement.combination, list);
        list = secondElement.nextElement;
        int isNotPair = secondElement.combination == NULL;

        if (isNotPair) {
            char* firstLetter = firstElement.combination[strlen(firstElement.combination)] == '1' ? firstElement.character : secondElement.character;

            firstNode -> nextElement = NULL;
            firstNode -> character = createEncodedPairOfNodes(firstLetter, "");
            firstNode -> combination = malloc(sizeof(char) * (strlen(firstElement.combination) + 1));
            strcpy(firstNode -> combination, firstElement.combination);

            nextElementInList = list -> nextElement;
            list -> nextElement = firstNode;
            firstNode -> nextElement = nextElementInList;
        } else {
            char* firstLetter = firstElement.combination[strlen(firstElement.combination) - 1] == '1' ? firstElement.character : secondElement.character;
            char* secondLetter = firstElement.combination[strlen(firstElement.combination) - 1] == '1' ? secondElement.character : firstElement.character;

            firstNode -> nextElement = NULL;
            firstNode -> character = createEncodedPairOfNodes(firstLetter, secondLetter);
            firstNode -> combination = getParentFromCombination(firstElement.combination);

            if (list != NULL) {
                nextElementInList = list -> nextElement;
                list -> nextElement = firstNode;
                firstNode -> nextElement = nextElementInList;
            }
        }
    }

    encodedTree = firstNode -> character;

    return encodedTree;
}

char* getParentFromCombination(char *combination) {
    char *parent = malloc(sizeof(char) * (strlen(combination) + 1));
    strncpy(parent, combination, strlen(combination) - 1);
    parent[strlen(combination) - 1] = '\0';
    return parent;
}

char* createEncodedPairOfNodes(char* leftNodeCombination, char* rightNodeCombination) {
    if (rightNodeCombination == "") {
        char *letter = malloc(sizeof(char) * (strlen(leftNodeCombination) + 1));
        strcpy(letter, leftNodeCombination);
        letter[strlen(letter) - 1] = '\0';
        return letter;
    } else {
        int newCombinationLength = strlen(leftNodeCombination) + strlen(rightNodeCombination) + 3 + 1;
        char* encodedPair = malloc(sizeof(char) *  newCombinationLength);

        strcpy(encodedPair, "1\0");
        if (leftNodeCombination[0] != '0' && leftNodeCombination[0] != '1') {
            strcat(encodedPair, "0\0");
        }
        strcat(encodedPair, leftNodeCombination);
        if (rightNodeCombination[0] != '0' && rightNodeCombination[0] != '1') {
            strcat(encodedPair, "0\0");
        }

        strcat(encodedPair, rightNodeCombination);
        encodedPair[strlen(encodedPair)] = '\0';

        return encodedPair;
    }
}

struct ListElement findLongestPathInListAndRemove(struct ListElement *list) {
    int longestElement = 0;
    struct ListElement *currentElement = list;
    struct ListElement foundElement = {
            .combination = NULL
    };

    while(currentElement != NULL) {
        if (strlen(currentElement-> combination) > longestElement) {
            longestElement = strlen(currentElement -> combination);
        }
        currentElement = currentElement -> nextElement;
    }
    currentElement = list;

    while(currentElement != NULL) {
        if (strlen(currentElement -> combination) == longestElement) {
            foundElement.character = calloc(strlen(currentElement -> character) + 1, sizeof(char));
            foundElement.combination = calloc(strlen(currentElement -> combination) + 1, sizeof(char));

            foundElement.character[strlen(currentElement -> character)] = '\0';
            foundElement.combination[strlen(currentElement -> combination)] = '\0';

            strcpy(foundElement.character, currentElement -> character);
            strcpy(foundElement.combination, currentElement -> combination);

            foundElement.nextElement = removeElementFromList(list, currentElement);
            return foundElement;
        }

        currentElement = currentElement -> nextElement;
    }

    return foundElement;
}

struct ListElement findNeighbourPathElementInListAndRemove(char* sequence, struct ListElement *list) {
    int sequenceParentsMatch = 0;
    struct ListElement *currentElement = list;
    struct ListElement returnElement = {
            .combination = NULL
    };

    while (currentElement != NULL) {
        sequenceParentsMatch = !strncmp(currentElement -> combination, sequence, strlen(currentElement -> combination) - 1);
        if (strlen(currentElement -> combination) == strlen(sequence) && sequenceParentsMatch) {
            returnElement.character = calloc(strlen(currentElement -> character) + 1, sizeof(char));
            returnElement.combination = calloc(strlen(currentElement -> combination) + 1, sizeof(char));

            strcpy(returnElement.character, currentElement -> character);
            strcpy(returnElement.combination, currentElement -> combination);

            returnElement.character[strlen(currentElement -> character)] = '\0';
            returnElement.combination[strlen(currentElement -> combination)] = '\0';

            returnElement.nextElement = removeElementFromList(list, currentElement);
            return returnElement;
        }
        currentElement = currentElement -> nextElement;
    }

    return returnElement;
}

int isLetterInList(struct ListElement* list, char letter) {
    while(list != NULL) {
        if (list -> character[0] == letter) {
            return 1;
        }

        list = list -> nextElement;
    }

    return 0;
}

char* findCharacterCodeInList(struct ListElement* list, char letter) {
    if (list -> character[0] == letter) {
        return list -> combination;
    }

    while(list -> nextElement) {
        list = list -> nextElement;

        if (list -> character[0] == letter) {
            return list -> combination;
        }
    }

    return NULL;
}

void pushUniqueLetterAndPathToList(struct ListElement *list, char letter, char *path) {
    if (list -> nextElement == NULL && list -> combination == NULL) {
        list -> combination = path;
        list -> character[0] = letter;
        list -> character[1] = '\0';
    } else if (!isLetterInList(list, letter)) {
        struct ListElement *newElement = malloc(sizeof(*newElement));
        newElement -> character = malloc(sizeof(char) * 2);
        newElement -> character[0] = letter;
        newElement -> character[1] = '\0';
        newElement -> combination = path;
        newElement -> nextElement = list -> nextElement;

        list -> nextElement = newElement;
    }
}

struct ListElement* removeElementFromList(struct ListElement *list, struct ListElement *elementToRemove) {
    struct ListElement *previousElement = NULL, *currentElement, *copyForIterating = list;
    int hasSameCharacter = 0;


    while(copyForIterating != NULL) {
        hasSameCharacter = strcmp(copyForIterating -> character, elementToRemove -> character) == 0;

        if (hasSameCharacter) {
            int isFirstElementInList = previousElement == NULL;

            if (isFirstElementInList) {
                currentElement = list;
                list = list -> nextElement;
                currentElement -> nextElement = NULL;
                free(currentElement -> character);
                free(currentElement -> combination);
                free(currentElement);
                return list;
            } else {
                currentElement = copyForIterating;
                copyForIterating = previousElement;
                copyForIterating -> nextElement = currentElement -> nextElement;
                //free(currentElement);
                return list;
            }
        }

        previousElement = copyForIterating;
        copyForIterating = copyForIterating -> nextElement;
    }

    return NULL;
}
