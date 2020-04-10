from commsControl import commsControl
import logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
import sys

comms = commsControl(port = sys.argv[1])


class Dependant(object):
    def __init__(self, lli):
        self._llipacket = None
        self._lli = lli
        self._lli.bind_to(self.update_llipacket)

    def update_llipacket(self, payload):
        logging.info(f"payload received: {payload.fsm_state!r}")
        self._llipacket = payload
        # pop from queue - protects against Dependant going down and not receiving packets
        self._lli.pop_payloadrecv()

dep = Dependant(comms)

start = 0x1 
stop = 0x2

comms.registerData(1)
# comms.sender()
while True:
    pass

