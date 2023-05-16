FROM unit:python3.11

COPY app/requirements.txt /app/

RUN pip install -r /app/requirements.txt

COPY config.json /docker-entrypoint.d/config.json

COPY app /app/

ENV NETRC /app/.netrc

EXPOSE 80 443
