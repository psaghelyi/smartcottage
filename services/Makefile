.PHONY: all

all:
	@echo "Usage: make [service]"

shinobi: stop_shinobi start_shinobi

stop_shinobi:
	cd shinobi; docker compose down

start_shinobi:
	cd shinobi; docker compose pull; docker compose up -d


portainer: stop_portainer start_portainer

stop_portainer:
	cd portainer; ./stop.sh

start_portainer:
	cd portainer; ./start.sh


prometheus_exporters: stop_prometheus_exporters start_prometheus_exporters

stop_prometheus_exporters:
	cd prometheus/exporters; docker compose down

start_prometheus_exporters:
	cd prometheus/exporters; docker compose pull; docker compose up -d
