#include "../src/grammar.hpp"
#include "test_helpers.hpp"

int main()
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