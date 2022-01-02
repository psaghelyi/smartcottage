import time
import json
import logging
import influxdb
import bottle
from datetime import datetime

logger = logging.getLogger('smartcottage')
logging.basicConfig(level=logging.INFO)


def create_routes(app):
    second2nano = 1_000_000_000
    one_day = 24 * 60 * 60

    client = influxdb.InfluxDBClient(
        host='127.0.0.1', port=8086, database='smartcottage')

    @app.get('/heartbeat')
    def index():
        return dict(url=bottle.request.url, source=bottle.request.remote_route, status='OK')

    @app.get('/sensor/<sensor_id>')
    def get_sensor(sensor_id):
        epoch_now = int(time.time())

        to_epoch = int(bottle.request.query.to_epoch or epoch_now)
        from_epoch = int(bottle.request.query.from_epoch or to_epoch - one_day * 2)
        limit = int(bottle.request.query.limit or 1000)

        result = client.query("\
            SELECT time, temperature, humidity \
            FROM cottage \
            WHERE time >= $start_time AND time < $end_time AND sensor = $sensor \
            LIMIT $limit",
            bind_params={
                "start_time": from_epoch * second2nano,
                "end_time": to_epoch * second2nano,
                "sensor": sensor_id,
                "limit": limit},
            epoch=True)
        
        rows = [{
            "epoch": int(row["time"]) // second2nano,
            "data": {"temperature": row["temperature"], "humidity": row["humidity"]}
        } for row in result.get_points() if row["temperature"] > -30.]

        return json.dumps(rows)

    @app.put('/sensor/<sensor_id>')
    def put_sensor(sensor_id):
        data = dict_variable = {key:float(value) for (key,value) in bottle.request.json.items()}
        logger.info(f"{datetime.now()} - {sensor_id}: {data}")
        item = {
            "measurement": "cottage",
            "tags": {
                "sensor": sensor_id
            },
            "time": int(time.time()) * second2nano,
            "fields": data
        }
        client.write_points([item])

    @app.route('/')
    def default_route():
        return server_static('index.html')

    # Static Routes    
    @app.route('/<filename:re:.*>')    
    def server_static(filename):
        return bottle.static_file(filename, root='./web')

