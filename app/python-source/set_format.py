import re

fdat = open("project.cpp","r").readlines()

fout = open("fproject.cpp","w")

    # h_tower.set(dnan, dmin, dmax, "h_tower", "Tower height", "m", "Variables");
	# number_heliostats.set(-1, "number_heliostats", true, "Heliostat count", "-", "Design|Outputs");

for line in fdat:
    res = re.search("(.*?)\.set\((.*?),(.*?),(.*?),\ *?\"(.*?)\",\ *?\"(.*?)\",\ *?\"(.*?)\",\ *?\"(.*?)\"\ *?\);", line)
    if res:
        items = res.groups()
        newline = "    {:<32s} {:>9s}, {:>9s}, {:>9s}, {:>20s}, {:>50s}, {:>10s}, {:>35s} );\n".format(
            items[0].strip()+".set(",
            items[1],
            items[2],
            items[3],
            '"'+items[4]+'"',
            '"'+items[5]+'"',
            '"'+items[6]+'"',
            '"'+items[7]+'"'
        )
    else:
        res = re.search("(.*?)\.set\((.*?),\ *?\"(.*?)\", (.*?),\ *?\"(.*?)\",\ *?\"(.*?)\",\ *?\"(.*?)\"\ *?\);", line)
        if res:
            items = res.groups()
            newline = "    {:<32s} {:>9s}, {:>30s}, {:>10s}, {:>50s}, {:>10s}, {:>35s} );\n".format(
                items[0].strip()+".set(",
                items[1],
                '"'+items[2]+'"',
                items[3],
                '"'+items[4]+'"',
                '"'+items[5]+'"',
                '"'+items[6]+'"'
            )
        else:
            res = re.search("(.*?)\.set\((.*?),\ *?\"(.*?)\", (.*?)\ *?\);", line)
            if res:
                items = res.groups()
                newline = "    {:<32s} {:>9s}, {:>30s}, {:>10s} );\n".format(
                    items[0].strip()+".set(",
                    items[1],
                    '"'+items[2]+'"',
                    items[3]
                )            
            else:
                newline = line
    fout.write(newline)


fout.close()