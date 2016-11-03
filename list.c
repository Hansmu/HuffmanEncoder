#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "list.h"

struct EncodedNode {
    char *parentSequence;
    char *encodedNode;
    struct EncodedNode* nextNode;
};

char *getEncodedTree(struct ListElement *list) {
    char *encodedTree;
    struct EncodedNode *firstNode = malloc(sizeof(*firstNode));
    struct EncodedNode *secondNode = malloc(sizeof(*secondNode));
    struct ListElement firstElement, secondElement;
    struct ListElement *copyForRemoving, *copyForIterating = list;

    while (copyForIterating -> nextElement) {
        firstElement = findLongestPathInListAndRemove(list);
        list = firstElement.nextElement;
        secondElement = findNeighbourPathElementInListAndRemove(firstElement.combination, list);
        list = secondElement.nextElement;
        copyForIterating = list;
        int hasNoRemainingNodes = secondElement.combination == NULL;

        if (hasNoRemainingNodes) {
            int hasASingleCharacter = firstNode -> parentSequence == NULL && firstNode -> nextNode == NULL;
            if (hasASingleCharacter) {
                firstNode -> parentSequence = malloc(sizeof(char) * (strlen(firstElement.combination) + 1));
                char * charAsPointer = malloc(sizeof(char) * 2);
                charAsPointer[0] = firstElement.character;

                firstNode -> encodedNode = createEncodedPairOfNodes(charAsPointer, "");
                strncpy(firstNode -> parentSequence, firstElement.combination, strlen(firstElement.combination) - 1);
            }
        } else {
            strncpy(firstNode -> parentSequence, firstElement.combination, strlen(firstElement.combination) - 1);
        }

        firstElement = findLongestPathInListAndRemove(list);
        secondElement = findNeighbourPathElementInListAndRemove(firstElement.combination, list);
    }

    return encodedTree;
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
        return letter;
    } else {
        int newCombinationLength = strlen(leftNodeCombination) + strlen(rightNodeCombination) + 1;
        char* encodedPair = malloc(sizeof(char) *  newCombinationLength);

        strcpy(encodedPair, "&(");
        strcat(encodedPair, leftNodeCombination);
        strcat(encodedPair, ",");
        strcat(encodedPair, leftNodeCombination);
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
    struct ListElement *previousElement = NULL, *currentElement;
    int hasMatchingCombination = 0, hasSameCharacter = 0, isMatchingElement = 0;


    while(list != NULL) {
        hasMatchingCombination = strcmp(elementToRemove -> combination, elementToRemove -> combination) == 0;
        hasSameCharacter = list -> character == elementToRemove -> character;
        isMatchingElement = hasMatchingCombination && hasSameCharacter;

        if (isMatchingElement) {
            int isFirstElementInList = previousElement == NULL;

            if (isFirstElementInList) {
                currentElement = list;
                list = list -> nextElement;
                currentElement -> nextElement = NULL;
                free(currentElement);
                return list;
            } else {
                currentElement = list;
                list = previousElement;
                list -> nextElement = currentElement -> nextElement;
                currentElement -> nextElement = NULL;
                free(currentElement);
                return list;
            }
        }

        previousElement = list;
        list = list -> nextElement;
    }

    return NULL;
}
