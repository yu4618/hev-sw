# HEV monitoring application
# USAGE:  python3 arduino_recorder.py
#
# Last update: March 28, 2020


#!/usr/bin/env python3


import os
import sys
import time
import argparse
import sqlite3
from datetime import datetime
from random import random


SQLITE_FILE = 'database/HEC_monitoringDB.sqlite'  # name of the sqlite database file
TABLE_NAME = 'HEC_monitor'  # name of the table to be created
FIELD_TYPE = 'INTEGER' 


COLUMNS = []
COLUMNS.append('time')
COLUMNS.append('temperature')
COLUMNS.append('pressure')



def get_temperature():
    """
    Returns a random number to simulate data obtained from a sensor
    """
    return random()*20


def get_pressure():
    """
    Returns a random number to simulate data obtained from a sensor
    """
    return random()*10



def database_setup():
   '''
   This function creates the sqlite3 table with the timestamp column 
   and the columns for temperature and humidity
   '''
   sql_command = 'CREATE TABLE IF NOT EXISTS {tn} ({tc} {ft}'.format(
           tn=TABLE_NAME, ft=FIELD_TYPE, tc=COLUMNS[0]
   )
   for col in COLUMNS[1:]:
       sql_command += ', {} {}'.format(col, FIELD_TYPE)
   sql_command += ')'



   # Create the table if it does not exist
   try:
       # Connecting to the database file
       conn = sqlite3.connect(SQLITE_FILE)
       c = conn.cursor()
       c.execute(sql_command)
       conn.commit()
       conn.close()
   except sqlite3.Error as err:
       raise Exception("sqlite3 Error. Create failed: {}".format(str(err)))



def monitoring(source_address):
   '''
   Store arduino data in the sqlite3 table. 
   '''
  
   epoch = datetime(1970, 1, 1)
   
   with sqlite3.connect(SQLITE_FILE) as conn:
       cursor = conn.cursor()
       while True:
           current_time = datetime.now()
           # Computing the time in seconds since the epoch because easier to manipulate. 
           timestamp = (current_time -epoch).total_seconds() * 1000
       
           try:
               cursor.execute(
                       'INSERT INTO {tn} VALUES '
                       '(:time, :temperature, :pressure)'
                       .format(tn=TABLE_NAME),
                       {
                           'time':timestamp,
                           'temperature':get_temperature(),
                           'pressure':get_pressure()                           
                       })
               conn.commit()
           except sqlite3.Error as err:
                raise Exception("sqlite3 error. Insert into database failed: {}".format(str(err)))
       
 
           print("Writing to database...")
           sys.stdout.flush()
           time.sleep(1)
   

def parse_args():
    parser = argparse.ArgumentParser(description='Python script monitorign Arduino data')
    parser.add_argument('--source', default='192.168.0.10') 
    return parser.parse_args()    



if __name__ == "__main__":
   ARGS = parse_args()
   database_setup()
   monitoring(ARGS.source)
   get_sensor_data()












