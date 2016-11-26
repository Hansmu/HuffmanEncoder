#ifndef HUFFMANENCODER_DECODE_H
#define HUFFMANENCODER_DECODE_H

struct NodeAndContent {
    struct Node *node;
    char *content;
};

struct Node* decodeTree(FILE *tempBitsFile);
void decodeText(FILE *outputFile, FILE *tempBitsFile, struct Node* decodedTree);

#endif //HUFFMANENCODER_DECODE_H
