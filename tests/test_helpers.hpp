#pragma once
#include <iostream>
inline int failures = 0;

inline void expectThrow(const std::string &name, std::function<void()> fn)
{
    try
    {
        fn();
        std::cerr << "FAIL (expected throw): " << name << '\n';
        failures++;
    }
    catch (const std::exception &)
    {
        std::cout << "PASS: " << name << '\n';
    }
}

inline void expectNoThrow(const std::string &name, std::function<void()> fn)
{
    try
    {
        fn();
        std::cerr << "PASS: " << name << '\n';
    }
    catch (const std::exception &e)
    {
        std::cout << "FAILURE (expected no throw): " << name << '\n'
                  << " (" << e.what() << ")\n";
        failures++;
    }
}

inline void expectRules(const std::string &name, const Grammar &g,
                        const std::map<char, std::set<std::string>> &expected)
{
    if (g.rules() == expected)
        std::cout << "PASS: " << name << '\n';
    else
    {
        std::cout << "FAIL: " << name << '\n';
        failures++;
    }
}

inline void printRules(Grammar &g)
{
    for (auto const &[variable, alternatives] : g.rules())
    {
        size_t index = 0;
        std::cout << variable << ": ";
        for (auto const &alternative : alternatives)
        {
            if (index != 0)
                std::cout << '|';
            if (alternative == "")
                std::cout << '#';
            std::cout << alternative;
            index++;
        }

        std::cout << std::endl;
    }
}

inline void printStartVariable(Grammar &g)
{
    std::cout << g.startSymbol() << '\n';
}