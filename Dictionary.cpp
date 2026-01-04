#include "Dictionary.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

Dictionary::Dictionary() {
    LoadFromFile("dictionary.json");
}

Dictionary::Dictionary(const std::string& filename) {
    LoadFromFile(filename);
    InitializeDistributions(); 
}

void Dictionary::LoadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open dictionary file: " + filename);
    }

    nlohmann::json json_data;
    try {
        file >> json_data;
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("Error parsing JSON file: " + std::string(e.what()));
    }

    if (json_data.contains("users") && json_data["users"].is_array()) {
        users_vec = json_data["users"].get<std::vector<std::string>>();
    } else {
        throw std::runtime_error("JSON file missing 'users' array or it's not an array.");
    }

    if (json_data.contains("actions") && json_data["actions"].is_array()) {
        actions_vec = json_data["actions"].get<std::vector<std::string>>();
    } else {
        throw std::runtime_error("JSON file missing 'actions' array or it's not an array.");
    }

    if (json_data.contains("resources") && json_data["resources"].is_array()) {
        resources_vec = json_data["resources"].get<std::vector<std::string>>();
    } else {
        throw std::runtime_error("JSON file missing 'resources' array or it's not an array.");
    }

    if (json_data.contains("error_types") && json_data["error_types"].is_array()) {
        error_types_vec = json_data["error_types"].get<std::vector<std::string>>();
    } else {
        throw std::runtime_error("JSON file missing 'error_types' array or it's not an array.");
    }

    if (json_data.contains("modules") && json_data["modules"].is_array()) {
        modules_vec = json_data["modules"].get<std::vector<std::string>>();
    } else {
        throw std::runtime_error("JSON file missing 'modules' array or it's not an array.");
    }

    if (json_data.contains("statuses") && json_data["statuses"].is_array()) {
        statuses_vec = json_data["statuses"].get<std::vector<std::string>>();
    } else {
        throw std::runtime_error("JSON file missing 'statuses' array or it's not an array.");
    }

    if (json_data.contains("adverbs") && json_data["adverbs"].is_array()) {
        adverbs_vec = json_data["adverbs"].get<std::vector<std::string>>();
    } else {
        throw std::runtime_error("JSON file missing 'adverbs' array or it's not an array.");
    }

    if (json_data.contains("templates") && json_data["templates"].is_array()) {
        templates_vec = json_data["templates"].get<std::vector<std::string>>();
    } else {
        throw std::runtime_error("JSON file missing 'templates' array or it's not an array.");
    }

    users = ArraySequence<std::string>(users_vec.data(), users_vec.size());
    actions = ArraySequence<std::string>(actions_vec.data(), actions_vec.size());
    resources = ArraySequence<std::string>(resources_vec.data(), resources_vec.size());
    error_types = ArraySequence<std::string>(error_types_vec.data(), error_types_vec.size());
    modules = ArraySequence<std::string>(modules_vec.data(), modules_vec.size());
    statuses = ArraySequence<std::string>(statuses_vec.data(), statuses_vec.size());
    adverbs = ArraySequence<std::string>(adverbs_vec.data(), adverbs_vec.size());
    templates = ArraySequence<std::string>(templates_vec.data(), templates_vec.size());
}

void Dictionary::InitializeDistributions() {
}

std::string Dictionary::GetRandomUser(std::mt19937& gen) const {
    if (users.GetLength() == 0) return "";
    std::uniform_int_distribution<> dist(0, users.GetLength() - 1);
    return users.Get(dist(gen));
}

std::string Dictionary::GetRandomAction(std::mt19937& gen) const {
    if (actions.GetLength() == 0) return "";
    std::uniform_int_distribution<> dist(0, actions.GetLength() - 1);
    return actions.Get(dist(gen));
}

std::string Dictionary::GetRandomResource(std::mt19937& gen) const {
    if (resources.GetLength() == 0) return "";
    std::uniform_int_distribution<> dist(0, resources.GetLength() - 1);
    return resources.Get(dist(gen));
}

std::string Dictionary::GetRandomErrorType(std::mt19937& gen) const {
    if (error_types.GetLength() == 0) return "";
    std::uniform_int_distribution<> dist(0, error_types.GetLength() - 1);
    return error_types.Get(dist(gen));
}

std::string Dictionary::GetRandomModule(std::mt19937& gen) const {
    if (modules.GetLength() == 0) return "";
    std::uniform_int_distribution<> dist(0, modules.GetLength() - 1);
    return modules.Get(dist(gen));
}

std::string Dictionary::GetRandomStatus(std::mt19937& gen) const {
    if (statuses.GetLength() == 0) return "";
    std::uniform_int_distribution<> dist(0, statuses.GetLength() - 1);
    return statuses.Get(dist(gen));
}

std::string Dictionary::GetRandomAdverb(std::mt19937& gen) const {
    if (adverbs.GetLength() == 0) return "";
    std::uniform_int_distribution<> dist(0, adverbs.GetLength() - 1);
    return adverbs.Get(dist(gen));
}

std::string Dictionary::GenerateRandomMessage(std::mt19937& gen) const {
    if (templates.GetLength() == 0) return "No templates available.";

    std::uniform_int_distribution<> template_dist(0, templates.GetLength() - 1);
    std::string message = templates.Get(template_dist(gen));

    size_t pos;
    std::string placeholder;

    placeholder = "{user}";
    while ((pos = message.find(placeholder)) != std::string::npos) {
        message.replace(pos, placeholder.length(), GetRandomUser(gen)); 
    }

    placeholder = "{action}";
    while ((pos = message.find(placeholder)) != std::string::npos) {
        message.replace(pos, placeholder.length(), GetRandomAction(gen)); 
    }

    placeholder = "{resource}";
    while ((pos = message.find(placeholder)) != std::string::npos) {
        message.replace(pos, placeholder.length(), GetRandomResource(gen)); 
    }

    placeholder = "{error_type}";
    while ((pos = message.find(placeholder)) != std::string::npos) {
        message.replace(pos, placeholder.length(), GetRandomErrorType(gen)); 
    }

    placeholder = "{module}";
    while ((pos = message.find(placeholder)) != std::string::npos) {
        message.replace(pos, placeholder.length(), GetRandomModule(gen)); 
    }

    placeholder = "{status}";
    while ((pos = message.find(placeholder)) != std::string::npos) {
        message.replace(pos, placeholder.length(), GetRandomStatus(gen)); 
    }

    placeholder = "{adverb}";
    while ((pos = message.find(placeholder)) != std::string::npos) {
        message.replace(pos, placeholder.length(), GetRandomAdverb(gen)); 
    }

    return message;
}