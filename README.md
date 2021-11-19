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
