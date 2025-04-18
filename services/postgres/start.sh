#!/bin/bash

# Create necessary directories if they don't exist
mkdir -p .postgres-data

docker run -d \
    --name postgres \
    --pull=always \
    --restart=always \
    --user=$(id -u):$(id -g) \
    -p 5432:5432 \
    --add-host host.docker.internal:host-gateway \
    -v $PWD/.postgres-data:/var/lib/postgresql/data \
    -v $PWD/postgresql.conf:/etc/postgresql/postgresql.conf \
    -e POSTGRES_PASSWORD=postgres \
    postgres:17


# wait for postgres to be ready
echo "Waiting for PostgreSQL to be ready..."
until docker exec -it postgres pg_isready -U postgres > /dev/null 2>&1; do
  echo "PostgreSQL is starting up. Waiting..."
  sleep 2
done
echo "PostgreSQL is ready!"


#docker exec -it postgres psql -U postgres -c "ALTER USER postgres WITH PASSWORD 'postgres';"

# create user 'sirway' with password 'sirway'
docker exec -it postgres psql -U postgres -c "CREATE USER sirway WITH PASSWORD 'sirway';"

# create database 'sirway'
docker exec -it postgres psql -U postgres -c "CREATE DATABASE sirway;"

# grant privileges to sirway user
docker exec -it postgres psql -U postgres -c "GRANT ALL PRIVILEGES ON DATABASE sirway TO sirway;"
docker exec -it postgres psql -U postgres -c "ALTER DATABASE sirway OWNER TO sirway;"
docker exec -it postgres psql -U postgres -d sirway -c "GRANT ALL ON SCHEMA public TO sirway;"
docker exec -it postgres psql -U postgres -d sirway -c "GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO sirway;"
docker exec -it postgres psql -U postgres -d sirway -c "GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public TO sirway;"



