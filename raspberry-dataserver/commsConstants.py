from struct import Struct

class dataFormat():
    def __init__(self, byteArray):
        # struct will set the num bytes per variable
        # B = unsigned char = 1 byte
        # H = unsigned short = 2 bytes
        # I = unsigned int = 4 bytes
        # < = little endian
        # > = big endian
        # ! = network format (big endian)
        self.dataStruct = Struct("!BBBHHHHHHHHHBBBBBB")
        self.byteArray = byteArray
        self.rpi_version = 0xA0

        # make all zero to start with
        self.version = 0
        self.size = 0
        self.fsm_state = 0
        self.pressure_air_supply = 0
        self.pressure_air_regulated = 0
        self.pressure_o2_supply = 0
        self.pressure_o2_regulated = 0
        self.pressure_buffer = 0
        self.pressure_inhale = 0
        self.pressure_patient = 0
        self.temperature_buffer = 0
        self.pressure_diff_patient = 0
        self.readback_valve_air_in = 0
        self.readback_valve_o2_in = 0
        self.readback_valve_inhale = 0
        self.readback_valve_exhale = 0
        self.readback_valve_purge = 0
        self.readback_mode = 0

    def unpack(self):
        (self.version,
        self.size,
        self.fsm_state,
        self.pressure_air_supply,
        self.pressure_air_regulated,
        self.pressure_o2_supply,
        self.pressure_o2_regulated,
        self.pressure_buffer,
        self.pressure_inhale,
        self.pressure_patient,
        self.temperature_buffer,
        self.pressure_diff_patient,
        self.readback_valve_air_in,
        self.readback_valve_o2_in,
        self.readback_valve_inhale,
        self.readback_valve_exhale,
        self.readback_valve_purge,
        self.readback_mode) = self.dataStruct.unpack(self.byteArray) 

    def checkversion(self):
        if self.rpi_version == self.version :
            return True
        else:
            return False
