FROM python:3.8-slim

RUN apt-get -y update --fix-missing && apt-get -y upgrade
RUN apt-get -y install gcc g++ make libffi-dev libssl-dev git xinetd
RUN python3 -m pip install pycryptodome
RUN useradd cbcbc

ADD challenge/xinetd /etc/xinetd.d/cbcbc
WORKDIR /home/cbcbc
ADD challenge/chal.py chal.py
ADD challenge/secret.py secret.py

CMD ["/usr/sbin/xinetd", "-dontfork"]