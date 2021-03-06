#!/usr/bin/env python3
# client side for the data server to manage comms between UIs and LLI
#
# Author: Dónal Murray <donal.murray@cern.ch>

import asyncio
import time
import json
import threading
from typing import List, Dict
import logging
logging.basicConfig(level=logging.INFO,
                    format='hevclient %(asctime)s - %(levelname)s - %(message)s')

polling = True
setflag = False

# alarms: manual, gas supply, apnea, expired minute volume,
#         upper pressure limit, power failure


class HEVClient(object):
    def __init__(self):
        self._alarms = []  # db for alarms
        self._values = None  # db for sensor values
        self._thresholds = []  # db for threshold settings
        self._polling = True  # keep reading data into db
        self._lock = threading.Lock()  # lock for the database

        # start worker thread to update db in the background
        worker = threading.Thread(target=self.start_client, daemon=True)
        worker.start()

    async def polling(self) -> None:
        # open persistent connection with server
        reader, writer = await asyncio.open_connection("127.0.0.1", 54320)

        # grab data from the socket as soon as it is available and dump it in the db
        while self._polling:
            data = await reader.read(500)
            data = data.decode("utf-8")
            data = json.loads(data)
            with self._lock:
                self._values = data["sensors"]
                self._alarms = data["alarms"]

        # close connection
        writer.close()
        await writer.wait_closed()

    def start_client(self) -> None:
        asyncio.run(self.polling())

    async def send_request(self, type: str, mode: str = None, thresholds: List[float] = None) -> bool:
        # open connection and send packet
        reader, writer = await asyncio.open_connection("127.0.0.1", 54321)
        payloads = {
            "setmode": f"""{{"type": "setmode", "mode": \"{mode}\" }}""",
            "setthresholds": f"""{{"type": "setthresholds", "thresholds": \"{thresholds}\"}}""",
            "setup": f"""{{"type": "setup", "mode": \"{mode}\", "thresholds": \"{thresholds}\"}}"""
        }
        writer.write(payloads[type].encode())
        await writer.drain()

        # wait for acknowledge
        data = await reader.read(300)
        data = json.loads(data.decode("utf-8"))

        # close connection to free up socket
        writer.close()
        await writer.wait_closed()

        # check that acknowledge is meaningful
        if type == "setmode" and data["type"] == "ackmode":
            logging.info(f"Mode {mode} set successfully")
            return True
        if type == "setthresholds" and data["type"] == "ackthresholds":
            logging.info(f"Thresholds {thresholds} set successfully")
            return True
        if type == "setup" and data["type"] == "ack":
            logging.info(
                f"Mode {mode} and thresholds {thresholds} set successfully")
            return True
        else:
            logging.warning(f'Setting {type} failed')
            return False

    def set_mode(self, mode: str) -> bool:
        # set a mode and wait for acknowledge
        return asyncio.run(self.send_request("setmode", mode=mode))

    def set_thresholds(self, thresholds: List[float]) -> bool:
        # set a threshold and wait for acknowledge
        return asyncio.run(self.send_request("setthresholds", thresholds=thresholds))

    def setup(self, mode: str, thresholds: List[float]) -> bool:
        # set a mode and thresholds
        return asyncio.run(self.send_request("setup", mode=mode, thresholds=thresholds))

    def get_values(self) -> Dict:
        # get sensor values from db
        return self._values

    def get_alarms(self) -> List[str]:
        # get alarms from db
        return self._alarms


if __name__ == "__main__":
    # example implementation
    # just import hevclient and do something like the following
    hevclient = HEVClient()

    # Play with sensor values and alarms
    for i in range(30):
        values = hevclient.get_values() # returns a dict or None
        alarms = hevclient.get_alarms() # returns a list of alarms currently ongoing
        if values is None:
            i = i+1 if i > 0 else 0
        else:
            print(f"Values: {json.dumps(values, indent=4)}")
            print(f"Alarms: {alarms}")
        time.sleep(1)

    # set modes and thresholds (this will change)
    print(hevclient.set_mode("CPAP"))
    print(hevclient.set_thresholds([12.3, 45.6, 78.9]))
    print(hevclient.setup("CPAP", [12.3, 45.6, 78.9]))
    print("The next one should fail as it is not a valid mode:")
    print(hevclient.set_mode("foo"))

    # print some more values
    for i in range(10):
        print(f"Alarms: {hevclient.get_alarms()}")
        print(f"Values: {json.dumps(hevclient.get_values(), indent=4)}")
        print(f"Alarms: {hevclient.get_alarms()}")
        time.sleep(1)
