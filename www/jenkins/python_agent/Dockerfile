FROM debian:bookworm-slim

RUN apt update && apt upgrade -y && apt install -y python3 php php-cgi

RUN useradd testmaster

USER testmaster
