/* 
===================================================================================================
 Name        : caesar_encrypt.c
 Copyright   : Edges For Training
 Description : A function that encrypts text using the Caesar cipher algorithm.
=================================================================================================== 
*/
#include "caesar_cipher.h"

void caesar_encrypt(char *text, int key) {
    while (*text) {
        if (*text >= 'a' && *text <= 'z') {
            *text = (*text - 'a' + key) % 26 + 'a';
        } else if (*text >= 'A' && *text <= 'Z') {
            *text = (*text - 'A' + key) % 26 + 'A';
        }
        text++;
    }
}
