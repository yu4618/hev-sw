#!/usr/bin/env python3
# client side for the data server to manage comms between UIs and LLI
#
# Author: Dónal Murray <donal.murray@cern.ch>

import asyncio
import time
import json
import threading
from typing import List

polling = True
setflag = False

# alarms: manual, gas supply, apnea, expired minute volume,
#         upper pressure limit, power failure


class HEVClient(object):
    def __init__(self):
        self._alarms = []
        self._values = []
        self._polling = True
        self._lock = threading.Lock()  # lock for the database

        worker = threading.Thread(target=self.start_client, daemon=True)
        worker.start()

    async def polling(self) -> None:
        reader, writer = await asyncio.open_connection("127.0.0.1", 54322)

        while self._polling:
            data = await reader.read(300)
            data = json.loads(data.decode("utf-8"))
            with self._lock:
                self._values = data["sensors"]
                self._alarms = data["alarms"]

        writer.close()
        await writer.wait_closed()

    def start_client(self) -> None:
        asyncio.run(self.polling())

    async def send_request(self, mode) -> str:
        reader, writer = await asyncio.open_connection("127.0.0.1", 54321)
        writer.write(f"""{{"type": "setmode", "mode": \"{mode}\" }}""".encode())
        await writer.drain()

        data = await reader.read(300)
        data = json.loads(data.decode("utf-8"))

        writer.close()
        await writer.wait_closed()

        if data["type"] == "ackmode":
            return(f"Mode {data['mode']} set successfully")
        else:
            return(f'Mode setting failed')

    def set_mode(self, mode: str) -> str:
        return asyncio.run(self.send_request(mode))

    def set_thresholds(self, thresholds: List[float]) -> str:
        return "Not implemented"

    def get_values(self) -> List[float]:
        return self._values

    def get_alarms(self) -> List[str]:
        return self._alarms


if __name__ == "__main__":
    # example implementation
    # just import hevclient and do something like below
    hevclient = HEVClient()
    for i in range(10):
        print(f"Sensor values: {hevclient.get_values()}")
        print(f"Alarms: {hevclient.get_alarms()}")
        time.sleep(1)
    print(hevclient.set_mode("CPAP"))
    print(hevclient.set_thresholds([12.3, 45.6, 78.9]))
