start = 0
filecount = 0
filesize = 0
with open('filelist.txt') as input_file:
    #input_file = sorted(input_file)
    for i, line in enumerate((input_file)):
        line = line.replace('\n', '').replace('\r', '') 
        if(start == 1):
            line_contents = line.split('\t',2)
            if len(line_contents)>1:
                print line_contents
                filecount = filecount+1
                filesize = filesize + int(line_contents[1])
        #print line,
        if(line.find("end", 20) > 0):
            start = 1
            print "START"
        
print filecount," files"
print filesize," size"
#print "{0} line(s) printed".format(i+1)
