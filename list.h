#ifndef HUFFMANENCODER_LIST_H
#define HUFFMANENCODER_LIST_H

struct ListElement {
    char character;
    char *combination;
    struct ListElement* nextElement;
};

struct Node {
    int frequency;
    char character;
    struct Node* leftNode;
    struct Node* rightNode;
};

char *getEncodedTree(struct ListElement *list);
void encodeText(char *text, struct ListElement* list);
int isLetterInList(struct ListElement* list, char letter);
char* concatenateStrings(char* stringOne, char* stringTwo);
char* find(struct Node* node, struct ListElement *list, char* path);
char* findCharacterCodeInList(struct ListElement* list, char letter);
struct ListElement findLongestPathInListAndRemove(struct ListElement *list);
void pushUniqueLetterAndPathToList(struct ListElement *list, char letter, char *path);
char* createEncodedPairOfNodes(char* leftNodeCombination, char* rightNodeCombination);
struct ListElement findNeighbourPathElementInListAndRemove(char* sequence, struct ListElement *list);
struct ListElement* removeElementFromList(struct ListElement *list, struct ListElement *elementToRemove);

#endif //HUFFMANENCODER_LIST_H
