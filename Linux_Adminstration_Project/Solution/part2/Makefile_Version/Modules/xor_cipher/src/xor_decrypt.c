/* 
===================================================================================================
 Name        : xor_decrypt.c
 Copyright   : Edges For Training
 Description : A function that decrypts text using the XOR cipher algorithm.
=================================================================================================== 
*/
#include "xor_cipher.h"

void xor_decrypt(char *text, char key) {
    while (*text) {
        *text = *text ^ key;
        text++;
    }
}
