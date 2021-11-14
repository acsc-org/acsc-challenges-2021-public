FROM php:7.3-apache

RUN echo "ACSC{it_is_hard_to_name_a_flag..isn't_it?}" > /flag
RUN chmod 0444 /flag

COPY ./public/ /var/www/html/
COPY ./000-default.conf /etc/apache2/sites-available/
RUN chmod 0777 /var/www/html/lib/db/

RUN chown -R root:www-data /var/www/html/

RUN chmod 0755 /var/www/html/
