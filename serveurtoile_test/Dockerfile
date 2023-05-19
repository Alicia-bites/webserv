FROM alpine:3.16.2

RUN apk update

RUN apk add php8 \
	php8-fpm \
	nginx \
	php8-cgi \
	php8-common \
	php8-mbstring \
	php8-zip \
	php8-phar \
	php8-gettext \
	php8-bcmath \
	php8-openssl

COPY ../front/ /usr/share/nginx/html
COPY entrypoint.sh /entrypoint.sh
COPY nginx.conf /etc/nginx/http.d/default.conf

RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]