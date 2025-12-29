
#include "DependencyGraph.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <set>
#include <cctype>
#include <locale>
#include <codecvt>
#include <numeric>
#include <functional> 

void DependencyGraph::AddEvent(const Event& event) {
    nodes[event.GetId()] = GraphNode{event, {}};
}

void DependencyGraph::AddEdge(const std::string& id1, const std::string& id2, double weight,
                              const ArraySequence<std::string>& common_tags,
                              const std::set<std::string>& common_words) {
    if (nodes.count(id1) && nodes.count(id2)) {
        EdgeDetails details{weight, common_tags, common_words};
        auto current_it = nodes[id1].neighbors.find(id2);
        if (current_it != nodes[id1].neighbors.end() && current_it->second.weight >= weight) {
             return;
        }
        nodes[id1].neighbors[id2] = details;
        nodes[id2].neighbors[id1] = details;
    }
}

void DependencyGraph::ProcessEventForSimilarity(const Event& new_event) {
    ProcessEventForSimilarity(new_event, DEFAULT_TAG_WEIGHT, DEFAULT_MESSAGE_WEIGHT);
}

void DependencyGraph::ProcessEventForSimilarity(const Event& new_event, double tag_weight, double message_weight) {
    AddEvent(new_event);

    auto words_new = ExtractWords(new_event.GetMessage());

    for (const auto& [id, node] : nodes) {
        if (id != new_event.GetId()) {
            double similarity = 0.0;
            double total_weight = tag_weight + message_weight;

            ArraySequence<std::string> common_tags(1); 
            if (tag_weight > 0) {
                double tag_sim = CalculateSimilarity(new_event.GetTags(), node.event.GetTags(), common_tags);
                similarity += tag_sim * tag_weight;
            }

            std::set<std::string> common_words;
            if (message_weight > 0) {
                auto words_existing = ExtractWords(node.event.GetMessage());
                double msg_sim = CalculateSimilarity(words_new, words_existing, common_words);
                similarity += msg_sim * message_weight;
            }

            if (total_weight > 0) {
                similarity /= total_weight;
            }

            if (similarity > 0.0) {
                AddEdge(new_event.GetId(), id, similarity, common_tags, common_words);
            }
        }
    }
}

std::set<std::string> DependencyGraph::ExtractWords(const std::string& message) const {
    std::set<std::string> words;
    std::istringstream iss(message);
    std::string word;
    while (iss >> word) {
        size_t start = 0;
        size_t end = word.length();
        while (start < end && !std::isalnum(static_cast<unsigned char>(word[start]))) {
            ++start;
        }
        while (end > start && !std::isalnum(static_cast<unsigned char>(word[end - 1]))) {
            --end;
        }
        if (end > start) {
            std::string clean_word = word.substr(start, end - start);
            std::transform(clean_word.begin(), clean_word.end(), clean_word.begin(),
                           [](char c){ return std::tolower(static_cast<unsigned char>(c)); });

            if (clean_word.length() > 3) {
                words.insert(clean_word);
            }
        }
    }
    return words;
}

void DependencyGraph::SortArraySequence(ArraySequence<std::string>& arr) const {
    if (arr.GetLength() == 0) {
        return;
    }
    size_t n = arr.GetLength();
    for (size_t i = 0; i < n - 1; ++i) {
        for (size_t j = 0; j < n - i - 1; ++j) {
            if (arr.Get(j) > arr.Get(j + 1)) {
                std::string temp = arr.Get(j);
                arr.SetAt(j, arr.Get(j + 1));
                arr.SetAt(j + 1, temp);
            }
        }
    }
}

ArraySequence<std::string> DependencyGraph::ArraySetIntersection(const ArraySequence<std::string>& arr1, const ArraySequence<std::string>& arr2) const {
    if (arr1.GetLength() == 0 || arr2.GetLength() == 0) {
        return ArraySequence<std::string>(0); 
    }

    ArraySequence<std::string> sorted_arr1(arr1);
    ArraySequence<std::string> sorted_arr2(arr2);

    SortArraySequence(sorted_arr1);
    SortArraySequence(sorted_arr2);

    size_t max_size = (sorted_arr1.GetLength() < sorted_arr2.GetLength()) ? sorted_arr1.GetLength() : sorted_arr2.GetLength();
    if (max_size == 0) {
        return ArraySequence<std::string>(0); 
    }

    ArraySequence<std::string> temp_result(max_size);

    size_t i = 0, j = 0, k = 0;
    while (i < sorted_arr1.GetLength() && j < sorted_arr2.GetLength()) {
        if (sorted_arr1.Get(i) < sorted_arr2.Get(j)) {
            i++;
        } else if (sorted_arr1.Get(i) > sorted_arr2.Get(j)) {
            j++;
        } else { 
            temp_result.SetAt(k, sorted_arr1.Get(i));
            i++;
            j++;
            k++;
        }
    }

    ArraySequence<std::string> result(k);
    for (size_t idx = 0; idx < k; ++idx) {
        result.SetAt(idx, temp_result.Get(idx));
    }

    return result;
}

double DependencyGraph::CalculateSimilarity(const ArraySequence<std::string>& tags1, const ArraySequence<std::string>& tags2,
                                           ArraySequence<std::string>& intersection) const {
    intersection = ArraySetIntersection(tags1, tags2);
    int common_count = intersection.GetLength();
    ArraySequence<std::string> sorted_tags1(tags1);
    ArraySequence<std::string> sorted_tags2(tags2);
    SortArraySequence(sorted_tags1);
    SortArraySequence(sorted_tags2);

    size_t max_union_size = tags1.GetLength() + tags2.GetLength();
    if (max_union_size == 0) {
        return 0.0; 
    }
    ArraySequence<std::string> temp_union(max_union_size);
    size_t i = 0, j = 0, k = 0;

    while (i < sorted_tags1.GetLength() && j < sorted_tags2.GetLength()) {
        if (sorted_tags1.Get(i) < sorted_tags2.Get(j)) {
            temp_union.SetAt(k++, sorted_tags1.Get(i++));
        } else if (sorted_tags1.Get(i) > sorted_tags2.Get(j)) {
            temp_union.SetAt(k++, sorted_tags2.Get(j++));
        } else { 
            temp_union.SetAt(k++, sorted_tags1.Get(i++)); 
            j++; 
        }
    }
    while (i < sorted_tags1.GetLength()) {
        temp_union.SetAt(k++, sorted_tags1.Get(i++));
    }
    while (j < sorted_tags2.GetLength()) {
        temp_union.SetAt(k++, sorted_tags2.Get(j++));
    }

    int total_count = k; 

    return total_count > 0 ? static_cast<double>(common_count) / total_count : 0.0;
}

double DependencyGraph::CalculateSimilarity(const std::set<std::string>& words1, const std::set<std::string>& words2,
                                           std::set<std::string>& intersection) const {
    intersection.clear();
    std::set_intersection(words1.begin(), words1.end(),
                          words2.begin(), words2.end(),
                          std::inserter(intersection, intersection.begin()));
    int common_count = intersection.size();
    std::deque<std::string> temp_words1, temp_words2, temp_union;
    for (const auto& w : words1) temp_words1.push_back(w);
    for (const auto& w : words2) temp_words2.push_back(w);
    std::set_union(temp_words1.begin(), temp_words1.end(),
                   temp_words2.begin(), temp_words2.end(),
                   std::back_inserter(temp_union));
    int total_count = temp_union.size();

    return total_count > 0 ? static_cast<double>(common_count) / total_count : 0.0;
}

void DependencyGraph::PrintGraph() const {
    std::cout << "\n Dependency Graph" << std::endl;

    int total_vertices = nodes.size();
    int total_edges = 0;
    std::set<std::pair<std::string, std::string>> printed_edges;
    for (const auto& [id, node] : nodes) {
        for (const auto& [neighbor_id, details] : node.neighbors) {
            auto edge = (id < neighbor_id) ? std::make_pair(id, neighbor_id) : std::make_pair(neighbor_id, id);
            if (printed_edges.insert(edge).second) {
                total_edges++;
            }
        }
    }
    std::cout << "Total Events (Vertices): " << total_vertices << std::endl;
    std::cout << "Total Connections (Edges): " << total_edges << std::endl << std::endl;

    if (nodes.empty()) {
        std::cout << "Graph is empty." << std::endl;
        return;
    }

    std::cout << "Events:" << std::endl;
    for (const auto& [id, node] : nodes) {
        std::cout << "  - " << id << " (Type: " << node.event.GetType()
                  << ", Tags: ";
        for (size_t i = 0; i < node.event.GetTags().GetLength(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << node.event.GetTags().Get(i);
        }
        std::cout << ")" << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Connections:" << std::endl;
    for (const auto& [id, node] : nodes) {
        if (!node.neighbors.empty()) {
            std::cout << "  " << id << " is connected to:" << std::endl;
            for (const auto& [neighbor_id, details] : node.neighbors) {
                if (id < neighbor_id) {
                    std::cout << "    - " << neighbor_id
                              << " (Similarity: " << details.weight << "; ";

                    if (details.common_tags.GetLength() > 0) {
                        std::cout << "Tags: ";
                        for (size_t i = 0; i < details.common_tags.GetLength(); ++i) {
                            if (i > 0) std::cout << ", ";
                            std::cout << details.common_tags.Get(i);
                        }
                    } else {
                        std::cout << "Tags: None";
                    }

                    std::cout << "; ";

                    if (!details.common_words.empty()) {
                        std::cout << "Mutual words: ";
                        auto it = details.common_words.begin();
                        std::cout << *it;
                        ++it;
                        for (; it != details.common_words.end(); ++it) {
                            std::cout << ", " << *it;
                        }
                    } else {
                        std::cout << "Mutual words: None";
                    }

                    std::cout << ")" << std::endl;
                }
            }
        }
    }
}

void DependencyGraph::PrintEventDetails(const std::string& id) const {
    auto it = nodes.find(id);
    if (it != nodes.end()) {
        const Event& event = it->second.event;
        std::cout << "\n Event Details for ID: " << id << std::endl;
        std::cout << "Type: " << event.GetType() << std::endl;
        std::cout << "Message: " << event.GetMessage() << std::endl;
        std::cout << "Tags: ";
        const auto& tags = event.GetTags();
        for (size_t i = 0; i < tags.GetLength(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << tags.Get(i);
        }
        std::cout << std::endl;
        std::cout << "Timestamp (ms since start): "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(event.GetTimestamp().time_since_epoch()).count()
                  << std::endl;
    } else {
        std::cout << "Event with ID '" << id << "' not found in the graph." << std::endl;
    }
}

ArraySequence<std::string> DependencyGraph::GetVertices() const {
    ArraySequence<std::string> vertices(1);
    for (const auto& [id, _] : nodes) {
        vertices.Append(id);
    }
    return vertices;
}

ArraySequence<std::string> DependencyGraph::GetNeighbors(const std::string& id) const {
    ArraySequence<std::string> neighbors(1);
    auto it = nodes.find(id);
    if (it != nodes.end()) {
        for (const auto& [neighbor_id, _] : it->second.neighbors) {
            neighbors.Append(neighbor_id);
        }
    }
    return neighbors;
}