# Python monitoring code
# USAGE:  python3 app.py
#
# Last update: March 28, 2020


#!/usr/bin/env python3

import json
from time import time
from random import random
from flask import Flask, render_template, make_response
import operator
import sqlite3

WEBAPP = Flask(__name__)


@WEBAPP.route('/')
def hello_world():
    return render_template('index.html', data='test')

@WEBAPP.route('/live-data')
def live_data():
    """
    Query the sqlite3 table
    Output in json format    
    """
     
    data = {'ts' : None, 'temperature' : None, 'pressure' : None}  
  
    sqlite_file = 'database/HEC_monitoringDB.sqlite'    
    with sqlite3.connect(sqlite_file) as conn:
        cursor = conn.cursor()
        cursor.execute("SELECT time, temperature, pressure FROM HEC_monitor ORDER BY ROWID DESC LIMIT 1")
        fetched = cursor.fetchone()
        data['ts'] = float(fetched[0])
        data['temperature'] = fetched[1]
        data['pressure'] = fetched[2]       

    response = make_response(json.dumps(data))  
    response.content_type = 'application/json'
    return response



if __name__ == '__main__':
    WEBAPP.run(debug=True, host='127.0.0.1', port=5000)






