FROM ubuntu:20.04 

LABEL author="jason810496"

# Install dependencies for c development
RUN apt-get update && apt-get install -y
RUN apt-get install -y build-essential
RUN apt-get install -y gcc
RUN apt-get install -y make
# Development tools
RUN apt-get install -y git
RUN apt-get install -y wget
RUN apt-get install -y curl
RUN apt-get install -y vim
# Python tools
RUN apt-get install -y python3
RUN apt-get install -y python3-pip
RUN apt-get install -y python3-dev
# Linters
RUN apt-get install -y clang-format
# Misc
RUN apt-get install -y sudo


# zsh quick setup
RUN git clone https://github.com/jotyGill/ezsh
RUN cd ezsh && ./install.sh -c

# pthread
RUN apt-get install -y libpthread-stubs0-dev

CMD [ "tail", "-f", "/dev/null" ]