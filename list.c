#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "list.h"

struct EncodedNode {
    char *parentSequence;
    char *encodedNode;
    struct EncodedNode* nextNode;
};

char* charAsPointer(char character);
char* getParentFromCombination(char *combination);
struct EncodedNode* createEncodedPairsFromTree(struct ListElement *list);

char *getEncodedTree(struct ListElement *list) {
    char *encodedTree;
    struct EncodedNode* encodedNodes = createEncodedPairsFromTree(list);
    struct EncodedNode* firstElement, *secondElement;

    while(encodedNodes) {

    }

    return encodedTree;
}

struct EncodedNode* createEncodedPairsFromTree(struct ListElement *list) {
    struct EncodedNode *currentElement, *firstNode, *encodedListHead = NULL;
    struct ListElement firstElement, secondElement;

    while (list) {
        firstNode = malloc(sizeof(*firstNode));

        firstElement = findLongestPathInListAndRemove(list);
        list = firstElement.nextElement;
        secondElement = findNeighbourPathElementInListAndRemove(firstElement.combination, list);
        list = secondElement.nextElement;
        int isNotPair = secondElement.combination == NULL;

        if (isNotPair) {
            char* firstLetter = firstElement.combination[strlen(firstElement.combination)] == '1' ?
                                charAsPointer(firstElement.character) : charAsPointer(secondElement.character);

            firstNode -> nextNode = NULL;
            firstNode -> encodedNode = createEncodedPairOfNodes(firstLetter, "");
            firstNode -> parentSequence = malloc(sizeof(char) * (strlen(firstElement.combination) + 1));
            strcpy(firstNode -> parentSequence, firstElement.combination);

            currentElement -> nextNode = firstNode;
            currentElement = currentElement -> nextNode;
        } else {
            char* firstLetter = firstElement.combination[strlen(firstElement.combination) - 1] == '1' ?
                                charAsPointer(firstElement.character) : charAsPointer(secondElement.character);
            char* secondLetter = firstLetter[0] == firstElement.character ?
                                 charAsPointer(secondElement.character) : charAsPointer(firstElement.character);

            firstNode -> nextNode = NULL;
            firstNode -> encodedNode = createEncodedPairOfNodes(firstLetter, secondLetter);
            firstNode -> parentSequence = getParentFromCombination(firstElement.combination);

            if (encodedListHead == NULL) {
                encodedListHead = firstNode;
                currentElement = encodedListHead;
            } else {
                currentElement -> nextNode = firstNode;
                currentElement = currentElement -> nextNode;
            }
        }
    }

    return encodedListHead;
}

char* getParentFromCombination(char *combination) {
    char *parent = malloc(sizeof(char) * strlen(combination));
    strncpy(parent, combination, strlen(combination) - 1);
    parent[strlen(combination) - 1] = '\0';
    return parent;
}

char* charAsPointer(char character) {
    char *string = malloc(sizeof(char) * 2);
    string[0] = character;
    string[1] = '\0';
    return string;
}

char* concatenateStrings(char* stringOne, char* stringTwo) {
    int newStringLength = strlen(stringOne) + strlen(stringTwo) + 1;
    char* concatenatedString = malloc(sizeof(char) * newStringLength);

    strcpy(concatenatedString, stringOne);
    strcat(concatenatedString, stringTwo);

    return concatenatedString;
}

char* createEncodedPairOfNodes(char* leftNodeCombination, char* rightNodeCombination) {
    if (rightNodeCombination == "") {
        char *letter = malloc(sizeof(char) * strlen(leftNodeCombination) + 1);
        strcpy(letter, leftNodeCombination);
        letter[1] = '\0';
        return letter;
    } else {
        int newCombinationLength = strlen(leftNodeCombination) + strlen(rightNodeCombination) + 1;
        char* encodedPair = malloc(sizeof(char) *  newCombinationLength);

        strcpy(encodedPair, "&(");
        strcat(encodedPair, leftNodeCombination);
        strcat(encodedPair, ",");
        strcat(encodedPair, rightNodeCombination);
        strcat(encodedPair, ")");

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
            foundElement.character = currentElement -> character;
            foundElement.combination = currentElement -> combination;
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
            returnElement.combination = currentElement -> combination;
            returnElement.character = currentElement -> character;
            returnElement.nextElement = removeElementFromList(list, currentElement);

            return returnElement;
        }
        currentElement = currentElement -> nextElement;
    }

    return returnElement;
}

int isLetterInList(struct ListElement* list, char letter) {
    while(list != NULL) {
        if (list -> character == letter) {
            return 1;
        }

        list = list -> nextElement;
    }

    return 0;
}

char* findCharacterCodeInList(struct ListElement* list, char letter) {
    if (list -> character == letter) {
        return list -> combination;
    }

    while(list -> nextElement) {
        list = list -> nextElement;

        if (list -> character == letter) {
            return list -> combination;
        }
    }

    return NULL;
}

void pushUniqueLetterAndPathToList(struct ListElement *list, char letter, char *path) {
    if (list -> nextElement == NULL && list -> combination == NULL) {
        list -> combination = path;
        list -> character = letter;
    } else if (!isLetterInList(list, letter)) {
        struct ListElement *newElement = malloc(sizeof(*newElement));
        newElement -> character = letter;
        newElement -> combination = path;
        newElement -> nextElement = list -> nextElement;

        list -> nextElement = newElement;
    }
}

struct ListElement* removeElementFromList(struct ListElement *list, struct ListElement *elementToRemove) {
    struct ListElement *previousElement = NULL, *currentElement, *copyForIterating = list;
    int hasSameCharacter = 0;


    while(copyForIterating != NULL) {
        hasSameCharacter = copyForIterating -> character == elementToRemove -> character;

        if (hasSameCharacter) {
            int isFirstElementInList = previousElement == NULL;

            if (isFirstElementInList) {
                currentElement = list;
                list = list -> nextElement;
                currentElement -> nextElement = NULL;
                free(currentElement);
                return list;
            } else {
                currentElement = copyForIterating;
                copyForIterating = previousElement;
                copyForIterating -> nextElement = currentElement -> nextElement;
                currentElement -> nextElement = NULL;
                free(currentElement);
                return list;
            }
        }

        previousElement = copyForIterating;
        copyForIterating = copyForIterating -> nextElement;
    }

    return NULL;
}
