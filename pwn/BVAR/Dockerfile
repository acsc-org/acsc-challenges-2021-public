FROM ubuntu:20.04

RUN groupadd -r bvar && useradd -r -g bvar bvar

RUN apt-get update
RUN apt-get install xinetd -y
RUN chmod 774 /tmp
RUN chmod -R 774 /var/tmp
RUN chmod -R 774 /dev
RUN chmod -R 774 /run
RUN chmod 1733 /tmp /var/tmp /dev/shm

COPY ./xinetd /etc/xinetd.d/bvar

WORKDIR /home/bvar/
COPY ./challenge/ ./
RUN chown root:bvar ./ -R
RUN chmod 550 ./bvar
RUN chmod 550 ./run.sh

CMD ["/usr/sbin/xinetd","-dontfork"]
