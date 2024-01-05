.PHONY: all

all: prometheus elk bitwarden deluge filebrowser grafana jellyfin portainer nginx-reverse-proxy squid-cache

prometheus: stop_prometheus start_prometheus

stop_prometheus:
	cd prometheus; docker compose down

start_prometheus:
	cd prometheus; docker compose pull; UID=$$(id -u) GID=$$(id -g) docker compose up -d

prometheus_exporters: stop_prometheus_exporters start_prometheus_exporters

stop_prometheus_exporters:
	cd prometheus/exporters; docker compose down

start_prometheus_exporters:
	cd prometheus/exporters; docker compose pull; docker compose up -d


elk: stop_elk start_elk

stop_elk:
	cd ELK; docker compose down

start_elk:
	cd ELK; docker compose pull; docker compose up -d


bitwarden: stop_bitwarden start_bitwarden

stop_bitwarden:
	cd bitwarden; ./stop.sh

start_bitwarden:
	cd bitwarden; ./start.sh


deluge: stop_deluge start_deluge

stop_deluge:
	cd deluge; ./stop.sh

start_deluge:
	cd deluge; ./start.sh


filebrowser: stop_filebrowser start_filebrowser

stop_filebrowser:
	cd filebrowser; ./stop.sh

start_filebrowser:
	cd filebrowser; ./start.sh


grafana: stop_grafana start_grafana

stop_grafana:
	cd grafana; ./stop.sh

start_grafana:
	cd grafana; ./start.sh


jellyfin: stop_jellyfin start_jellyfin

stop_jellyfin:
	cd jellyfin; ./stop.sh

start_jellyfin:
	cd jellyfin; ./start.sh


photoprism: stop_photoprism start_photoprism

stop_photoprism:
	cd photoprism; docker compose down

start_photoprism:
	cd photoprism; docker compose pull; docker compose up -d


portainer: stop_portainer start_portainer

stop_portainer:
	cd portainer; ./stop.sh

start_portainer:
	cd portainer; ./start.sh


nginx-reverse-proxy: stop_nginx-reverse-proxy start_nginx-reverse-proxy

stop_nginx-reverse-proxy:
	cd nginx-reverse-proxy; ./stop.sh

start_nginx-reverse-proxy:
	cd nginx-reverse-proxy; ./start.sh


squid-cache: stop_squid-cache start_squid-cache

stop_squid-cache:
	cd squid-cache; ./stop.sh

start_squid-cache:
	cd squid-cache; ./start.sh


shinobi: stop_shinobi start_shinobi

stop_shinobi:
	cd shinobi; docker compose down

start_shinobi:
	cd shinobi; docker compose pull; docker compose up -d

