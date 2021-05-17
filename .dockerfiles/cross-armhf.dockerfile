FROM debian:testing

RUN dpkg --add-architecture armhf
RUN apt-get update
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y bison gcc gcc-arm-linux-gnueabihf git make netbase pps-tools python3.9 python-is-python2 wget
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y libssl-dev:armhf libpython3.9-dev:armhf
