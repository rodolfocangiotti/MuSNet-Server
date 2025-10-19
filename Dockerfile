FROM ubuntu:latest

RUN apt-get update
RUN apt-get install -y g++ git openssh-client

# By default the user which operates inside the image is root...
RUN mkdir -p /root/.ssh
RUN chmod 700 /root/.ssh
RUN ssh-keyscan github.com > /root/.ssh/known_hosts

WORKDIR /home/ubuntu
RUN --mount=type=ssh GIT_SSH_COMMAND="ssh -v" git clone git@github.com:rodolfocangiotti/MuSNet-Server

WORKDIR /home/ubuntu/MuSNet-Server
RUN git checkout beta_draft
RUN g++ -std=c++11 -DDEBUG -DVERBOSENESS=3 -omainserver *.cpp

RUN mkdir -v /home/ubuntu/muslogs

EXPOSE 50000
EXPOSE 50001/udp

ENTRYPOINT ["./mainserver"]
