"""
    read tia variable table and create master data config from it
    variables are taken from config.json
    if passed by argument this values are taken
   
    example call:
    
"""

#
# Imports
#
import argparse
import os.path
import json




def ReadConfigValues(args):
    """
    read config value from config file
    {
        "password" : "",
        "username" : "auto_home",
    } 
    :param filepath: json config file where the key id and value are stored
    :return dict: with all config data
    """

    filepath = args.config

    # TODO: change to print and not to exception 
    if not os.path.exists(filepath):
        raise Exception(f'config file not found: {filepath}')

    with open(filepath, 'r') as f:
        config = json.load(f)   
    # return config

    if args.verbose:
        print(f" -info- config from file: {config}")
        print(f" -info- args: {args}")
        
        # loop through args even if not none
        ##-info- args: Namespace(config='config.json', input=None, output='plcVar.txt', verbose=True, xx=None)
        ##for arg: config -> config.json
        ##for arg: input -> None
        ##for arg: output -> plcVar.txt
        ##for arg: xx -> None
        ##for arg: verbose -> True
#        for arg in vars(args):
#            print(f"for arg: {arg} -> {getattr(args, arg)}")
        

    ## merge the variable values from args and from config file to one list
    # a value set by args has priority
    for arg in vars(args):
        attr = getattr(args, arg, None)
        if attr is not None:
            config[arg] = getattr(args, arg)
            if args.verbose:
                print(f"arg to config: {arg} -> {config[arg]}")
    #for key, value in args:
    #    config[key] = value
    # config['verbose'] = args.verbose
    print(f" -info- merged config: {config}")
    return config

def parse_xls_file(path):
    print('parse_xls_file')
    pass

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("-c", "--config", default="config.json", help="path to config file")
    parser.add_argument("-i", "--input", help="path to plc_xlsx file which should be parsed")
    parser.add_argument("-o", "--output", default="plcVar.txt", help="generated md print to file")
    parser.add_argument("-x", "--xx", help="xx")
    parser.add_argument("-v", '--verbose', help="set verbose", action='store_true')
    

    args = parser.parse_args()

    config = ReadConfigValues(args)

    #if args.verbose:
    #    print(f"merged config in main: {config}")
    
    parse_xls_file(config.input)


    if args.verbose:
        print("## done ##")    

if __name__ == '__main__':
    main()
