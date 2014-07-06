#include "StringUtils.h"

/**
 * Split the given text using the delimiter.
 * @param text The text to split
 * @param splitChar The character which separates the data
 * @return Array of split pieces
 */
int StringUtils::split(char* text, char splitChar, String* pieces) {
	char *p = strtok(text, splitChar);
	int count = 0;
	while (p) {
		pieces[count] = p;
		Serial.println(p);
		count++;
		p = strtok(NULL, splitChar);
	}
	return count;
}

/** 
 * Count the number of delimiters in the given text.
 * @param text The text to inspect
 * @param splitChar The character which separates the data
 * @return The number of pieces to expect
 */
int StringUtils::countSplitCharacters(String text, char splitChar) {
	int returnValue = 0;
	int index = 0;

	while (index > -1) {
		index = text.indexOf(splitChar, index + 1);
		if(index > -1) {
			returnValue += 1;
		}
	}
	return returnValue;
}

