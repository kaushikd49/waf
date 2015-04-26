/*
 * foo.c
 *
 *  Created on: Feb 5, 2015
 *      Author: kaushik
 */

#include<stdio.h>
#include <limits.h>

#define NUMBER 1
#define ALPHABET 2
#define SPL_SYMBOL 4

int getCharset(char c) {
	if (c >= 48 && c <= 57)
		return NUMBER;
	else if ((c >= 65 && c <= 90) || c >= 97 && c <= 122)
		return ALPHABET;
	else
		return SPL_SYMBOL;
}

int getAllCharset(char *str, int* charsetList) {
	int i = 0;
	
	int charSetsCovered = 0; // lower 3 bits represent charsets present
	while (str[i]) {
		int charSet = getCharset(str[i]);
		charSetsCovered |= charSet;

		if (charSetsCovered == 7)
			break;
		i++;
	}

	int j = 0;
	int myarr[] = {1,2,4};
	while (j < 3) {
		if (charSetsCovered & myarr[j] ) {
			charsetList[j] = 1;
		} else {
			charsetList[j] = charsetList[j] ==1 ? 1: 0;
		}
		j++;
	}

}

int main() {

	int csets[3];
	getAllCharset("abc8", csets);
	printf(
			"1 stands for Numbers, 2 for alphabets, 4 for specialSymbols, -1 for none of the above\n");
	printf("charsets found are as follows:");
	int i = 0;
	for (;i < 3; i++) {
		printf("\n%d", csets[i]);
	}

	return 0;
}
