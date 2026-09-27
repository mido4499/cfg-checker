#include "../src/rule_parser.hpp"
#include "../src/grammar.hpp"
#include "test_helpers.hpp"

int main()
{
    expectNoThrow("Invalid characters", []
                  { Grammar g({'S', 'B'}, {'b', 'a'}, 'S'); parseRuleLines("S->bBa", g); });
    expectNoThrow("Alternatives with |", []
                  { Grammar g({'S', 'B'}, {'b', 'a'}, 'S'); parseRuleLines("S->aBb|ab", g); });
    expectNoThrow("Isolated epsilon", []
                  { Grammar g({'S', 'B'}, {'b'}, 'S'); parseRuleLines("S->#", g); if (g.rules().at('S').count("") == 0) throw std::runtime_error("Epsilon is not stored internally"); });

    expectThrow("More than one variable on the LHS", []
                { Grammar g({'S', 'B'}, {'b', 'a'}, 'S'); parseRuleLines("SB->aBb|ab", g); });
    expectThrow("Invalid LHS", []
                { Grammar g({'S', 'B'}, {'b'}, 'S'); parseRuleLines("b->@B", g); });
    expectThrow("Invalid characters", []
                { Grammar g({'S', 'B'}, {'b'}, 'S'); parseRuleLines("S->@B", g); });
    expectThrow("Invalid syntax", []
                { Grammar g({'S', 'B'}, {'b'}, 'S'); parseRuleLines("SbB", g); });
    expectThrow("Empty Alternative", []
                { Grammar g({'S', 'B'}, {'b'}, 'S'); parseRuleLines("S -> bB||b", g); });
    expectThrow("Concatenated epsilon", []
                { Grammar g({'S', 'B'}, {'b'}, 'S'); parseRuleLines("S->#B", g); });

    std::cout << "Number of failures: " << failures << '\n';
}