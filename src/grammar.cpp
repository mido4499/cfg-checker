#include "grammar.hpp"
#include <string>

Grammar::Grammar(std::set<char> variables, std::set<char> terminals, char start)
    : variables_(std::move(variables)), terminals_(std::move(terminals)), start_(start)
{
    if (variables_.empty())
        throw GrammarError("Variable set can't be empty.");

    for (char c : variables_)
    {
        if (terminals_.count(c))
        {
            throw GrammarError("The same symbol was found in both the set of variables and the set of terminals.");
        }
    }

    if (variables_.find(start_) == variables_.end())
        throw GrammarError("The set of variables doesn't include the starting variable.");
}

const std::map<char, std::set<std::string>> &Grammar::rules() const
{
    return rules_;
}

// Epsilon is represented as '#' in text and as an empty string "" internally
void Grammar::addRule(char lhs, const std::string &rhs)
{
    if (variables_.count(lhs) == 0)
    {
        throw GrammarError("The variable on the left-hand side is not in the set of variables.");
    }

    if (!rhs.empty())
    {
        for (char c : rhs)
        {
            if (variables_.count(c) == 0 && terminals_.count(c) == 0)
            {
                throw GrammarError("The variable/terminal: " + std::string(1, c) + " was not found in the set of variables or the set of terminals.");
            }
        }
    }

    if (!rules_[lhs].insert(rhs).second)
        throw GrammarError("The rule already exists.");
}

// freshVariables provide new variables in case the CNF conversion needs new variables
// It gets them from the pool of capital letters that are not yet used by the current rules set.
char Grammar::freshVariable()
{
    for (char c = 'A'; c <= 'Z'; ++c)
    {
        if (!variables_.count(c) && !terminals_.count(c))
        {
            variables_.insert(c);
            return c;
        }
    }
    throw GrammarError("No more characters found.");
    return '!';
}

void Grammar::addStartVariable()
{
    char ogStart = start_;
    start_ = freshVariable();
    addRule(start_, std::string(1, ogStart));
}

/*
First step is eliminating the epsilon rules and keeping track of all nullable variables and LHS variables
whose rules were removed.
set<char> nullableVariables keeps track of all nullable variables over the whole grammar; persistent
set<char> removed keeps track of all LHS whose epsilon rules were removed, so that, when adding the
new rules, we avoid adding the same epsilon unit rule again.
Second step: 1. Collecting all rules that need to be added to compensate for the removed epsilon rules
map<char, set<string>> additions keep track of all the rules to be added with their LHS variable
set<char> targetsInRule used to collect nullable variables in a single rule to pass to getAllOccurences()
which gets all combinations of this rule after removing each occurence of all variables together.
set<string> addedRhs stores all the occurences returned by getAllOccurences() to be stored in additions
All of these steps are repeated as long as changedSomething is true (if something is changed in the grammar)
*/
void Grammar::eliminateEpsilonRules()
{
    bool changedSomething = true;
    std::set<char> nullableVariables = {}; // To collect all nullable variables over the whole grammar
    while (changedSomething)
    {
        changedSomething = false;
        // First step: eliminate the epsilon rules and keep track of the variables that had these rules
        std::set<char> removed; // To keep track of the removed unit epsilon rules and avoid adding them again
        for (auto &[key, inner_set] : rules_)
        {
            if (key != start_ && inner_set.erase("") > 0) // Only remove the epsilon rules
            {
                changedSomething = true;
                nullableVariables.insert(key);
                removed.insert(key);
            }
        }

        if (!changedSomething)
            continue;

        // Second step, replace the occurences of these variables
        // First, collect all of the rules that should be added
        std::map<char, std::set<std::string>> additions; // Another map to collect all of the rules to be added, by LHS
        for (auto &[lhs, inner_set] : rules_)
        {
            for (const std::string &rhs : inner_set)
            {
                if (rhs.length() == 1 && nullableVariables.count(rhs[0]) != 0 && removed.count(lhs) != 0) // Avoids adding unit epsilon rules that were already removed
                    continue;

                std::set<char> targetsInRule; // A set to collect all of the nullable variables in a single rule
                for (char c : rhs)
                    if (nullableVariables.count(c) != 0)
                        targetsInRule.insert(c);

                if (targetsInRule.empty()) // No nullable variables appear
                    continue;

                std::set<std::string> addedRhs = getAllOccurences(rhs, targetsInRule);
                additions[lhs].insert(addedRhs.begin(), addedRhs.end());
            }
        }

        // Then, add all of the rules in their respective places
        for (const auto &pair : additions)
            rules_[pair.first].insert(pair.second.begin(), pair.second.end());
    }
}

std::set<std::string> Grammar::getAllOccurences(std::string rhs, const std::set<char> &targets)
{
    std::set<std::string> results = {};
    getAllOcurrencesRecursive(rhs, targets, 0, "", results);
    return results;
}

// The recursive function iterates over the string to get all combinations of removal of the variable from the RHS of the rule
// It gets the combinations by building up strings starting from an empty string and inserting them all in a set @results that it returns
void Grammar::getAllOcurrencesRecursive(const std::string &str, const std::set<char> &targets, int index, std::string current, std::set<std::string> &results)
{
    // Base case, we have reached the end of the string
    if (index == static_cast<int>(str.length()))
    {
        if (current != str)
        {
            results.insert(current);
        }
        return;
    }

    if (targets.count(str[index]) != 0)
    {
        // Since we encountered the variable we want to remove, we have two choices at each encounter.
        // The first one is to include the target and keep iterating
        getAllOcurrencesRecursive(str, targets, index + 1, current + str[index], results);

        // The second choice is to not include the target and keep iterating
        getAllOcurrencesRecursive(str, targets, index + 1, current, results);
    }
    else
    {
        // If the character encountered is not the target, we always incldue it
        getAllOcurrencesRecursive(str, targets, index + 1, current + str[index], results);
    }
}

void Grammar::eliminateUnitRules()
{
    bool changedSomething = true;
    std::map<char, std::set<char>> removedUnits;
    while (changedSomething)
    {
        changedSomething = false;
        // First step: Collect all LHS and RHS that need to be removed.
        std::map<char, std::set<char>> toRemove;
        for (const auto &[lhs, alternatives] : rules_)
            for (const std::string &rule : alternatives)
                // Find any A->B and add them to toRemove
                if (rule.length() == 1 && variables_.count(rule[0]) > 0)
                {
                    changedSomething = true;
                    toRemove[lhs].insert(rule[0]);
                }

        if (!changedSomething)
            break;

        // Second step: Collect all rules to add
        std::map<char, std::set<std::string>> toAdd;
        for (const auto &[lhs, rhs] : toRemove)
        {
            // Find all B->u
            // The iterator lhs here is A and rhs is all B's
            // Get all the alternatives for B in rules_ and add them to A in toAdd
            // Copy the alternatives of B to A in toAdd
            for (char c : rhs)
            {
                if (c == lhs)
                    continue; // Would be a meaningless cycle
                const auto &src = rules_[c];
                toAdd[lhs].insert(src.begin(), src.end());
            }
        }

        // Third step: Remove all the unit rules
        for (const auto &[lhs, rhs] : toRemove)
            // Iterate over A->B in toRemove and delete B from A in rules_
            for (char c : rhs)
                rules_[lhs].erase(std::string(1, c));

        // Record them for later cycles
        for (const auto &[lhs, targets] : toRemove)
            removedUnits[lhs].insert(targets.begin(), targets.end());

        // Fourth step: Remove all the unit rules that were already removed from the toAdd collection
        for (auto &[lhs, alternatives] : removedUnits)
        {
            auto it = toAdd.find(lhs);
            if (it == toAdd.end())
                continue;
            for (char c : alternatives)
                it->second.erase(std::string(1, c));
        }

        // Fifth step: Add the rules
        for (const auto &[lhs, alternatives] : toAdd)
            // Iterator lhs here is A
            // Add all the alternatives copied from B to the alternatives of A
            rules_[lhs].insert(alternatives.begin(), alternatives.end());
    }
}

/*
    This function will perform two steps:
    1. Terminal Cleanup: Break up the rules that contain more than one terminal on the RHS
    by creating a chain of new variables for every terminal so that the RHS only contains
    variables.
    2. Variable Cleanup: After the long rules only contain variables next to each other,
    break up these rules to a chain so that each rule contains at most two variables
*/
void Grammar::breakLongRules()
{
    // First step: Collect all long rules
    // longRules contains all rules in focus, including rules that need terminal cleanup (k>=2)
    // and rules that need to be broken down to chains (k>=3).
    std::map<char, std::set<std::string>> longRules;
    std::map<char, char> terminalToVariable; // Lookup table for variables assigned to terminals when cleaning up terminals

    for (const auto &[lhs, alternatives] : rules_)
        for (const auto &alternative : alternatives)
            if (alternative.length() >= 2)
                longRules[lhs].insert(alternative);

    for (const auto &[lhs, alternatives] : longRules)
    {
        for (const auto &alternative : alternatives)
        {
            std::string cleaned; // The rule produced after cleaning up terminals
            for (char c : alternative)
            {
                if (terminals_.count(c)) // If this is a terminal, it needs cleanup
                {
                    if (terminalToVariable.find(c) == terminalToVariable.end()) // If this is a terminal we didn't encounter before
                    {
                        char u = freshVariable();            // Assign a variable to this terminal
                        terminalToVariable[c] = u;           // Record the variable
                        rules_[u].insert(std::string(1, c)); // Add a rule for the variable -> terminal
                    }
                    cleaned += terminalToVariable[c];
                }
                else
                    cleaned += c; // Add the variable to cleaned, gradually forming the new rule
            }
            rules_[lhs].erase(alternative); // Remove the original rule after cleaning it

            int k = cleaned.length();
            if (k >= 3)
            {
                char lhsVariable = lhs;            // Assign variables of the new rule
                for (size_t i = 0; i < k - 2; ++i) // Iterate over the long rule to break them down
                {
                    char rhsVariable = freshVariable(); // Generate a new rhsVariable
                    rules_[lhsVariable].insert(std::string(1, cleaned[i]) + rhsVariable);
                    lhsVariable = rhsVariable; // Pass the RHS variable to the next rule in the chain
                }
                rules_[lhsVariable].insert(cleaned.substr(k - 2)); // Insert the last two terminals at the last rule in the chain since two terminals are allowed
            }
            else // The rule doesn't need variable cleanup
            {
                rules_[lhs].insert(cleaned);
            }
        }
    }
}

Grammar Grammar::CNFConvert() const
{
}

// Helper methods to differentiate between terminals (lowercase) and variables (uppercase)
bool Grammar::isTerminal(char c) const
{
    return (c < 'z' && c > 'a');
}
bool Grammar::isVariable(char c) const
{
    return (c < 'Z' && c > 'A');
}