############################
# STEP 1 build executable binary
############################

FROM python:3.7.7-buster AS extracting
LABEL maintainer="azoulayos@protonmail.com"
RUN apt-get update &&\
    apt-get install -y --no-install-recommends git autoconf libtool swig texinfo build-essential gcc python3-libxml2 && \
    LIBXML2VER=2.9.9 && \
    mkdir libxmlInstall && cd libxmlInstall && \
    wget ftp://xmlsoft.org/libxml2/libxml2-$LIBXML2VER.tar.gz && \
    tar xf libxml2-$LIBXML2VER.tar.gz && \
    cd libxml2-$LIBXML2VER/ && \
    ./configure && \
    make && \
    make install && \
    cd /libxmlInstall && \
    rm -rf gg libxml2-$LIBXML2VER.tar.gz libxml2-$LIBXML2VER
WORKDIR /app
RUN git clone git://git.sv.gnu.org/libredwg.git && \
     cd libredwg && \
     sh autogen.sh && \
     ./configure --enable-trace --prefix=/usr/local && \
     make && \
     mkdir ldwg && \
     make install DESTDIR=$PWD/ldwg && \
     make check DESTDIR=$PWD/ldwg
     
############################
# STEP 2 copy the executable binary to thinest image
############################

FROM debian:stable-slim
COPY --from=extracting /app/libredwg/ldwg/usr/local/bin/* /usr/local/bin/
COPY --from=extracting /app/libredwg/ldwg/usr/local/include/* /usr/local/include/
COPY --from=extracting /app/libredwg/ldwg/usr/local/lib/* /usr/local/lib/
RUN ldconfig

CMD [ "sh" ]


