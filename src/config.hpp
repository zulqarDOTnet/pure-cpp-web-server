// @Author Zulqarnain Z.
// @HireMe https://www.fiverr.com/zulqar

#include <string>
#include <fstream>
#include <unordered_map>
#include <nlohmann/json.hpp>

// Function to load configuration from a JSON file
std::pair<int, std::unordered_map<std::string, std::string>> load_config(const std::string &filename)
{
    // Open the file
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    // Parse the JSON
    nlohmann::json config;
    file >> config;

    // Extract the port number
    int port = config["port"];

    // Create the map for domain-to-document root mappings
    std::unordered_map<std::string, std::string> domain_doc_roots;
    for (auto &[key, value] : config["domains"].items())
    {
        domain_doc_roots[key] = value;
    }

    return {port, domain_doc_roots};
}