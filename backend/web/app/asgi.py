import logging
from statsd import StatsClient
from time import sleep
from datetime import datetime, timedelta
from asyncio import create_task, gather
from aiohttp import ClientSession

from influxdb_client import Point
from influxdb_client.client.influxdb_client_async import InfluxDBClientAsync
from influxdb_client.client.write_api_async import WriteApiAsync
from influxdb_client.client.query_api_async import QueryApiAsync

from fastapi import FastAPI, Request
from fastapi.staticfiles import StaticFiles
from fastapi.encoders import jsonable_encoder
from fastapi.responses import JSONResponse, FileResponse

from secrets import influxdb_token, homeassistant_token

influxdb_org = "smartcottage"
influxdb_bucket = "smartcottage"

logger = logging.getLogger('web')
logging.basicConfig(level=logging.INFO)

statsd = StatsClient('host.docker.internal', 8125, prefix='app')

app = FastAPI()
# app.mount("/", StaticFiles(directory="/web", html = True), name="web")

db_client: InfluxDBClientAsync
write_api: WriteApiAsync
query_api: QueryApiAsync

background_tasks = set()


@app.on_event('startup')
async def startup_event():
    global db_client
    global write_api
    global query_api

    with statsd.timer('startup'):
        logger.info(f"{datetime.now()} - starting up")
        while True:
            ready = False
            try:
                db_client = InfluxDBClientAsync(url="http://host.docker.internal:8086", token=influxdb_token, org=influxdb_org)
                ready = await db_client.ping()
            except Exception as ex:
                logger.error(f"{datetime.now()} - influxdb: {ex}")
            logger.info(f"{datetime.now()} - influxdb: ready={ready}")
            if ready:
                break
            await db_client.close()
            sleep(3)

        write_api = db_client.write_api()
        query_api = db_client.query_api()


@app.on_event("shutdown")
async def shutdown_event():
    logger.info(f"{datetime.now()} - shutting down")
    with statsd.timer('shutdown'):
        await db_client.close()


@app.get("/index.html")
async def read_index():
    with statsd.timer('read_index'):
        return FileResponse('/app/index.html')


@app.get("/")
async def read_root():
    return await read_index()


@app.get("/api/heartbeat")
async def heartbeat(request: Request):
    return JSONResponse(content=jsonable_encoder({"request": request.url}))


async def get_from_db(query, params):
    with statsd.timer('get_sensor_db'):
        tables = await query_api.query(query=query, params=params)

    if not tables:
        return []

    table_humidity,table_temperature = tables
    
    rows = [{
        "epoch": int(t["_time"].timestamp()),
        "data": {"temperature": t["_value"], "humidity": h["_value"]}
    } for (t,h) in zip(table_temperature.records,table_humidity.records)]

    return rows


@app.get('/api/last/{measurement}/{sensor_id}')
async def get_measurement_last(measurement: str, sensor_id: str):
    logger.info(f"{datetime.now()} - get measurement mean: {measurement}/{sensor_id}")
    with statsd.timer('get_measurement'):
        params = {
            "_measurement": measurement,
            "_sensor": sensor_id
        }

        query = f'''
            from(bucket:"{influxdb_bucket}")
                |> range(start: -1h)
                |> filter(fn: (r) => r["_measurement"] == _measurement)
                |> filter(fn: (r) => r["sensor"] == _sensor)
        '''

        rows = await get_from_db(query, params)
        t = sum(i["data"]["temperature"] for i in rows) / len(rows)
        h = sum(i["data"]["humidity"] for i in rows) / len(rows)

        return JSONResponse(content=jsonable_encoder( {"t": t, "h": h} ))


@app.get('/api/{measurement}/{sensor_id}')
async def get_measurement(measurement: str, sensor_id: str, days: int = 2):
    logger.info(f"{datetime.now()} - get measurement: {measurement}/{sensor_id}")
    with statsd.timer('get_measurement'):
        params = {
            "_start": timedelta(days=-days),
            "_measurement": measurement,
            "_sensor": sensor_id
        }

        query = f'''
            from(bucket:"{influxdb_bucket}")
                |> range(start: _start)
                |> filter(fn: (r) => r["_measurement"] == _measurement)
                |> filter(fn: (r) => r["sensor"] == _sensor)
        '''

        rows = await get_from_db(query, params)
        return JSONResponse(content=jsonable_encoder(rows))


@app.put('/api/{measurement}/{sensor_id}')
async def put_measurement(measurement: str, sensor_id: str, request: Request):
    with statsd.timer('put_measurement'):
        json_body = await request.json()
        data = {key:float(value) for (key,value) in json_body.items()}
        logger.info(f"{datetime.now()} - put measurement: {measurement}/{sensor_id}: {data}")
        record = [Point(measurement).tag("sensor", sensor_id).field(field,value) for (field,value) in data.items()]
        with statsd.timer('put_measurement_db'):
            await write_api.write(bucket=influxdb_bucket, org=influxdb_org, record=record)

    task = create_task(homeassistant_send(measurement, sensor_id, data))
    background_tasks.add(task)
    task.add_done_callback(background_tasks.discard)



async def homeassistant_post(session, payload):
    url = f'http://192.168.192.1:8123/api/states/sensor.{payload["attributes"]["friendly_name"]}'
    headers = { "Authorization": f"Bearer {homeassistant_token}"}
    async with session.post(url, json=payload, headers=headers) as resp:
        return await resp.text()


'''
    this is a fire and forget approach of forwarding measurements to HomeAssistant
'''
async def homeassistant_send(measurement, sensor_id, data):
    payloads = []
    if "temperature" in data:
        payloads.append({
            "state": data["temperature"],
            "attributes": {
                "state_class": "measurement",
                "device_class": "temperature",
                "unit_of_measurement": "°C",
                "friendly_name": f"{sensor_id}_temperature"
            }
        })
    if "humidity" in data:
        payloads.append({
            "state": data["humidity"],
            "attributes": {
                "state_class": "measurement",
                "device_class": "humidity",
                "unit_of_measurement": "%",
                "friendly_name": f"{sensor_id}_humidity"
            }
        })
    if "pm1p0" in data:
        payloads.append({
            "state": data["pm1p0"],
            "attributes": {
                "state_class": "measurement",
                "device_class": "pm1",
                "unit_of_measurement": "µg/m³",
                "friendly_name": f"{sensor_id}_pm1p0"
            }
        })
    if "pm2p5" in data:
        payloads.append({
            "state": data["pm2p5"],
            "attributes": {
                "state_class": "measurement",
                "device_class": "pm25",
                "unit_of_measurement": "µg/m³",
                "friendly_name": f"{sensor_id}_pm2p5"
            }
        })
    if "pm10p0" in data:
        payloads.append({
            "state": data["pm10p0"],
            "attributes": {
                "state_class": "measurement",
                "device_class": "pm10",
                "unit_of_measurement": "µg/m³",
                "friendly_name": f"{sensor_id}_pm10p0"
            }
        })

    async with ClientSession() as session:
        responses = await gather(*[homeassistant_post(session, payload) for payload in payloads])
        logger.info(f"{datetime.now()} - send measurements: {responses}")


# deprecated
@app.get('/sensor/{sensor_id}')
async def get_sensor(sensor_id: str):
    return await get_measurement("cottage", sensor_id)


# deprecated
@app.put('/sensor/{sensor_id}')
async def put_sensor(sensor_id: str, request: Request):
    await put_measurement("cottage", sensor_id, request)
