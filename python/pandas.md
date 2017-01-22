# pandas

![](./images/pandas-dataframe-shadow.png)


For the sake of simplicity, I am not showing the OrderedDict approach because the from_items approach is probably a more likely real world solution.

If this is a little hard to read, you can also get the PDF version.

Simple Example
This may seem like a lot of explaining for a simple concept. However, I frequently use these approaches to build small DataFrames that I combine with my more complicated analysis.

For one example, let’s say we want to save our DataFrame and include a footer so we know when it was created and who it was created by. This is much easier to do if we populate a DataFrame and write it to Excel than if we try to write individual cells to Excel.

Take our existing DataFrame:

```py
sales = [('account', ['Jones LLC', 'Alpha Co', 'Blue Inc']),
         ('Jan', [150, 200, 50]),
         ('Feb', [200, 210, 90]),
         ('Mar', [140, 215, 95]),
         ]
df = pd.DataFrame.from_items(sales)
```

Now build a footer (in a column oriented manner):

```py
from datetime import date

create_date = "{:%m-%d-%Y}".format(date.today())
created_by = "CM"
footer = [('Created by', [created_by]), ('Created on', [create_date]), ('Version', [1.1])]
df_footer = pd.DataFrame.from_items(footer)
```

Created by	Created on	Version
0	CM	09-05-2016	1.1

Combine into a single Excel sheet:

```py
writer = pd.ExcelWriter('simple-report.xlsx', engine='xlsxwriter')
df.to_excel(writer, index=False)
df_footer.to_excel(writer, startrow=6, index=False)
writer.save()
```
