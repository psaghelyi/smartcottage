import logging
from statsd import StatsClient
from time import sleep
from datetime import datetime, timedelta
from timeit import default_timer as timer

from influxdb_client import Point
from influxdb_client.client.influxdb_client_async import InfluxDBClientAsync
from influxdb_client.client.write_api_async import WriteApiAsync
from influxdb_client.client.query_api_async import QueryApiAsync

from fastapi import FastAPI, Request
from fastapi.staticfiles import StaticFiles
from fastapi.encoders import jsonable_encoder
from fastapi.responses import JSONResponse, FileResponse


influxdb_token = "fHy2mZrpErZMlshloWnhk6jMp14C4QPjyCgYYpR3S1nox1oLax3upiK5IdsUBhH_nDQ1gRQwpclUlpz46x8azA=="
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
            except:
                pass
            logger.info(f"{datetime.now()} - ready: {ready}")
            if ready:
                break
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
    return FileResponse('/app/index.html')


@app.get("/")
async def read_root():
    return await read_index()


@app.get("/heartbeat")
async def heartbeat(request: Request):
    return JSONResponse(content=jsonable_encoder({"request": request.url}))


@app.get('/sensor/{sensor_id}')
async def get_sensor(sensor_id):
    with statsd.timer('get_sensor'):
        logger.info(f"{datetime.now()} - get sensor: {sensor_id}")

        start = timer()

        params = {
            "_start": timedelta(days=-2),
            "_sensor": sensor_id
        }

        query = f'''
            from(bucket:"{influxdb_bucket}") 
                |> range(start: _start)
                |> filter(fn: (r) => r["_measurement"] == "cottage")
                |> filter(fn: (r) => r["sensor"] == _sensor)
        '''

        with statsd.timer('get_sensor_db'):
            tables = await query_api.query(query=query, params=params)

        if not tables:
            return JSONResponse(content=jsonable_encoder({}))

        table_humidity,table_temperature = tables
        
        rows = [{
            "epoch": int(t["_time"].timestamp()),
            "data": {"temperature": t["_value"], "humidity": h["_value"]}
        } for (t,h) in zip(table_temperature.records,table_humidity.records)]

        logger.info(f"{datetime.now()} - get sensor: {sensor_id} [{timer() - start}]")

        return JSONResponse(content=jsonable_encoder(rows))


@app.put('/sensor/{sensor_id}')
async def put_sensor(sensor_id: str, request: Request):
    with statsd.timer('put_sensor'):
        await put_measurement("cottage", sensor_id, request)


@app.put('/{measurement}/{sensor_id}')
async def put_measurement(measurement: str, sensor_id: str, request: Request):
    json_body = await request.json()
    data = {key:float(value) for (key,value) in json_body.items()}
    logger.info(f"{datetime.now()} - {sensor_id}: {data}")
    
    record = [Point(measurement).tag("sensor", sensor_id).field(field,value) for (field,value) in data.items()]
    with statsd.timer('put_sensor_db'):
        await write_api.write(bucket=influxdb_bucket, org=influxdb_org, record=record)
