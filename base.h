#ifndef BASE_H
#define BASE_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string>
#include <assert.h>

#define ATLASSERT assert

#define MIN_CODE_LEN    9                   /* min # bits in a code word */
#define MAX_CODE_LEN    20

#define CURRENT_MAX_CODES(x)     (1UL << (x))

#define FIRST_CODE      (1 << CHAR_BIT)     /* value of 1st string code */

#define BITS 17                   /* Setting the number of bits to 12, 13*/
#define HASHING_SHIFT (BITS-8)    /* or 14 affects several constants.    */
#define MAX_VALUE (1 << BITS) - 1 /* Note that MS-DOS machines need to   */
#define MAX_CODE MAX_VALUE - 1    /* compile their code in large model if*/
/* 14 bits are selected.               */

#if BITS == 20
#define TABLE_SIZE 1048583
#elif BITS == 19
#define TABLE_SIZE 524309
#elif BITS == 18
#define TABLE_SIZE 262147
#elif BITS == 17
#define TABLE_SIZE 131101
#elif BITS == 16
#define TABLE_SIZE 65543
#elif BITS == 15
#define TABLE_SIZE 32797
#elif BITS == 14
#define TABLE_SIZE 18041        /* The string table size needs to be a */
/* prime number that is somewhat larger*/
#elif BITS == 13                  /* than 2**BITS.                       */
#define TABLE_SIZE 9029
#elif BITS == 12
#define TABLE_SIZE 5021
#else
#error define smaller or bigger table sizes
#endif


class Base {
protected:
    FILE *lzw_file_old; //TOTO tiez pouzivam uz len lokalne mozes to zmazat alebo presunut k sebe


    int *code_value;                  /* This is the code value array        */
    unsigned int *prefix_code;        /* This array holds the prefix codes   */
    unsigned char *append_character;  /* This array holds the appended chars */
    unsigned char decode_stack[4000]; /* This array holds the decoded string */
    unsigned char CUR_BITS;           /* ~nab: added for variable bit size */

    /* -1 return indicates either EOF or some IO error */
    virtual int getc_src() {
        ATLASSERT(io_file);
        int ch = getc(io_file);
        if(EOF == ch)
            return -1;

        u_io++;
        return ch;
    }
    virtual int getc_comp() {
        ATLASSERT(lzw_file_old);
        int ch = getc(lzw_file_old);
        if(EOF == ch)
            return -1;

        u_comp++;
        return ch;
    }
    virtual int putc_comp(int ch) {
        ATLASSERT(lzw_file_old);
        ATLASSERT(ch >= 0 && ch < 256);
        int ret = putc(ch, lzw_file_old);

        if(ret != EOF) {
            ATLASSERT(ret == ch);
            u_comp++;
        }
        else
            ret = -1;

        return ret;
    }
    virtual int putc_out(int ch) {
        ATLASSERT(io_file);
        ATLASSERT(ch >= 0 && ch < 256);
        int ret = putc(ch, io_file);

        if(ret != EOF)
            u_io++;
        else
            ret = -1;

        return ret;
    }

    FILE* io_file; //JA TUTO PROPERTY NEPOTREBUJEM TAK SI JU PRESUN DO DEKODERA A ZMAZ ODTIALTO
    int io_error;
public:
    unsigned long u_io, u_comp;
    Base() {
        code_value=(int*)malloc(TABLE_SIZE*sizeof(int));
        prefix_code=(unsigned int*)malloc(TABLE_SIZE*sizeof(unsigned int));
        append_character=(unsigned char*)malloc(TABLE_SIZE*sizeof(unsigned char));

        u_comp = 0;
        u_io = 0;
    }
};

#endif // BASE_H

