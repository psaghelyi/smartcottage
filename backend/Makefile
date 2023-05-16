


all: influxdb telegraf web

influxdb: start_influxdb

telegraf: start_telegraf

web: start_web

start_influxdb: stop_influxdb
	cd influxdb2; ./start.sh

stop_influxdb:
	cd influxdb2; ./stop.sh	

start_telegraf: stop_telegraf
	cd telegraf; ./start.sh

stop_telegraf:
	cd telegraf; ./stop.sh

start_web: build_web stop_web
	cd web; ./start.sh

stop_web:
	cd web; ./stop.sh

build_web:
	cd web; ./build.sh
