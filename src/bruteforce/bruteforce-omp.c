#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "../hash/hash.h"
#include "bruteforce-util.c"

#define CHUNK_SIZE 100000

int bruteforce_crack(char *password_hash, char *characters, int password_max_length, int verbose)
{
    int number_of_characters = strlen(characters);
    printf("Brute force of hash: %s\n", password_hash);
    printf("Using %d characters: %s\n", number_of_characters, characters);
    printf("Calculating to a length of %d\n", password_max_length);

    int result = 1;
    int i, j;

    for (i = 1; i <= password_max_length; i++)
    {
        long possibilities = (long)pow(number_of_characters, i);
        if (verbose)
        {
            printf("Now calculating password length of %d, it has %ld possibilities\n", i, possibilities);
        }
        for (j = 0; j < possibilities;)
        {
            if (result == 0)
            {
                // found password early, break out!
                return 0;
            }
            int nextStep;
            if (j + CHUNK_SIZE > possibilities) 
            {
                nextStep = possibilities;
            }
            else 
            {
                nextStep = j + CHUNK_SIZE;
            }
            int k;
            #pragma omp parallel
            {
                #pragma omp for schedule(auto)
                for (k = j; k < nextStep; k++)
                {
                    unsigned char buffer[65];
                    char passwordToTest[i + 1];
                    int val = k;
                    int l;
                    for (l = 0; l < i; l++)
                    {
                        passwordToTest[l] = characters[val % number_of_characters];
                        val = (int)(val / number_of_characters);
                    }
                    passwordToTest[i] = '\0';
                    hash(passwordToTest, buffer);
                    if (!strcmp(password_hash, buffer))
                    {
                        #pragma omp critical
                        {
                            printf("Password found: %s\n", passwordToTest);
                            result = 0;
                        }
                    }
                }
            }
            j=nextStep;
        }
    }

    if (result)
    {
        printf("Password not found.\n");
    }
    return result;
}