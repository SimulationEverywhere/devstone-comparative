import argparse
import sqlite3
import uuid

DB_FILENAME = "db.sqlite"

SQL_CREATE_USERS = \
    """CREATE TABLE users (
    username TEXT PRIMARY KEY,
    api_key TEXT NOT NULL
    );"""

SQL_CREATE_REQUESTS = \
    """CREATE TABLE requests (
    id TEXT PRIMARY KEY,
    username TEXT NOT NULL,
    depth INTEGER NOT NULL,
    width INTEGER NOT NULL,
    int_cycles INTEGER NOT NULL,
    ext_cycles INTEGER NOT NULL,
    status TEXT NOT NULL,
    FOREIGN KEY(username) REFERENCES users(username)
    );"""

SQL_INSERT_USER = "INSERT INTO users VALUES('{username}', '{api_key}');"
SQL_DELETE_USER = "DELETE FROM users WHERE username='{username}';"
SQL_SELECT_USERS = "SELECT username, api_key FROM users;"

conn = sqlite3.connect(DB_FILENAME)
c = conn.cursor()


def init_db():
    c.execute(SQL_CREATE_USERS)
    c.execute(SQL_CREATE_REQUESTS)


def add_user(username):
    api_key = uuid.uuid4().hex
    sql = SQL_INSERT_USER.format(username=username, api_key=api_key)
    c.execute(sql)
    conn.commit()
    print("User '%s' added succesfully (api_key: %s)" % (username, api_key))


def remove_user(username):
    sql = SQL_DELETE_USER.format(username=username)
    c.execute(sql)
    conn.commit()
    print("User '%s' removed succesfully from DB" % username)


def list_users():
    c.execute(SQL_SELECT_USERS)
    for user, key in c.fetchall():
        print("%s\t%s" % (user, key))


def parse_args():
    parser = argparse.ArgumentParser(
        description='Script to manage the DB related to the DEVStone comparative web service.')

    parser.add_argument('-i', '--init', action='store_true', help='Init the DB')
    parser.add_argument('-a', '--add_user', type=str, help='Add a user to the DB')
    parser.add_argument('-r', '--remove_user', type=str, help='Remove a user from the DB')
    parser.add_argument('-l', '--list_users', action='store_true', help='List existing users')

    return parser.parse_args()


if __name__ == '__main__':
    args = parse_args()

    if args.init:
        init_db()

    if args.add_user:
        add_user(args.add_user)

    if args.remove_user:
        remove_user(args.remove_user)

    if args.list_users:
        list_users()
