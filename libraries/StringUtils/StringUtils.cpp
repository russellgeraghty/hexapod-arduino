#include "StringUtils.h"

/**
 * Split the given text using the delimiter.
 * @param text The text to split
 * @param splitChar The character which separates the data
 * @return Array of split pieces
 */
String* StringUtils::split(String text, char splitChar) {
	int splitCount = StringUtils::countSplitCharacters(text, splitChar);
	String* returnValue = new String[splitCount];
	int index = -1;
	int index2;

	for(int i = 0; i < splitCount - 1; i++) {
		index = text.indexOf(splitChar, index + 1);
		index2 = text.indexOf(splitChar, index + 1);

		if(index2 < 0) {
			index2 = text.length() - 1;
		}

		returnValue[i] = text.substring(index, index2);
	}

	return returnValue;
}

/** 
 * Count the number of segments in the given text.
 * @param text The text to inspect
 * @param splitChar The character which separates the data
 * @return The number of pieces to expect
 */
int StringUtils::countSplitCharacters(String text, char splitChar) {
	int returnValue = 0;
	int index = 0;

	while (index > -1) {
		Serial.println(index);
		index = text.indexOf(splitChar, index + 1);
		if(index > -1) {
			returnValue+=1;
		}
	}
	return returnValue;
}

