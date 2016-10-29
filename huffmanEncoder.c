#import <stdio.h>
#include <string.h>
#include <malloc.h>

struct Node* createNewNode();
struct Node* createEncodingTree(char *text);
void removeCharacterFromString(char* str, char c);
struct Node* getLowestFrequencyNode(char *string);
int getLetterCountFromString(char *string, char letter);
struct Node* findLowestFrequencyLetterAndRemoveFromString(char *string);
struct Node* createNodePair(struct Node* leftNode, struct Node* rightNode);
struct Node* getLowestFrequencyNodeOrPair(char* text, int currentNodeFrequency, struct Node* smallestFrequencyNode, struct Node* secondSmallestFrequencyNode);

typedef struct Node {
    int frequency;
    char character;
    struct Node* leftNode;
    struct Node* rightNode;
};

int main() {
    char word[] = "Mississippi river";
    struct Node* encodingTree = createEncodingTree(word);
    return 0;
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
    node -> character = NULL;
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