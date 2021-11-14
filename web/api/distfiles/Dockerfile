FROM php:7.3-apache

RUN echo "ACSC{this is fake flag}" > /flag
RUN chmod 0444 /flag

COPY ./public/ /var/www/html/
COPY ./000-default.conf /etc/apache2/sites-available/
RUN chmod 0777 /var/www/html/lib/db/

RUN chown -R root:www-data /var/www/html/

RUN chmod 0755 /var/www/html/
