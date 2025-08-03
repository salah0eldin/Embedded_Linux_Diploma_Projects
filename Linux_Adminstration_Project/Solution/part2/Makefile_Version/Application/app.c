/* 
===================================================================================================
 Name        : app.c
 Copyright   : Edges For Training
 Description : A program that demonstrates encryption and decryption using Caesar and XOR ciphers.
=================================================================================================== 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "caesar_cipher.h"
#include "xor_cipher.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <caesar_key> <xor_key> <text>\n", argv[0]);
        return 1;
    }

    int caesar_key = atoi(argv[1]); // converts string to int
    char xor_key = argv[2][0];
    char text[256];
    strcpy(text, argv[3]);

    caesar_encrypt(text, caesar_key);
    printf("text encrypted with caesar: %s\n", text);

    caesar_decrypt(text, caesar_key);
    printf("text decrypted with caesar: %s\n", text);

    xor_encrypt(text, xor_key);
    printf("text encrypted with xor: %s\n", text);

    xor_decrypt(text, xor_key);
    printf("text decrypted with xor: %s\n", text);

    return 0;
}