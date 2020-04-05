#!/usr/bin/env python3

# Python monitoring code
# USAGE:  python3 app.py
#
# Last update: March 29, 2020

from time import time
from flask import Flask, render_template, make_response, jsonify, Response
import sqlite3
#import json
from flask import json
import chardet

WEBAPP = Flask(__name__)

@WEBAPP.route('/')
def hello_world():
    return render_template('index.html', result=live_data())

@WEBAPP.route('/new')
def hello_worlds():
    return render_template('index_v3.html', result=live_data())

@WEBAPP.route('/live-data', methods=['GET'])
def live_data():
    """
    Query the sqlite3 table for variables
    Output in json format
    """

    data = {'created_at' : None, 'temperature' : None, 
            'pressure' : None, 'variable3' : None, 
            'variable4' : None, 'variable5' : None, 'variable6' : None}

    sqlite_file = 'database/HEC_monitoringDB.sqlite'
    with sqlite3.connect(sqlite_file) as conn:
        cursor = conn.cursor()
        cursor.execute("SELECT created_at, temperature, "
        "pressure, variable3, variable4, variable5, variable6 "
        "FROM hec_monitor ORDER BY ROWID DESC LIMIT 1")
        
        fetched = cursor.fetchone()
        data['created_at'] = fetched[0]
        data['temperature'] = round(fetched[1],3)
        data['pressure'] = round(fetched[2],3)
        data['variable3'] = round(fetched[3],3)
        data['variable4'] = round(fetched[4],3)                
        data['variable5'] = round(fetched[5],3)                
        data['variable6'] = round(fetched[6],3)                


    #json_string = json.dumps(data, ensure_ascii = False)
    #json_str = json.dumps(data, ensure_ascii = False, indent=4, sort_keys=True)
    #json_utf8 = json_str.encode('utf-8')
    #response = make_response(json_utf8)
    #response.headers['Content-Type'] = 'application/json; charset=utf-8'
    #response.headers['mimetype'] = 'application/json'


    response = make_response(json.dumps(data).encode('utf-8') )
    response.content_type = 'application/json'

    

    #return Response(json.dumps(data),  mimetype='application/json')
    return response

@WEBAPP.route('/live-alarms', methods=['GET'])
def live_alarms():
    """
    Query the sqlite3 table for alarms
    Output in json format
    """

    data = {'alarms' : None}

    sqlite_file = 'database/HEC_monitoringDB.sqlite'
    with sqlite3.connect(sqlite_file) as conn:
        cursor = conn.cursor()
        cursor.execute("SELECT alarms "
        "FROM hec_monitor ORDER BY ROWID DESC LIMIT 1")

        fetched = cursor.fetchone()
        data['alarms'] = fetched[0]

    response = make_response(json.dumps(data).encode('utf-8') )
    response.content_type = 'application/json'

    return response



if __name__ == '__main__':
    WEBAPP.run(debug=True, host='127.0.0.1', port=5000)






