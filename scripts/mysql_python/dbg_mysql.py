"""
    Project Name: 
    adapter mysql database

    example call:

"""

#
# Imports
#
import argparse
import os.path
import json


# mariaDB libs 
import mysql.connector
from mysql.connector import Error

# read stack outputs
import boto3


#
# Code
#


def ReadConfigValues(filepath = 'config.json'):
    """
    read config value from config file
    {
        "password" : "",
        "username" : "auto_home",
        "host" : "192.168.1.200",
        "database" : "auto_home" 
    } 
    :param filepath: json config file where the key id and value are stored
    :return dict: with all config data
    """

    if not os.path.exists(filepath):
        raise Exception(f'config file not found: {filepath}')

    with open(filepath, 'r') as f:
        config = json.load(f)   
    return config

def __exec_query(query, args):
    """
    execute query at testsystem db
    """
    conn = None
    try:
        conn = mysql.connector.connect(host=args.host, database=args.database, user=args.username, password=args.password)

        if not conn.is_connected():
            print('NOT connecting to database')
            return False
        else:
            if args.verbose:
                print('connection to DB established')
            cursor = conn.cursor()
            try:
                cursor.execute(query)
                conn.commit()
                return True
            except Exception as e:
                print(f'EXCEPTION executing [{query}] {e}')

    except Error as e:
        print(f'EXCEPTION connecting to db: {e}')
        return False
    finally:
        if conn is not None and conn.is_connected():
            if args.verbose:
                print('closing connection to DB')
            cursor.close()
            conn.close()
        else:
            if args.verbose:
                print('__exec_query finally: no connection to close...')

def insert_into(args):
    """
        insert_into database
    """

    table = 'node_log'

    if args['verbose']:
        print(f"insert_into with args: {args}")

    q = f"INSERT INTO " + args.database + "." + table + f" (ID, testcase_total, ) VALUES('{args.blie}', {args.bla}, ')"

    if args.verbose:
        print(f"execute query [{q}]")
    return __exec_query(q, args)


def select_from(args):
    if args['verbose']:
        print(f"select_from args: {args}")
        
    table = 'node_log'
    conn = None
    try:
        conn = mysql.connector.connect(host=args['host'], database=args['database'], user=args['username'], password=args['password'])

        if not conn.is_connected():
            print('NOT connecting to database')
            return False
        else:
            cursor = conn.cursor()
            try:
                if args['verbose']:
                    print('executing select')
                cursor.execute(f"SELECT * FROM " + args['database'] + "." + table + " ORDER BY timestamp DESC LIMIT " + args['query_limit'])
                records = cursor.fetchall()
                for row in records:
                    print(f"record: {row}")
            except Exception as e:
                print(f'EXCEPTION executing query {e}')
    except Error as e:
        print(f'EXCEPTION connecting to db: {e}')
        return False
    finally:
        if conn is not None and conn.is_connected():
            if args['verbose']:
                print('closing connection to DB')
            cursor.close()
            conn.close()
        else:
            if args['verbose']:
                print('__exec_query finally: no connection to close...')
  
def main():
    """
    database adapter
    variables are taken from config.json
    if passed by argument this values are taken
    """
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("-c", "--config_file", default="config.json", help="path to config file")
    parser.add_argument("-x", "--xx", help="xx")
    parser.add_argument("-l", "--query_limit", help="how many rows from database are returned")
    parser.add_argument("-v", '--verbose', help="set verbose", action='store_true')
    

    args = parser.parse_args()

    config = ReadConfigValues(args.config_file)

    if args.verbose:
        print(f"args: {args}")
        for arg in vars(args):
            print(f"for arg: {arg} -> {getattr(args, arg)}")
        print(f"config: {config}")

    for arg in vars(args):
        attr = getattr(args, arg, None)
        if attr is not None:
            config[arg] = getattr(args, arg)
    #for key, value in args:
    #    config[key] = value
    # config['verbose'] = args.verbose

    if args.verbose:
        print(f"merged config: {config}")

    select_from(config)

    if args.verbose:
        print("## done ##")    

if __name__ == '__main__':
    main()
