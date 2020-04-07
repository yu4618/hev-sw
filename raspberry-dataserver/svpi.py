#!/usr/bin/env python3

# Front end generator for HEV raspberry pi data server
# author Dónal Murray <donal.murray@cern.ch>

import time
import numpy as np
from typing import List
import logging
logging.basicConfig(level=logging.INFO,
                    format='svpi %(asctime)s - %(levelname)s - %(message)s')

class svpi:
    def __init__(self):
        # use input file for testing
        self.input = None
        # store current line in the file so we know when to stop
        self.cur_line = None
        # flag to check if we can continue to generate numbers
        self.run   = True
        
    def getValues(self) -> List[float]:
        # check for file
        if (self.input):
            sensor_value: List[float] = [ float(v) for v in self.cur_line.replace('\n','').split(',') ]
            # grab next line, if it's empty we stop the run
            self.cur_line = self.input.readline()
            if (self.cur_line == ''):
                self.input.close()
                self.run = False
        else:
            # All sensor readings 32 bit floats
            sensor_value: List[float] = np.random.uniform(0.0, 1000.0, 6).tolist()
        return sensor_value

    def addInputFile(self, fileName):
        #open file and read first line
        self.input = open(fileName, 'r')
        self.cur_line = self.input.readline()
    
    def getAlarms(self) -> List[str]:
        # give a random alarm a twentieth of the time
        alarms = {
            0: "manual",
            1: "gas supply",
            2: "apnea",
            3: "expired minute volume",
            4: "upper pressure limit",
            5: "power failure",
        }
        if np.random.randint(0, 20) == 0:
            return [alarms[np.random.randint(0, 6)]]
        return ["none"]


    def getThresholds(self) -> List[float]:
        # All thresholds 32 bit floats
        thresholds: List[float] = np.random.uniform(0.0, 1000.0, 3).tolist()
        return thresholds


    def setMode(self, mode: str) -> bool:
        # setting a mode - just print it
        if mode in ("PRVC", "SIMV-PC", "CPAP"):
            logging.info(f"Setting mode {mode}")
            return True
        else:
            logging.error(f"Requested mode {mode} does not exist")
            return False


    def setThresholds(self, thresholds: List[float]) -> str:
        # setting thresholds - just print them
        logging.info(f"Setting thresholds {thresholds}")
        return thresholds


if __name__ == "__main__":
    generator = svpi()
    while generator.run:
        print(generator.getValues())
        time.sleep(0.5)
