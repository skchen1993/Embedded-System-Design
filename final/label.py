#!/usr/bin/env python
import sys
import os.path
import random

if __name__ == "__main__":       
    BASE_PATH="/home/vincent/Desktop/archive"    
    SEPARATOR=";"    
    fh = open("./at.txt",'w')        
    lines = []
    for dirname, dirnames, filenames in os.walk(BASE_PATH):      
        for subdirname in dirnames:     
            if(subdirname[0] == 's'):       
                subject_path = os.path.join(dirname, subdirname)            
                for filename in os.listdir(subject_path):                
                    abs_path = "%s/%s" % (subject_path, filename)                          
                    # fh.write(abs_path)                
                    # fh.write(SEPARATOR)
                    # fh.write(subdirname[1:])                         
                    # fh.write("\n")       
                    lines.append(abs_path+';'+subdirname[1:]+'\n')
    random.shuffle(lines)
    for line in lines:
        fh.write(line)
    fh.close()
