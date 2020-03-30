#!/usr/bin/env python3

# HEV monitoring application
# USAGE:  python3 arduino_recorder.py
#
# Last update: March 29, 2020

import sys
import time
import argparse
import sqlite3
from random import random
from datetime import datetime


SQLITE_FILE = 'database/HEC_monitoringDB.sqlite'  # name of the sqlite database file
TABLE_NAME = 'hec_monitor'  # name of the table to be created

def get_temperature():
    """
    Returns a random number to simulate data obtained from a sensor
    """
    return random() * 20

def get_pressure():
    """
    Returns a random number to simulate data obtained from a sensor
    """
    return random() * 10

def database_setup():
    '''
    This function creates the sqlite3 table with the timestamp column 
    and the columns for temperature and humidity
    '''
    print('Creating ' + TABLE_NAME + ' table..' )

    # Create the table if it does not exist
    try:
        # Connecting to the database file
        conn = sqlite3.connect(SQLITE_FILE)
        conn.execute('''CREATE TABLE IF NOT EXISTS ''' + TABLE_NAME + ''' (
           created_at     INTEGER        NOT NULL,
           temperature    FLOAT           NOT NULL,
           pressure       FLOAT           NOT NULL
           );'''
        )
        conn.commit()
        conn.close()
    except sqlite3.Error as err:
        raise Exception("sqlite3 Error. Create failed: {}".format(str(err)))
    finally:
        print('Table ' + TABLE_NAME + ' created successfully!')

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

            random_data = {
                'time' : timestamp,
                'temperature': get_temperature(),
                'pressure': get_pressure()
            }
            print("Writing to database...")
            try:
                cursor.execute(
                        'INSERT INTO {tn} VALUES '
                        '(:time, :temperature, :pressure)'
                        .format(tn=TABLE_NAME),
                        random_data
                )
                conn.commit()
            except sqlite3.Error as err:
                 raise Exception("sqlite3 error. Insert into database failed: {}".format(str(err)))
            finally:
                print("temperature: {temperature}  pressure: {pressure}".format(**random_data))
                sys.stdout.flush()
                time.sleep(1)

def parse_args():
    parser = argparse.ArgumentParser(description='Python script monitorign Arduino data')
    parser.add_argument('--source', default='ttys0 or similar')
    return parser.parse_args()

if __name__ == "__main__":
    ARGS = parse_args()
    database_setup()
    monitoring(ARGS.source)
    get_sensor_data()
