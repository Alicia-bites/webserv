#!/bin/bash

DATA=./data
CGI_PATH=./cgi

CGI="${CGI_PATH}/ubuntu_tester"
INFILE="${DATA}/GNU_Library.html"

export REQUEST_METHOD=GET
export SERVER_PROTOCOL=HTTP/1.1
#export PATH_INFO=./data
export PATH_INFO="."
#./${CGI} < ${INFILE}
./${CGI} ${INFILE}
