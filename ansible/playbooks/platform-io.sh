python3 -c "$(curl -fsSL https://raw.githubusercontent.com/platformio/platformio/develop/scripts/get-platformio.py)"

# add to path /home/karol/.platformio/penv/bin

pio lib install VariableTimedAction
pio lib install ArduinoQueue
pio lib install uCRC16Lib
pio run -e uno -t upload
#pio run -t nobuild -t upload
