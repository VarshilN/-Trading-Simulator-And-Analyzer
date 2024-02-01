from flask import Flask, render_template, request, redirect, url_for, flash, session,jsonify
# import secrets
import sys
import os
from jugaad_data.nse import NSELive
from user.user import user_bp, User, watchlist, db
from user.stock import stock_bp
from user.config import Config
from user.watchlist import watchlist_bp

from itsdangerous import URLSafeTimedSerializer, SignatureExpired, BadTimeSignature
app = Flask(__name__, template_folder='../templates',static_folder='../static')
app.config.from_object(Config)  
email_verification_serializer = URLSafeTimedSerializer(app.config['SECRET_KEY'])
app.register_blueprint(user_bp)
app.register_blueprint(stock_bp)
app.register_blueprint(watchlist_bp)
db.init_app(app)
with app.app_context():
    db.create_all()
@app.route('/',methods=['POST','GET'])
def home():
    return render_template('HomePage.html',image_path='../static/images/stock@india.jpeg')
@app.route('/host')
def host():
    n=NSELive()
    q=n.live_index('NIFTY 50')
    n=NSELive()
    stocks=sorted(q['data'][1:],key=lambda x:x['pChange'],reverse=True)
    gainers=stocks[:11]
    losers=stocks[len(stocks)-12:]
    return render_template('host.html',gainers=gainers,losers=losers)
if __name__ == '__main__':
    app.run(debug=True)