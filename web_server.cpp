// @Author Zulqarnain Z.
// @HireMe https://www.fiverr.com/zulqar

#include <fstream>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>

#include "src/config.hpp"
#include "src/header.hpp"
#include "src/encode.hpp"
#include "src/protect.hpp"
#include "src/mime.hpp"

namespace fs = std::filesystem;
namespace ba = boost::asio;
using ba::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(ba::io_context &io_context, ba::strand<ba::io_context::executor_type> &strand, tcp::socket socket, const std::unordered_map<std::string, std::string> &domain_doc_roots, const std::string &doc_root)
        : strand_(strand),
          socket_(std::move(socket)),
          domain_doc_roots_(domain_doc_roots),
          doc_root_(doc_root)
    {
        //
    }

    void start()
    {
        do_read();
    }

private:
    void send_file_content(std::string content)
    {
        // Create a shared pointer to the content string
        auto content_ptr = std::make_shared<std::string>(std::move(content));
        auto self(shared_from_this());

        // Start asynchronous operation to write the content to the socket
        ba::async_write(socket_, ba::buffer(*content_ptr), boost::asio::bind_executor(strand_, [this, self, content_ptr](boost::system::error_code ec, std::size_t)
                                                                                      {
        if (!ec) {
            // The content has been successfully sent
            //std::cout << "Content has been sent successfully." << std::endl;
        } else {
            std::ostringstream response;
            response << "HTTP/1.1 500 Internal Server Error\r\n";
            response << "Content-Type: text/html\r\n\r\n";
            response << "<html><body><h1>500 Internal Server Error</h1></body></html>";
            ba::async_write(socket_, ba::buffer(response.str()), boost::asio::bind_executor(strand_, [this, self, content_ptr](boost::system::error_code ec, std::size_t) {}));
            //std::cerr << "Error sending content: " << ec.message() << std::endl;
        } }));
    }

    void serve_file(const std::string &path, const std::string &encoding)
    {
        std::string sanitized_path = sanitize_path(path);
        std::string full_path = doc_root_.back() == '/' ? doc_root_ : doc_root_ + "/";
        full_path += (sanitized_path.find("/") == 0) ? sanitized_path.substr(1) : sanitized_path;

        // Use filesystem to check if the path is a directory
        if (fs::is_directory(full_path))
        {
            full_path += "/index.html";
        }

        // Debugging output
        // std::cout << "Attempting to serve file: " << full_path << std::endl;

        std::ifstream file(full_path, std::ios::binary);
        if (!file)
        {
            // File not found, respond with 404
            std::ostringstream response;
            response << "HTTP/1.1 404 Not Found\r\n";
            response << "Server: zulqarDOTnet_C++/1.2.0 (Unix)\r\n";
            response << "Content-Type: text/html\r\n\r\n";
            response << "<html><body><h1>404 Not Found</h1></body></html>";
            ba::async_write(socket_, ba::buffer(response.str()), boost::asio::bind_executor(strand_, [self = shared_from_this()](boost::system::error_code ec, std::size_t)
                                                                                            {
            if (ec) {
                std::cerr << "Error sending response: " << ec.message() << std::endl;
            } }));
        }
        else
        {
            // Determine the MIME type
            std::string mime_type = get_mime_type(full_path);

            // Define cache expiry times
            const std::string cache_control_html = "public, max-age=14400, immutable";        // 4 hours for HTML
            const std::string cache_control_image = "public, max-age=86400, must-revalidate"; // 24 hours for images

            // Select the cache control header based on the MIME type
            std::string cache_control;
            if (mime_type == "text/html" || mime_type == "application/xhtml+xml")
            {
                cache_control = cache_control_html;
            }
            else if (mime_type == "image/png" || mime_type == "image/jpeg" || mime_type == "image/gif" || mime_type == "image/avif" || mime_type == "image/webp")
            {
                cache_control = cache_control_image;
            }
            else
            {
                // Default or other MIME types
                cache_control = "public, max-age=3600, must-revalidate"; // 1 hour for others
            }

            // Get the file size
            file.seekg(0, std::ios::end);
            std::size_t file_size = file.tellg();
            file.seekg(0, std::ios::beg);

            // Read the entire file into a string
            std::string file_content;
            file_content.resize(file_size);
            file.read(&file_content[0], file_size);

            // Check the Accept-Encoding header and compress accordingly
            // std::string encoding = "";
            // std::string encoding = "zstd";
            // std::string encoding = "gzip";
            // std::string encoding = "deflate";
            // std::string encoding = "br";
            std::string compressed_content;

            if (encoding.find("zstd") != std::string::npos)
            {
                compressed_content = compress_zstd(file_content);
                mime_type += "\r\nContent-Encoding: zstd";
            }
            else if (encoding.find("gzip") != std::string::npos)
            {
                compressed_content = compress_gzip(file_content);
                mime_type += "\r\nContent-Encoding: gzip";
            }
            else if (encoding.find("deflate") != std::string::npos)
            {
                compressed_content = compress_deflate(file_content);
                mime_type += "\r\nContent-Encoding: deflate";
            }
            else if (encoding.find("br") != std::string::npos)
            {
                compressed_content = compress_brotli(file_content);
                mime_type += "\r\nContent-Encoding: br";
            }
            else
            {
                compressed_content = file_content; // No compression
            }

            // Update the file size to the size of the compressed content
            file_size = compressed_content.size();

            // Build the response headers
            std::ostringstream response;
            response << "HTTP/1.1 200 OK\r\n";
            response << "Server: zulqarDOTnet_C++/1.2.0 (Unix)\r\n";
            response << "Content-Type: " << mime_type << "\r\n";
            response << "Cache-Control: " << cache_control << "\r\n";
            response << "Content-Length: " << file_size << "\r\n\r\n";

            // Send the headers
            ba::async_write(socket_, ba::buffer(response.str()), boost::asio::bind_executor(strand_, [this, self = shared_from_this(), compressed_content](boost::system::error_code ec, std::size_t) mutable
                                                                                            {
            if (!ec) {
                // If headers were successfully sent, send the file content
                send_file_content(std::move(compressed_content));
                // Clear the compressed_content to free memory
                compressed_content.clear();
            } else {
                std::cerr << "Error sending headers: " << ec.message() << std::endl;
            } }));
        }
    }

    // ... [Other methods like handle_read, serve_file, etc.] ...
    void handle_read(const boost::system::error_code &error, std::size_t bytes_transferred)
    {
        if (!error)
        {
            // Convert the streambuf data to a std::istream object
            std::istream request_stream(&request_buf_);
            std::string request_line;
            std::getline(request_stream, request_line);

            // Parse the request line
            std::istringstream request_ss(request_line);
            std::string method, path, version;
            request_ss >> method >> path >> version;

            // Read the rest of the request headers
            std::string headers;
            std::string header_line;
            while (std::getline(request_stream, header_line) && header_line != "\r")
            {
                headers += header_line + "\n";
            }

            // Parse the Host header to determine the domain
            std::string host = get_host(headers);
            doc_root_ = domain_doc_roots_[host]; // Update the doc_root_ based on the domain

            // Check the HTTP method
            if (method == "GET")
            {
                // Get the accepted encodings from the request headers
                std::string encoding = get_accepted_encoding(headers);

                // Sanitize the path and serve the file
                serve_file(path, encoding); // Pass the encoding to the serve_file method
            }
            else
            {
                // Handle other HTTP methods (e.g., POST, PUT)
                std::ostringstream response;
                response << "HTTP/1.1 405 Method Not Allowed\r\n";
                response << "Content-Type: text/html\r\n\r\n";
                response << "<html><body><h1>405 Method Not Allowed</h1></body></html>";
                ba::write(socket_, ba::buffer(response.str()));
            }
        }
        else
        {
            std::ostringstream response;
            response << "HTTP/1.1 500 Internal Server Error\r\n";
            response << "Content-Type: text/html\r\n\r\n";
            response << "<html><body><h1>500 Internal Server Error</h1></body></html>";
            ba::write(socket_, ba::buffer(response.str()));
            // std::cerr << "Error in handle_read: " << error.message() << std::endl;
        }
    }

    void do_read()
    {
        auto self(shared_from_this());
        ba::async_read_until(socket_, request_buf_, "\r\n\r\n",
                             boost::asio::bind_executor(strand_,
                                                        boost::bind(&Session::handle_read, self,
                                                                    boost::asio::placeholders::error,
                                                                    boost::asio::placeholders::bytes_transferred)));
    }

    // Declare other member variables and methods as needed
    ba::strand<ba::io_context::executor_type> &strand_;
    tcp::socket socket_;
    ba::streambuf request_buf_;
    std::unordered_map<std::string, std::string> domain_doc_roots_;
    std::string doc_root_;
};

class WebServer
{
public:
    WebServer(ba::io_context &io_context, const std::unordered_map<std::string, std::string> &domain_doc_roots, short port)
        : io_context_(io_context),
          strand_(io_context.get_executor()),
          domain_doc_roots_(domain_doc_roots),
          acceptor_v4_(io_context_, ba::ip::tcp::endpoint(ba::ip::address::from_string("127.0.0.1"), port)),
          acceptor_v6_(io_context_, ba::ip::tcp::endpoint(ba::ip::address::from_string("::1"), port))
    {
        acceptor_v4_.set_option(ba::ip::tcp::acceptor::reuse_address(true));
        acceptor_v6_.set_option(ba::ip::tcp::acceptor::reuse_address(true));
        start_accept();
    }

    void run()
    {
        // Run the io_context object to perform asynchronous operations
        io_context_.run();
    }

private:
    void start_accept()
    {
        // Create a new socket for the incoming connection
        auto socket_v4 = std::make_shared<ba::ip::tcp::socket>(io_context_);
        auto socket_v6 = std::make_shared<ba::ip::tcp::socket>(io_context_);

        // Start an asynchronous accept operation for IPv4
        acceptor_v4_.async_accept(*socket_v4,
                                  boost::asio::bind_executor(strand_,
                                                             boost::bind(&WebServer::handle_accept, this, socket_v4,
                                                                         boost::asio::placeholders::error)));

        // Start an asynchronous accept operation for IPv6
        acceptor_v6_.async_accept(*socket_v6,
                                  boost::asio::bind_executor(strand_,
                                                             boost::bind(&WebServer::handle_accept, this, socket_v6,
                                                                         boost::asio::placeholders::error)));
    }

    void handle_accept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code &error)
    {
        if (!error)
        {
            // Start the session on the strand
            std::make_shared<Session>(io_context_, strand_, std::move(*socket), domain_doc_roots_, "./domains")->start();
        }
        start_accept();
    }

    // Declare other member variables and methods as needed
    ba::io_context &io_context_;
    ba::strand<ba::io_context::executor_type> strand_;
    tcp::acceptor acceptor_v4_;
    tcp::acceptor acceptor_v6_;
    std::unordered_map<std::string, std::string> domain_doc_roots_;
};

int main()
{
    try
    {
        // Load the configuration
        auto [port, domain_doc_roots] = load_config("config.json");
        // std::cout << "Port: " << port << std::endl;
        for (const auto &[domain, doc_root] : domain_doc_roots)
        {
            // std::cout << "Domain: " << domain << " -> Document Root: " << doc_root << std::endl;
        }

        // Start the web server
        ba::io_context io_context;
        ba::strand<ba::io_context::executor_type> strand_(io_context.get_executor());
        WebServer server(io_context, domain_doc_roots, port);
        server.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
