FROM ubuntu:18.04

RUN apt-get update && apt-get install -y \
  build-essential \
  cmake \
  git \
  openjdk-11-jdk-headless \
  libboost-all-dev \
  python3.6 \
  python3-pip


RUN pip3 install setuptools flask

COPY .git/ /root/devstone_comparative/.git/
COPY devstone/ /root/devstone_comparative/devstone/
COPY web_service/ /root/devstone_comparative/web_service/
COPY events_devstone.txt clean.sh setup.sh devstone_comparative.py .gitmodules /root/devstone_comparative/

RUN pip3 install setuptools flask

WORKDIR /root/devstone_comparative
RUN ./clean.sh
RUN ./setup.sh

RUN python3 web_service/manage.py -i -a test
ENTRYPOINT python3 web_service/web_service.py

