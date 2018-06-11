FROM ubuntu

WORKDIR /root

RUN apt-get update;
RUN apt-get install -y cmake build-essential libboost-system-dev libboost-thread-dev libssl-dev python libtcmalloc-minimal4 libcurl4-openssl-dev

RUN ln -s /usr/lib/libtcmalloc_minimal.so.4 /usr/lib/libtcmalloc_minimal.so

ADD . crow
WORKDIR crow

RUN cmake .
RUN make

CMD ./src/gather