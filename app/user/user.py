
from flask import Blueprint, render_template, redirect, url_for, flash, session, request
from flask_bcrypt import Bcrypt
import secrets
from flask_sqlalchemy import SQLAlchemy
from sqlalchemy.exc import IntegrityError
# user_bp = Blueprint('user',__name__)
user_bp = Blueprint('user', __name__, template_folder='../../templates', static_folder='../../static')

# user_bp.secret_key =secrets.token_hex(16) 
db=SQLAlchemy()
bcrypt=Bcrypt()
# user.config['MAIL_SERVER'] = 'smtp.gmail.com'
# user.config['MAIL_PORT'] = 587
# user.config['MAIL_USE_TLS'] = True
# user.config['MAIL_USERNAME'] = 's29207488@gmail.com'
# user.config['MAIL_PASSWORD'] = 'hellobgmi'
class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(100), nullable=False)
    email = db.Column(db.String(120), unique=True, nullable=False)
    password = db.Column(db.String(60), nullable=False)
    verified = db.Column(db.Boolean, default=False)
class watchlist(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    user_id = db.Column(db.Integer, db.ForeignKey('user.id'), nullable=False)
    symbol = db.Column(db.String(10), nullable=False)
    last_price = db.Column(db.Float)
    price_change = db.Column(db.Float)
    percent_change = db.Column(db.Float)
    market_status = db.Column(db.String(10))
    vwap = db.Column(db.Float)
    market_cap = db.Column(db.Float)

@user_bp.route('/login',methods=['GET','POST'])
def login():
    def valid_mail(mail):
        if '@' in mail and '.' in mail:
            n, p = mail.split('@', 1)
            d, e = p.split('.', 1)
            return n and p and d and e
        return False
    if request.method =='POST':
        email = request.form.get('email')
        password = request.form.get('password')
        user = User.query.filter_by(email=email).first()
        if user and bcrypt.check_password_hash(user.password, password):
            session['email']=email
            return render_template('User.html')
        else:
            if(not valid_mail(email)):
                error_message='Invalid email-id,Try again.'
            else:
                error_message='Invalid email or password. Please try again'
            return render_template('login.html',error_message=error_message)
    else:
        return render_template('login.html')
@user_bp.route('/register',methods=['GET','POST'])
def register():
    def valid_mail(mail):
        if '@' in mail and '.' in mail:
            n, p = mail.split('@', 1)
            d, e = p.split('.', 1)
            return n and p and d and e
        return False
    error_message = None 
    registration_success_message = None
    if request.method == 'POST':
        name = request.form['name']
        email = request.form['email']
        confirm = request.form['confirm_password'] 
        password = request.form['password']
        if confirm != password: 
            error_message='invalid_password'
            flash('please re-enter password')
           # return redirect(url_for('register'))
        elif(not valid_mail(email)):
            error_message='invalid email-id'
           # return redirect(url_for('register'))
        else:
            hashed_password = bcrypt.generate_password_hash(password).decode('utf-8')
            new_user = User(name=name, email=email, password=hashed_password)
            try:
                db.session.add(new_user)
                db.session.commit()
                registration_success_message = 'Registration successful! A verification email has been sent to your email address. Please check your inbox and click the verification link to activate your account.'
                flash(registration_success_message, 'success')
                return redirect(url_for('user.dashboard'))
            except IntegrityError: 
                db.session.rollback()
                error_message = 'Email is already in use. Please choose a different email.'
    return render_template('register.html', error_message=error_message, registration_success_message=registration_success_message) 
@user_bp.route('/dashboard',methods=['GET','POST'])
def dashboard():
    return render_template('User.html')