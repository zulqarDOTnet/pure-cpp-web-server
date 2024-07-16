# Pure C++ HTTP Web Server
Fast, lightweight, pure C++ HTTP Web Server

## Download

```bash
git clone https://github.com/zulqarDOTnet/pure-cpp-web-server.git
cd pure-cpp-web-server
```

# Compile & Run

### Docker
```bash
docker compose build && docker compose up -d
```

### Update Docker Container
```bash
docker compose build --no-cache && docker compose up -d --force-recreate
```

### Removing Docker Container
```bash
docker stop pure-cpp-web-server
docker rm pure-cpp-web-server
```

### Debian/Ubuntu and other derivatives
```bash
sudo apt-get install g++ build-essential nlohmann-json3-dev libboost-all-dev zlib1g-dev zstd brotli
sh build_run.sh
```

## Configuration

Please check in the config [`config.json`](config.json) for configuration options or to enable or disable certain features or behaviors.

## License

[![Author](https://img.shields.io/static/v1?label=author&message=Zulqarnain%20Zafar&color=green)](https://zulqar.net)
[![License](https://img.shields.io/static/v1?label=v3.0&message=GNU%20GENERAL%20PUBLIC%20LICENSE%20&color=red)](LICENSE)
