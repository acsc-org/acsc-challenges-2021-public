FROM tiangolo/uwsgi-nginx-flask:python3.8

RUN apt update
RUN groupadd -r ctf && useradd -r -g ctf ctf
RUN pip install Flask

WORKDIR /home/ctf
ADD ./challenge/app.py ./
ADD ./distfiles/histogram.bin ./
ADD ./challenge/flag.txt ./
ADD ./challenge/uwsgi.ini ./
ADD ./challenge/templates ./templates
RUN chown root:ctf ./ -R
