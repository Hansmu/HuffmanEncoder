#ifndef HUFFMANENCODER_DECODE_H
#define HUFFMANENCODER_DECODE_H

struct ReturnNodeAndLength {
    struct Node *tree;
    int length;
};

struct ReturnNodeAndLength decodeTree(char* bitString);
char* decodeText(char *bitString, struct Node* decodedTree);

#endif //HUFFMANENCODER_DECODE_H
