FROM psaghelyi/nginx-unit:1.28.0-python3.10

COPY requirements.txt /fastapi/requirements.txt

RUN pip install -r /fastapi/requirements.txt

COPY config.json /docker-entrypoint.d/config.json

COPY .cert/bundle.pem /docker-entrypoint.d/bundle.pem

COPY . /fastapi

COPY ./web /web

#COPY netrc /root/.netrc

#RUN chown nobody:nogroup /root/.netrc

ENV NETRC /nonexistent

EXPOSE 80 443
