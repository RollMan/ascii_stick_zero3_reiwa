FROM alpine:3.19.1

# Build Dependencies
RUN apk add curl gcc alpine-sdk
## GCC Dependencies
RUN apk add gmp mpfr

# Build binutils
RUN curl -LO https://ftp.jaist.ac.jp/pub/GNU/binutils/binutils-2.42.tar.xz && \
    tar Jxvf binutils-2.42.tar.xz && \
    cd binutils-2.42.tar.xz && \
    ./configure --target=avr && \
    make -j6 && \
    make install

# Build GCC
RUN curl -LO https://ftp.jaist.ac.jp/pub/GNU/gcc/gcc-13.2.0/gcc-13.2.0.tar.xz && \
    tar Jxvf gcc-13.2.0.tar.xz && \
    cd gcc-13.2.0.tar.xz # TODO

