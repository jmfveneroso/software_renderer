docker build -t sybil .
docker run -d -p 8888:8888 -v $(pwd):/code --rm --user root sybil
