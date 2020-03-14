"""
    Project Name: 
    adapter mysql database

    example call:
    python3 testrun_statistic.py -i SYST-XXXX -r output.xml -ref https://s3.console.aws.amazon.com/s3/buckets/applicati_usw --git_branch SYST-1785 --trigger_source myself -v -s
"""

#
# Imports
#
import argparse
import os.path


# mariaDB libs 
import mysql.connector
from mysql.connector import Error

# read stack outputs
import boto3

# read aws env
import requests
import json

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
        print(f'EXCEPTION connecting to testsystemdb: {e}')
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
        insert_testrun_info
    """

    table = 'node_log'

    if args.verbose:
        print(f"insert_into args: {args}")

    q = f"INSERT INTO " + args.database + "." + table + f" (ID, testcase_total, ) VALUES('{args.blie}', {args.bla}, ')"

    if args.verbose:
        print(f"execute query [{q}]")
    return __exec_query(q, args)

def select_from(args):
    conn = None
    try:
        conn = mysql.connector.connect(host='test-system-db.cjjgx9h6x7d1.eu-central-1.rds.amazonaws.com',database='testsystemdb',user='testUsername_XYZ',password='testPassword_XYZ')

        if not conn.is_connected():
            print('NOT connecting to testsystem database')
            return False
        else:
            cursor = conn.cursor()
            try:
                print('executing select from testrun db')
                cursor.execute(f"SELECT * FROM testsystemdb.testrun")
                records = cursor.fetchall()
                for row in records:
                    print(f"record: {row}")
            except Exception as e:
                print(f'EXCEPTION executing query {e}')

    except Error as e:
        print(f'EXCEPTION connecting to testsystemdb: {e}')
        return False
    finally:
        if conn is not None and conn.is_connected():    
            print('closing connection to DB')
            cursor.close()
            conn.close()
        else:
            print('__exec_query finally: no connection to close...')



def main():
    """
     database adapter
    """
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("-c", "--config_file", default="config.json", help="path to config file")
    parser.add_argument("-x", "--xx", help="xx")
    parser.add_argument("-v", '--verbose', help="set verbose", action='store_true')
    

    args = parser.parse_args()

    config = ReadConfigValues(args.config_file)
    if args.verbose:
        print(f"args: {args}")
        print(f"config: {config}")

    for key, value in args.items():
        config[key] = value

    if args.verbose:
        print(f"merged config: {config}")

    select_from(config)

    if args.verbose:
        print("## done ##")    

if __name__ == '__main__':
    main()
