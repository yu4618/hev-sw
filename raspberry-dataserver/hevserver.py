#!/usr/bin/env python3
# data server to manage comms between UIs and LLI
#
# Author: Dónal Murray <donal.murray@cern.ch>

import asyncio
import json
import svpi
import time
import threading
from typing import List


class HEVServer(object):
    def __init__(self):
        self._alarms = []
        self._values = []
        self._polling = True
        self._broadcasting_period = 1
        self._broadcasting = True

        worker = threading.Thread(target=self.polling, daemon=True)
        worker.start()

    def __repr__(self):
        return f"alarms: {self._alarms}. sensor values: {self._values}"

    def polling(self) -> None:
        while self._polling:
            self._values = svpi.getValues()
            self._alarms = svpi.getAlarms()
            time.sleep(self._broadcasting_period*2)

    async def handle_request(self, reader: asyncio.StreamReader, writer: asyncio.StreamWriter) -> None:
        data = await reader.read(300)
        request = json.loads(data.decode("utf-8"))
        addr = writer.get_extra_info("peername")
        payload = ""

        if request["type"] == "setmode":
            mode = request["mode"]
            print(f"{addr!r} requested to change to mode {mode!r}")
            payload = svpi.setMode(mode)
            packet = f"""{{"type": "ackmode", "mode": \"{payload}\"}}""".encode()
        elif request["type"] == "setthresholds":
            thresholds = request["thresholds"]
            print(f"{addr!r} requested to set thresholds to {thresholds!r}")
            payload = svpi.setThresholds(thresholds)
            packet = f"""{{"type": "ackthresholds", "thresholds": \"{payload}\"}}""".encode()
        elif request["type"] == "setup":
            mode = request["mode"]
            thresholds = request["thresholds"]
            print(f"{addr!r} requested to change to mode {mode!r}")
            print(f"{addr!r} requested to set thresholds to {thresholds!r}")
            payload1 = svpi.setMode(mode)
            payload2 = svpi.setThresholds(thresholds)
            packet = f"""{{"type": "ack", "mode": \"{payload1}\", "thresholds": \"{payload2}\"}}""".encode()

        writer.write(packet)
        await writer.drain()
        writer.close()

    async def handle_broadcast(self, reader: asyncio.StreamReader, writer: asyncio.StreamWriter) -> None:
        # log connection
        addr = writer.get_extra_info("peername")
        print(f"Broadcasting to {addr!r}")

        while self._broadcasting:
            payload: List[float] = self._values
            alarms: List[str] = self._alarms
            broadcast_packet = f"""{{ "type":"broadcast", "sensors":{payload}, "alarms":{str(alarms).replace("'",'"')} }}"""

            #print(f"Send: {broadcast_packet}")
            try:
                writer.write(broadcast_packet.encode())
                await writer.drain()
                await asyncio.sleep(self._broadcasting_period)
            except (ConnectionResetError, BrokenPipeError):
                # Connection lost, stop trying to broadcast and free up socket
                print(f"Connection lost with {addr!r}")
                self._broadcasting = False

        self._broadcasting = True
        writer.close()

    async def serve_request(self, ip: str, port: int) -> None:
        server = await asyncio.start_server(
            self.handle_request, ip, port)

        addr = server.sockets[0].getsockname()
        print(f"Serving on {addr}")

        async with server:
            await server.serve_forever()

    async def serve_broadcast(self, ip: str, port: int) -> None:
        server = await asyncio.start_server(
            self.handle_broadcast, ip, port)

        addr = server.sockets[0].getsockname()
        print(f"Serving on {addr}")

        async with server:
            await server.serve_forever()

    async def create_sockets(self) -> None:
        LOCALHOST = "127.0.0.1"
        b1 = self.serve_broadcast(LOCALHOST, 54320)  # WebUI broadcast socket
        r1 = self.serve_request(LOCALHOST, 54321)   # joint request socket
        # NativeUI broadcast socket
        b2 = self.serve_broadcast(LOCALHOST, 54322)
        tasks = [b1, r1, b2]
        await asyncio.gather(*tasks, return_exceptions=True)

    def serve_all(self) -> None:
        asyncio.run(self.create_sockets())


if __name__ == "__main__":
    hevsrv = HEVServer()
    hevsrv.serve_all()
