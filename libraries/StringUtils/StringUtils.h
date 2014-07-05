#include <Arduino.h>

class StringUtils
{
   public:
	/** 
	 * Count the number of segments in the given text.
	 * @param text The text to inspect
	 * @param splitChar The character which separates the data
	 * @return The number of pieces to expect
	 */
	static int countSplitCharacters(String text, char splitChar);
	
	/**
	 * Split the given text using the delimiter.
	 * @param text The text to split
	 * @param splitChar The character which separates the data
	 * @return Array of split pieces
	 */
	static String* split(String text, char splitChar);

	private:
		StringUtils();
};

