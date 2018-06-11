#include <iostream>

/**
 * Class for logging messages to the console
 */
class Log
{
public:
    static void logError(std::string message)
    {
        std::cout << "ERROR: " << message << std::endl;
    }

    static void logSuccess(std::string message)
    {
        std::cout << "SUCCESS: " << message << std::endl;
    }
};