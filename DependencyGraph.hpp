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
                 const std::set<std::string>& common_words,
                 double time_similarity,
                 bool user_id_match, 
                 bool ip_match); 
    void ProcessEventForSimilarity(const Event& new_event);
    void ProcessEventForSimilarity(const Event& new_event, double tag_weight, double message_weight, double time_weight, double user_id_weight, double ip_weight); // Новые параметры
    void PrintGraph() const;
    void PrintEventDetails(const std::string& id) const;
    ArraySequence<std::string> GetVertices() const;
    ArraySequence<std::string> GetNeighbors(const std::string& id) const;

    void PrintTopConnectedEvents() const;
    void PrintIsolatedEvents() const;

private:
    struct EdgeDetails {
        double weight;
        ArraySequence<std::string> common_tags;
        std::set<std::string> common_words;
        double time_similarity;
        bool user_id_match; 
        bool ip_match; 
    };

    struct GraphNode {
        Event event;
        std::unordered_map<std::string, EdgeDetails> neighbors;
    };

    std::unordered_map<std::string, GraphNode> nodes;

    std::set<std::string> ExtractWords(const std::string& message) const;
    double CalculateSimilarity(const ArraySequence<std::string>& tags1, const ArraySequence<std::string>& tags2,
                               ArraySequence<std::string>& intersection) const;
    double CalculateSimilarity(const std::set<std::string>& words1, const std::set<std::string>& words2,
                               std::set<std::string>& intersection) const;
    double CalculateTimeSimilarity(const std::chrono::steady_clock::time_point& t1,
                                  const std::chrono::steady_clock::time_point& t2) const;
    // --- Новые функции ---
    double CalculateUserIdSimilarity(const std::string& id1, const std::string& id2) const;
    double CalculateIpSimilarity(const std::string& ip1, const std::string& ip2) const;
    // ---------------------
    void SortArraySequence(ArraySequence<std::string>& arr) const;
    ArraySequence<std::string> ArraySetIntersection(const ArraySequence<std::string>& arr1, const ArraySequence<std::string>& arr2) const;

    static constexpr double DEFAULT_TAG_WEIGHT = 1.0;
    static constexpr double DEFAULT_MESSAGE_WEIGHT = 1.0;
    static constexpr double DEFAULT_TIME_WEIGHT = 1.0;
    static constexpr double DEFAULT_USER_ID_WEIGHT = 1.0; 
    static constexpr double DEFAULT_IP_WEIGHT = 1.0; 
    static constexpr auto TIME_WINDOW_MS = std::chrono::milliseconds(5000);
};

#endif // DEPENDENCY_GRAPH_H