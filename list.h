#ifndef HUFFMANENCODER_LIST_H
#define HUFFMANENCODER_LIST_H

struct ListElement {
    char *character;
    char *combination;
    struct ListElement* nextElement;
};

struct Node {
    int frequency;
    char character;
    struct Node* leftNode;
    struct Node* rightNode;
};

struct Node* createNewNode();
char* convertCharToBitString(char character);
unsigned long getEncodedTree(struct ListElement *list, FILE* tempHeaderFile);
char* findCharacterCodeInList(struct ListElement* list, unsigned char letter);
struct Node* createNodePair(struct Node* leftNode, struct Node* rightNode);
void pushUniqueLetterAndPathToList(struct ListElement *list, char letter, char *path);

#endif //HUFFMANENCODER_LIST_H
