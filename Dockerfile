FROM ubuntu:18.04

RUN apt-get update && apt-get install -y \
  build-essential \
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
RUN ./setup.py

WORKDIR /root/devstone_comparative/web_service
RUN python3 manage.py -i -a test
RUN python3 web_service.py

