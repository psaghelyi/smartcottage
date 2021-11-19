import influxdb
import sqlite3
import json
import datetime

def create_point(measurement, sensor, time, temperature, humidity):
    item = {
        "measurement": measurement,
        "tags": {
            "sensor": sensor
        },
        "time": time,
        "fields": {
            "temperature": temperature,
            "humidity": humidity
        }
    }
    return item


def main():
    client = influxdb.InfluxDBClient(host='localhost', port=8086, database='smartcottage')
    
    with sqlite3.connect('sensor.db') as conn:
        cur = conn.cursor()
        cur.execute('''SELECT * FROM sensor''')
 
        points = [create_point(
            measurement="cottage", 
            sensor=row[1], 
            time=int(row[0]) * 1_000_000_000, 
            temperature=float(json.loads(row[2])["temperature"]),
            humidity=float(json.loads(row[2])["humidity"])) for row in cur.fetchall()]
  
    res = client.write_points(points)
    pass


if __name__ == "__main__":
    main()