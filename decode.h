#ifndef HUFFMANENCODER_DECODE_H
#define HUFFMANENCODER_DECODE_H

struct Node* decodeTree(FILE *file);
char* decodeText(FILE *file, struct Node* decodedTree);

#endif //HUFFMANENCODER_DECODE_H
