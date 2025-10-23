#include <ctype.h>
#include <string.h>
#include<stdbool.h>

int strlowercasecompare(const char *word1, const char *word2);

#ifndef strlowcmp
#define strlowcmp(word1, word2) strlowercasecompare((word1), (word2))
#endif

int strlowercasecompare(const char *word1, const char *word2){
    while (*word1 && *word2) {
        if(tolower((unsigned char)*word1) != tolower((unsigned char)*word2)){
            return tolower((unsigned char)*word1) - tolower((unsigned char)*word2);
        }
        word1++;
   	 	word2++;
    }
    return tolower((unsigned char)*word1) - tolower((unsigned char)*word2);
}


