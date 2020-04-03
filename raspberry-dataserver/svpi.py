#!/usr/bin/env python3

# Front end generator for HEV raspberry pi data server
# author Dónal Murray <donal.murray@cern.ch>

import time
import numpy as np
from typing import List


def getValues() -> List[float]:
    # All sensor readings 32 bit floats
    sensor_value: List[float] = np.random.uniform(0.0, 1000.0, 6).tolist()
    return sensor_value


def getAlarms() -> List[str]:
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


def setMode(mode: str) -> str:
    # setting a mode - just print it
    print(f"Setting mode {mode}")
    return f"{mode}"


def setThresholds(thresholds: List[float]) -> str:
    # setting thresholds - just print them
    print(f"Setting thresholds {thresholds}")
    return f"{mode}"


if __name__ == "__main__":
    while True:
        print(getValues())
        time.sleep(0.5)
