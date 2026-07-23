#!/usr/bin/env python3
import argparse
import socket
import tempfile
from pathlib import Path


DEFAULT_HOST = "0.0.0.0"
DEFAULT_PORT = 39091
DEFAULT_OUTPUT = Path("ios_head_logs/longying_head_render.log")
SELF_TEST_LINE = "marker=LONGYING_IOS_HEAD_RECEIVER_SELF_TEST"


def append_line(output_path: Path, line: str) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    with output_path.open("a", encoding="utf-8", newline="\n") as output:
        output.write(line + "\n")


def receive(host: str, port: int, output_path: Path, once: bool) -> None:
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as server:
        server.bind((host, port))
        print(f"Listening for iOS head logs on {host}:{port}")
        print(f"Writing payloads to {output_path.resolve()}")
        while True:
            payload, address = server.recvfrom(65535)
            line = payload.decode("utf-8", errors="replace").rstrip("\r\n")
            append_line(output_path, line)
            print(f"[{address[0]}:{address[1]}] {line}", flush=True)
            if once:
                return


def self_test(port: int) -> None:
    with tempfile.TemporaryDirectory() as directory:
        output_path = Path(directory) / "receiver.log"
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as server:
            server.bind((DEFAULT_HOST, port))
            with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as client:
                client.sendto(SELF_TEST_LINE.encode("utf-8"), ("127.0.0.1", port))
            payload, _ = server.recvfrom(65535)
            append_line(output_path, payload.decode("utf-8"))

        if output_path.read_text(encoding="utf-8") != SELF_TEST_LINE + "\n":
            raise RuntimeError("UDP receiver self-test output did not match the sent line")
        print(f"UDP receiver self-test passed on 0.0.0.0:{port}")


def main() -> None:
    parser = argparse.ArgumentParser(description="Receive Longying iOS head-render diagnostics")
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--port", type=int, default=DEFAULT_PORT)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--once", action="store_true")
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args()

    if args.self_test:
        self_test(args.port)
        return
    receive(args.host, args.port, args.output, args.once)


if __name__ == "__main__":
    main()
