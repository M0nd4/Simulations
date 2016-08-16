FROM ubuntu:latest
MAINTAINER Vincent Beffara <vbeffara@gmail.com>

RUN apt-get update -qq
RUN apt-get install -y -qq python-pip cmake gcc g++ ccache lcov git ninja-build xvfb
