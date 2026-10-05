#include "../src/grammar.hpp"
#include "test_helpers.hpp"

void testGrammarClass()
{
    // Grammar Constructor
    expectNoThrow("valid grammar", []
                  { Grammar g({'S', 'A', 'T'}, {'a', 'b'}, 'S'); });
    expectThrow("Start variable not in set V", []
                { Grammar g({'S'}, {'a', 'b'}, 'T'); });
    expectThrow("Same character in V and Sigma", []
                { Grammar g({'S', 'a'}, {'a', 'b'}, 'S'); });
    expectThrow("Empty V", []
                { Grammar g({}, {'a', 'b'}, 'V'); });

    // addRule()
    expectNoThrow("valid grammar", []
                  { Grammar g({'S', 'A', 'T'}, {'a', 'b'}, 'S'); g.addRule('S', "aSb"); });
    expectNoThrow("epsilon as empty string", []
                  {Grammar g({'S', 'A', 'T'}, {'a', 'b'}, 'S'); g.addRule('S', ""); });

    expectThrow("Variable not in V", []
                {Grammar g({'S', 'A', 'T'}, {'a', 'b'}, 'S'); g.addRule('X', "ab"); });
    expectThrow("Terminal not in Sigma", []
                {Grammar g({'S', 'A', 'T'}, {'a', 'b'}, 'S'); g.addRule('S', "cc"); });
    expectThrow("duplicate rule", []
                {Grammar g({'S', 'A', 'T'}, {'a', 'b'}, 'S'); g.addRule('S', ""); g.addRule('S', ""); });

    expectNoThrow("two variables on rhs", []
                  {Grammar g({'S', 'A', 'T'}, {'a', 'b'}, 'S'); g.addRule('S', "AT"); });

    std::cout << "Number of failures: " << failures << '\n';

    // Final test to demonstrate all the rules are organized by the LHS variable.
    Grammar g({'S', 'A', 'T'}, {'a', 'b'}, 'S');
    g.addRule('S', "AT");
    g.addRule('A', "ab");
    g.addRule('T', "Sb");
    for (const auto &[key, inner_set] : g.rules())
    {
        std::cout << key << '\n';
        std::cout << "contains: \n";
        for (const auto &element : inner_set)
        {
            std::cout << element << " ";
        }
        std::cout << '\n';
    }
}

void testCNFConvert()
{
    Grammar g({'S', 'A', 'B'}, {'a', 'b'}, 'S');
    g.addRule('S', "aSb");
    g.addRule('S', "AB");
    g.addRule('S', "");
    g.addRule('A', "");
    g.addRule('B', "b");

    printRules(g);
    std::cout << '\n';
    Grammar cnf = g.CNFConvert();
    printRules(cnf);
    printStartVariable(cnf);
}
int main()
{
    testCNFConvert();
}

/*
 * To perform the following tests, first move eliminateEpsilonRules(), eliminateUnitRules(),
 * and breakLongRules() in grammar.hpp from private to public. Then, move these methods to the
 * top of the file to be defined in main(), and call the one you want to test in main().
 */

// void testEliminateEpsilonRules()
// {
//     // T1 No epsilon rules
//     Grammar g({'S', 'A'}, {'a'}, 'S');
//     g.addRule('S', "aA");
//     g.addRule('A', "a");
//     g.eliminateEpsilonRules();
//     expectRules("no epsilon rules", g, {{'S', {"aA"}}, {'A', {"a"}}});

//     // T2 Basic removal
//     Grammar g2({'S', 'A', 'B'}, {'a', 'b'}, 'S');
//     g2.addRule('S', "AB");
//     g2.addRule('A', "a");
//     g2.addRule('A', "");
//     g2.addRule('B', "b");
//     g2.eliminateEpsilonRules();
//     expectRules("basic removal", g2, {{'S', {"AB", "B"}}, {'A', {"a"}}, {'B', {"b"}}});

//     // T3 Two occurences of the same nullable variable
//     Grammar g3({'S', 'A'}, {'a', 'b'}, 'S');
//     g3.addRule('S', "AbA");
//     g3.addRule('A', "a");
//     g3.addRule('A', "");
//     g3.eliminateEpsilonRules();
//     expectRules("two occurences of the same nullable variable", g3, {{'S', {"AbA", "bA", "Ab", "b"}}, {'A', {"a"}}});

//     // T4 Two different nullable variables in one rule
//     Grammar g4({'S', 'A', 'X'}, {'x', 'a'}, 'S');
//     g4.addRule('S', "AXA");
//     g4.addRule('A', "a");
//     g4.addRule('A', "");
//     g4.addRule('X', "x");
//     g4.addRule('X', "");
//     g4.eliminateEpsilonRules();
//     expectRules("two different nullable variables", g4, {{'S', {"AXA", "AX", "AA", "A", "XA", "X", ""}}, {'A', {"a"}}, {'X', {"x"}}});

//     // T5 Chain propagation across rounds
//     Grammar g5({'S', 'A', 'B'}, {}, 'S');
//     g5.addRule('S', "A");
//     g5.addRule('A', "B");
//     g5.addRule('B', "");
//     g5.eliminateEpsilonRules();
//     expectRules("chain propagation across rounds", g5, {{'S', {"A", ""}}, {'A', {"B"}}, {'B', {}}});
// }

// void testEliminateUnitRules()
// {
//     // No unit rules
//     Grammar g({'S', 'A'}, {'a'}, 'S');
//     g.addRule('S', "aA");
//     g.addRule('A', "a");
//     g.eliminateUnitRules();
//     expectRules("no unit rules", g, {{'S', {"aA"}}, {'A', {"a"}}});

//     // Single Unit Rule
//     Grammar g2({'S', 'A'}, {'a'}, 'S');
//     g2.addRule('S', "A");
//     g2.addRule('A', "a");
//     g2.eliminateUnitRules();
//     expectRules("no unit rules", g2, {{'S', {"a"}}, {'A', {"a"}}});

//     // Chain with multiple rounds
//     Grammar g3({'S', 'A', 'B'}, {'a'}, 'S');
//     g3.addRule('S', "A");
//     g3.addRule('A', "B");
//     g3.addRule('B', "a");
//     g3.eliminateUnitRules();
//     expectRules("chain propagation", g3, {{'S', {"a"}}, {'A', {"a"}}, {'B', {"a"}}});

//     // Multiple unit rules with one LHS variable
//     Grammar g4({'S', 'A', 'B'}, {'a', 'b'}, 'S');
//     g4.addRule('S', "A");
//     g4.addRule('S', "B");
//     g4.addRule('A', "a");
//     g4.addRule('B', "b");
//     g4.eliminateUnitRules();
//     expectRules("multiple unit rules with one LHS", g4, {{'S', {"a", "b"}}, {'A', {"a"}}, {'B', {"b"}}});

//     // Self-loop
//     Grammar g5({'S'}, {'a'}, 'S');
//     g5.addRule('S', "S");
//     g5.addRule('S', "a");
//     g5.eliminateUnitRules();
//     expectRules("Self loop", g5, {{'S', {"a"}}});
// }

// void testBreakingLongRules()
// {
//     // No long rules, same rules returned
//     Grammar g({'S', 'A', 'B'}, {'a', 'b'}, 'S');
//     g.addRule('S', "AB");
//     g.addRule('A', "a");
//     g.addRule('B', "b");
//     g.breakLongRules();
//     expectRules("no long rules", g, {{'S', {"AB"}}, {'A', {"a"}}, {'B', {"b"}}});

//     // Two terminals together
//     Grammar g2({'S'}, {'a', 'b'}, 'S');
//     g2.addRule('S', "ab");
//     g2.breakLongRules();
//     expectRules("Two terminals", g2, {{'S', {"AB"}}, {'A', {"a"}}, {'B', {"b"}}});

//     // A terminal with a variable
//     Grammar g3({'S', 'T'}, {'a', 't'}, 'S');
//     g3.addRule('S', "aT");
//     g3.addRule('T', "t");
//     g3.breakLongRules();
//     expectRules("terminal and variable", g3, {{'S', {"AT"}}, {'T', {"t"}}, {'A', {"a"}}});

//     // Long rules of only variables
//     Grammar g4({'S', 'P', 'Q', 'R'}, {'p', 'q', 'r'}, 'S');
//     g4.addRule('S', "PQR");
//     g4.addRule('P', "p");
//     g4.addRule('Q', "q");
//     g4.addRule('R', "r");
//     g4.breakLongRules();
//     expectRules("Long rule of variables", g4,
//                 {{'S', {"PA"}}, {'A', {"QR"}}, {'P', {"p"}}, {'Q', {"q"}}, {'R', {"r"}}});

//     // Same terminal used in two rules
//     Grammar g5({'S', 'T', 'U', 'V'}, {'a', 't', 'v'}, 'S');
//     g5.addRule('S', "aT");
//     g5.addRule('U', "aV");
//     g5.addRule('T', "t");
//     g5.addRule('V', "v");
//     g5.breakLongRules();
//     printRules(g5);
// }