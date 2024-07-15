// @Author Zulqarnain Z.
// @HireMe https://www.fiverr.com/zulqar

#include <sstream>
#include <string>

// Sanitize the input path to prevent directory traversal
std::string sanitize_path(const std::string &path)
{
    // Find the position of the '?' character, which starts the query string
    std::size_t query_pos = path.find('?');
    // If there is a query string, remove it from the path
    std::string path_without_query = (query_pos != std::string::npos) ? path.substr(0, query_pos) : path;

    if (path_without_query == "/")
    {
        return "/index.html";
    }

    // Remove any ".." components to prevent directory traversal
    std::string sanitized_path;
    std::istringstream iss(path_without_query);
    std::string component;
    while (std::getline(iss, component, '/'))
    {
        if (component == ".." || component.empty())
        {
            // Skip ".." and empty components resulting from consecutive slashes
            continue;
        }
        sanitized_path += "/" + component;
    }

    // Ensure the path starts with a single slash
    if (!sanitized_path.empty() && sanitized_path[0] != '/')
    {
        sanitized_path = "/" + sanitized_path;
    }
    return sanitized_path;
}