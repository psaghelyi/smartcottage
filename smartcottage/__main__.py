import bottle
import time
import json
import sqlite3
from bottle.ext import sqlite

app = application = bottle.default_app()

@app.get('/heartbeat')
def index():
    return dict(status='OK')


@app.get('/sensor/<id>')
def get_sensor(id, db):
    one_day = 24 * 60 * 60
    epoch_time = int(time.time())

    from_epoch = int(bottle.request.query.from_epoch or epoch_time - one_day)
    to_epoch = int(bottle.request.query.to_epoch or '-1')
    limit = int(bottle.request.query.limit or '100')

    cursor = db.execute('''SELECT *
                            FROM sensor
                            WHERE id=? AND epoch>=?
                            ORDER BY epoch, id
                            LIMIT ?;''', (id, from_epoch, limit)) \
                if to_epoch < 0 else \
              db.execute('''SELECT *
                            FROM sensor
                            WHERE id=? AND epoch>=? AND epoch<?
                            ORDER BY epoch, id
                            LIMIT ?;''', (id, from_epoch, to_epoch, limit))
    
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

    db.execute('''INSERT INTO sensor (epoch, id, data) VALUES (?,?,?);''', (epoch_time, id, data))
    db.commit()


def init_sensor_db(conn):
    conn.cursor().execute('''CREATE TABLE IF NOT EXISTS sensor (epoch INTEGER, id TEXT, data TEXT);''')
    conn.cursor().execute('''CREATE INDEX IF NOT EXISTS idx_epoch_id ON sensor (epoch, id);''')
    conn.commit()


def main():
    with sqlite3.connect('sensor.db') as conn:
        init_sensor_db(conn)

    plugin = sqlite.Plugin(dbfile='sensor.db')
    app.install(plugin)

    bottle.run(server='gunicorn', host='0.0.0.0', port=8080, workers=1, timeout=90)


if __name__ == "__main__":
    main()
