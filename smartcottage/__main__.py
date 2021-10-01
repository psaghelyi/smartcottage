import argparse
import bottle
import time
import json
import logging
import sqlite3
from bottle_sqlite import SQLitePlugin
from datetime import datetime

logger = logging.getLogger('smartcottage')

app = application = bottle.default_app()


@app.get('/heartbeat')
def index():
    return dict(status='OK')


@app.get('/sensor/<id>')
def get_sensor(db, id):
    one_day = 24 * 60 * 60
    epoch_now = int(time.time())

    to_epoch = int(bottle.request.query.to_epoch or epoch_now)
    from_epoch = int(bottle.request.query.from_epoch or to_epoch - one_day)
    limit = int(bottle.request.query.limit or 1000)

    cursor = \
        db.execute('''
                    SELECT * FROM (
                        SELECT * FROM sensor
                        WHERE id=?
                        ORDER BY epoch DESC
                        LIMIT ?)
                    ORDER BY epoch;
                   ''', (id, -limit)) \
        if limit < 0 else \
        db.execute('''
                    SELECT * FROM (
                        SELECT * FROM sensor
                        WHERE id=? AND epoch>=? AND epoch<?
                        ORDER BY epoch DESC
                        LIMIT ?)
                    ORDER BY epoch;
                   ''', (id, from_epoch, to_epoch, limit))

    rows = [{
        "epoch": row["epoch"],
        "id": row["id"],
        "data": json.loads(row["data"])
    } for row in cursor.fetchall()]
    return json.dumps(rows)


@app.put('/sensor/<id>')
def put_sensor(id, db):
    epoch_time = int(time.time())
    try:
        data = json.dumps(bottle.request.json)
    except:
        logger.error(
            f"{datetime.now()} - {id}: cannot process sensor data: {bottle.request.body.read()}")
        return

    logger.info(f"{datetime.now()} - {id}: {data}")

    db.execute('''INSERT INTO sensor (epoch, id, data) VALUES (?,?,?);''',
               (epoch_time, id, data))
    db.commit()


def init_sensor_db(conn):
    conn.cursor().execute(
        '''CREATE TABLE IF NOT EXISTS sensor (epoch INTEGER, id TEXT, data TEXT);''')
    conn.cursor().execute('''CREATE INDEX IF NOT EXISTS idx_epoch_id ON sensor (epoch, id);''')
    conn.commit()


def parseargs():
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", type=int, default=12345)
    parser.add_argument("--workers", type=int, default=4)
    parser.add_argument("--timeout", type=int, default=30)
    return parser.parse_args()


def main():
    args = parseargs()
    logging.basicConfig(level=logging.INFO)

    with sqlite3.connect('sensor.db') as conn:
        init_sensor_db(conn)

    app.install(SQLitePlugin(dbfile='sensor.db'))

    bottle.run(
        server='gunicorn',
        host='0.0.0.0',
        port=args.port,
        workers=args.workers,
        timeout=args.timeout,
        debug=True,
        keyfile='key.pem',
        certfile='cert.pem')


if __name__ == "__main__":
    main()
