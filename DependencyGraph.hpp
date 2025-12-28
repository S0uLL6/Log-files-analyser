
#ifndef DEPENDENCY_GRAPH_H
#define DEPENDENCY_GRAPH_H

#include "Event.hpp"
#include "Sequence.hpp"
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <set>

class DependencyGraph {
public:
    void AddEvent(const Event& event);
    void AddEdge(const std::string& id1, const std::string& id2, double weight,
                 const ArraySequence<std::string>& common_tags,
                 const std::set<std::string>& common_words);
    void ProcessEventForSimilarity(const Event& new_event);
    void ProcessEventForSimilarity(const Event& new_event, double tag_weight, double message_weight);
    void PrintGraph() const;
    void PrintEventDetails(const std::string& id) const;
    ArraySequence<std::string> GetVertices() const;
    ArraySequence<std::string> GetNeighbors(const std::string& id) const;

private:
    struct EdgeDetails {
        double weight;
        ArraySequence<std::string> common_tags;
        std::set<std::string> common_words;
    };

    struct GraphNode {
        Event event;
        std::unordered_map<std::string, EdgeDetails> neighbors;
    };

    std::unordered_map<std::string, GraphNode> nodes;

    std::set<std::string> ExtractWords(const std::string& message) const;
    // ArraySequence -> ArraySequence
    double CalculateSimilarity(const ArraySequence<std::string>& tags1, const ArraySequence<std::string>& tags2,
                               ArraySequence<std::string>& intersection) const;
    double CalculateSimilarity(const std::set<std::string>& words1, const std::set<std::string>& words2,
                               std::set<std::string>& intersection) const;

    // --- Вспомогательные функции ---
    void SortArraySequence(ArraySequence<std::string>& arr) const; // Вспомогательная сортировка
    ArraySequence<std::string> ArraySetIntersection(const ArraySequence<std::string>& arr1, const ArraySequence<std::string>& arr2) const;
    // -------------------------------

    static constexpr double DEFAULT_TAG_WEIGHT = 1.0;
    static constexpr double DEFAULT_MESSAGE_WEIGHT = 1.0;
};

#endif // DEPENDENCY_GRAPH_H