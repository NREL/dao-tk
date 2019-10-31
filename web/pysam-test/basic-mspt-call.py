import json
import PySAM.TcsmoltenSalt as MSPT
import PySAM.Singleowner as Singleowner
from PySAM.PySSC import *

ssc = PySSC()

with open("mspt.json") as f:
    dic = json.load(f)
    gs_dat = dict_to_ssc_table(dic, "tcsmolten_salt")
    so_dat = dict_to_ssc_table(dic, "singleowner")
    gs = MSPT.wrap(gs_dat)
    so = Singleowner.wrap(so_dat)

gs_params = gs.export()
# so_params = so.export()

gs.SystemDesign.P_ref = 85.

gs.execute()

print( sum(gs.Outputs.gen )/1e6 )