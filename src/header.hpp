// @Author Zulqarnain Z.
// @HireMe https://www.fiverr.com/zulqar

#include <algorithm>
#include <filesystem>
#include <string>

// Function to get the domain/hostname from the request headers
std::string get_host(const std::string &headers)
{
    std::istringstream stream(headers);
    std::string line;
    std::string host;

    // Iterate through each line of the headers
    while (std::getline(stream, line))
    {
        // Convert the line to lowercase for case-insensitive comparison
        std::transform(line.begin(), line.end(), line.begin(), ::tolower);

        // Check if the line contains the Host header
        if (line.find("host:") != std::string::npos)
        {
            // Extract the value part of the header
            std::size_t pos = line.find(":");
            if (pos != std::string::npos)
            {
                host = line.substr(pos + 1);
                // Trim whitespace from the host
                host.erase(0, host.find_first_not_of(" \t"));
                host.erase(host.find_last_not_of(" \t") + 1);
                // Remove the port number if present
                std::size_t port_pos = host.find(":");
                if (port_pos != std::string::npos)
                {
                    host = host.substr(0, port_pos);
                }
                break;
            }
        }
    }

    return host;
}

// Function to get the accepted encodings from the request headers
std::string get_accepted_encoding(const std::string &request_headers)
{
    std::istringstream stream(request_headers);
    std::string line;
    std::string accepted_encodings;

    // Iterate through each line of the headers
    while (std::getline(stream, line))
    {
        // Convert the line to lowercase for case-insensitive comparison
        std::transform(line.begin(), line.end(), line.begin(), ::tolower);

        // Check if the line contains the Accept-Encoding header
        if (line.find("accept-encoding:") != std::string::npos)
        {
            // Extract the value part of the header
            std::size_t pos = line.find(":");
            if (pos != std::string::npos)
            {
                accepted_encodings = line.substr(pos + 1);
                break;
            }
        }
    }

    // Trim whitespace and return the accepted encodings
    accepted_encodings.erase(accepted_encodings.begin(), std::find_if(accepted_encodings.begin(), accepted_encodings.end(), [](unsigned char ch)
                                                                      { return !std::isspace(ch); }));
    return accepted_encodings;
}