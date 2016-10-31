#ifndef HUFFMANENCODER_LIST_H
#define HUFFMANENCODER_LIST_H

struct ListElement {
    char character;
    char *combination;
    struct ListElement* nextElement;
};

void encodeText(char *text, struct ListElement* list);
int isLetterInList(struct ListElement* list, char letter);
char* concatenateStrings(char* stringOne, char* stringTwo);
char* find(struct Node* node, struct ListElement *list, char* path);
char* findCharacterCodeInList(struct ListElement* list, char letter);
struct ListElement findLongestPathInListAndRemove(struct ListElement *list);
void pushUniqueLetterAndPathToList(struct ListElement *list, char letter, char *path);
char* createEncodedPairOfNodes(char* leftNodeCombination, char* rightNodeCombination);
struct ListElement findNeighbourPathElementInListAndRemove(char* sequence, struct ListElement *list);

#endif //HUFFMANENCODER_LIST_H
