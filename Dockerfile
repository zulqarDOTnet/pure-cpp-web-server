FROM alpine:latest

# By default, Alpine Linux has a smaller thread stack size than other platforms.
# To increase it, set the PTHREAD_STACK_MIN environment variable during Docker image build.
ENV PTHREAD_STACK_MIN=2097152

RUN echo "fs.file-max = 65535" > /etc/sysctl.conf

RUN echo "http://dl-cdn.alpinelinux.org/alpine/edge/main" >> /etc/apk/repositories
RUN echo "http://dl-cdn.alpinelinux.org/alpine/edge/community" >> /etc/apk/repositories
RUN echo "http://dl-cdn.alpinelinux.org/alpine/edge/testing" >> /etc/apk/repositories

RUN apk update && apk upgrade && apk --no-cache add tzdata \
    g++ boost-dev boost-libs nlohmann-json zlib-dev zstd brotli-dev

RUN mkdir -p /app

COPY src /source/src
COPY web_server.cpp /source/web_server.cpp

WORKDIR /source/

RUN g++ -std=gnu++17 -o /app/web_server web_server.cpp -lboost_system -lboost_thread -lboost_iostreams -lpthread -lz -lzstd -lbrotlienc -lbrotlicommon

WORKDIR /app/

RUN ["chmod", "a+x", "web_server"]

EXPOSE 8080
# EXPOSE 8443

HEALTHCHECK --interval=60s --timeout=15s \
            CMD netstat -lntp | grep -q ':8080'

STOPSIGNAL SIGTERM
# ENTRYPOINT [ "./web_server", "127.0.0.1", "::1" ]
# ENTRYPOINT [ "./web_server", "0.0.0.0", "::1" ]
ENTRYPOINT [ "./web_server", "0.0.0.0", "::" ]

