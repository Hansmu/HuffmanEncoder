#ifndef HUFFMANENCODER_DECODE_H
#define HUFFMANENCODER_DECODE_H

struct Node* decodeTree(char* encodedTree);
char* decodeText(char* string, struct Node* decodedTree);

#endif //HUFFMANENCODER_DECODE_H
