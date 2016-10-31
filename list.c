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

    while (list -> nextElement) {
        firstElement = findLongestPathInListAndRemove(list);
        secondElement = findNeighbourPathElementInListAndRemove(firstElement.combination, list);

        if (secondElement == NULL) {
            if (firstNode -> parentSequence == NULL && firstNode -> nextNode == NULL) {
                strncpy(firstNode -> parentSequence, firstElement.combination, strlen(firstElement.combination) - 1);
            }
        } else {
            strncpy(firstNode.parentSequence, firstElement.combination, strlen(firstElement.combination) - 1);
        }

        free(firstElement);
        free(secondElement);

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
    if (rightNodeCharacter == "") {
        char *letter = malloc(sizeof(char) * strlen(leftNodeCombination) + 1);
        strcpy(letter, leftNodeCharacter);
        return letter;
    } else {
        int newCombinationLength = strlen(leftNodeCombination) + strlen(rightNodeCombination) + 1;
        char* encodedPair = malloc(sizeof(char) *  newCombinationLength);

        strcpy(encodedPair, "&(");
        strcat(encodedPair, leftNodeCharacter);
        strcat(encodedPair, ',');
        strcat(encodedPair, rightNodeCharacter);
        strcat(encodedPair, ')');

        return encodedPair;
    }
}

struct ListElement findLongestPathInListAndRemove(struct ListElement *list) {
    int longestElement = 0;
    struct ListElement *listElement = list;

    while(listElement -> nextElement) {
        if (strlen(listElement -> combination) > longestElement) {
            longestElement = strlen(listElement -> combination);
        }
    }

    while(list -> nextElement) {
        if (strlen(list -> combination) == longestElement) {
            struct ListElement foundElement = {
                    .character = list -> character,
                    .combination = list -> combination
            };

            if (list -> nextElement != NULL) {
                listElement = list -> nextElement -> nextElement;
                free(list -> nextElement);
                list -> nextElement = listElement;
            } else {
                free(list);
            }

            return foundElement;
        }
    }

    return NULL;
}

struct ListElement findNeighbourPathElementInListAndRemove(char* sequence, struct ListElement *list) {
    int sequenceParentsMatch;
    struct ListElement *previousElement;

    while (list) {
        sequenceParentsMatch = !strncmp(list -> combination, sequence, strlen(list -> combination) - 1);
        if (strlen(list -> combination) == strlen(sequence) && sequenceParentsMatch) {
            previousElement -> nextElement = previousElement -> nextElement -> nextElement;
            struct ListElement returnElement = {
                    .combination = list -> combination,
                    .character = list -> character
            };

            free(list);
            return returnElement;
        }
        previousElement = list;
        list = list -> nextElement;
    }

    return NULL;
}

int isLetterInList(struct ListElement* list, char letter) {
    while(list -> nextElement) {
        list = list -> nextElement;

        if (list -> character == letter) {
            return 1;
        }
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