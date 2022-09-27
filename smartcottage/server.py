import json
import logging
import bottle
from datetime import datetime, timedelta

import influxdb_client
from influxdb_client.client.write_api import SYNCHRONOUS

token = "iHi9f5K-BXUS0-fJBuGM7O6W5Irc0kmG97bKLXnRYc1zjnLugTeDqNNFz9zaI6GAKeTE_glSABH_U2h-WeWKpw=="
org = "smartcottage"
bucket = "smartcottage"

logger = logging.getLogger('smartcottage')
logging.basicConfig(level=logging.INFO)


def create_routes(app):

    client = influxdb_client.InfluxDBClient(url="http://localhost:8086", token=token, org=org)


    @app.get('/heartbeat')
    def index():
        return dict(url=bottle.request.url, source=bottle.request.remote_route, status='OK')


    @app.get('/sensor/<sensor_id>')
    def get_sensor(sensor_id):
        query_api = client.query_api()

        p = {
            "_start": timedelta(days=-2),
            "_sensor": sensor_id
        }

        tables = query_api.query('''
            from(bucket:"smartcottage") |> range(start: _start)
                |> filter(fn: (r) => r["_measurement"] == "cottage")
                |> filter(fn: (r) => r["sensor"] == _sensor)
        ''', params=p)
        
        if not tables:
            return "{}"

        table_humidity,table_temperature = tables
        
        rows = [{
            "epoch": int(t["_time"].timestamp()),
            "data": {"temperature": t["_value"], "humidity": h["_value"]}
        } for (t,h) in zip(table_temperature.records,table_humidity.records)]

        return json.dumps(rows)


    @app.put('/sensor/<sensor_id>')
    def put_sensor(sensor_id):
        data = {key:float(value) for (key,value) in bottle.request.json.items()}
        logger.info(f"{datetime.now()} - {sensor_id}: {data}")
        
        write_api = client.write_api(write_options=SYNCHRONOUS)

        record = [influxdb_client.Point("cottage").tag("sensor", sensor_id).field(field,value) for (field,value) in data.items()]
        write_api.write(bucket="smartcottage", org="smartcottage", record=record)


    @app.put('/<measurement>/<sensor_id>')
    def put_sensor(measurement, sensor_id):
        data = {key:float(value) for (key,value) in bottle.request.json.items()}
        logger.info(f"{datetime.now()} - {sensor_id}: {data}")
        
        write_api = client.write_api(write_options=SYNCHRONOUS)

        record = [influxdb_client.Point(measurement).tag("sensor", sensor_id).field(field,value) for (field,value) in data.items()]
        write_api.write(bucket="smartcottage", org="smartcottage", record=record)


    @app.route('/')
    def default_route():
        return server_static('index.html')

    # Static Routes    
    @app.route('/<filename:re:.*>')    
    def server_static(filename):
        return bottle.static_file(filename, root='./web')
    
