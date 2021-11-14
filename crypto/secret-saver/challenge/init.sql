create database secret;
CREATE USER 'secret'@'%' IDENTIFIED BY 'Secret1234!@#$';
FLUSH PRIVILEGES;

GRANT ALL ON secret.* TO 'secret'@'%';
FLUSH PRIVILEGES;

USE secret;
CREATE TABLE msgs (id int AUTO_INCREMENT, name text, msg text, PRIMARY KEY(id))