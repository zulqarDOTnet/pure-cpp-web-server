// @Author Zulqarnain Z.
// @HireMe https://www.fiverr.com/zulqar

#include <string>
#include <unordered_map>

// Function to determine the MIME type based on the file extension
std::string get_mime_type(const std::string &path)
{
    static const std::unordered_map<std::string, std::string> mime_types = {
        {".html", "text/html; charset=UTF-8"},
        {".htm", "text/html; charset=UTF-8"},
        {".txt", "text/plain; charset=UTF-8"},
        {".xml", "application/xml; charset=UTF-8"},
        {".xhtml", "application/xhtml+xml; charset=UTF-8"},
        {".js", "application/javascript; charset=UTF-8"},
        {".json", "application/json; charset=UTF-8"},
        {".css", "text/css; charset=UTF-8"},
        {".ico", "image/vnd.microsoft.icon"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".png", "image/png"},
        {".gif", "image/gif"},
        {".avif", "image/avif"},
        {".webp", "image/webp"},
        {".svg", "image/svg+xml"},
        {".webm", "video/webm"},
        {".mp4", "video/mp4"},
        {".ogv", "video/ogg"},
        {".otf", "font/otf"},
        {".ttf", "font/ttf"},
        {".woff", "font/woff"},
        {".woff2", "font/woff2"},
        // Additional MIME types
        {".aac", "audio/aac"},
        {".abw", "application/x-abiword"},
        {".arc", "application/x-freearc"},
        {".avi", "video/x-msvideo"},
        {".azw", "application/vnd.amazon.ebook"},
        {".bin", "application/octet-stream"},
        {".bmp", "image/bmp"},
        {".bz", "application/x-bzip"},
        {".bz2", "application/x-bzip2"},
        {".csh", "application/x-csh"},
        {".csv", "text/csv"},
        {".doc", "application/msword"},
        {".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
        {".eot", "application/vnd.ms-fontobject"},
        {".epub", "application/epub+zip"},
        {".gz", "application/gzip"},
        {".ics", "text/calendar"},
        {".jar", "application/java-archive"},
        {".jsonld", "application/ld+json"},
        {".midi", "audio/midi"},
        {".mjs", "text/javascript"},
        {".mp3", "audio/mpeg"},
        {".mpeg", "video/mpeg"},
        {".mpkg", "application/vnd.apple.installer+xml"},
        {".odp", "application/vnd.oasis.opendocument.presentation"},
        {".ods", "application/vnd.oasis.opendocument.spreadsheet"},
        {".odt", "application/vnd.oasis.opendocument.text"},
        {".oga", "audio/ogg"},
        {".ogx", "application/ogg"},
        {".opus", "audio/ogg"},
        {".otf", "font/otf"},
        {".pdf", "application/pdf"},
        {".wasm", "application/wasm"},
        {".zip", "application/zip"},
        // Default MIME type
        {"", "application/octet-stream"},
    };

    // Find the last dot position
    std::size_t last_dot_pos = path.find_last_of('.');
    if (last_dot_pos != std::string::npos)
    {
        std::string extension = path.substr(last_dot_pos);
        auto mime_type_it = mime_types.find(extension);
        if (mime_type_it != mime_types.end())
        {
            return mime_type_it->second;
        }
    }
    return "application/octet-stream"; // Default MIME type
}