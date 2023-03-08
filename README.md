# SmartCottage

## download modules

`$ python -m venv .venv`

`$ .venv/bin/activate`

`$ python -m pip install -r requirements.txt`

## install service

`$ sudo cp smartcottage.service /etc/systemd/system`

reload systemd: `$ sudo systemctl daemon-reload`

enable service: `$ sudo systemctl enable smartcottage.service`

start service:  `$ sudo systemctl start smartcottage.service`

# iMac camera streaming

```
ffmpeg \
  -f avfoundation \
  -pix_fmt yuyv422 \
  -video_size 1280x720 \
  -framerate 15 \
  -i "0:0" -ac 2 \
  -vf format=yuyv422 \
  -vcodec libx264 -maxrate 2000k \
  -bufsize 2000k -acodec aac -ar 44100 -b:a 128k \
  -f rtp_mpegts udp://127.0.0.1:9988
```


## protforwarding to homeassistant



```
sudo iptables -t nat -A PREROUTING -i eth0 -p tcp --dport 8123 -j DNAT --to-destination 192.168.192.1:8123
sudo iptables -t nat -A POSTROUTING -o ztugawpd7r -p tcp -d 192.168.192.1 --dport 8123 -j SNAT --to-source 192.168.1.40
```
