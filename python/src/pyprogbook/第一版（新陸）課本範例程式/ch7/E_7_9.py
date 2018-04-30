#E_7_9.py功能: 使用CSV套件讀寫檔案
import csv
with open('./file/SP3008_201511.csv', 'r') as fin:
    with open('./file/SP3008_csv_out.csv', 'w') as fout:
        csvreader = csv.reader(fin, delimiter=',')
        csvwriter = csv.writer(fout, delimiter=',')
        header = next(csvreader)
        print(header)
        csvwriter.writerow(header)
        for row in csvreader:
            row[0] = row[0].replace('/', '-')
            print(','.join(row))
            csvwriter.writerow(row)