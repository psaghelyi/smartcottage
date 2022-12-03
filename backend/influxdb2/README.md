

```
influx backup \
    /mnt/sda1/influxdb_backup_$(date '+%Y-%m-%d_%H-%M') \
    -t <token>
```


```
influx delete \
    -t <token> \
    --bucket smartcottage \
    --org smartcottage \
    --start '2022-11-26T02:00:00Z' \
    --stop '2022-11-26T02:10:00Z' \
    --predicate '_measurement="cottage" AND sensor="s1"'
 ```

