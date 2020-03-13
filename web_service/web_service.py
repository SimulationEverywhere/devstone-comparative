#!/usr/bin/env python3
import uuid
from flask import Flask, request, jsonify, Response
import sqlite3
import os
import time

app = Flask(__name__)

DB_FILENAME = "db.sqlite"
SQL_INSERT_REQUEST = "INSERT INTO requests ('id', 'username', 'depth', 'width', 'int_cycles', 'ext_cycles', 'status') VALUES ('{request_id}', '{username}', '{depth}', '{width}', '{int_cycles}', '{ext_cycles}', '{status}');"
SQL_SELECT_USERNAME = "SELECT username FROM users WHERE api_key='{api_key}'"
SQL_SELECT_REQUEST_STATUS = "SELECT status FROM requests WHERE id='{request_id}'"
SQL_UPDATE_STATUS = "UPDATE requests SET status='{status}' WHERE id='{request_id}'"


def get_username_from_api_key(api_key):
    conn = sqlite3.connect(DB_FILENAME)
    c = conn.cursor()
    sql = SQL_SELECT_USERNAME.format(api_key=api_key)
    res = c.execute(sql).fetchall()
    conn.close()
    return res[0][0] if res else None


def get_request_status(request_id):
    conn = sqlite3.connect(DB_FILENAME)
    c = conn.cursor()
    sql = SQL_SELECT_REQUEST_STATUS.format(request_id=request_id)
    res = c.execute(sql).fetchall()
    conn.close()
    return res[0][0] if res else None


def submit_devstone(request_id, username, depth, width, int_cycles, ext_cycles):
    conn = sqlite3.connect(DB_FILENAME)
    c = conn.cursor()
    sql = SQL_INSERT_REQUEST.format(request_id=request_id, username=username, depth=depth, width=width, int_cycles=int_cycles, ext_cycles=ext_cycles, status="pending")
    c.execute(sql)
    conn.commit()
    conn.close()
    # TODO: here it would be the devstone comparative script call
    # time.sleep(5)
    open("results/%s.csv" % request_id, "w").write(username)

    conn = sqlite3.connect(DB_FILENAME)
    sql = SQL_UPDATE_STATUS.format(request_id=request_id, status="finished")
    c = conn.cursor()
    c.execute(sql)
    conn.commit()
    conn.close()


@app.route('/')
def hello():
    return 'Hey there! Ready to execute some DEVStone models?'


@app.route('/run_devstone')
def run_devstone():
    try:
        api_key = request.args["api_key"]
    except Exception as e:
        return jsonify({"status": "denied", "reason": "No api_key was specified."})
    
    try:
        depth = int(request.args["depth"])
        width = int(request.args["width"])
        int_cycles = int(request.args["int_cycles"]) if "int_cycles" in request.args else 0
        ext_cycles = int(request.args["ext_cycles"]) if "ext_cycles" in request.args else 0
    except Exception as e:
        return jsonify({"status": "denied", "reason": "Invalid params."})

    # Check that api key exists
    username = get_username_from_api_key(api_key)
    if username is None:
        return jsonify({"status": "denied", "reason": "Invalid api key."})

    # Execute task
    request_id = uuid.uuid4().hex
    submit_devstone(request_id, username, depth, width, int_cycles, ext_cycles)
    status_url = request.host_url + "status?request_id=" + request_id
    return jsonify({"status": "submitted", "request_id": request_id, "status_url": status_url})


@app.route('/status')
def status():
    try:
        request_id = request.args["request_id"]
    except Exception as e:
        return jsonify({"status": "denied", "reason": "No request_id was specified."})
    
    status = get_request_status(request_id)
    res = {"status": status, "request_id": request_id}
    if status == "finished":
        download_url = request.host_url + "download?request_id=" + request_id
        res["download_link"] = download_url
    
    return jsonify(res)


@app.route('/download')
def download():
    try:
        request_id = request.args["request_id"]
    except Exception as e:
        return jsonify({"status": "denied", "reason": "No request_id was specified."})

    status = get_request_status(request_id)
    if status != "finished":
        status_url = request.host_url + "status?request_id=" + request_id
        return redirect(status_url)

    with open("results/%s.csv" % request_id) as f:
        csv = f.read()

    return Response(
        csv,
        mimetype="text/csv",
        headers={"Content-disposition":
                 "attachment; filename=%s.csv" % request_id})


if __name__ == '__main__':
    if not os.path.exists('results'):
        os.makedirs('results')
        
    app.run(debug=True, port=8080)
