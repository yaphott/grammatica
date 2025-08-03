#!/usr/bin/env python3

import argparse
import http.server
import logging
import os
from dataclasses import dataclass
from pathlib import Path

PROJECT_DIR: Path = Path(__file__).parent.parent

logger: logging.Logger = logging.getLogger(__name__)


@dataclass
class Args:
    html_dir: Path
    host: str
    port: int
    verbosity: int


def parse_args() -> Args:
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "html_dir",
        type=Path,
        default=PROJECT_DIR / "html",
        metavar="HTML_DIR",
        help="Directory containing HTML files to serve.",
    )
    parser.add_argument(
        "--host",
        type=str,
        default="localhost",
        metavar="HOST",
        help="Web server host to listen on.",
    )
    parser.add_argument(
        "--port",
        type=int,
        default=9000,
        metavar="PORT",
        help="Web server port to listen on.",
    )
    parser.add_argument(
        "-v",
        "--verbose",
        action="count",
        dest="verbosity",
        default=0,
        help="Increase logging verbosity (0-1).",
    )
    args = parser.parse_args()
    return Args(
        html_dir=args.html_dir.resolve(),
        host=args.host,
        port=args.port,
        verbosity=args.verbosity,
    )


def main() -> None:
    args = parse_args()

    log_level: int
    if args.verbosity == 0:
        log_level = logging.INFO
    elif args.verbosity == 1:
        log_level = logging.DEBUG
    else:
        raise ValueError(f"Invalid verbosity level: {args.verbosity}")
    logging.basicConfig(
        level=log_level,
        format="%(asctime)s - %(name)s - %(levelname)s - %(message)s",
    )

    if not args.html_dir.is_dir():
        raise FileNotFoundError(f"HTML directory not found: {args.html_dir}")

    os.chdir(args.html_dir)
    httpd = http.server.HTTPServer(
        (args.host, args.port),
        http.server.SimpleHTTPRequestHandler,
    )
    url = "http://" + ":".join(map(str, httpd.server_address))
    logger.info(f"Starting HTTP server on {url}")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        logger.info("Shutting down HTTP server")
        httpd.shutdown()
    finally:
        logger.info("Closing HTTP server")
        httpd.server_close()


if __name__ == "__main__":
    main()
