FROM psaghelyi/nginx-unit:1.28.0-python3.10

COPY app/requirements.txt /app/

RUN pip install -r /app/requirements.txt

COPY config.json /docker-entrypoint.d/config.json

COPY app /app/

#COPY netrc /root/.netrc

#RUN chown nobody:nogroup /root/.netrc

ENV NETRC /nonexistent

EXPOSE 80 443
