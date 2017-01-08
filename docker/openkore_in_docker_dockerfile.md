# OpenKore in Docker  Dockerfile


https://github.com/John-Lin/docker-openkore

- Dockerfile

```sh
# OpenKore in Docker
FROM ubuntu:14.04.4

MAINTAINER John Lin <linton.tw@gmail.com>

ENV HOME /root
# Define working directory.
WORKDIR /root

RUN apt-get update && \
    apt-get install -y \
        python-setuptools \
        python-pip \
        curl \
        build-essential \
        g++ \
        perl \
        libreadline6-dev \
        libcurl4-gnutls-dev \
        zlib1g-dev \
        vim
        # vim &&
        # echo $'set encoding=utf-8\n\
        # set fileencoding=utf-8' >> /root/.vimrc

# RUN curl -kL https://github.com/OpenKore/openkore/archive/twRO_New.tar.gz | tar -xvz \
RUN curl -kL https://github.com/John-Lin/openkore/archive/test.tar.gz | tar -xvz \
    && mv openkore-test openkore \
    && cd openkore \
    && make

# Clean up APT when done.
RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
```

- docker build

```
sudo docker build .
```