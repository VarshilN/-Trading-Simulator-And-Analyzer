from jugaad_data.nse import stock_df
import datetime
from datetime import date,time,timedelta
import pandas as pd 
import sys
if __name__ == '__main__':
    print(sys.argv)
    sym=sys.argv[1]
    st=sys.argv[2]
    end=sys.argv[3]
    n=int(sys.argv[4])
    filename=sys.argv[5]
    format = '%d/%m/%Y'
    # print(st+" "+end)
    st = datetime.datetime.strptime(st, format)
    end = datetime.datetime.strptime(end, format)
    new_st=st
    if n!=-1:  
        st= st - timedelta(days=3*n)  
    df=stock_df(symbol=sym,from_date=date(st.year,st.month,st.day),to_date=date(end.year,end.month,end.day),series='EQ')
    # df=stock_df(symbol=sym,from_date=date(st.year,st.month,st.day),to_date=date(end.year,end.month,end.day),series='EQ')
    df.to_csv(f'{filename}.csv')
    print('data_loaded')