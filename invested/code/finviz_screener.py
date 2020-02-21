from finviz.screener import Screener

filters = ['fa_eps5years_pos', 'fa_pe_u20' ,'fa_roa_o5', 'fa_sales5years_pos', 'idx_sp500']  # Shows companies in NASDAQ which are in the S&P500
# Get the first 50 results sorted by price ascending
# 

'''
def __init__(self, tickers=None, filters=None, rows=None, order='', signal='', table=None, custom=None):
    """
    Initilizes all variables to its values

    :param tickers: collection of ticker strings eg.: ['AAPL', 'AMD', 'WMT']
    :type tickers: list
    :param filters: collection of filters strings eg.: ['exch_nasd', 'idx_sp500', 'fa_div_none']
    :type filters: list
    :param rows: total number of rows to get
    :type rows: int
    :param order: table order eg.: '-price' (to sort table by descending price)
    :type order: str
    :param signal: show by signal eg.: 'n_majornews' (for stocks with major news)
    :type signal: str
    :param table: table type eg.: 'Performance'
    :type table: str
    :param custom: collection of custom columns eg.: ['1', '21', '23', '45']
    :type custom: list
    :var self.data: list of dictionaries containing row data
    :type self.data: list
    """
'''

stock_list = Screener(filters=filters, order='price', table='Valuation')

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
