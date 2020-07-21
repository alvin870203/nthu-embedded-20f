# Part. 1
#=======================================
# Import module
#  csv -- fileIO operation
import csv
#=======================================


# Part. 2
#=======================================
# Read cwb weather data
cwb_filename = '105033110.csv'
#cwb_filename = 'sample_input.csv'
data = []
header = []
with open(cwb_filename) as csvfile:
   mycsv = csv.DictReader(csvfile)
   header = mycsv.fieldnames
   for row in mycsv:
      data.append(row)
data = list(filter(lambda item: item['WDSD'] != '-99.000', data))  #delete the data that WDSD=-99.000
data = list(filter(lambda item: item['WDSD'] != '-999.000', data)) #delete the data that WDSD=-999.000

#=======================================


# Part. 3
#=======================================
# Analyze data depend on your group and store it to target_data like:
# Retrive all data points which station id is "C0X260" as a list.
# target_data = list(filter(lambda item: item['station_id'] == 'C0X260', data))
target_station = ['C0A880', 'C0F9A0', 'C0G640', 'C0R190', 'C0X260']                  #stations to be analyize
output_data = []                                                                     #the output list
for n in target_station :
   target_data = list(filter(lambda item: item['station_id'] == n, data))            #get all data of the target station
   target_data = sorted(target_data, key=lambda item: item['WDSD'])                        #sort target_data by WDSD
   if len(target_data)>=2 :                                                          #max range exist
      output_data.append([n, float(target_data[-1]['WDSD']) - float(target_data[0]['WDSD'])])
   else :                                                                            #max range not exsit
      output_data.append([n, 'None'])
#=======================================


# Part. 4
#=======================================
# Print result
print(output_data)
#========================================