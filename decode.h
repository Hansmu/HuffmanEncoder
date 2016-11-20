#ifndef HUFFMANENCODER_DECODE_H
#define HUFFMANENCODER_DECODE_H

struct NodeAndContent {
    struct Node *node;
    char *content;
};

struct NodeAndContent decodeTree(char* encodedBits);
char* decodeText(char *bitString, struct Node* decodedTree);

#endif //HUFFMANENCODER_DECODE_H
