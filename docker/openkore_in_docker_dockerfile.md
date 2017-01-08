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

- run

```sh
sudo docker images

[sudo] password for shihyu: 
REPOSITORY          TAG                 IMAGE ID            CREATED             SIZE
<none>              <none>              d2ebaeec21d8        4 minutes ago       530.9 MB
ubuntu              12.04               f0d07a756afd        3 weeks ago         103.6 MB
ubuntu              14.04.4             0ccb13bf1954        5 months ago        188 MB
hello-world         latest              c54a2cc56cbb        6 months ago        1.848 kB


// REPOSITORY is ubuntu, TAG is 14.04
sudo docker run -t -i ubuntu:14.04 bash
```



