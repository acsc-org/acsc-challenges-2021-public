FROM ubuntu:20.04

RUN groupadd -r carot
RUN useradd -r -g carot carot

RUN apt update
RUN apt install -y xinetd python3

COPY ./xinetd_proxy_conf /etc/xinetd.d/proxy
COPY ./xinetd_carot_conf /etc/xinetd.d/carot

WORKDIR /home/carot
COPY ./challenge/ ./
RUN chown -R root:carot ./
RUN chmod 440 ./flag.txt
RUN chmod 550 ./run_carot.sh
RUN chmod 550 ./run_proxy.sh
RUN chmod 550 ./carot
RUN chmod 550 ./proxy.py

CMD ["/usr/sbin/xinetd", "-dontfork"]
