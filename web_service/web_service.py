#!/usr/bin/env python3
import uuid
from flask import Flask, request, jsonify
import sqlite3

app = Flask(__name__)

DB_FILENAME = "db.sqlite"
SQL_INSERT_REQUEST = "INSERT INTO requests ('username', 'depth', 'width', 'int_cycles', 'ext_cycles', 'status') VALUES ('{username}', '{depth}', '{width}', '{int_cycles}', '{ext_cycles}', '{status}');"
SQL_SELECT_USERNAME = "SELECT username FROM users WHERE api_key='{api_key}'"


def get_username_from_api_key(api_key):
    conn = sqlite3.connect(DB_FILENAME)
    c = conn.cursor()

    sql = SQL_SELECT_USERNAME.format(api_key=api_key)
    return c.execute(sql)


def submit_devstone(request_id, username, depth, width, int_cycles, ext_cycles):
    conn = sqlite3.connect(DB_FILENAME)
    c = conn.cursor()
    sql = SQL_INSERT_REQUEST.format(username=username, depth=depth, width=width, int_cycles=int_cycles, ext_cycles=ext_cycles)
    c.execute(sql)
    conn.commit()
    open("%s.csv" % request_id, "w").write(username)
    #sql = SQL_INSERT_REQUEST.format()


@app.route('/')
def hello():
    return 'Hello, World!'


@app.route('/run_devstone')
def run_devstone():
    try:
        api_key = int(request.args["api_key"])
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
    run_devstone(request_id, username, depth, width, int_cycles, ext_cycles)
    return jsonify({"status": "submitted", "request_id": request_id})


if __name__ == '__main__':
    app.run(debug=True, port=8080)
