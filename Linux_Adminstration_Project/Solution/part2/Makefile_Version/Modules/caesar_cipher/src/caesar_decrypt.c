/* 
===================================================================================================
 Name        : caesar_decrypt.c
 Copyright   : Edges For Training
 Description : A function that decrypts text using the Caesar cipher algorithm.
=================================================================================================== 
*/
#include "caesar_cipher.h"

void caesar_decrypt(char *text, int key) {
    while (*text) {
        if (*text >= 'a' && *text <= 'z') {
            *text = (*text - 'a' + (26 - key)) % 26 + 'a';
        } else if (*text >= 'A' && *text <= 'Z') {
            *text = (*text - 'A' + (26 - key)) % 26 + 'A';
        }
        text++;
    }
}
