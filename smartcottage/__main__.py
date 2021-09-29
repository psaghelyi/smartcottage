import argparse
import bottle
import time
import json
import logging
import sqlite3
from bottle.ext import sqlite

logger = logging.getLogger('smartcottage')

app = application = bottle.default_app()

@app.get('/heartbeat')
def index():
    return dict(status='OK')


@app.get('/sensor/<id>')
def get_sensor(id, db):
    one_day = 24 * 60 * 60
    epoch_time = int(time.time())

    from_epoch = int(bottle.request.query.from_epoch or epoch_time - one_day)
    to_epoch = int(bottle.request.query.to_epoch or epoch_time)  # until now
    limit = int(bottle.request.query.limit or '-100')  # last hundred

    cursor = db.execute('''
                        SELECT * FROM sensor
                        WHERE id=? AND epoch>=? AND epoch<?
                        ORDER BY epoch, id
                        LIMIT ?;
                        ''', (id, from_epoch, to_epoch, limit)) \
                if limit > 0 else \
             db.execute('''
                        SELECT * FROM (
                            SELECT * FROM sensor 
                            WHERE id=? AND epoch>=? AND epoch<? 
                            ORDER BY epoch DESC 
                            LIMIT ?)                           
                        ORDER BY epoch, id;
                        ''', (id, from_epoch, to_epoch, -limit))
    
    rows = [{
                "epoch": row["epoch"],
                "id": row["id"],
                "data": json.loads(row["data"])
            } for row in cursor.fetchall()]
    return json.dumps(rows)


@app.put('/sensor/<id>')
def put_sensor(id, db):
    epoch_time = int(time.time())
    data = json.dumps(bottle.request.json)
    
    logger.info(data)
    
    db.execute('''INSERT INTO sensor (epoch, id, data) VALUES (?,?,?);''', (epoch_time, id, data))
    db.commit()


def init_sensor_db(conn):
    conn.cursor().execute('''CREATE TABLE IF NOT EXISTS sensor (epoch INTEGER, id TEXT, data TEXT);''')
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

    plugin = sqlite.Plugin(dbfile='sensor.db')
    app.install(plugin)

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
