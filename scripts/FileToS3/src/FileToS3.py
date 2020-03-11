"""
upload a file to s3 bucket.

If no positional arguments are given the settings from config.json are used. 

mendataroy: {
    "KeyID" : "xxxx",
    "AccessKey" : "xxxx",
    "destination_bucket_name" : "xxxx"
}
"""

# https://boto3.amazonaws.com/v1/documentation/api/latest/guide/s3-uploading-files.html

import logging
import boto3
from botocore.exceptions import ClientError
import os.path
import json
import sys
import argparse

# print('os.getcwd() ' + os.getcwd())

def GetConfigValues(filepath = 'config.json'):
    """
    read config value from config file

    {
        "KeyID" : "xxx_value",
        "AccessKey" : "xxx_value",
        "source_bucket_name" : "xxx_value"
    }   
  
    :param filepath: json config file where the key id and value are stored
    :return dict: with all config data
    """

    if not os.path.exists(filepath):
        raise Exception(f'config file not found: {filepath}')

    with open(filepath, 'r') as f:
        config = json.load(f)   
    return config


def UploadFileToClient(s3_client, file_name, bucket, object_name='None'):
    """Upload a file to an S3 bucket

    :param file_name: source file to upload
    :param bucket: destination S3 bucket name
    :param object_name: S3 object name. If not specified then file_name is used
    :return: True if file was uploaded, else False
    """

    # If S3 object_name was not specified, use file_name
    if object_name is None:
        object_name = file_name

    try:
        s3_client.upload_file(file_name, bucket, object_name)
    except ClientError as e:
        print("s3_client.upload_file EXCEPTION: {}".format(e))
        logging.error(e)
        return False
    except Exception:
        raise Exception('wrong credentials')

    return True


# s3 = boto3.client('s3')
# with open(my_testfile, "rb") as f:
#    s3.upload_fileobj(f, my_bucket_name)

def PrepareUploadToS3(conf, filename):
    
    if os.path.isfile(filename):
        s3_client = boto3.client('s3', aws_access_key_id=conf['KeyID'], aws_secret_access_key=conf['AccessKey'])

        if UploadFileToClient(s3_client, filename, conf['destination_bucket_name']):
            return('PrepareUploadToS3 success')
        else:
            return('PrepareUploadToS3 error')
    else:
        raise Exception('file to upload not found')


def CreateConfigFile(filename):
    conf =     {
        "KeyID" : "AWS_ACCESS_KEY_ID",
        "AccessKey" : "AWS_SECRET_ACCESS_KEY",
        "destination_bucket_name" : "my_s3_bucket_name"
    } 
    with open(filename, 'w') as fp:
        json.dump(conf, fp)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source_file", help='path to file which should be uploaded to s3\ne.g. home/data.txt')
    parser.add_argument("-s3", "--destination_bucket_name", help="name of the aws s3 bucket\nwhere the file should be uploaded to", default=None)
    parser.add_argument("-ki", "--KeyID", help="set the AWS_ACCESS_KEY_ID", default=None)
    parser.add_argument("-ak", "--AccessKey", help="set the AWS_SECRET_ACCESS_KEY", default=None)
    parser.add_argument("-conf", "--ConfigFilePath", help="set the path to config.json", default='config.json')
    parser.add_argument("--ConfigTemplate", help="dont upload file\nbut only create a default config.json file", action='store_true')
    parser.add_argument("-v", '--verbose', help="set verbose", action='store_true')
    parser.add_argument("-k", '--keep_file_prefix', help="TODO: append file path from source file\nat s3 object prefix", action='store_true')
    
    args = parser.parse_args()
    
    if args.verbose:
        print('args: ' + str(args))

    # check if all necessary arguments are available
    try:
        config_dict = GetConfigValues(args.ConfigFilePath)
    except Exception:
        if args.KeyID is not None and args.AccessKey is not None and args.destination_bucket_name is not None:
            config_dict = {}
        else:
            raise Exception('necessary arguments are missing')

    if args.verbose:
        print(f'config_dict       : {config_dict}')

    if args.KeyID is not None:
        print(f"override KeyID with args [{args.KeyID}]")
        config_dict['KeyID'] = args.KeyID

    if args.AccessKey is not None:
        print(f"override AccessKey with args [{args.AccessKey}]")
        config_dict['AccessKey'] = args.AccessKey

    if args.destination_bucket_name is not None:
        print(f"override destination_bucket_name with args [{args.destination_bucket_name}]")
        config_dict['destination_bucket_name'] = args.destination_bucket_name
    
    if args.verbose:
        print(f'config_dict merged: {config_dict}')

    if args.ConfigTemplate:
        CreateConfigFile('config_template.json')
        return(f'created config_template.json')

    print(f'PrepareUploadToS3: {PrepareUploadToS3(config_dict, args.source_file)}')

    return f'upload file: {args.source_file} success'


if __name__ == '__main__':
    print(main())