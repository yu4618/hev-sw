#!/usr/bin/env python3
# client side for the data server to manage comms between UIs and LLI
#
# Author: Dónal Murray <donal.murray@cern.ch>

import asyncio
import time
import json
import threading

polling = True
setflag = False

# alarms: manual, gas supply, apnea, expired minute volume,
#         upper pressure limit, power failure


class HEVClient(object):
    def __init__(self):
        self._alarms = []
        self._values = []
        self._polling = True

        worker = threading.Thread(target=self.start_client)
        worker.start()

    async def polling(self) -> None:
        reader, writer = await asyncio.open_connection("127.0.0.1", 54322)

        while self._polling:
            data = await reader.read(300)
            data = json.loads(data.decode("utf-8"))
            print(json.dumps(data, indent=4))

        writer.close()
        await writer.wait_closed()

    def start_client(self) -> None:
        asyncio.run(self.polling())

    async def send_request(self, mode) -> None:
        reader, writer = await asyncio.open_connection("127.0.0.1", 54321)
        writer.write(f"""{{"type": "cmd", "setmode": \"{mode}\" }}""".encode())
        await writer.drain()

        data = await reader.read(300)
        data = json.loads(data.decode("utf-8"))
        print(json.dumps(data, indent=4))

        writer.close()
        await writer.wait_closed()

    def set_mode(self, mode):
        asyncio.run(self.send_request(mode))


if __name__ == "__main__":
    hevclient = HEVClient()
    time.sleep(10)
    hevclient.set_mode("CPAP")
