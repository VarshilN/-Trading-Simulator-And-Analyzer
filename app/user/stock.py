from flask import Blueprint,render_template,request
from datetime import datetime,timedelta,date
from jugaad_data.nse import NSELive,stock_df,index_df,history
import plotly.express as px
import plotly.graph_objects as go
import yfinance as yf

stock_bp = Blueprint('stock',__name__,template_folder='../../templates',static_folder='../../static')
@stock_bp.route('/search')
def search():
    return render_template('searchPanel.html')
@stock_bp.route('/plot',methods=['POST','GET'])
def plot():
    def get_stock_info(ticker_symbol):
        stock=yf.Ticker(ticker_symbol)
        market_cap = stock.info.get('marketCap', 'N/A')
        trailing_pe_ratio = stock.info.get('trailingPE', 'N/A')
        pb_ratio = stock.info.get('priceToBook', 'N/A')
        roe = stock.info.get('returnOnEquity', 'N/A')
        peg_ratio = stock.info.get('pegRatio', 'N/A')
        peg_ratio=0
        roe=0 
        dividend_yield = stock.info['dividendYield']
        average_price = stock.info['regularMarketDayHigh'] + stock.info['regularMarketDayLow'] / 2
        dic={'mcap':market_cap,'pe':trailing_pe_ratio,'pb':pb_ratio,'peg':peg_ratio,'roe':roe,'div':dividend_yield,'avg':average_price}
        return dic 
    search_type=request.args['search_type']
    current_date=datetime.now()   
    interval=request.args.get('interval','1w')
    back_date=current_date
    if interval=='1w':
        back_date=current_date-timedelta(weeks=1)  
    if interval == '1M':
        back_date = current_date - timedelta(weeks=4)
    if interval == '1Yr':
        back_date = current_date - timedelta(weeks=52)
    if interval == '3Yr':
        back_date = current_date - timedelta(weeks=3*52)
    if interval == '5Yr':
        back_date = current_date - timedelta(weeks=5*52)
    elif interval=='all':
        back_date=current_date-timedelta(weeks=30*52)
    src=request.args['src']
    n=NSELive()
    market_status_data = n.market_status()
    # market_cap_in_crs = market_status_data.get('marketcap', {}).get('marketCapinCRRupees', None)
    status=market_status_data['marketState'][0]['marketStatus']
    if(search_type=='symbol'):
        df=stock_df(src,from_date=date(back_date.year,back_date.month,back_date.day),to_date=date(current_date.year,current_date.month,current_date.day),series='EQ') 
        symbol=src+'.NS'
        dic=get_stock_info(symbol)
        market_cap=dic['mcap']
        div=dic['div']
        pe=dic['pe']
        pb=dic['pb']
        peg=dic['peg']
        roe=dic['roe']
        avg=dic['avg']
        n=NSELive()
        q=n.stock_quote(src)
        industry=q['info'].get('industry','##')
        dat =df.iloc[0]['DATE']
        dat=date(dat.year,dat.month,dat.day)
        vol = round(df.iloc[0]['VOLUME'],2)
        val =round(df.iloc[0]['VALUE'],2)
        YH = df.iloc[0]['52W H']
        YL = df.iloc[0]['52W L']
        no_of_trades=df.iloc[0]['NO OF TRADES']
        if request.args.get('g_type')=='sct':
            fig = px.line(df, x='DATE', y='HIGH', title=f"{q['info']['companyName']} Stock Price")
            fig.update_layout(title='Scatter Chart', xaxis_title='Date', yaxis_title='Price')
        else:
            fig= go.Figure(data=[go.Candlestick(x=df['DATE'],
                                              open=df['OPEN'],
                                              high=df['HIGH'],
                                              low=df['LOW'],
                                              close=df['CLOSE'])])
            fig.update_layout(title='Candlestick Chart', xaxis_title='Date', yaxis_title='Price')
        fig_html = fig.to_html()
        return render_template("plot.html",lcp=q['priceInfo']['lowerCP'],ucp=q['priceInfo']['upperCP'],industry=industry,name=q['info']['companyName'],price=q['priceInfo']['lastPrice'],open=q['priceInfo']['open'],close=q['priceInfo']['close'],HIGH=q['priceInfo']['intraDayHighLow']['max'],LOW=q['priceInfo']['intraDayHighLow']['min'],WH=q['priceInfo']['weekHighLow']['max'],WL=q['priceInfo']['weekHighLow']['min'],fig_html=fig_html,src=src,search_type='symbol',isTop10=q['info']['isTop10'],st=status,change=round(q['priceInfo']['change'],2),chg=round(q['priceInfo']['pChange'],2),val=val,vol=vol,YH=YH,YL=YL,dat=dat,notrades=no_of_trades,mcap=market_cap,div=div,pe=pe,pb=pb,roe=roe,avg=avg,peg=peg)
    else:       
        div='N/A'
        pe='N/A'
        pb='N/A'
        peg='N/A'
        roe='N/A'
        avg='N/A'
        df = history.index_df(symbol=src,from_date=date(back_date.year,back_date.month,back_date.day),to_date=date(current_date.year,current_date.month,current_date.day))
        q=n.live_index(src)
        fig=px.line
        if request.args.get('g_type')=='sct':
            fig = px.line(df, x='HistoricalDate', y='HIGH', title=f"{q['name']} Stock Price")
            fig.update_layout(title='Scatter Chart', xaxis_title='Date', yaxis_title='Price')
        else:
            fig= go.Figure(data=[go.Candlestick(x=df['HistoricalDate'],
                                              open=df['OPEN'],
                                              high=df['HIGH'],
                                              low=df['LOW'],
                                              close=df['CLOSE'])])
            fig.update_layout(title='Candlestick Chart', xaxis_title='Date', yaxis_title='Price')
        fig_html=fig.to_html()
        vol =q['data'][0]['totalTradedVolume']
        val=q['data'][0]['totalTradedValue']
        dat=date(current_date.year,current_date.month,current_date.day)
        return render_template("plot.html",name=src,price=q['data'][0]['lastPrice'],YH=q['data'][0]['yearHigh'],YL=q['data'][0]['yearLow'],HIGH=q['data'][0]['dayHigh'],LOW=q['data'][0]['dayLow'],open=q['data'][0]['open'],close=q['data'][0]['previousClose'],change=q['data'][0]['change'],chg=q['data'][0]['pChange'],st=status,fig_html=fig_html,src=src,search_type='index',vol=vol,div=div,pe=pe,peg=peg,avg=avg,pb=pb,roe=roe,val=val,dat=dat) 
@stock_bp.route('/user')
def user():
    return render_template('compare_stocks.html')
selected_stocks=[]
@stock_bp.route('/compare_stocks')
def compare_stocks():
    if 'remove_graph' in request.args:
        selected_stocks.clear()
        graph_html=None
        return render_template('compare_stocks.html')
    else:
        symbol=str(request.args.get('src'))
        interval=str(request.args.get('interval','1w'))
        selected_stocks.append(symbol)
        current_date=datetime.now()   
        back_date=current_date-timedelta(weeks=1)  
        if interval == '1M':
            back_date = current_date - timedelta(weeks=4)    
        elif interval == '1Yr':
            back_date = current_date - timedelta(weeks=52)
        elif interval == '3Yr':
            back_date = current_date - timedelta(weeks=3*52)
        elif interval == '5Yr':
            back_date = current_date - timedelta(weeks=5*52)
        elif interval=='All':
            back_date=current_date-timedelta(weeks=30*52)
        fig=px.line()     
        for symbol in selected_stocks:
            df=stock_df(symbol=symbol,from_date=date(back_date.year,back_date.month,back_date.day),to_date=date(current_date.year,current_date.month,current_date.day),series='EQ')
            if request.args.get('g_type')=='sct':
                fig.add_scatter(x=df['DATE'],y=df['HIGH'],mode='lines',name=f'{symbol}Close Price')
                fig.update_layout(title='Stock Price Comparison', xaxis_title='Date', yaxis_title='Close Price')
            else:
                fig.add_trace(go.Candlestick(x=df['DATE'],
                                             open=df['OPEN'],
                                             high=df['HIGH'],
                                             low=df['LOW'],
                                             close=df['CLOSE'],
                                             name=f'{symbol} Candlestick'))
                fig.update_layout(title='Candlestick Chart', xaxis_title='Date', yaxis_title='Price') 
        graph_html = fig.to_html(full_html=False)
        return render_template('compare_stocks.html',selected_stocks=selected_stocks,graph_html=graph_html)
@stock_bp.route('/get_filters')                                                                                                 
def get_filters():
    return render_template('filters.html')
@stock_bp.route('/filters')
def filters():
    src=request.args.get('src')
    def get_stock_info(ticker_symbol):
        stock=yf.Ticker(ticker_symbol)
        market_cap = stock.info.get('marketCap', 'N/A')
        trailing_pe_ratio = stock.info.get('trailingPE', 'N/A')
        pb_ratio = stock.info.get('priceToBook', 'N/A')
        roe = stock.info.get('returnOnEquity', 'N/A')
        peg_ratio = stock.info.get('pegRatio', 'N/A')
        peg_ratio=0
        roe=0 
        dividend_yield = stock.info.get('dividendYield','N/A')
        average_price = stock.info['regularMarketDayHigh'] + stock.info['regularMarketDayLow'] / 2
        dic={'mcap':market_cap,'pe':trailing_pe_ratio,'pb':pb_ratio,'peg':peg_ratio,'roe':roe,'div':dividend_yield,'avg':average_price}
        return dic  
    n=NSELive()
    def get_stocks_from_index(index_symbol):
        q=n.live_index(index_symbol)
        cnt=0
        for stk in q['data']:
            if cnt==0:
                cnt=1
                continue
            sym=stk['symbol']+'.NS'
            dic=get_stock_info(sym)
            dic['sym']=stk['symbol']
            dic['pchg']=stk['pChange']
            dic['cname']=stk['meta']['companyName']
            filtered_stocks.append(dic)
    def custom_sort(item):
        return item[selected] if item[selected] != 'N/A' else -float('inf')
    filtered_stocks=[]
    get_stocks_from_index(src)
    idx=src
    selected=request.args.get('filters')
    if selected=='pchg':
        filtered_stocks=sorted(filtered_stocks,key=custom_sort,reverse=True)
        return render_template('filters.html',filtered_stocks=filtered_stocks,idx=idx,flag=1)
    elif selected=='peg':
        filtered_stocks=sorted(filtered_stocks,key=custom_sort,reverse=True)
        return render_template('filters.html',filtered_stocks=filtered_stocks,flag=5)
    elif selected=='pe':
        filtered_stocks=sorted(filtered_stocks,key=custom_sort,reverse=True)
        return render_template('filters.html',filtered_stocks=filtered_stocks,flag=3)
    elif selected=='avg':
        filtered_stocks=sorted(filtered_stocks,key=custom_sort,reverse=True)
        return render_template('filters.html',filtered_stocks=filtered_stocks,idx=idx,flag=2)
    elif selected=='pb':
        filtered_stocks=sorted(filtered_stocks,key=custom_sort,reverse=True)
        return render_template('filters.html',filtered_stocks=filtered_stocks,flag=4)
    elif selected=='mcap':
        filtered_stocks=sorted(filtered_stocks,key=custom_sort,reverse=True)
        return render_template('filters.html',filtered_stocks=filtered_stocks,flag=8)
    elif selected=='roe':
        filtered_stocks=sorted(filtered_stocks,key=custom_sort,reverse=True)
        return render_template('filters.html',filtered_stocks=filtered_stocks,flag=6)
    elif selected=='div':
        filtered_stocks=sorted(filtered_stocks,key=custom_sort,reverse=True)
        return render_template('filters.html',filtered_stocks=filtered_stocks,idx=idx,flag=7)
    

    