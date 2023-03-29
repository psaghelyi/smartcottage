all: prometheus bitwarden deluge filebrowser grafana jellyfin pigallery2 portainer nginx-reverse-proxy

prometheus: stop_prometheus start_prometheus exporters

exporters: stop_cadvisor start_cadvisor stop_node_exporter start_node_exporter stop_nginx_exporter start_nginx_exporter

stop_cadvisor:
	cd prometheus/exporters/cAdvisor; ./stop.sh

start_cadvisor:
	cd prometheus/exporters/cAdvisor; ./start.sh

stop_node_exporter:
	cd prometheus/exporters/node_exporter; ./stop.sh

start_node_exporter:
	cd prometheus/exporters/node_exporter; ./start.sh

stop_nginx_exporter:
	cd prometheus/exporters/nginx_exporter; ./stop.sh

start_nginx_exporter:
	cd prometheus/exporters/nginx_exporter; ./start.sh

stop_prometheus:
	cd prometheus; ./stop.sh

start_prometheus:
	cd prometheus; ./start.sh

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

pigallery2: stop_pigallery2 start_pigallery2

stop_pigallery2:
	cd pigallery2; ./stop.sh

start_pigallery2:
	cd pigallery2; ./start.sh

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
