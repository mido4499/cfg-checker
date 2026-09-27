#pragma once
#include "grammar.hpp"
#include <string>
#include <vector>

std::vector<std::string> splitOnPipe(std::string text);
void parseRuleLines(const std::string &line, Grammar &grammar);
