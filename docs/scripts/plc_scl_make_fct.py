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

def get_names(fname='transfer.txt'):
    with open(fname) as f:
        content = f.readlines()
        content = [x.strip() for x in content]
        print(f'content from list {content}')
        return content

def make_file(list_fct, new_file='scl_output.txt'):
    file = open(new_file, "w")
    
    for item in list_fct:
        file.write(f'#{item}(signal_input:="I{item}",\n') 
        file.write(f'input_name:=\'{item}\',\n')
        file.write(f'signal_changed=>"MI_{item}");\n\n')
    file.close()

def main():
    list_fct = get_names()
    make_file(list_fct)

if __name__ == "__main__":
    main()
