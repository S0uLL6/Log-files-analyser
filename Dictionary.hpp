#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP

#include "Sequence.hpp"
#include <random>
#include <string>
#include <vector>
#include "json.hpp" 

class Dictionary {
public:
    Dictionary();
    Dictionary(const std::string& filename);

    std::string GetRandomUser(std::mt19937& gen) const;
    std::string GetRandomAction(std::mt19937& gen) const;
    std::string GetRandomResource(std::mt19937& gen) const;
    std::string GetRandomErrorType(std::mt19937& gen) const;
    std::string GetRandomModule(std::mt19937& gen) const;
    std::string GetRandomStatus(std::mt19937& gen) const;
    std::string GetRandomAdverb(std::mt19937& gen) const;

    std::string GenerateRandomMessage(std::mt19937& gen) const; 

private:
    std::vector<std::string> users_vec;
    std::vector<std::string> actions_vec;
    std::vector<std::string> resources_vec;
    std::vector<std::string> error_types_vec;
    std::vector<std::string> modules_vec;
    std::vector<std::string> statuses_vec;
    std::vector<std::string> adverbs_vec;
    std::vector<std::string> templates_vec;

    ArraySequence<std::string> users;
    ArraySequence<std::string> actions;
    ArraySequence<std::string> resources;
    ArraySequence<std::string> error_types;
    ArraySequence<std::string> modules;
    ArraySequence<std::string> statuses;
    ArraySequence<std::string> adverbs;
    ArraySequence<std::string> templates;

    void InitializeDistributions(); 
    void LoadFromFile(const std::string& filename);
};

#endif // DICTIONARY_HPP