from flask import Flask, render_template,Blueprint
from jugaad_data.nse import NSELive
host_bp= Blueprint('host', __name__)
@host_bp.route('/dashboard',methods=['GET','POST'])
def dashboard():
    return render_template('User.html')
@host_bp.route('/host')
def host():
    n=NSELive()
    q=n.live_index('NIFTY 50')
    n=NSELive()
    stocks=sorted(q['data'][1:],key=lambda x:x['pChange'],reverse=True)
    gainers=stocks[:11]
    losers=stocks[len(stocks)-12:]
    return render_template('host.html',gainers=gainers,losers=losers)
