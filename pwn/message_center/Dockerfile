FROM ubuntu:20.04
MAINTAINER angelboy
RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install xinetd -y
RUN apt-get install libssl1.1 -y
RUN useradd -m message_center
RUN chmod 774 /tmp
RUN chmod -R 774 /var/tmp
RUN chmod -R 774 /dev
RUN chmod -R 774 /run
RUN chmod 1733 /tmp /var/tmp /dev/shm
RUN chown -R root:root /home/message_center
CMD ["/usr/sbin/xinetd","-dontfork"]
