FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
  build-essential \
  cmake \
  curl \
  git \
  openjdk-11-jdk-headless \
  libboost-all-dev \
  python3.8 \
  python3-pip \

RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
RUN rustup update
RUN pip3 install setuptools flask

COPY .git/ /root/devstone_comparative/.git/
COPY devstone/ /root/devstone_comparative/devstone/
COPY simulators/ /root/devstone_comparative/simulators/
COPY web_service/ /root/devstone_comparative/web_service/
COPY events_devstone.txt clean.sh setup.sh devstone_comparative.py .gitmodules /root/devstone_comparative/

WORKDIR /root/devstone_comparative
RUN ./clean.sh
RUN ./setup.sh

RUN python3 web_service/manage.py -i -a test
ENTRYPOINT python3 web_service/web_service.py
