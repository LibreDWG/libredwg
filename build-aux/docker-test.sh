#!/bin/sh
docker build -t libredwg -f build-aux/Dockerfile.jammy .
# run extra to keep the term lines for longer scrollback buffer
docker run -e COLUMNS=$COLUMNS -e LINES=$LINES -e TERM=$TERM -it libredwg sh -c "sh autogen.sh && ./configure --enable-release && make && make distcheck" || \
  docker run -e COLUMNS=$COLUMNS -e LINES=$LINES -e TERM=$TERM -it libredwg bash --login -i
