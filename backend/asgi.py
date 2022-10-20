import json
import logging
import influxdb_client
from influxdb_client.client.write_api import ASYNCHRONOUS
from fastapi import FastAPI, Request
from fastapi.staticfiles import StaticFiles
from fastapi.encoders import jsonable_encoder
from fastapi.responses import JSONResponse

influxdb_token = "iHi9f5K-BXUS0-fJBuGM7O6W5Irc0kmG97bKLXnRYc1zjnLugTeDqNNFz9zaI6GAKeTE_glSABH_U2h-WeWKpw=="
influxdb_org = "smartcottage"
influxdb_bucket = "smartcottage"

logger = logging.getLogger('smartcottage')
logging.basicConfig(level=logging.INFO)

app = FastAPI()
app.mount("/", StaticFiles(directory="/web", html = True), name="web")

@app.get("/heartbeat")
async def index(request: Request):
    return JSONResponse(content=jsonable_encoder({"request": request}))

