#!/usr/bin/env python3

# HEV database dump for debugging
# USAGE:  python3 database_dump.py --backup_db=True --start_date=20200408-1834
# 
# Dumps the data from the backup or main db for debugging purposes.
#

import sys
import time
import argparse
import sqlite3
from datetime import datetime, timedelta
import threading

SQLITE_FILE = 'database/HEC_monitoringDB.sqlite'  # name of the sqlite database file
SQLITE_BACKUPFILE = 'database/HEC_monitoringDB_backup.sqlite'  # name of the sqlite backup database file
TABLE_NAME = 'hec_monitor'  # name of the table to be created

def load_data(sqBackup, startTime):
    """
    Load the database table and select data between a time+date and the end of the file
    """
    if( sqBackup ):
        sqFile = SQLITE_BACKUPFILE
    else:
        sqFile = SQLITE_FILE
    try:
        conn = sqlite3.connect(sqFile)
    except sqlite3.Error as err:
        raise Exception("sqlite3 Error. Load {} failed: {}".format(sqFile,str(err)))
    finally:
        print("Loaded {} ".format(sqFile))
    #
    # load data 
    #
    cursor = conn.cursor()
    try:
        cursor.execute('PRAGMA table_info({tablename})'.format(tablename=TABLE_NAME))
        tableInfo = cursor.fetchall()
        # Computing the time in seconds since the epoch because easier to manipulate. 
        epoch = datetime(1970, 1, 1, 0, 0)
        startFromEpoch = (startTime - epoch).total_seconds() * 1000
        cursor.execute('SELECT * FROM {tablename} WHERE created_at > {startFromEpoch};'.\
                       format(tablename=TABLE_NAME, startFromEpoch=startFromEpoch))
        rows = cursor.fetchall()
    except sqlite3.Error as err:
        raise Exception("sqlite3 Error. Reading {} failed: {}".format(sqFile,str(err)))
    return {'tableInfo':tableInfo, 'rows':rows}

def printRows(dataTable):
    """
    Format a row to the screen
    """
    #
    # format print string
    #
    tableInfo = dataTable['tableInfo']
    rows = dataTable['rows']
    fmt=" ".join(["{yyyy:4d}-{mm:02d}-{dd:02d} {HH:02d}:{MM:02d}", # date of datetaken
                  "{temperature:6.2f}", #temp
                  "{pressure:6.2f}"]) #pressure
    fmtOther = []
    # others added later
    for col in tableInfo[4:]:
        if( col[2] == "FLOAT" ):
            fmtOther.append(" {:6.2f}")
        if( col[2] == "INTEGER" ):
            fmtOther.append(" {:6i}")
    fmt += " {other} : {alarm}"
    epoch = datetime(1970, 1, 1, 0, 0)
    #
    # header
    #
    header = "#  Date    Time  Temp.  Pres. "
    for col in tableInfo[4:]:
        header += " {:6s}".format(col[1][:6])
    header += "  : Alarm state"
    #
    # Loop over rows passed and print
    #
    nPrint = 0 
    for r in rows:
        if( nPrint%50 == 0 ): print(header) # header every 50 rows
        nPrint += 1
        otherTxt = ""
        for i in range(len(r[4:])):
            otherTxt += fmtOther[i].format(r[4+i])
        rowDateTime = (epoch + timedelta(seconds=r[0]/1000))
        print(fmt.format(yyyy=rowDateTime.year,
                         mm=rowDateTime.month,
                         dd=rowDateTime.day,
                         HH=rowDateTime.hour,
                         MM=rowDateTime.minute,
                         temperature=r[2],
                         pressure=r[3],
                         other=otherTxt,
                         alarm=r[1]))

def parse_args():
    parser = argparse.ArgumentParser(description='Python script for checking monitoring database')
    parser.add_argument('--backup_db', type=bool, default=True, 
                        help="Check the backup dd if True (default), or current db if False")
    parser.add_argument("-s", "--start_date", dest="start_date", 
                        default=datetime.today() - timedelta(days = 1, seconds=0), 
                        type=lambda d: datetime.strptime(d, '%Y%m%d-%H%M'),
                        help="Date in the format yyyymmdd-HHMM (for example 20200409-14:30)")
    return parser.parse_args()

if __name__ == "__main__":
    ARGS = parse_args()
    dataTable = load_data(ARGS.backup_db, ARGS.start_date)
    print("dataTable Info =")
    print("\n".join("  {} \t: {}".format(t[1],t[2]) for t in dataTable['tableInfo']))
    print("Num rows = ",len(dataTable['rows']))
    printRows(dataTable)
