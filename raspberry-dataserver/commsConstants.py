from struct import Struct 
from enum import Enum


# VERSIONING
# change version in baseFormat for all data
# i.e. if and of dataFormat, commandFormat or AlarmFormat change
# then change the RPI_VERSION

class BaseFormat():
    def __init__(self):
        self.RPI_VERSION = 0xA0
        self.rpi_size    = 0 
        self.version = 0
        self.size = 0
    # check for mismatch between pi and microcontroller version
    def checkVersion(self):
        if self.RPI_VERSION == self.version :
            self.version_error = True
        else : 
            self.version_error = False

    def checkSize(self):
        if self.rpi_size == self.size:
            self.version_error = True
        else : 
            self.version_error = False

class dataFormat(BaseFormat):

    # define the format here, including version and size
    def __init__(self):
        super().__init__()
        # struct will set the num bytes per variable
        # B = unsigned char = 1 byte
        # H = unsigned short = 2 bytes
        # I = unsigned int = 4 bytes
        # < = little endian
        # > = big endian
        # ! = network format (big endian)
        self.dataStruct = Struct("!BBBHHHHHHHHHBBBBBB")
        self.byteArray = None
        self.rpi_size    = 27

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

    def __repr__(self):
        return f"""{{
    "version"                : {self.version},
    "size"                   : {self.size},
    "fsm_state"              : {self.fsm_state},
    "pressure_air_supply"    : {self.pressure_air_supply},
    "pressure_air_regulated" : {self.pressure_air_regulated},
    "pressure_o2_supply"     : {self.pressure_o2_supply},
    "pressure_o2_regulated"  : {self.pressure_o2_regulated},
    "pressure_buffer"        : {self.pressure_buffer},
    "pressure_inhale"        : {self.pressure_inhale},
    "pressure_patient"       : {self.pressure_patient},
    "temperature_buffer"     : {self.temperature_buffer},
    "pressure_diff_patient"  : {self.pressure_diff_patient},
    "readback_valve_air_in"  : {self.readback_valve_air_in},
    "readback_valve_o2_in"   : {self.readback_valve_o2_in},
    "readback_valve_inhale"  : {self.readback_valve_inhale},
    "readback_valve_exhale"  : {self.readback_valve_exhale},
    "readback_valve_purge"   : {self.readback_valve_purge},
    "readback_mode"          : {self.readback_mode}
}}"""
        
    # for receiving dataFormat from microcontroller
    # fill the struct from a byteArray, 
    def fromByteArray(self, byteArray):
        self.byteArray = byteArray
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


    # for sending dataFormat to microcontroller
    # this is for completeness.  Probably we never send this data
    # to the microcontroller
    def toByteArray(self):
        # since pi is sender
        self.version = self.RPI_VERSION
        self.size = self.rpi_size

        self.byteArray = self.dataStruct.pack(
            self.RPI_VERSION,
            self.rpi_size, 
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
            self.readback_mode
        ) 

    def getDict(self):
        data = {
            "version"                : self.version,
            "size"                   : self.size,
            "fsm_state"              : self.fsm_state,
            "pressure_air_supply"    : self.pressure_air_supply,
            "pressure_air_regulated" : self.pressure_air_regulated,
            "pressure_o2_supply"     : self.pressure_o2_supply,
            "pressure_o2_regulated"  : self.pressure_o2_regulated,
            "pressure_buffer"        : self.pressure_buffer,
            "pressure_inhale"        : self.pressure_inhale,
            "pressure_patient"       : self.pressure_patient,
            "temperature_buffer"     : self.temperature_buffer,
            "pressure_diff_patient"  : self.pressure_diff_patient,
            "readback_valve_air_in"  : self.readback_valve_air_in,
            "readback_valve_o2_in"   : self.readback_valve_o2_in,
            "readback_valve_inhale"  : self.readback_valve_inhale,
            "readback_valve_exhale"  : self.readback_valve_exhale,
            "readback_valve_purge"   : self.readback_valve_purge,
            "readback_mode"          : self.readback_mode
        }
        return data

class commandFormat(BaseFormat):
    def __init__(self):
        super().__init__()
        self.dataStruct = Struct("!BBBI")
        self.byteArray = None
        self.rpi_size    = 7

        self.version = 0
        self.size  = 0
        self.cmdCode   = 0
        self.param = 0
        
    def fromByteArray(self, byteArray):
        self.byteArray = byteArray
        (self.version,
        self.size,
        self.cmdCode,
        self.param) = self.dataStruct.unpack(self.byteArray) 

    def toByteArray(self):
        # since pi is sender
        self.byteArray = self.dataStruct.pack(
            self.RPI_VERSION,
            self.rpi_size, 
            self.cmdCode,
            self.param
        ) 
    
class alarmFormat(BaseFormat):
    def __init__(self):
        super().__init__()
        self.dataStruct = Struct("!BBBI")
        self.byteArray = None
        self.rpi_size    = 7

        self.version = 0
        self.size  = 0
        self.alarmCode   = 0
        self.param = 0
        
    def fromByteArray(self, byteArray):
        self.byteArray = byteArray
        (self.version,
        self.size,
        self.alarmCode,
        self.param) = self.dataStruct.unpack(self.byteArray) 

    def toByteArray(self):
        self.byteArray = self.dataStruct.pack(
            self.RPI_VERSION,
            self.rpi_size, 
            self.alarmCode,
            self.param
        ) 

class command_codes(Enum):
    CMD_START = 1
    CMD_STOP  = 2
