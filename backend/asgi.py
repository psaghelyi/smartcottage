import logging
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

logger = logging.getLogger('smartcottage')
logging.basicConfig(level=logging.INFO)

app = FastAPI()
# app.mount("/", StaticFiles(directory="/web", html = True), name="web")

client: InfluxDBClientAsync
write_api: WriteApiAsync
query_api: QueryApiAsync

@app.on_event('startup')
async def startup_event():
    global client
    global write_api
    global query_api
    
    logger.info(f"{datetime.now()} - starting up")
    while True:
        ready = False
        try:
            client = InfluxDBClientAsync(url="http://host.docker.internal:8086", token=influxdb_token, org=influxdb_org)
            ready = await client.ping()
        except:
            pass
        logger.info(f"{datetime.now()} - ready: {ready}")
        if ready:
            break
        sleep(3)

    write_api = client.write_api()
    query_api = client.query_api()


@app.on_event("shutdown")
async def shutdown_event():
    logger.info(f"{datetime.now()} - shutting down")
    await client.close()


@app.get("/")
async def read_index():
    return FileResponse('/web/index.html')


@app.get("/heartbeat")
async def heartbeat(request: Request):
    return JSONResponse(content=jsonable_encoder({"request": request.url}))


@app.get('/sensor/{sensor_id}')
async def get_sensor(sensor_id):
    logger.info(f"{datetime.now()} - get sensor: {sensor_id}")

    start = timer()

    p = {
        "_start": timedelta(days=-2),
        "_sensor": sensor_id
    }

    tables = await query_api.query(f'''
        from(bucket:"{influxdb_bucket}") 
            |> range(start: _start)
            |> filter(fn: (r) => r["_measurement"] == "cottage")
            |> filter(fn: (r) => r["sensor"] == _sensor)
    ''', params=p)
    
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
    json_body = await request.json()
    data = {key:float(value) for (key,value) in json_body.items()}
    logger.info(f"{datetime.now()} - {sensor_id}: {data}")
    
    record = [Point("cottage").tag("sensor", sensor_id).field(field,value) for (field,value) in data.items()]
    await write_api.write(bucket=influxdb_bucket, org=influxdb_org, record=record)


@app.put('/<measurement>/{sensor_id}')
async def put_sensor(measurement: str, sensor_id: str, request: Request):
    json_body = await request.json()
    data = {key:float(value) for (key,value) in json_body.items()}
    logger.info(f"{datetime.now()} - {sensor_id}: {data}")
    
    record = [Point(measurement).tag("sensor", sensor_id).field(field,value) for (field,value) in data.items()]
    await write_api.write(bucket=influxdb_bucket, org=influxdb_org, record=record)
