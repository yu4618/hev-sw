#!/usr/bin/env python3

# Python monitoring code
# USAGE:  python3 app.py
#
# Last update: March 29, 2020

from time import time
import json
from flask import Flask, render_template, make_response
import sqlite3
import json

WEBAPP = Flask(__name__)

@WEBAPP.route('/')
def hello_world():
    return render_template('index.html', result=live_data())

@WEBAPP.route('/live-data')
def live_data():
    """
    Query the sqlite3 table
    Output in json format
    """

    data = {'created_at' : None, 'temperature' : None, 'pressure' : None}

    sqlite_file = 'database/HEC_monitoringDB.sqlite'
    with sqlite3.connect(sqlite_file) as conn:
        cursor = conn.cursor()
        cursor.execute("SELECT created_at, temperature, pressure FROM hec_monitor ORDER BY ROWID DESC LIMIT 1")
        fetched = cursor.fetchone()
        data['created_at'] = fetched[0]
        data['temperature'] = fetched[1]
        data['pressure'] = fetched[2]

    response = make_response(json.dumps(data).encode('utf-8'))
    response.content_type = 'application/json'
    return response

if __name__ == '__main__':
    WEBAPP.run(debug=True, host='127.0.0.1', port=5000)






