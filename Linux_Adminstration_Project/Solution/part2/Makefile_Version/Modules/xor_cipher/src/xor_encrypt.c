/* 
===================================================================================================
 Name        : xor_encrypt.c
 Copyright   : Edges For Training
 Description : A function that encrypts text using the XOR cipher algorithm.
=================================================================================================== 
*/
#include "xor_cipher.h"

void xor_encrypt(char *text, char key) {
    while (*text) {
        *text = *text ^ key;
        text++;
    }
}
