FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive
RUN sed -i.bak -e "s%http://archive.ubuntu.com/ubuntu/%http://ftp.jaist.ac.jp/pub/Linux/ubuntu/%g" /etc/apt/sources.list
RUN apt-get update && apt-get -y dist-upgrade && apt-get install -y tzdata
ENV TZ=Asia/Tokyo

RUN apt update --fix-missing && apt -y upgrade --fix-missing
RUN apt-get install xinetd -y
RUN apt-get install qemu-system -y

COPY ./xinetd /etc/xinetd.d/sysnote
COPY ./distfiles/bzImage /
COPY ./rootfs.cpio /
COPY ./run.sh /

RUN chmod +x /run.sh

CMD ["/usr/sbin/xinetd","-dontfork"]
