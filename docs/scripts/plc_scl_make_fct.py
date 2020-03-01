"""
generate scl functions with same pattern for all di input variables 

input:
diningSouth_down
diningWest_leftDown
...

output:
#diningSouth_down(signal_input:="IdiningSouth_down",
input_name:='diningSouth_down',
signal_changed=>"MI_diningSouth_down");

#diningWest_leftDown(signal_input:="IdiningWest_leftDown",
input_name:='diningWest_leftDown',
signal_changed=>"MI_diningWest_leftDown");

...

"""

import argparse

def get_names(fname):
    with open(fname) as f:
        content = f.readlines()
        content = [x.strip() for x in content]
        print(f'content from list {content}')
        return content

def make_file(output_file, list_fct):
    file = open(output_file, "w")
    
    for item in list_fct:
        file.write(f'#{item}(signal_input:="I{item}",\n') 
        file.write(f'input_name:=\'{item}\',\n')
        file.write(f'signal_changed=>"MI_{item}");\n\n')
    file.close()
    print(f'writing file {output_file} -> done')

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("-i", "--input_file", default='input_file.txt')
    parser.add_argument("-o", "--output_file", default='output_file.txt')
    args = parser.parse_args()
    
    list_fct = get_names(args.input_file)
    make_file(args.output_file, list_fct)

if __name__ == "__main__":
    main()
