#!/usr/bin/env python3

# HEV monitoring application
# USAGE:  python3 arduino_recorder.py
#
# Last update: April 4, 2020

import sys
import time
import argparse
import sqlite3
from random import random
from datetime import datetime
import threading
from hevclient import HEVClient


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
           alarms         STRING         NOT NULL,
           temperature    FLOAT           NOT NULL,
           pressure       FLOAT           NOT NULL,
           variable3    FLOAT           NOT NULL,
           variable4    FLOAT           NOT NULL,
           variable5    FLOAT           NOT NULL,
           variable6    FLOAT           NOT NULL           
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

    # Instantiating the client
    hevclient = HEVClient()
    hevclient.set_thresholds([12.3, 45.6, 78.9])

    epoch = datetime(1970, 1, 1)

    with sqlite3.connect(SQLITE_FILE) as conn:
        cursor = conn.cursor()
        while True:
            current_time = datetime.now()

            # Computing the time in seconds since the epoch because easier to manipulate. 
            timestamp = (current_time -epoch).total_seconds() * 1000
            
            if hevclient.get_values() != []:
                data_receiver = hevclient.get_values()
                data_alarms = hevclient.get_alarms()

                # data alarms can have length of 6, joining all the strings
                data_alarms = ','.join(data_alarms) 

                random_data = {
                    'time' : timestamp,
                    'alarms' : data_alarms,
                    'temperature': data_receiver[0],
                    'pressure': data_receiver[1],
                    'variable3': data_receiver[2],
                    'variable4': data_receiver[3],
                    'variable5': data_receiver[4],
                    'variable6': data_receiver[5]    
                }

                print("Writing to database ...")
                try:
                    cursor.execute(
                            'INSERT INTO {tn} VALUES '
                            '(:time, :alarms, :temperature, :pressure, :variable3, :variable4, :variable5, :variable6)'.format(tn=TABLE_NAME), random_data
                    )
                    conn.commit()
                except sqlite3.Error as err:
                     raise Exception("sqlite3 error. Insert into database failed: {}".format(str(err)))
                finally:
                    #print("temperature: {temperature}  pressure: {pressure}".format(**random_data))
                    #print("Received values: {}".format(random_data))
                   
                    sys.stdout.flush()
                    time.sleep(1)

def progress(status, remaining, total):
    print(f'Copied {total-remaining} of {total} pages...')


def db_backup():
    threading.Timer(600, db_backup).start()
    print("Executing DB backup")
    try:
        # Existing DB
        sqliteCon = sqlite3.connect(SQLITE_FILE)
        # Backup DB
        backupCon = sqlite3.connect("database/HEC_monitoringDB_backup.sqlite")    
        with backupCon:
            sqliteCon.backup(backupCon, pages=5, progress=progress)
        print("Backup successful")
    except sqlite3.Error as err:
        raise Exception("sqlite3 error. Error during backup: {}".format(str(err)))
    finally: 
        if(backupCon):
            backupCon.close()
            sqliteCon.close()
    

def parse_args():
    parser = argparse.ArgumentParser(description='Python script monitorign Arduino data')
    parser.add_argument('--source', default='ttys0 or similar')
    parser.add_argument('--backup_time', type=int, default=600)
    return parser.parse_args()

if __name__ == "__main__":
    ARGS = parse_args()
    database_setup()
    db_backup()
    monitoring(ARGS.source)
