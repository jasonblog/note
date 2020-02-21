# finviz 篩選


```sh
https://github.com/mariostoev/finviz
```

- Installation finviz
    - pip install finviz 


```py
from finviz.screener import Screener

filters = ['fa_eps5years_pos', 'fa_pe_u20' ,'fa_roa_o5', 'fa_sales5years_pos', 'idx_sp500']  # Shows companies in NASDAQ which are in the S&P500
# Get the first 50 results sorted by price ascending
stock_list = Screener(filters=filters, order='price')

# Export the screener results to .csv
stock_list.to_csv()

# Create a SQLite database
stock_list.to_sqlite('./aa')

#for stock in stock_list[9:19]:  # Loop through 10th - 20th stocks
#    print(stock['Ticker'], stock['Price']) # Print symbol and price
#
## Add more filters
#stock_list.add(filters=['fa_div_high'])  # Show stocks with high dividend yield
## or just stock_list(filters=['fa_div_high'])
#
## Print the table into the console
#print(stock_list)
```

- Installation sqlitebrowser
    -  sudo apt-get install sqlitebrowser
    


- Browse sqite data
    -  sqlitebrowser aa