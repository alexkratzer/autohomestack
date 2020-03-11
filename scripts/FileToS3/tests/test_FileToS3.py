###
### call with: 
### py.test -s test_upload_to_s3.py
### ~/repo/scripts/FileToS3$ python3 -m pytest tests/test_FileToS3.py

import pytest
import boto3

import os
import sys
sys.path.append(f"{os.getcwd()}")
import FileToS3


##############################
## test_GetConfigValue      ##
##############################

def test_GetConfigValues_file_not_found():
    file_not_existing = 'file_not_existing'
    with pytest.raises(Exception, match=f'config file not found: {file_not_existing}'):
        assert FileToS3.GetConfigValues(file_not_existing)


def test_GetConfigValues_check_content():
    """test that config file includes credentials"""
    confDict = FileToS3.GetConfigValues()
    
    assert 'KeyID' in confDict
    assert 'AccessKey' in confDict
    assert 'self_test_key' in confDict


def test_GetConfigValues_check_value():
    """test that config file value matches"""
    confDict = FileToS3.GetConfigValues()

    assert confDict['self_test_key'] == 'self_test_value'
  

##############################
## test_UploadFileToClient  ##
##############################

def test_UploadFileToClient_wrong_credentials_id():
    """test that exception is raised for invalid credentials id"""

    s3_client = boto3.client('s3', aws_access_key_id='not_Existing', aws_secret_access_key='not_Existing')
    with pytest.raises(Exception, match='wrong credentials'):
        assert FileToS3.UploadFileToClient(s3_client, 'tests/upload_test_file.md', 'BucketName')


def test_UploadFileToClient_wrong_credentials_access_key():
    """test that exception is raised for invalid credentials access_key"""

    tmp = FileToS3.GetConfigValues()

    s3_client = boto3.client('s3', aws_access_key_id=tmp['KeyID'], aws_secret_access_key='not_Existing')
    with pytest.raises(Exception, match='wrong credentials'):
        assert FileToS3.UploadFileToClient(s3_client, 'tests/upload_test_file.md', 'BucketName')


def test_UploadFileToClient_good():
    """ good test """
    pass


##############################
## test_PrepareUploadToS3          ##
##############################

def test_PrepareUploadToS3_OK():
    """ good test """
    confDict = FileToS3.GetConfigValues('tests/config.json')
    assert FileToS3.PrepareUploadToS3(confDict, 'tests/upload_test_file.md') == 'PrepareUploadToS3 success'


def test_PrepareUploadToS3_no_file():
    """test that exception is raised for invalid file path"""
    confDict = FileToS3.GetConfigValues('tests/config.json')
    with pytest.raises(Exception) as e:
        assert FileToS3.PrepareUploadToS3(confDict, 'not/existing/path.csv')
    assert str(e.value) == 'file to upload not found'


def test_PrepareUploadToS3_no_file_ext():
    """test that exception is raised for invalid file path"""
    confDict = FileToS3.GetConfigValues('tests/config.json')
    with pytest.raises(Exception) as e:
        assert FileToS3.PrepareUploadToS3(confDict, 'tests/path')
    assert str(e.value) == 'file to upload not found'


def test_PrepareUploadToS3_wrong_credentials_id():
    """test that exception is raised for invalid credentials"""
    confDict = FileToS3.GetConfigValues('tests/config.json')
    confDict['KeyID'] = '12None34'
    confDict['AccessKey'] = '56None78'

    with pytest.raises(Exception, match='wrong credentials'):
        assert FileToS3.PrepareUploadToS3(confDict, 'tests/upload_test_file.md')


##############################
## test_FileToS3            ##
##############################

real_file = 'tests/upload_test_file.md'

##@pytest.mark.parametrize('name', ['spam', 'eggs', 'bacon'])
def test_FileToS3_main(monkeypatch): #, name):
    with monkeypatch.context() as m:
        m.setattr(sys, 'argv', ['FileToS3', real_file, '--ConfigFilePath', 'tests/config.json'])
        assert FileToS3.main() == f'upload file: {real_file} success'

def test_FileToS3_main_args_from_cmd(monkeypatch):
    confDict = FileToS3.GetConfigValues('tests/config.json')
    with monkeypatch.context() as m:
        m.setattr(sys, 'argv', ['FileToS3', real_file, '-ki', confDict['KeyID'], '-ak', confDict['AccessKey'], '-s3', confDict['destination_bucket_name'], '--ConfigFilePath', 'not_existing'])
        assert FileToS3.main() == f'upload file: {real_file} success'

def test_FileToS3_main_missing_args(monkeypatch):
    with monkeypatch.context() as m:
        m.setattr(sys, 'argv', ['FileToS3', real_file, '--ConfigFilePath', 'not_existing'])
        with pytest.raises(Exception, match='necessary arguments are missing'):
            assert FileToS3.main() 

def test_FileToS3_main_ConfigTemplate(monkeypatch):
    with monkeypatch.context() as m:
        m.setattr(sys, 'argv', ['FileToS3', 'unused_file_name', '--ConfigTemplate'])
        assert FileToS3.main() == f'created config_template.json'
