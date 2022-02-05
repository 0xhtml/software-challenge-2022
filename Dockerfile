FROM ubuntu:20.04 as build

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update
RUN apt install -y --no-install-recommends cmake build-essential zip
RUN apt install -y --no-install-recommends libboost-all-dev
RUN apt install -y --no-install-recommends libpugixml-dev

WORKDIR /wrk
COPY src /wrk/src
COPY utils /wrk/utils
COPY CMakeLists.txt /wrk/

RUN cmake -DCMAKE_BUILD_TYPE=release .

RUN cmake --build . --target main
RUN zip -9j build.zip main

FROM scratch as out
COPY --from=build /wrk/build.zip /build.zip
