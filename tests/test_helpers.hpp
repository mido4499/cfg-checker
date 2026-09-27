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
