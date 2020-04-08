#!/usr/bin/env python3

# Python monitoring code
# USAGE:  python3 app.py
#
# Last update: April 7, 2020

from time import time
from flask import Flask, render_template, make_response, jsonify, Response, request
import sqlite3
#import json
from flask import json
import chardet
from hevclient import HEVClient


WEBAPP = Flask(__name__)

# Instantiating the client
hevclient = HEVClient()


@WEBAPP.route('/')
def hello_world():
    return render_template('index.html', result=live_data())

@WEBAPP.route('/new')
def hello_worlds():
    return render_template('index_v3.html', result=live_data())

@WEBAPP.route('/settings')
def settings():
    return render_template('settings.html', result=live_data())


def multiple_appends(listname, *element):
    listname.extend(element)


@WEBAPP.route('/data_handler', methods=['POST'])
def data_handler():
    """
    Send configuration data to the Arduino
    """
    output = []
    var_1 = request.form['variable1']
    var_2 = request.form['variable2']
    var_3 = request.form['variable3']
    var_4 = request.form['variable4']
    var_5 = request.form['variable5']
    var_6 = request.form['variable6']
  
    patient_name = request.form['patient_name']

 
    multiple_appends(output, var_1, var_2, var_3, var_4, var_5, var_6)
    
    converted_output = [float(i) for i in output] 

    hevclient.set_thresholds(converted_output)

    return render_template('index.html', result=live_data(), patient=patient_name)




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
        data['temperature'] = round(fetched[1],2)
        data['pressure'] = round(fetched[2],2)
        data['variable3'] = round(fetched[3],2)
        data['variable4'] = round(fetched[4],2)                
        data['variable5'] = round(fetched[5],2)                
        data['variable6'] = round(fetched[6],2)                

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






