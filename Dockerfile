FROM debian:12.7 as builder


ENV DEBIAN_FRONTEND=noninteractive \
    CONAN_PATH=.conan2 

RUN apt-get update && \
    apt-get -y install \
    git gcc-12 g++-12 clang-format-19 make cmake ninja-build \
    autoconf automake libtool binutils \
    libstdc++-12-dev \
    libdigest-sha-perl libipc-run-perl \
    google-perftools glibc-source libgoogle-perftools-dev \
    cppcheck libev-dev libpcre3-dev \
    gettext flex \
    python3 python3-pip \
    lcov wget unzip

RUN pip3 install conan --break-system-packages && \
    mkdir build && \
    cd build && \
    conan profile detect --force 

RUN cp ~/.conan2/profiles/default ~/.conan2/profiles/debug && sed -i -e 's/Release/Debug/g' ~/.conan2/profiles/debug

RUN apt-get -y install \
    valgrind

WORKDIR build
COPY . . 

RUN scripts/cpp-check-install
RUN scripts/cpp-check
RUN scripts/style-check
RUN conan install . -of .build -pr debug --build missing
RUN cmake -H. -B.build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_CXX_CPPCHECK=/usr/local/bin/cppcheck
RUN cmake --build .build
RUN cmake --install .build
RUN ctest --test-dir .build -T Test -T Coverage -V
RUN scripts/mem-check bin/test_sort_keys
RUN bin/test_bench --benchmark_counters_tabular=true

RUN lcov --directory .build --capture --output-file coverage.info
RUN lcov --extract coverage.info '*src/*' -o coverage.info
RUN genhtml --demangle-cpp -o coverage coverage.info

################################################################################

FROM debian:12.7-slim as runtime

LABEL org.opencontainers.image.vendor="LCC Svyazcom"
LABEL org.opencontainers.image.author="Aleksey.Ozhigov<burnes@svyazcom.ru>"
LABEL org.opencontainers.image.source="github.com:AlexBurnes/practice_lua_bind.git"

RUN apt-get update && \
    apt-get install -y libgoogle-perftools4 nginx

COPY --from=builder /build/bin/* /usr/local/bin/*
COPY --from=builder /build/coverage /var/www/html

EXPOSE 80/tcp

CMD ["/usr/sbin/nginx", "-g", "daemon off;"]

