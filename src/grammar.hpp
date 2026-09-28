#pragma once
#include <map>
#include <set>
#include <string>

struct GrammarError : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

struct FormatError : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class Grammar
{
public:
    Grammar(std::set<char> variables, std::set<char> terminals, char start);
    void addRule(char lhs, const std::string &rhs);

    const std::set<char> variables() const;
    const std::set<char> terminals() const;
    char startSymbol() const;
    const std::map<char, std::set<std::string>> &rules() const;

    Grammar CNFConvert() const;
    void eliminateEpsilonRules();

private:
    std::set<char> variables_;
    std::set<char> terminals_;
    char start_;
    std::map<char, std::set<std::string>> rules_;

    void addStartVariable();
    std::set<std::string> getAllOccurences(std::string rhs, const std::set<char> &targets);
    void getAllOcurrencesRecursive(const std::string &str, const std::set<char> &targets, int index, std::string current, std::set<std::string> &results);
    void eliminateUnitRules();
    void breakLongRules();
    char freshVariable() const;
};