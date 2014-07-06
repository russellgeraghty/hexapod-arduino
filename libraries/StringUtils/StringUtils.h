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
	static int countSplitCharacters(char* text, char splitChar);
	
	/**
	 * Split the given text using the delimiter.
	 * @param text The text to split
	 * @param splitChar The character which separates the data
	 * @param pieces A suitably sized array for the number of pieces
	 * @return the number of pieces actually returned
	 */
	static int split(String text, char splitChar, String* pieces);

	private:
		StringUtils();
};

