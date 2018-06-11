#include <iostream>

/**
 * Class for various utility functions
 */
class Util
{
public:
    static const int LINE_QTY = 55;

    /**
     * Show repeating lines
     * eg. --------------
     * @param string
     * @param width
     */
    static void showLines(std::string string, int width = LINE_QTY)
    {
        std::string output = "";
        for (int i = 0; i < width; i++)
        {
            output += string;
        }
        std::cout << output << std::endl;
    }

    /**
     * Center the text on screen
     * @param input
     * @param width
     */
    static void centerText(std::string input, int width = LINE_QTY) {
        std::cout << std::string((width - input.length()) / 2, ' ') + input << std::endl;
    }
};