// @Author Zulqarnain Z.
// @HireMe https://www.fiverr.com/zulqar

#include <sstream>
#include <zstd.h>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <brotli/encode.h>

// Function to compress content using zstd
std::string compress_zstd(const std::string &content)
{
    size_t const maxCompressedSize = ZSTD_compressBound(content.size());
    std::vector<char> compressedData(maxCompressedSize);
    size_t const compressedSize = ZSTD_compress(compressedData.data(), maxCompressedSize,
                                                content.data(), content.size(), 3);
    if (ZSTD_isError(compressedSize))
    {
        throw std::runtime_error(ZSTD_getErrorName(compressedSize));
    }
    return std::string(compressedData.begin(), compressedData.begin() + compressedSize);
}

// Function to compress content using gzip
std::string compress_gzip(const std::string &content)
{
    boost::iostreams::gzip_params gzip_options;
    gzip_options.level = 3;
    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    in.push(boost::iostreams::gzip_compressor(gzip_options));
    in.push(boost::iostreams::array_source(content.data(), content.size()));
    std::ostringstream compressed_stream;
    boost::iostreams::copy(in, compressed_stream);
    return compressed_stream.str();
}

// Function to compress content using zlib/deflate
std::string compress_deflate(const std::string &content)
{
    boost::iostreams::zlib_params zlib_options;
    zlib_options.level = 3;
    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    in.push(boost::iostreams::zlib_compressor(zlib_options));
    in.push(boost::iostreams::array_source(content.data(), content.size()));
    std::ostringstream compressed_stream;
    boost::iostreams::copy(in, compressed_stream);
    return compressed_stream.str();
}

// Function to compress content using brotli
std::string compress_brotli(const std::string &content)
{
    std::vector<uint8_t> output_buffer(BrotliEncoderMaxCompressedSize(content.size()));
    size_t encoded_size = output_buffer.size();
    BrotliEncoderCompress(3,
                          BROTLI_DEFAULT_WINDOW,
                          BROTLI_DEFAULT_MODE,
                          content.size(),
                          reinterpret_cast<const uint8_t *>(content.data()),
                          &encoded_size,
                          output_buffer.data());
    return std::string(output_buffer.begin(), output_buffer.begin() + encoded_size);
}