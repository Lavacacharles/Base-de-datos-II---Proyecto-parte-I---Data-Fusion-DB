import prueba 
import pandas as pd

# print(dir(prueba))
atributos = [['D', 'o', 'e'], ['3', '5'], ['3'], ['1', '0']]
atributos = [['J', 'u', 'a', 'n'], ['1', '9'], ['9'], ['6']]
record = prueba.Record(atributos)

FixedFILE = prueba.FixedFILE("data.dat")
FixedFILE.writeRecords(record)
records = FixedFILE.readAll()

for record in records: 
    for atributos in record.record:
        atr = ''
        for j in atributos:
            atr += j
        print(atr)
    print("--------------------")



# for atributos in record.record:
#     atr = ''
#     for j in atributos:
#         atr += j
#     print(atr)

