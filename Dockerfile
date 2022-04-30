FROM ubuntu:20.04 as build

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update
RUN apt install -y --no-install-recommends cmake build-essential zip
RUN apt install -y --no-install-recommends libboost-all-dev
RUN apt install -y --no-install-recommends libpugixml-dev
RUN apt install -y --no-install-recommends default-jre
RUN apt install -y --no-install-recommends wget p7zip-full

WORKDIR /wrk

RUN wget https://github.com/software-challenge/backend/releases/download/22.1.2/software-challenge-server.zip
RUN 7z x -aoa software-challenge-server.zip -oserver

COPY src /wrk/src
COPY utils /wrk/utils
COPY CMakeLists.txt /wrk/

RUN cmake -DCMAKE_BUILD_TYPE=release-gen .
RUN cmake --build . --target main

RUN python3 utils/test-with-server.py

RUN cmake -DCMAKE_BUILD_TYPE=release-use .
RUN cmake --build . --target main

RUN zip -9j build.zip main

FROM scratch as out
COPY --from=build /wrk/build.zip /build.zip
