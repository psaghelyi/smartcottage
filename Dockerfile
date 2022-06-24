FROM python:3

WORKDIR /usr/src/smartcottage

COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

COPY . .

CMD [ "python", "smartcottage", "--host", "127.0.0.1", "--port", "443" ]

EXPOSE 443
