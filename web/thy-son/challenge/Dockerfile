FROM gradle:7.0.2-jdk8

COPY flag /flag

RUN mkdir -p /usr/local/app
WORKDIR /usr/local/app

RUN adduser --system --group spring
RUN chown spring:spring .
USER spring:spring

#RUN gradle init

COPY build.gradle ./build.gradle
COPY src/ ./src

RUN gradle build --no-daemon

ENTRYPOINT ["java","-jar","/usr/local/app/build/libs/app-0.0.1-SNAPSHOT.jar"]
