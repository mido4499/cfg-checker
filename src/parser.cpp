#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>
#include <set>

struct FormatError : std::runtime_error
{
    using std::runtime_error::runtime_error;
};
bool isTerminal;

bool isCapital(char c)
{
    return ((c >= 'A' && c <= 'Z'));
}

bool isSmall(char c)
{
    return ((c >= 'a' && c <= 'z'));
}

std::vector<char> parseLetters(const std::string &line)
{
    if (line.empty())
        throw FormatError("Input is empty");

    std::vector<char> letters;
    std::set<char> seen;

    for (size_t i = 0; i < line.size(); ++i)
    {
        char c = line[i];
        if (i % 2 == 0)
        {
            if (!isTerminal)
            {
                if (!isCapital(c))
                    throw FormatError("Position: " + std::to_string(i + 1) + ": expected an uppercase letter, got '" + std::string(1, c) + "'");
            }
            if (isTerminal)
            {
                if (!isSmall(c))
                    throw FormatError("Position: " + std::to_string(i + 1) + ": expected a lowercase letter, got '" + std::string(1, c) + "'");
            }

            if (!seen.insert(c).second)
            {
                throw FormatError("Position: " + std::to_string(i + 1) + ": a duplicate letter '" + std::string(1, c) + "'");
            }
            letters.push_back(c);
        }
        else
        {
            if (c != ',')
                throw FormatError("Position: " + std::to_string(i + 1) + ": expected a comma, got '" + std::string(1, c) + "'");
        }
    }

    if (line.size() % 2 == 0)
        throw FormatError("Input ends with a comma, expected a variable after it.");

    return letters;
}

int main()
{
    // Inputting Variables
    std::string line;
    std::cout << "Step 1: Enter the variables separated by commas, where the first variable is the start variable. All variable names are capital letters. \n";
    std::getline(std::cin, line);
    if (!line.empty() && line.back() == '\r')
        line.pop_back();

    try
    {
        std::vector<char> variables = parseLetters(line);
        std::cout << "Parsed " << variables.size() << " letters: ";
        for (char c : variables)
            std::cout << ' ' << c;
        std::cout << '\n';
    }
    catch (const FormatError &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    // Inputting Terminals
    isTerminal = true;
    std::cout << "Step 2: Enter the terminals separated by commas. All terminal names are small letters. \n";
    line.clear();
    std::getline(std::cin, line);
    if (!line.empty() && line.back() == '\r')
        line.pop_back();

    try
    {
        std::vector<char> terminals = parseLetters(line);
        std::cout << "Parsed " << terminals.size() << " letters: ";
        for (char c : terminals)
            std::cout << ' ' << c;
        std::cout << '\n';
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}