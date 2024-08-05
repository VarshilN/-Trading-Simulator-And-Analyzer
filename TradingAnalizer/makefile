.PHONY: all

all: $(strategy)

BASIC: basic
DMA: dma
DMA++ : dma++
MACD : macd
RSI : rsi
ADX : adx
LINEAR_REGRESSION : lreg
BEST_OF_ALL : best_of_all
PAIRS : pairs
basic:
	g++ main.cpp momentum_stratagy.cpp mean_reverting_stratagy.cpp linear_regression.cpp file_operations.cpp best_of_all.cpp cashFlow.cpp -o basic
	./basic BASIC $(symbol) $(n) $(x) $(start_date) $(end_date)
	rm basic
	rm -f 'output.csv'
dma:
	g++ main.cpp momentum_stratagy.cpp mean_reverting_stratagy.cpp linear_regression.cpp file_operations.cpp best_of_all.cpp cashFlow.cpp -o dma
	./dma DMA $(symbol) $(n) $(x) $(p) $(start_date) $(end_date)
	rm dma
	rm -f 'output.csv'
dma++:
	g++ main.cpp momentum_stratagy.cpp mean_reverting_stratagy.cpp linear_regression.cpp file_operations.cpp best_of_all.cpp cashFlow.cpp -o dma_p
	./dma_p DMA++ $(symbol) $(x) $(p) $(n) $(max_hold_days) $(c1) $(c2) $(start_date) $(end_date)
	rm dma_p
	rm -f "output.csv"
macd:
	g++ main.cpp momentum_stratagy.cpp mean_reverting_stratagy.cpp linear_regression.cpp file_operations.cpp best_of_all.cpp cashFlow.cpp -o macd
	./macd MACD $(symbol) $(x) $(start_date) $(end_date)
	rm macd
	rm -f 'output.csv'
rsi:
	g++ main.cpp momentum_stratagy.cpp mean_reverting_stratagy.cpp linear_regression.cpp file_operations.cpp best_of_all.cpp cashFlow.cpp -o rsi
	./rsi RSI $(symbol) $(x) $(n) $(oversold_threshold) $(overbought_threshold) $(start_date) $(end_date)
	rm rsi
	rm -f 'output.csv'
adx:
	g++ main.cpp momentum_stratagy.cpp mean_reverting_stratagy.cpp linear_regression.cpp file_operations.cpp best_of_all.cpp cashFlow.cpp -o adx
	./adx ADX $(symbol)  $(x)  $(n) $(adx_threshold) $(start_date) $(end_date)
	rm adx
	rm -f 'output.csv'
best_of_all:
	g++ main.cpp momentum_stratagy.cpp mean_reverting_stratagy.cpp linear_regression.cpp file_operations.cpp best_of_all.cpp cashFlow.cpp -o best_of_all
	./best_of_all BEST_OF_ALL $(symbol) $(start_date) $(end_date)
	rm best_of_all
	rm -f 'output.csv'
	rm -f 'train_output.csv'
lreg:
	g++ main.cpp momentum_stratagy.cpp mean_reverting_stratagy.cpp linear_regression.cpp file_operations.cpp best_of_all.cpp cashFlow.cpp -o linear_regression
	./linear_regression LINEAR_REGRESSION $(symbol) $(x) $(p) $(train_start_date) $(train_end_date) $(start_date) $(end_date)
	rm linear_regression
	rm -f 'output.csv'
	rm -f 'train_output.csv'
pairs:
	if [ "$(stop_loss_threshold)" != "" ]; then\
		g++ main.cpp momentum_stratagy.cpp mean_reverting_stratagy.cpp linear_regression.cpp file_operations.cpp best_of_all.cpp cashFlow.cpp -o pairs2; \
		./pairs2 PAIRS $(symbol1) $(symbol2) $(x) $(n) $(threshold) $(stop_loss_threshold) $(start_date) $(end_date); \
		rm pairs2; \
		rm -f 'output1.csv'; \
		rm -f 'output2.csv'; \
	else \
		g++ main.cpp momentum_stratagy.cpp mean_reverting_stratagy.cpp linear_regression.cpp file_operations.cpp best_of_all.cpp cashFlow.cpp -o pairs1; \
		./pairs1 PAIRS $(symbol1) $(symbol2) $(x) $(n) $(threshold) $(start_date) $(end_date); \
		rm pairs1; \
		rm -f 'output1.csv'; \
		rm -f 'output2.csv'; \
	fi