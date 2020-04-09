#!/usr/bin/env python3
# data server to manage comms between UIs and LLI
#
# Author: Dónal Murray <donal.murray@cern.ch>

import asyncio
import json
import time
import threading
import argparse
import svpi
import commsControl
from serial.tools import list_ports
from typing import List
import logging
logging.basicConfig(level=logging.DEBUG,
                    format='%(asctime)s - %(levelname)s - %(message)s')


class HEVServer(object):
    def __init__(self, lli):
        self._alarms = ''
        self._values = []
        self._thresholds = []
        self._dblock = threading.Lock()  # make db threadsafe
        self._generator = svpi.svpi()
        self._lli = lli
        self._lli.bind_to(self.polling)

        self._broadcasting = True
        self._datavalid = None           # something has been received from arduino. placeholder for asyncio.Event()
        self._dvlock = threading.Lock()  # make datavalid threadsafe
        self._dvlock.acquire()           # come up locked to wait for loop
        # start worker thread to send values in background
        worker = threading.Thread(target=self.serve_all, daemon=True)
        worker.start()

    def __repr__(self):
        with self._dblock:
            return f"Alarms: {self._alarms}.\nSensor values: {self._values}\nSettings: {self._thresholds}"

    def set_input_file(self, input_file):
        self._generator.addInputFile(input_file)
        
    def polling(self, payload) -> None:
        # get values when we get a callback from commsControl (lli)
        logging.debug(f"Payload received: {payload}")
        # TODO check what type of broadcast it is
        with self._dblock:
            self._alarms = payload
        with self._dvlock:
            self._datavalid.set()
        # only pop from queue now - protects against dataserver going down and not receiving alarms
        self._lli.pop_payloadrecv()
            
    async def handle_request(self, reader: asyncio.StreamReader, writer: asyncio.StreamWriter) -> None:
        # listen for queries on the request socket
        data = await reader.read(300)
        request = json.loads(data.decode("utf-8"))
        addr = writer.get_extra_info("peername")
        logging.info(f"Answering request from {addr}")
        payload = ""

        # three possible queries: set mode, set thresholds or both
        if request["type"] == "setmode":
            mode = request["mode"]
            logging.debug(f"{addr!r} requested to change to mode {mode!r}")

            # send via protocol and prepare reply
            if self._generator.setMode(mode):
                packet = f"""{{"type": "ackmode", "mode": \"{mode}\"}}""".encode()
            else:
                packet = f"""{{"type": "nack"}}""".encode()
        elif request["type"] == "setthresholds":
            thresholds = request["thresholds"]
            logging.debug(
                f"{addr!r} requested to set thresholds to {thresholds!r}")

            # send via protocol
            payload = self._generator.setThresholds(thresholds)
            # prepare reply
            packet = f"""{{"type": "ackthresholds", "thresholds": \"{payload}\"}}""".encode()
        elif request["type"] == "setup":
            mode = request["mode"]
            thresholds = request["thresholds"]
            logging.debug(f"{addr!r} requested to change to mode {mode!r}")
            logging.debug(
                f"{addr!r} requested to set thresholds to {thresholds!r}")

            # send via protocol and prepare reply
            if self._generator.setMode(mode):
                self._generator.setThresholds(thresholds)
                packet = f"""{{"type": "ack", "mode": \"{mode}\", "thresholds": \"{thresholds}\"}}""".encode()
            else:
                packet = f"""{{"type": "nack"}}""".encode()

        # send reply and close connection
        writer.write(packet)
        await writer.drain()
        writer.close()

    async def handle_broadcast(self, reader: asyncio.StreamReader, writer: asyncio.StreamWriter) -> None:
        # log address
        addr = writer.get_extra_info("peername")
        logging.info(f"Broadcasting to {addr!r}")

        while self._broadcasting:
            # wait for data from serial port
            try:
                await asyncio.wait_for(self._datavalid.wait(), timeout=0.5) # set timeout such that there is never pileup
            except asyncio.TimeoutError:
                continue
            # take lock of db and prepare packet
            with self._dblock:
                sensors: List[float] = self._values
                alarms: str = self._alarms
                thresholds: List[float] = self._thresholds
            broadcast_packet = f"""{{ "type": "broadcast", "sensors": {sensors}, "alarms": "{alarms}", "thresholds": {thresholds}}}"""
            logging.debug(f"Send: {broadcast_packet}")

            try:
                writer.write(broadcast_packet.encode())
                await writer.drain()
            except (ConnectionResetError, BrokenPipeError):
                # Connection lost, stop trying to broadcast and free up socket
                logging.warning(f"Connection lost with {addr!r}")
                self._broadcasting = False

            # take control of datavalid and reset it
            with self._dvlock:
                self._datavalid.clear()

        self._broadcasting = True
        writer.close()

    async def serve_request(self, ip: str, port: int) -> None:
        server = await asyncio.start_server(
            self.handle_request, ip, port)

        # get address for log
        addr = server.sockets[0].getsockname()
        logging.info(f"Listening for requests on {addr}")

        async with server:
            await server.serve_forever()

    async def serve_broadcast(self, ip: str, port: int) -> None:
        server = await asyncio.start_server(
            self.handle_broadcast, ip, port)

        # get address for log
        addr = server.sockets[0].getsockname()
        logging.info(f"Serving on {addr}")

        async with server:
            await server.serve_forever()

    async def create_sockets(self) -> None:
        self._datavalid = asyncio.Event() # initially false
        self._dvlock.release()
        LOCALHOST = "127.0.0.1"
        b1 = self.serve_broadcast(LOCALHOST, 54320)  # WebUI broadcast
        r1 = self.serve_request(LOCALHOST, 54321)    # joint request socket
        #b2 = self.serve_broadcast(LOCALHOST, 54322)  # NativeUI broadcast
        #tasks = [b1, r1, b2]
        tasks = [b1, r1]
        await asyncio.gather(*tasks, return_exceptions=True)

    def serve_all(self) -> None:
        asyncio.run(self.create_sockets())


if __name__ == "__main__":
    # parser to allow us to pass arguments to hevserver
    parser = argparse.ArgumentParser(description='Arguments to run hevserver')
    parser.add_argument('--inputFile', type=str, default = '', help='a test file to load data')
    parser.add_argument('--randGen', action='store_true', help='use a front end emulator which generates random values')
    args = parser.parse_args()

    # get arduino serial port
    for port in list_ports.comports():
        if "ARDUINO" in port.manufacturer.upper():
            port = port.device 

    # initialise low level interface and dataserver
    lli = commsControl.commsControl(port=port)
    hevsrv = HEVServer(lli)

    # check if input file was specified
    if args.inputFile != '':
        hevsrv.set_input_file(args.inputFile)

    # serve forever
    while True:
        pass