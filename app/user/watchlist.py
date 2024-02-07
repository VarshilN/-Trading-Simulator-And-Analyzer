from user.user import watchlist,User,db,session
from jugaad_data.nse import NSELive
from flask import Blueprint,render_template,redirect,request,url_for
import yfinance as yf
watchlist_bp = Blueprint('watchlist',__name__,template_folder='../../templates',static_folder='../../static')
@watchlist_bp.route('/update _watchList',methods=['GET','POST'])
def update_watchList():
    user = User.query.filter_by(email=session['email']).first()
    n=NSELive()
    market_status_data = n.market_status()
    status=market_status_data['marketState'][0]['marketStatus']
    if user:
        watchlist_items = watchlist.query.filter_by(user_id=user.id).all()
        #updated_watchlist = []
        for item in watchlist_items:
            src=item.symbol
            q=n.stock_quote(src)['priceInfo']
            stock=yf.ticker(src+'.NS')
            market_cap = stock.info.get('marketCap', 'N/A')
            watchlist_item = watchlist.query.filter_by(user_id=user.id, symbol=src).first()
            watchlist_item.last_price = round(q['lastPrice'], 2)
            watchlist_item.price_change = round(q['change'], 2)
            watchlist_item.percent_change = round(q['pChange'], 2)
            watchlist_item.market_status = status
            watchlist_item.vwap = round(q['vwap'], 2)
            watchlist_item.market_cap = round(market_cap, 3)
    return redirect(url_for('watchlist.watchList'))
@watchlist_bp.route('/remove_stock',methods=['GET','POST'])
def remove_stock():
    src=request.args.get('symbol')
    user = User.query.filter_by(email=session['email']).first()
    watchlist_item = watchlist.query.filter_by(user_id=user.id,symbol=src).first()
    if watchlist_item:
        db.session.delete(watchlist_item)
        db.session.commit()
    return redirect(url_for('watchlist.watchList'))
@watchlist_bp.route('/watchList')
def watchList():
    user = User.query.filter_by(email=session['email']).first()
    if user:
        watchlist_items = watchlist.query.filter_by(user_id=user.id).all()
        return render_template('watchlist.html', watchlist_items=watchlist_items)
    return "User not found" 
@watchlist_bp.route('/add_stock',methods=['GET','POST'])
def add_stock():
        src=request.args.get('src')
        ticker_symbol=src+'.NS'
        stock=yf.Ticker(ticker_symbol)
        market_cap = stock.info.get('marketCap', 'N/A')
        selected=request.args.get('search_type')
        n=NSELive()
        user = User.query.filter_by(email=session['email']).first()   
        if selected=='symbol':
            market_status_data = n.market_status()
            status=market_status_data['marketState'][0]['marketStatus']
            q=n.stock_quote(src)['priceInfo']
            if user:
                if not watchlist.query.filter_by(user_id=user.id, symbol=src).first():    
                    watchlist_item = watchlist(
                        user_id=user.id,
                        symbol=src,
                        last_price=round(q['lastPrice'], 2),
                        price_change=round(q['change'], 2),
                        percent_change=round(q['pChange'], 2),
                        market_status=status,
                        vwap=round(q['vwap'], 2),
                        market_cap=round(market_cap, 3)
                    )
                    db.session.add(watchlist_item)
                else:
                    watchlist_item = watchlist.query.filter_by(user_id=user.id, symbol=src).first()
                    if watchlist_item:
                        watchlist_item.last_price = round(q['lastPrice'], 2)
                        watchlist_item.price_change = round(q['change'], 2)
                        watchlist_item.percent_change = round(q['pChange'], 2)
                        watchlist_item.market_status = status
                        watchlist_item.vwap = round(q['vwap'], 2)
                        watchlist_item.market_cap = round(market_cap, 3)
                db.session.commit()
                return redirect(url_for('watchlist.watchList'))
