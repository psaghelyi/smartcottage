import sys
import argparse
import bottle
from copy import copy
import server


def parseargs():
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", type=str, default="0.0.0.0")
    parser.add_argument("--port", type=int, default=12345)
    parser.add_argument("--workers", type=int, default=4)
    parser.add_argument("--timeout", type=int, default=30)
    return parser.parse_args()

def main():
    args = copy(parseargs())

    # https://stackoverflow.com/questions/55934681/prevent-bottles-backend-server-plugin-gunicorn-from-intercepting-command-line
    # must reset the args, otherwise gunicorn will go nuts
    sys.argv = sys.argv[:1]
    
    app = bottle.default_app()
    server.create_routes(app)

    bottle.run(
        server='gunicorn',
        host=args.host,
        port=args.port,
        workers=args.workers,
        timeout=args.timeout,
        debug=True,
        reloader=False,
        keyfile='key.pem',
        certfile='cert.pem')


if __name__ == "__main__":
    main()
