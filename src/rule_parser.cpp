#include "grammar.hpp"
#include "rule_parser.hpp"
#include <regex>
std::vector<std::string> splitOnPipe(std::string text)
{
    std::vector<std::string> alternatives;
    size_t start = 0;
    while (true)
    {
        size_t pos = text.find('|', start);
        if (pos == std::string::npos)
        {
            alternatives.push_back(text.substr(start));
            return alternatives;
        }
        alternatives.push_back(text.substr(start, pos - start));
        start = pos + 1;
    }
}

void parseRuleLines(const std::string &line, Grammar &grammar)
{
    // This pattern matches one capital letter, followed by -> followed by one or more strings of variables and terminals.
    // It also checks that the empty string, represented by '#', is not concatenated to anything else.
    static const std::regex rulePattern(R"(^[A-Z]->(#|[A-Za-z]+)(\|(#|[A-Za-z]+))*$)");

    if (!std::regex_match(line, rulePattern))
        throw FormatError("Invalid rule syntax.");

    char lhs = line[0];
    std::string rhsText = line.substr(3); // To skip the variable and ->

    for (const std::string &alt : splitOnPipe(rhsText))
    {
        std::string internal = (alt == "#") ? "" : alt;
        grammar.addRule(lhs, internal);
    }
}
