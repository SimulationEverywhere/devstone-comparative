#!/usr/bin/env python3
import subprocess
import uuid
from flask import Flask, request, jsonify, Response, redirect
from concurrent.futures import ProcessPoolExecutor
import sqlite3
import os
import time

app = Flask(__name__)

PATH_DB = "db.sqlite"
PATH_RESULTS = "web_service/results/"

SQL_INSERT_REQUEST = "INSERT INTO requests ('id', 'username', 'depth', 'width', 'int_cycles', 'ext_cycles', 'status') VALUES ('{request_id}', '{username}', '{depth}', '{width}', '{int_cycles}', '{ext_cycles}', '{status}');"
SQL_SELECT_USERNAME = "SELECT username FROM users WHERE api_key='{api_key}'"
SQL_SELECT_REQUEST_STATUS = "SELECT status FROM requests WHERE id='{request_id}'"
SQL_UPDATE_STATUS = "UPDATE requests SET status='{status}' WHERE id='{request_id}'"

CMD_DEVSTONE_COMPARATIVE = "python3 devstone_comparative.py -w {width} -d {depth} -n {num_rep} -i {int_cycles} -e {ext_cycles} -o {out_file}"

executor = ProcessPoolExecutor()

def get_username_from_api_key(api_key):
    conn = sqlite3.connect(PATH_DB)
    c = conn.cursor()
    sql = SQL_SELECT_USERNAME.format(api_key=api_key)
    res = c.execute(sql).fetchall()
    conn.close()
    return res[0][0] if res else None


def get_request_status(request_id):
    conn = sqlite3.connect(PATH_DB)
    c = conn.cursor()
    sql = SQL_SELECT_REQUEST_STATUS.format(request_id=request_id)
    res = c.execute(sql).fetchall()
    conn.close()
    return res[0][0] if res else None


def run_devstone_task(request_id, username, model_types, depth, width, int_cycles, ext_cycles, num_rep):

    # TODO: here it would be the devstone comparative script call
    out_file = PATH_RESULTS + "%s.csv" % request_id
    cmd = CMD_DEVSTONE_COMPARATIVE.format(depth=depth, width=width, int_cycles=int_cycles, ext_cycles=ext_cycles, num_rep=num_rep, out_file=out_file)
    result = subprocess.run(cmd.split(), stdout=subprocess.PIPE)
    with open("cmd_outputs.log", "a") as out_file:
        out_file.write("(%s): %s" % (username, cmd))
        out_file.write(str(result.stdout))

    conn = sqlite3.connect(PATH_DB)
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
        model_types = request.args["model_types"] if "model_types" in request.args else "LI,HI,HO,HOmod"
        num_rep = int(request.args["num_rep"]) if "num_rep" in request.args else 10
    except Exception as e:
        return jsonify({"status": "denied", "reason": "Invalid params."})

    # Check that api key exists
    username = get_username_from_api_key(api_key)
    if username is None:
        return jsonify({"status": "denied", "reason": "Invalid api key."})

    request_id = uuid.uuid4().hex

    # Insert task into DB
    conn = sqlite3.connect(PATH_DB)
    c = conn.cursor()
    sql = SQL_INSERT_REQUEST.format(request_id=request_id, username=username, depth=depth, width=width,
                                    int_cycles=int_cycles, ext_cycles=ext_cycles, status="pending")
    c.execute(sql)
    conn.commit()
    conn.close()

    # Execute task
    executor.submit(run_devstone_task, request_id, username, model_types, depth, width, int_cycles, ext_cycles, num_rep)

    # Return response
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

    with open(PATH_RESULTS + "%s.csv" % request_id) as f:
        csv = f.read()

    return Response(
        csv,
        mimetype="text/csv",
        headers={"Content-disposition":
                 "attachment; filename=%s.csv" % request_id})


if __name__ == '__main__':
    if not os.path.exists('results'):
        os.makedirs('results')
        
    app.run(debug=True, host="0.0.0.0", port=8080)
