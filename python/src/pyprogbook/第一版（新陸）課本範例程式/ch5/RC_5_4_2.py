# RC_5_4_2 功能: 敘述統計。
import statistics as st
sample=[5,8,9,6,4,1,5,3,6,2]
print('sample = ', sample)
print('mean = ',  st.mean(sample))
print('mean = ',  max(sample))
print('mean = ',  min(sample))
print('variance = ', round(st.variance(sample),2))
print('standard deviation = ',  round(st.stdev(sample),2))