FROM ubuntu:20.04

RUN apt update && apt install -y python3 python3-pip
COPY requirements.txt /tmp
RUN pip install -r /tmp/requirements.txt
COPY app.py /srv
COPY emojis.txt /srv

WORKDIR /srv
CMD ["python3", "app.py"]
