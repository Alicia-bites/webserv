#!/bin/sh

php-fpm8 &
nginx -g "daemon off;"