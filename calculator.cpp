#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "calculator.h"

enum { whwnd, whpro };
enum { testing, optimizing, debuging };
enum { light, medium, hard, unscalp };
enum { donottradecurrentbar=false, tradecurrentbar=true };
#define MODE_TRADES 0
#define MODE_HISTORY 1
#define OP_BUY 0
#define OP_SELL 1
#define OP_BUYLIMIT 2
#define OP_SELLLIMIT 3
#define OP_BUYSTOP 4
#define OP_SELLSTOP 5
#define OP_SL 0
#define OP_TP 1
#define OP_CLOSE 2
#define OP_DELETE 3
#define PRICE_CLOSE 0
#define PRICE_OPEN 1
#define PRICE_HIGH 2
#define PRICE_LOW 3
#define PRICE_MEDIAN 4
#define PRICE_TYPICAL 5
#define PRICE_WEIGHTED 6
#define MODE_MAIN 0
#define MODE_PLUSDI 1
#define MODE_MINUSDI 2
#define OTSKOK 1
#define IMPULS 2

int find(const char *s, const char *key)
{
	int len = strlen(key);
	int len2 = strlen(s);
	bool crc;
	if(len<len2)
	for(int i = 0; i<(len2-len); i++)
	{
		crc=true;
		for(int i1 = 0; i1<len; i1++)
		if(s[i+i1]!=key[i1])crc=false;

		if(crc) return 1;

	}
    return 0;
}
int clen;
char *cbuf;

calculator::calculator(){}
calculator::~calculator(){}
void calculator::wlog(const char *buff)
{
	printf(buff);
}
void calculator::title(int t,const char *buff)
{
	//printf(buff);printf("\n");
}
void calculator::lstrcat(char *buff,const char *past)
{
	int len=0,len0,len1;
	len0=strlen(buff);
	len1=strlen(past);
	memcpy(buff+len0,past,len1);memset(buff+len0+len1,0,1);
}

void calculator::action(int t,int tperiod,int historymode,bool tcurbar,int tbackbar)
{
	srand(time(0));
	kperiod=128;kperiod1=10;
	initrandbytes();initrandbytes();
	testerperiod=tperiod;
	actmode=historymode;
	tradecurbar=tcurbar;
	testerbacktest=tbackbar;
	if(t==testing){mode=testing;test();}
	if(t==optimizing){mode=optimizing;optimize();}
	//if(t==debuging){mode=debuging;debug();}
}
inline char* calculator::intToStr(int i){
	static char strt[18]="";memset(strt,0,18);
	snprintf(strt, 17, "%ld", i);

	return (char *)strt;
}
inline bool calculator::strToInt(const char *s, int *i){
	char *endChar;
	setlocale(LC_NUMERIC, "C");
	int t = strtol(s, &endChar, 10);
	*i=t;
	if(*endChar!='\0')return false;

	return true;
}
inline int calculator::strToInt(const char *s){
	char *endChar;
	setlocale(LC_NUMERIC, "C");
	int t = strtol(s, &endChar, 10);
	if(*endChar!='\0')return -1;

	return t;
}
inline char* calculator::doubleToStr(double d,int precsion) {
	static char strotskok[18]="";
	memset(strotskok,0,18);
	snprintf(strotskok, 12, "%.*f", precsion,d);

	return (char *)strotskok;
}
inline char* calculator::timeToStr(const time_t st) {
	static char strtime1[28]="";
    tm stm1;
    time_t st1;
    st1=time(0);
	memset(strtime1,0,28);
	memset(&stm1,0,sizeof(struct tm));
	if(st==0)
	stm1=*localtime((const time_t *)&st1);
	else
	stm1=*localtime((const time_t *)&st);
	strftime(strtime1,27,"%d.%m.%Y %H:%M:%S ",&stm1);
	return (char *)strtime1;
}
inline char* calculator::gmtimeToStr(const time_t st) {
	static char gmstrtime[28]="";
     tm stm1;
     time_t st1;
    st1=time(0);
	memset(&gmstrtime,0,28);
	memset(&stm1,0,sizeof(struct tm));
	if(st==0)
	stm1=*gmtime((const time_t *)&st1);
	else
	stm1=*gmtime((const time_t *)&st);
	strftime(gmstrtime,27,"%d.%m.%Y %H:%M ",&stm1);
	return (char *)gmstrtime;
}
void calculator::sleep(int delay){
    //DWORD sl1=GetTickCount();
    //while((GetTickCount()-sl1)<delay);
};
inline int calculator::rdtsc(){__asm__ __volatile__("rdtsc");}
inline char* calculator::substr(const char* S, const int pos, const int len){
	int i;
	char *SubS;
	if ( pos+len <= strlen(S)){
		SubS = (char *)malloc(len+1);
		for (i=0;i<len;i++) SubS[i] = S[pos+i];
		SubS[len] = (char) NULL;
	} else SubS = NULL;

	return (char *)SubS;
}
inline int calculator::find(const char *s, const char *key)
{
	int len = strlen(key);
	int len2 = strlen(s);
	if(len<len2)
	for(int i = 0; i<(len2-len); i++)
	{
		bool crc=true;
		for(int i1 = 0; i1<len; i1++)
		if(s[i+i1]!=key[i1])crc=false;

		if(crc) return 1;

	}
    return 0;
}
void calculator::journalsinit(bool init){
	if(!init){
		history = (struct deal *)malloc(sizeof(struct deal)*256);
		trades = (struct deal *)malloc(sizeof(struct deal)*256);
	}
	iTradesTotal=0;
	iTradesTotalOP_BUY=0;
	iTradesTotalOP_SELL=0;
	iTradesTotalOP_BUYLIMIT=0;
	iTradesTotalOP_SELLLIMIT=0;
	iTradesTotalOP_BUYSTOP=0;
	iTradesTotalOP_SELLSTOP=0;
	iHistoryTotal=0;
	iTradesCurrent=0;
	iHistoryCurrent=0;
	minprofit=0;
	maxprofit=0;
	totalprofit=0;
	curorderticket=0;
	totalorders=0;
	drawdowncnt=0;
	profitcnt=0;
	minprofitcntmax=0;
	maxprofitcntmax=0;
	drawdowncntmax=1;
	profitcntmax=0;
	prevorderprofitorno=false;
	lorderscnt=0;
	sorderscnt=0;
}
void calculator::add_row_history(double tOrderPrice, int opclose){
	history[iHistoryTotal].iOrderTicket=trades[iTradesCurrent].iOrderTicket;
	history[iHistoryTotal].iOrderType=trades[iTradesCurrent].iOrderType;
	history[iHistoryTotal].iOpenPrice=trades[iTradesCurrent].iOpenPrice;
	history[iHistoryTotal].iOrderOpenPrice=trades[iTradesCurrent].iOrderOpenPrice;
	history[iHistoryTotal].iOrderClosePrice=tOrderPrice;
	history[iHistoryTotal].iOrderStopLoss=trades[iTradesCurrent].iOrderStopLoss;
	history[iHistoryTotal].iOrderTakeProfit=trades[iTradesCurrent].iOrderTakeProfit;
	history[iHistoryTotal].iOrderOpenTime=trades[iTradesCurrent].iOrderOpenTime;
	history[iHistoryTotal].iOrderCloseTime=testercurdatetime;
	if(trades[iTradesCurrent].iOrderType<=OP_SELL)
	history[iHistoryTotal].iOrderOpclose=opclose;
	if(history[iHistoryTotal].iOrderType==OP_BUY)
	history[iHistoryTotal].iOrderProfit=(int)((history[iHistoryTotal].iOrderClosePrice
					-history[iHistoryTotal].iOrderOpenPrice)*tester2point-testerspread);else
	if(history[iHistoryTotal].iOrderType==OP_SELL)
	history[iHistoryTotal].iOrderProfit=(int)((history[iHistoryTotal].iOrderOpenPrice
					-history[iHistoryTotal].iOrderClosePrice)*tester2point-testerspread);
	if((history[iHistoryTotal].iOrderType==OP_SELL)||(history[iHistoryTotal].iOrderType==OP_BUY)){
		if(history[iHistoryTotal].iOrderProfit<=0){
			minprofit+=history[iHistoryTotal].iOrderProfit;
			drawdowncnt++;
			maxprofitcntmax=0;
			if(!prevorderprofitorno)minprofitcntmax++;
			prevorderprofitorno=false;
			if(drawdowncntmax<minprofitcntmax)drawdowncntmax=minprofitcntmax;
		}else
		if(history[iHistoryTotal].iOrderProfit>0){
			maxprofit+=history[iHistoryTotal].iOrderProfit;
			profitcnt++;
			minprofitcntmax=0;
			if(prevorderprofitorno)maxprofitcntmax++;
			prevorderprofitorno=true;
			if(profitcntmax<maxprofitcntmax)profitcntmax=maxprofitcntmax;
			if(history[iHistoryTotal].iOrderType==OP_BUY)lorderscnt++;else
			if(history[iHistoryTotal].iOrderType==OP_SELL)sorderscnt++;
		}
	}else
	if(opclose==OP_DELETE)history[iHistoryTotal].iOrderProfit=0;
	iHistoryTotal++;
	if((iHistoryTotal&255)==255)
	history = (struct deal *)realloc(history,sizeof(struct deal)*(iHistoryTotal+256));

}
void calculator::add_row_trades(short tCmd,double tPrice,double tStop,double tTake){
	curorderticket++;
	totalorders++;

	if(tCmd==OP_BUY)      iTradesTotalOP_BUY++;else
	if(tCmd==OP_SELL)     iTradesTotalOP_SELL++;else
	if(tCmd==OP_BUYLIMIT) iTradesTotalOP_BUYLIMIT++;else
	if(tCmd==OP_SELLLIMIT)iTradesTotalOP_SELLLIMIT++;else
	if(tCmd==OP_BUYSTOP)  iTradesTotalOP_BUYSTOP++;else
	if(tCmd==OP_SELLSTOP) iTradesTotalOP_SELLSTOP++;

	trades[iTradesTotal].iOrderTicket=curorderticket;
	trades[iTradesTotal].iOrderType=tCmd;
	trades[iTradesTotal].iOrderOpenPrice=tPrice;
	trades[iTradesTotal].iOpenPrice=testercuro;
	trades[iTradesTotal].iOrderStopLoss=tStop;
	trades[iTradesTotal].iOrderTakeProfit=tTake;
	trades[iTradesTotal].iOrderOpclose=OP_DELETE;
	trades[iTradesTotal].iOrderOpenTime=testercurdatetime;
	trades[iTradesTotal].iOrderCloseTime=testercurdatetime;
	iTradesTotal++;
	if((iTradesTotal&255)==255)
	trades = (struct deal *)realloc(trades,sizeof(struct deal)*(iTradesTotal+256));
}
void calculator::del_row_trades(double tOrderPrice, short opclose){
	if(iTradesTotal>0)if(iTradesCurrent>=0)
	{
        short tCmd=trades[iTradesCurrent].iOrderType;
		if(tCmd==OP_BUY)      iTradesTotalOP_BUY--;else
		if(tCmd==OP_SELL)     iTradesTotalOP_SELL--;else
		if(tCmd==OP_BUYLIMIT) iTradesTotalOP_BUYLIMIT--;else
		if(tCmd==OP_SELLLIMIT)iTradesTotalOP_SELLLIMIT--;else
		if(tCmd==OP_BUYSTOP)  iTradesTotalOP_BUYSTOP--;else
		if(tCmd==OP_SELLSTOP) iTradesTotalOP_SELLSTOP--;

		add_row_history(tOrderPrice,opclose);
		for(int i=iTradesCurrent;i<iTradesTotal;i++){
			trades[i].iOrderTicket=trades[i+1].iOrderTicket;
			trades[i].iOrderType=trades[i+1].iOrderType;
			trades[i].iOpenPrice=trades[i+1].iOpenPrice;
			trades[i].iOrderProfit=trades[i+1].iOrderProfit;
			trades[i].iOrderOpenPrice=trades[i+1].iOrderOpenPrice;
			trades[i].iOrderClosePrice=trades[i+1].iOrderClosePrice;
			trades[i].iOrderStopLoss=trades[i+1].iOrderStopLoss;
			trades[i].iOrderTakeProfit=trades[i+1].iOrderTakeProfit;
			trades[i].iOrderOpenTime=trades[i+1].iOrderOpenTime;
			trades[i].iOrderCloseTime=trades[i+1].iOrderCloseTime;
			trades[i].iOrderOpclose=trades[i+1].iOrderOpclose;
		}
		iTradesTotal--;if(iTradesTotal<0)iTradesTotal=0;
		iTradesCurrent--;if(iTradesCurrent<0)iTradesCurrent=0;
		//trades = (struct deal *)realloc(trades,sizeof(struct deal)*(iTradesTotal+2));
	}
}
void calculator::OrderSelect(int tIndex, int selmode){
	if(selmode==MODE_TRADES)
	{
		iTradesCurrent=tIndex;iHistoryCurrent=-1;
		curOrderTicket=trades[tIndex].iOrderTicket;
		curOrderType=trades[tIndex].iOrderType;
		curOrderProfit=trades[tIndex].iOrderProfit;
		curOrderOpenTime=trades[tIndex].iOrderOpenTime;
		curOrderCloseTime=trades[tIndex].iOrderCloseTime;
		curOpenPrice=trades[tIndex].iOpenPrice;
		curOrderOpenPrice=trades[tIndex].iOrderOpenPrice;
		curOrderClosePrice=trades[tIndex].iOrderClosePrice;
		curOrderStopLoss=trades[tIndex].iOrderStopLoss;
		curOrderTakeProfit=trades[tIndex].iOrderTakeProfit;
		curOrderOpclose=trades[tIndex].iOrderOpclose;
	} else
	if(selmode==MODE_HISTORY)
	{
		iHistoryCurrent=tIndex;iTradesCurrent=-1;
		curOrderTicket=history[tIndex].iOrderTicket;
		curOrderType=history[tIndex].iOrderType;
		curOrderProfit=history[tIndex].iOrderProfit;
		curOrderOpenTime=history[tIndex].iOrderOpenTime;
		curOrderCloseTime=history[tIndex].iOrderCloseTime;
		curOpenPrice=history[tIndex].iOpenPrice;
		curOrderOpenPrice=history[tIndex].iOrderOpenPrice;
		curOrderClosePrice=history[tIndex].iOrderClosePrice;
		curOrderStopLoss=history[tIndex].iOrderStopLoss;
		curOrderTakeProfit=history[tIndex].iOrderTakeProfit;
		curOrderOpclose=history[tIndex].iOrderOpclose;
	}
}
void calculator::OrderModify(double tOrderPrice,double tStop,double tTake){
	if(trades[iTradesCurrent].iOrderType<=OP_SELL){
		trades[iTradesCurrent].iOrderStopLoss=tStop;
		trades[iTradesCurrent].iOrderTakeProfit=tTake;
	}
}
void calculator::OrderSend(int cmd, double price, double stop, double take){
	if(cmd==OP_BUY)      add_row_trades(OP_BUY      ,price,stop,take);else
	if(cmd==OP_SELL)     add_row_trades(OP_SELL     ,price,stop,take);else
	if(cmd==OP_BUYLIMIT) add_row_trades(OP_BUYLIMIT ,price,stop,take);else
	if(cmd==OP_SELLLIMIT)add_row_trades(OP_SELLLIMIT,price,stop,take);else
	if(cmd==OP_BUYSTOP)  add_row_trades(OP_BUYSTOP  ,price,stop,take);else
	if(cmd==OP_SELLSTOP) add_row_trades(OP_SELLSTOP ,price,stop,take);
	iOrdersControl();
}
void calculator::OrderClose(int OrderTicket, double tOrderPrice, int opclose){
	if(trades[iTradesCurrent].iOrderTicket==OrderTicket)
	del_row_trades(tOrderPrice,opclose);
}
void calculator::OrderDelete(int OrderTicket){
	short tCmd=trades[iTradesCurrent].iOrderType;
	if((tCmd==OP_BUYLIMIT)||(tCmd==OP_BUYSTOP)||(tCmd==OP_SELLLIMIT)||(tCmd==OP_SELLSTOP))
	if(trades[iTradesCurrent].iOrderTicket==OrderTicket)
	if(((mode!=optimizing)&&(testermetadata->ctm[testercntper-1]!=testercurdatetime))||(mode!=testing))
	del_row_trades(0.0,OP_DELETE);
}
void calculator::iOrdersControl(){
	double price,priceh,pricel;int datetime;
	price=testercurc;
	priceh=testercurh;
	pricel=testercurl;
	datetime=testercurdatetime;
	for(int i=0;i<iTradesTotal;i++){
		OrderSelect(i,MODE_TRADES);
		if(((mode!=optimizing)&&(testermetadata->ctm[testercntper-1]!=testercurdatetime))||(mode!=testing))
		{
			if(trades[i].iOrderType==OP_BUYSTOP){
				if(trades[i].iOrderOpenPrice<=priceh){
	        			iTradesTotalOP_BUYSTOP--;iTradesTotalOP_BUY++;
					history[iHistoryTotal].iOrderTicket=trades[i].iOrderTicket;
					history[iHistoryTotal].iOrderType=trades[i].iOrderType;
					history[iHistoryTotal].iOpenPrice=trades[i].iOpenPrice;
					history[iHistoryTotal].iOrderOpenPrice=trades[i].iOrderOpenPrice;
					history[iHistoryTotal].iOrderClosePrice=0.0;
					history[iHistoryTotal].iOrderStopLoss=trades[i].iOrderStopLoss;
					history[iHistoryTotal].iOrderTakeProfit=trades[i].iOrderTakeProfit;
					history[iHistoryTotal].iOrderOpenTime=trades[i].iOrderOpenTime;
					history[iHistoryTotal].iOrderCloseTime=datetime;
					history[iHistoryTotal].iOrderOpclose=OP_BUY;
					history[iHistoryTotal].iOrderProfit=0;
					iHistoryTotal++;
					if((iHistoryTotal&255)==255)
					history = (struct deal *)realloc(history,sizeof(struct deal)*(iHistoryTotal+256));

					trades[i].iOrderType=OP_BUY;
					trades[i].iOrderOpenTime=testercurdatetime;
					trades[i].iOrderCloseTime=testercurdatetime;
					trades[i].iOrderProfit=(int)((price-trades[i].iOrderOpenPrice)*tester2point-testerspread);
				} else OrderDelete(trades[i].iOrderTicket);
			}else
			if(trades[i].iOrderType==OP_SELLSTOP){
				if(trades[i].iOrderOpenPrice>=pricel){
	        			iTradesTotalOP_SELLSTOP--;iTradesTotalOP_SELL++;
					history[iHistoryTotal].iOrderTicket=trades[i].iOrderTicket;
					history[iHistoryTotal].iOrderType=trades[i].iOrderType;
					history[iHistoryTotal].iOpenPrice=trades[i].iOpenPrice;
					history[iHistoryTotal].iOrderOpenPrice=trades[i].iOrderOpenPrice;
					history[iHistoryTotal].iOrderClosePrice=0.0;
					history[iHistoryTotal].iOrderStopLoss=trades[i].iOrderStopLoss;
					history[iHistoryTotal].iOrderTakeProfit=trades[i].iOrderTakeProfit;
					history[iHistoryTotal].iOrderOpenTime=trades[i].iOrderOpenTime;
					history[iHistoryTotal].iOrderCloseTime=datetime;
					history[iHistoryTotal].iOrderOpclose=OP_SELL;
					history[iHistoryTotal].iOrderProfit=0;
					iHistoryTotal++;
					if((iHistoryTotal&255)==255)
					history = (struct deal *)realloc(history,sizeof(struct deal)*(iHistoryTotal+256));

					trades[i].iOrderType=OP_SELL;
					trades[i].iOrderOpenTime=testercurdatetime;
					trades[i].iOrderCloseTime=testercurdatetime;
	    				trades[i].iOrderProfit=(int)((trades[i].iOrderOpenPrice-price)*tester2point-testerspread);
				} else OrderDelete(trades[i].iOrderTicket);
			}else
			if(trades[i].iOrderType==OP_BUYLIMIT){
				if(trades[i].iOrderOpenPrice>=pricel&&trades[i].iOrderOpenPrice<=priceh){
					iTradesTotalOP_BUYLIMIT--;iTradesTotalOP_BUY++;
					history[iHistoryTotal].iOrderTicket=trades[i].iOrderTicket;
					history[iHistoryTotal].iOrderType=trades[i].iOrderType;
					history[iHistoryTotal].iOpenPrice=trades[i].iOpenPrice;
					history[iHistoryTotal].iOrderOpenPrice=trades[i].iOrderOpenPrice;
					history[iHistoryTotal].iOrderClosePrice=0.0;
					history[iHistoryTotal].iOrderStopLoss=trades[i].iOrderStopLoss;
					history[iHistoryTotal].iOrderTakeProfit=trades[i].iOrderTakeProfit;
					history[iHistoryTotal].iOrderOpenTime=trades[i].iOrderOpenTime;
					history[iHistoryTotal].iOrderCloseTime=datetime;
					history[iHistoryTotal].iOrderOpclose=OP_BUY;
					history[iHistoryTotal].iOrderProfit=0;
					iHistoryTotal++;
					if((iHistoryTotal&255)==255)
					history = (struct deal *)realloc(history,sizeof(struct deal)*(iHistoryTotal+256));

					trades[i].iOrderType=OP_BUY;
					trades[i].iOrderOpenTime=testercurdatetime;
					trades[i].iOrderCloseTime=testercurdatetime;
					trades[i].iOrderProfit=(int)((price-trades[i].iOrderOpenPrice)*tester2point-testerspread);
				} else OrderDelete(trades[i].iOrderTicket);
			}else
			if(trades[i].iOrderType==OP_SELLLIMIT){
				if(trades[i].iOrderOpenPrice<=priceh&&trades[i].iOrderOpenPrice>=pricel){
					iTradesTotalOP_SELLLIMIT--;iTradesTotalOP_SELL++;
					history[iHistoryTotal].iOrderTicket=trades[i].iOrderTicket;
					history[iHistoryTotal].iOrderType=trades[i].iOrderType;
					history[iHistoryTotal].iOpenPrice=trades[i].iOpenPrice;
					history[iHistoryTotal].iOrderOpenPrice=trades[i].iOrderOpenPrice;
					history[iHistoryTotal].iOrderClosePrice=0.0;
					history[iHistoryTotal].iOrderStopLoss=trades[i].iOrderStopLoss;
					history[iHistoryTotal].iOrderTakeProfit=trades[i].iOrderTakeProfit;
					history[iHistoryTotal].iOrderOpenTime=trades[i].iOrderOpenTime;
					history[iHistoryTotal].iOrderCloseTime=datetime;
					history[iHistoryTotal].iOrderOpclose=OP_SELL;
					history[iHistoryTotal].iOrderProfit=0;
					iHistoryTotal++;
					if((iHistoryTotal&255)==255)
					history = (struct deal *)realloc(history,sizeof(struct deal)*(iHistoryTotal+256));

					trades[i].iOrderType=OP_SELL;
					trades[i].iOrderOpenTime=testercurdatetime;
					trades[i].iOrderCloseTime=testercurdatetime;
					trades[i].iOrderProfit=(int)((trades[i].iOrderOpenPrice-price)*tester2point-testerspread);
				} else OrderDelete(trades[i].iOrderTicket);
			}
		}
		if(trades[i].iOrderType==OP_BUY){
			if(trades[i].iOrderStopLoss>=pricel&&trades[i].iOrderStopLoss!=0.0)
				OrderClose(trades[i].iOrderTicket,trades[i].iOrderStopLoss,OP_SL);else
			if(trades[i].iOrderTakeProfit<=priceh&&trades[i].iOrderTakeProfit!=0.0)
				OrderClose(trades[i].iOrderTicket,trades[i].iOrderTakeProfit,OP_TP);else
			OrderClose(trades[i].iOrderTicket,testercurc,OP_CLOSE);
		}else
		if(trades[i].iOrderType==OP_SELL){
			if(trades[i].iOrderStopLoss<=priceh&&trades[i].iOrderStopLoss!=0.0)
				OrderClose(trades[i].iOrderTicket,trades[i].iOrderStopLoss,OP_SL);else
			if(trades[i].iOrderTakeProfit>=pricel&&trades[i].iOrderTakeProfit!=0.0)
				OrderClose(trades[i].iOrderTicket,trades[i].iOrderTakeProfit,OP_TP);else
			OrderClose(trades[i].iOrderTicket,testercurc,OP_CLOSE);
		}
	}
}

void calculator::testerinit()
{
	memset(testervals,0,sizeof(testervals));
	if(actmode==hard){
		strcpy(&testervals[0][0],"EURUSD");
		strcpy(&testervals[1][0],"USDCHF");
		strcpy(&testervals[2][0],"GBPUSD");
		strcpy(&testervals[3][0],"USDJPY");
		strcpy(&testervals[4][0],"AUDUSD");
		strcpy(&testervals[5][0],"USDCAD");
		strcpy(&testervals[6][0],"EURCHF");
		strcpy(&testervals[7][0],"EURGBP");
		strcpy(&testervals[8][0],"EURJPY");
		strcpy(&testervals[9][0],"NZDUSD");
		strcpy(&testervals[10][0],"AUDCAD");
		strcpy(&testervals[11][0],"AUDCHF");
		strcpy(&testervals[12][0],"AUDJPY");
		strcpy(&testervals[13][0],"CADCHF");
		strcpy(&testervals[14][0],"CADJPY");
		strcpy(&testervals[15][0],"CHFJPY");
		strcpy(&testervals[16][0],"NZDCAD");
		strcpy(&testervals[17][0],"NZDCHF");
		strcpy(&testervals[18][0],"NZDJPY");
		strcpy(&testervals[19][0],"EURAUD");
		strcpy(&testervals[20][0],"GBPCHF");
		strcpy(&testervals[21][0],"GBPJPY");
		strcpy(&testervals[22][0],"AUDNZD");
		strcpy(&testervals[23][0],"EURCAD");
		strcpy(&testervals[24][0],"EURNZD");
		strcpy(&testervals[25][0],"GBPAUD");
		strcpy(&testervals[26][0],"GBPCAD");
		strcpy(&testervals[27][0],"GBPNZD");
		strcpy(&testervals[28][0],"USDDKK");
		strcpy(&testervals[29][0],"USDNOK");
		strcpy(&testervals[30][0],"USDSEK");
		strcpy(&testervals[31][0],"USDZAR");
		strcpy(&testervals[32][0],"AUDCZK");
		strcpy(&testervals[33][0],"AUDDKK");
		strcpy(&testervals[34][0],"AUDHKD");
		strcpy(&testervals[35][0],"AUDHUF");
		strcpy(&testervals[36][0],"AUDMXN");
		strcpy(&testervals[37][0],"AUDNOK");
		strcpy(&testervals[38][0],"AUDPLN");
		strcpy(&testervals[39][0],"AUDSEK");
		strcpy(&testervals[40][0],"AUDSGD");
		strcpy(&testervals[41][0],"AUDZAR");
		strcpy(&testervals[42][0],"CADCZK");
		strcpy(&testervals[43][0],"CADDKK");
		strcpy(&testervals[44][0],"CADHKD");
		strcpy(&testervals[45][0],"CADHUF");
		strcpy(&testervals[46][0],"CADMXN");
		strcpy(&testervals[47][0],"CADNOK");
		strcpy(&testervals[48][0],"CADPLN");
		strcpy(&testervals[49][0],"CADSEK");
		strcpy(&testervals[50][0],"CADSGD");
		strcpy(&testervals[51][0],"CADZAR");
		strcpy(&testervals[52][0],"CHFCZK");
		strcpy(&testervals[53][0],"CHFDKK");
		strcpy(&testervals[54][0],"CHFHKD");
		strcpy(&testervals[55][0],"CHFHUF");
		strcpy(&testervals[56][0],"CHFMXN");
		strcpy(&testervals[57][0],"CHFNOK");
		strcpy(&testervals[58][0],"CHFPLN");
		strcpy(&testervals[59][0],"CHFSEK");
		strcpy(&testervals[60][0],"CHFSGD");
		strcpy(&testervals[61][0],"CHFZAR");
		strcpy(&testervals[62][0],"CZKJPY");
		strcpy(&testervals[63][0],"DKKJPY");
		strcpy(&testervals[64][0],"EURCZK");
		strcpy(&testervals[65][0],"EURDKK");
		strcpy(&testervals[66][0],"EURHKD");
		strcpy(&testervals[67][0],"EURHUF");
		strcpy(&testervals[68][0],"EURMXN");
		strcpy(&testervals[69][0],"EURNOK");
		strcpy(&testervals[70][0],"EURPLN");
		strcpy(&testervals[71][0],"EURSEK");
		strcpy(&testervals[72][0],"EURSGD");
		strcpy(&testervals[73][0],"EURZAR");
		strcpy(&testervals[74][0],"GBPCZK");
		strcpy(&testervals[75][0],"GBPDKK");
		strcpy(&testervals[76][0],"GBPHKD");
		strcpy(&testervals[77][0],"GBPHUF");
		strcpy(&testervals[78][0],"GBPMXN");
		strcpy(&testervals[79][0],"GBPNOK");
		strcpy(&testervals[80][0],"GBPPLN");
		strcpy(&testervals[81][0],"GBPSEK");
		strcpy(&testervals[82][0],"GBPSGD");
		strcpy(&testervals[83][0],"GBPZAR");
		strcpy(&testervals[84][0],"HKDJPY");
		strcpy(&testervals[85][0],"MXNJPY");
		strcpy(&testervals[86][0],"NOKJPY");
		strcpy(&testervals[87][0],"NZDCZK");
		strcpy(&testervals[88][0],"NZDDKK");
		strcpy(&testervals[89][0],"NZDHKD");
		strcpy(&testervals[90][0],"NZDHUF");
		strcpy(&testervals[91][0],"NZDMXN");
		strcpy(&testervals[92][0],"NZDNOK");
		strcpy(&testervals[93][0],"NZDPLN");
		strcpy(&testervals[94][0],"NZDSEK");
		strcpy(&testervals[95][0],"NZDSGD");
		strcpy(&testervals[96][0],"NZDZAR");
		strcpy(&testervals[97][0],"SEKJPY");
		strcpy(&testervals[98][0],"SGDJPY");
		strcpy(&testervals[99][0],"USDCZK");
		strcpy(&testervals[100][0],"USDHKD");
		strcpy(&testervals[101][0],"USDHUF");
		strcpy(&testervals[102][0],"USDMXN");
		strcpy(&testervals[103][0],"USDPLN");
		strcpy(&testervals[104][0],"USDSGD");
		strcpy(&testervals[105][0],"ZARJPY");
		strcpy(&testervals[106][0],"GOLD");
		strcpy(&testervals[107][0],"SILVER");
		testervalcnt=108;
	}else
	if(actmode==medium){
	   	strcpy(&testervals[0][0],"EURUSD");
		strcpy(&testervals[1][0],"USDCHF");
		strcpy(&testervals[2][0],"GBPCHF");
		strcpy(&testervals[3][0],"GBPUSD");
		strcpy(&testervals[4][0],"USDJPY");
		strcpy(&testervals[5][0],"USDCAD");
		strcpy(&testervals[6][0],"AUDUSD");
		strcpy(&testervals[7][0],"NZDUSD");
		strcpy(&testervals[8][0],"GBPJPY");
		strcpy(&testervals[9][0],"CHFJPY");
		strcpy(&testervals[10][0],"EURJPY");
		strcpy(&testervals[11][0],"EURGBP");
		strcpy(&testervals[12][0],"GOLD");
		strcpy(&testervals[13][0],"SILVER");
		strcpy(&testervals[14][0],"NZDJPY");
//		strcpy(&testervals[15][0],"USDHKD");
		strcpy(&testervals[15][0],"AUDNZD");
		strcpy(&testervals[16][0],"AUDCAD");
		strcpy(&testervals[17][0],"AUDCHF");
		strcpy(&testervals[18][0],"AUDJPY");
		strcpy(&testervals[19][0],"AUDSGD");
		strcpy(&testervals[20][0],"EURAUD");
		strcpy(&testervals[21][0],"EURCAD");
//		strcpy(&testervals[23][0],"EURCHF");
		strcpy(&testervals[22][0],"EURNZD");
		strcpy(&testervals[23][0],"GBPAUD");
		strcpy(&testervals[24][0],"GBPCAD");
		strcpy(&testervals[25][0],"NZDCAD");
		strcpy(&testervals[26][0],"EURNOK");
		strcpy(&testervals[27][0],"EURSEK");
		strcpy(&testervals[28][0],"USDDKK");
		strcpy(&testervals[29][0],"USDNOK");
		strcpy(&testervals[30][0],"USDSEK");
		strcpy(&testervals[31][0],"USDZAR");
//		strcpy(&testervals[34][0],"USDRUB");
		strcpy(&testervals[32][0],"USDSGD");
		testervalcnt=33;
	}else
	if(actmode==light){
	    strcpy(&testervals[0][0],"EURUSD");
	    strcpy(&testervals[1][0],"AUDUSD");
	    strcpy(&testervals[2][0],"GBPUSD");
	    strcpy(&testervals[3][0],"USDCAD");
	    strcpy(&testervals[4][0],"USDJPY");
	    strcpy(&testervals[5][0],"GBPCHF");
	/*	strcpy(&testervals[1][0],"USDCHF");
		strcpy(&testervals[2][0],"GBPCHF");
		strcpy(&testervals[3][0],"GBPUSD");
		strcpy(&testervals[4][0],"USDJPY");
		strcpy(&testervals[5][0],"USDCAD");
		strcpy(&testervals[6][0],"AUDUSD");
		strcpy(&testervals[7][0],"NZDUSD");
		strcpy(&testervals[8][0],"GBPJPY");
		strcpy(&testervals[9][0],"CHFJPY");
		strcpy(&testervals[10][0],"EURJPY");
		strcpy(&testervals[11][0],"GOLD");//USDSGD
		strcpy(&testervals[12][0],"SILVER");//EURGBP*/
		testervalcnt=6;//13
	}

	memset(testersmas,0,sizeof(testersmas));

	strcpy(&testersmas[0][0],"adxmain");
	strcpy(&testersmas[1][0],"adxup");
	strcpy(&testersmas[2][0],"adxdn");
	strcpy(&testersmas[3][0],"sma");
	strcpy(&testersmas[4][0],"cci");
	strcpy(&testersmas[5][0],"rsi");
	strcpy(&testersmas[6][0],"ichimoku");
	strcpy(&testersmas[7][0],"momentum");
	strcpy(&testersmas[8][0],"ema");
	strcpy(&testersmas[9][0],"bandsup");
	strcpy(&testersmas[10][0],"bandsdn");
	strcpy(&testersmas[11][0],"bears");
	strcpy(&testersmas[12][0],"bulls");
	strcpy(&testersmas[13][0],"wpr");//osma
	strcpy(&testersmas[14][0],"stddev");
	//    strcpy(&testersmas[15][0],"force");
	//    strcpy(&testersmas[16][0],"ac");
	//    strcpy(&testersmas[17][0],"tma");
	//    strcpy(&testersmas[18][0],"zerolagema");
	strcpy(&testersmas[15][0],"volumesma");
	strcpy(&testersmas[16][0],"wpr");
	strcpy(&testersmas[17][0],"demarker");
	strcpy(&testersmas[18][0],"atr");
	strcpy(&testersmas[19][0],"stochastic");
	strcpy(&testersmas[20][0],"adx");
	strcpy(&testersmas[21][0],"gma");
	strcpy(&testersmas[22][0],"stochastic2");
	strcpy(&testersmas[23][0],"demarker2");
	strcpy(&testersmas[24][0],"lssma");
	strcpy(&testersmas[25][0],"lwma");
	//    strcpy(&testersmas[30][0],"sinema");
	strcpy(&testersmas[26][0],"rvi");
	strcpy(&testersmas[27][0],"adx2");
	strcpy(&testersmas[28][0],"rvi2");

	testersmacnt=29;
	testerfxok=false;
	testerdataok=false;
	memset(testerpath,0,sizeof(testerpath));

	char cdir[214];memset(cdir,0,sizeof(cdir));
	if(actmode==hard)lstrcat(testerpath,"hard/");else
	if(actmode==medium)lstrcat(testerpath,"medium/");else
	if(actmode==light)lstrcat(testerpath,"light/");
	
	testermdrawdownclimit=2;
	testercuritem=0;
	memset(strategyset,0,sizeof(strategyset));
	if(actmode==light)lstrcat(strategyset,"MMCIS-Demo");else
	if(actmode==medium)lstrcat(strategyset,"MMCIS-Real");else
	if(actmode==hard)lstrcat(strategyset,"InstaForex-Demo.com");
	lstrcat(strategyset,".");
	lstrcat(strategyset,intToStr(testerperiod));

	if(mode==optimizing){
        testerbacktest=-1;

		int tmp0;tmp0=testervalcnt*testersmacnt;tmp0*=sizeof(struct optimizationvals);

		testeroptval = new optimizationvals[2];
		testeroptval = (optimizationvals*)realloc(testeroptval,tmp0);

		memset(testeroptval,0,tmp0);
    }else{
		testeroptval=0;
	}
	journalsinit(false);
    testermetadata=0;
}
void calculator::testerloaddata()
{
	if(testermetadata!=0){delete[] testermetadata;testermetadata=0;}
	testermetadata = new mdata[1];
	FILE *hFile;
	memset(testermetadata,0,sizeof(struct mdata));
	char fullpath[256],tmp[20];memset(fullpath,0,256);memset(tmp,0,20);
	lstrcat(fullpath,testerpath);
	memset(datfile,0,255);
	lstrcat(datfile,fullpath);
	lstrcat(fullpath,testervals[testercuritem]);
	snprintf(tmp, 20, "%ld", testerperiod);
	lstrcat(fullpath,tmp);
	lstrcat(fullpath,".hst");
	testerdataok=false;

	hFile = fopen(fullpath, "rb");
	int i1=0,testerbacktest2=0;int len;
	if(testerbacktest==-1){testerbacktest=0;testerbacktest2=-1;}
	if(!(!hFile)){
		fseek(hFile,0,SEEK_END);
		int dwFileSize = ftell(hFile);
		
		if(dwFileSize>=148+44){
			testerdataok=true;
			i1=0;
			char* membuf = new char[2];
			//testercntper=2100;
			testercntper=710;
			if(actmode==hard)testercntper=710;
			membuf = (char*)realloc(membuf,(testercntper+3)*44);

			int i=dwFileSize-44*testercntper-44*testerbacktest;
			if((int)((dwFileSize-148)/44)<testercntper-testerbacktest){i=(dwFileSize-148)/44;testercntper=i-testerbacktest;i=148;}
			//if(mode==optimizing)if(((testercntper>>1)<<1)==testercntper)testercntper--;

			fseek(hFile,i,SEEK_SET);
			fread(membuf,44*testercntper,1,hFile);
			i=0;double tstopavg1b,tstopavg1s,tstopavg2b,tstopavg2s;double tmp1b,tmp1s,tmp2b,tmp2s;
			while(i1<testercntper){
				memcpy(&testermetadata->ctm[i1],&membuf[i],4);i+=4;
				memcpy(&testermetadata->open[i1],&membuf[i],8);i+=8;
				memcpy(&testermetadata->low[i1],&membuf[i],8);i+=8;
				memcpy(&testermetadata->high[i1],&membuf[i],8);i+=8;
				memcpy(&testermetadata->close[i1],&membuf[i],8);i+=8;
				memcpy(&testermetadata->volume[i1],&membuf[i],8);i+=8;
				
				tmp1b=0.0;tmp1s=0.0;tmp2b=0.0;tmp2s=0.0;
				if(i1>256)
				for(int p1=0;p1<256;p1++){
				 if(testermetadata->open[i1-p1]<testermetadata->close[i1-p1]){
				 	if(tmp1b==0.0)tmp1b=fabs(testermetadata->open[i1-p1]-testermetadata->close[i1-p1]);
				 	else tmp1b=(tmp1b+fabs(testermetadata->open[i1-p1]-testermetadata->close[i1-p1]))*0.5;
				 }
				 if(testermetadata->open[i1-p1]>testermetadata->close[i1-p1]){
				 	if(tmp1s==0.0)tmp1s=fabs(testermetadata->open[i1-p1]-testermetadata->close[i1-p1]);
				 	else tmp1s=(tmp1s+fabs(testermetadata->open[i1-p1]-testermetadata->close[i1-p1]))*0.5;
				 }
				 if(testermetadata->open[i1-p1]<testermetadata->close[i1-p1]){
				 	if(tmp2b==0.0)tmp2b=(testermetadata->high[i1-p1]-fmin(testermetadata->open[i1-p1],testermetadata->close[i1-p1]) );
				 	else tmp2b=(tmp2b+(testermetadata->high[i1-p1]-fmin(testermetadata->open[i1-p1],testermetadata->close[i1-p1]) ))*0.5;
				 }
				 if(testermetadata->open[i1-p1]>testermetadata->close[i1-p1]){
				 	if(tmp2s==0.0)tmp2s=(  fmax(testermetadata->open[i1-p1],testermetadata->close[i1-p1])-testermetadata->low[i1-p1]);
				 	else tmp2s=(tmp2s+(  fmax(testermetadata->open[i1-p1],testermetadata->close[i1-p1])-testermetadata->low[i1-p1]))*0.5;
				 }
			    }
			    if(tstopavg1b==0.0)tstopavg1b=tmp1b;else tstopavg1b=(tstopavg1b+tmp1b)*0.5;
			    if(tstopavg1s==0.0)tstopavg1s=tmp1s;else tstopavg1s=(tstopavg1s+tmp1s)*0.5;
			    if(tstopavg2b==0.0)tstopavg2b=tmp2b;else tstopavg2b=(tstopavg2b+tmp2b)*0.5;
			    if(tstopavg2s==0.0)tstopavg2s=tmp2s;else tstopavg2s=(tstopavg2s+tmp2s)*0.5;
			    
				i1++;
			}
			fseek(hFile,84,SEEK_SET);
			fread(&testerdigits,4,1,hFile);
			fclose(hFile);
			tester2point=(int)pow((double)10.0,(int)testerdigits);
			testerpoint=1.0;testerpoint/=tester2point;
			testerspread=1;

			tstopavg1b*=tester2point;tstopavg1s*=tester2point;tstopavg2b*=tester2point;tstopavg2s*=tester2point;
			//if(actmode==medium&&testercuritem==12){tstopavg1b*=0.5;tstopavg1s*=0.5;tstopavg2b*=0.5;tstopavg2s*=0.5;}
			
			//if(testerperiod>1440){tstopavg1b*=0.666;tstopavg1s*=0.666;tstopavg2b*=0.666;tstopavg2s*=0.666;}
			//{tstopavg1b*=0.666;tstopavg1s*=0.666;tstopavg2b*=0.666;tstopavg2s*=0.666;}
			stopavg1b=(int)tstopavg1b;stopavg1s=(int)tstopavg1s;stopavg2b=(int)tstopavg2b;stopavg2s=(int)tstopavg2s;
			if(testerbacktest2==-1){
				double tmp;
				tmp=testermetadata->close[i1-1];
				testermetadata->ctm[i1]=(testermetadata->ctm[i1-1]+testerperiod*60);
				testermetadata->open[i1]=tmp;
				testermetadata->close[i1]=tmp;
				testermetadata->high[i1]=tmp;
				testermetadata->low[i1]=tmp;
				testermetadata->volume[i1]=testermetadata->volume[i1-1];
				testercntper++;
				testerbacktest=-1;
			}
			delete []membuf;
		}
		testermincnttrades=(int)(testercntper*0.1);testermincnttrades-=22;
	}
/*int h1=testercntper-1;printf(testervals[testercuritem]);
printf(" ");wlog(gmtimeToStr(testermetadata->ctm[h1]));printf(" %f %f %f %f",testermetadata->open[h1],testermetadata->high[h1],testermetadata->low[h1],testermetadata->close[h1]);
printf("\n");*/
}
void calculator::testersavefx(){
	testerfxok=true;
	FILE *os;os= fopen(strategyset,"wb");
	if (!os){
		wlog("\r\ncan't use .fx file\r\n");
		testerfxok=false;
	}else
	{
		int tmp0;tmp0=sizeof(struct optimizationvals)*testervalcnt*testersmacnt;
		fwrite(testeroptval,tmp0,1,os);
		fclose(os);
		
	}
}
void calculator::testerusefx()
{
    testerfxok=false;
//    if(testeroptval!=0){delete[] testeroptval;testeroptval=0;}

	FILE *is;is= fopen(strategyset,"rb");
	if (!is){
        	if(mode!=optimizing){
			wlog("\r\ncan't use .fx file!\r\n");
			wlog(strategyset);
		}
		testerfxok=false;
	}else
	{
		testerfxok=true;
		int tmp0;tmp0=testervalcnt*testersmacnt;tmp0*=sizeof(struct optimizationvals);
		if(testeroptval!=0){delete[] testeroptval;testeroptval=0;}
		testeroptval = new optimizationvals[2];
		testeroptval = (optimizationvals*)realloc(testeroptval,tmp0);
		fread(testeroptval,tmp0,1,is);
		fclose(is);
	}


}
int calculator::testertest(int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8,int p9)
{
	int t1=333,opt1=1;
	if(mode==testing)t1=testercntper-1;
	int t2=testercntper;
	//if(mode==optimizing){opt1=2;if((getrand()-kperiod)>(kperiod>>1))t1++;}
	for(int i=t1;i<t2;i+=opt1){
		testercurbar=i;
		testercuro=testermetadata->open[i];
		testercurdatetime=testermetadata->ctm[i];
		topen1=testermetadata->open[i];topen=testermetadata->open[i];
		thigh=testermetadata->high[i];if(thigh==0.0){thigh=topen;testermetadata->high[i]=topen;}
		tlow=testermetadata->low[i];if(tlow==0.0){tlow=topen;testermetadata->low[i]=topen;}
		tclose=testermetadata->close[i];if(tclose==0.0){tclose=topen;testermetadata->close[i]=topen;}
		tvolume=testermetadata->volume[i];if(tvolume<=1.0){tvolume=433;testermetadata->volume[i]=433;}
		if((!tradecurbar)&&(i==(t2-1))&&(mode!=optimizing))
		{
			//if(((iHighest(12,1)<iHighest(12,2))&&(optcurbuysell==1||optcurbuysell==3))||((iLowest(12,1)<iLowest(12,2))&&(optcurbuysell==0||optcurbuysell==2)))
			{
				double pf1b,pf1s,pf2b,pf2s,sig1,sig2,sig3,sig4;bool isin=false;
				pf1b=testerpoint;pf1b*=stopavg1b;//pf1b*=21;
				pf1s=testerpoint;pf1s*=stopavg1s;//pf1s*=21;
				pf2b=testerpoint;pf2b*=stopavg2b;//pf2b*=21;
				pf2s=testerpoint;pf2s*=stopavg2s;//pf2s*=21;


				testermetadata->open[i]=topen;
				testercurh=topen;testermetadata->high[i]=topen;
				testercurl=topen;testermetadata->low[i]=topen;
				testercurc=topen;testermetadata->close[i]=topen;


				if((optcurbuysell==0)||(optcurbuysell==2)){testermetadata->close[i]+=pf1b;testermetadata->high[i]+=pf2b;testercurh=testermetadata->high[i];}
				if((optcurbuysell==1)||(optcurbuysell==3)){testermetadata->close[i]-=pf1s;testermetadata->low[i]-=pf2s;testercurl=testermetadata->low[i];}
				testermetadata->volume[i]*=21;
				testercurc=testermetadata->close[i];


			}

		}else
		{
			testercurh=testermetadata->high[i];
			testercurl=testermetadata->low[i];
			testercurc=testermetadata->close[i];
		}
		testerstart(p1,p2,p3,p4,p5,p6,p7,p8,p9);
		testermetadata->open[i]=topen1;
		testermetadata->high[i]=thigh;
		testermetadata->low[i]=tlow;
		testermetadata->close[i]=tclose;
		testermetadata->volume[i]=tvolume;
		testercurh=thigh;
		testercurl=tlow;
		testercurc=tclose;

		if(drawdowncntmax>=2)if(mode==optimizing){return -1;}
	}
	return 0;

}
void calculator::testerstart(int k1,int d1,int k2,int d2,int k3,int d3,int l1,int l2,int limit){

//    if((iTradesTotalOP_BUY!=0)||(iTradesTotalOP_BUYLIMIT!=0)||(iTradesTotalOP_BUYSTOP!=0)||(iTradesTotalOP_SELL!=0)||(iTradesTotalOP_SELLLIMIT!=0)||(iTradesTotalOP_SELLSTOP!=0)){wlog("errors in testerstart\r\n");return;}
	double sig;
	sig=testersignal(k1,d1,k2,d2,k3,d3,l1,l2,limit);
	if(sig!=0.0){
		double SL,TP,tmp;
		if (sig>0.0)
//		if((iTradesTotalOP_BUY==0)&&(iTradesTotalOP_BUYLIMIT==0)&&(iTradesTotalOP_BUYSTOP==0))
		{
			tmp=testerspread;tmp+=4;tmp*=testerpoint;
			SL=0.0;TP=sig;TP+=tmp;
			OrderSend(OP_BUYSTOP, sig,SL,TP);
		}else
		if (sig<0.0)
//		if((iTradesTotalOP_SELL==0)&&(iTradesTotalOP_SELLLIMIT==0)&&(iTradesTotalOP_SELLSTOP==0))
		{
			sig*=-1.0;
			tmp=testerspread;tmp+=4;tmp*=testerpoint;
			SL=0.0;TP=sig;TP-=tmp;
			OrderSend(OP_SELLSTOP, sig,SL,TP);
		}
	}
//	iOrdersControl();
}
int calculator::iLowest(int count, int start){
	double Low=99999999;int cLow=0;
	for(int i = start;i < start+count;i++)
	{
		if(testermetadata->low[testercurbar-i]<Low){
		Low=testermetadata->low[testercurbar-i];cLow=i;}
	}
	return(cLow);
}
int calculator::iHighest(int count, int start){
	double High=-99999999;int cHigh=0;
	for(int i = start;i < start+count;i++)
	{
		if(testermetadata->high[testercurbar-i]>High){
		High=testermetadata->high[testercurbar-i];cHigh=i;}
	}
	return(cHigh);
}
double calculator::testersignal(int k1,int d1,int k2,int d2,int k3,int d3,int l1,int l2,int limit){
	double sig;
	sig=0.0;

	double topen,thigh,tlow,tclose,tvolume,ttemp,ttemp2;
	int avg1;
	
	if(mode==optimizing){
		avg1=4;
		if(testerperiod==1440)avg1=4;else
		if(testerperiod==240)avg1=24;else
		if(testerperiod==60)avg1=96;else
		if(testerperiod==15)avg1=184;
		if((testercntper-avg1)>testercurbar){
			topen=testermetadata->open[testercurbar];
			thigh=testermetadata->high[testercurbar];
			tlow=testermetadata->low[testercurbar];
			tclose=testermetadata->close[testercurbar];
			tvolume=testermetadata->volume[testercurbar];
		    for(int z=1;z<avg1;z++){
				testermetadata->close[testercurbar]=(testermetadata->close[testercurbar]+testermetadata->close[testercurbar+z])*0.5;
				testermetadata->high[testercurbar]=fmax(testermetadata->high[testercurbar],testermetadata->high[testercurbar+z]);
				testermetadata->low[testercurbar]=fmin(testermetadata->low[testercurbar],testermetadata->low[testercurbar+z]);
				testermetadata->volume[testercurbar]=(testermetadata->volume[testercurbar]+testermetadata->volume[testercurbar+z])*0.5;
			}
			switch(optcurbuysell){
				case 0:{testermetadata->close[testercurbar]=(testermetadata->close[testercurbar]+testermetadata->high[testercurbar])*0.5;break;}
				case 1:{testermetadata->close[testercurbar]=(testermetadata->close[testercurbar]+testermetadata->low[testercurbar])*0.5;break;}
				case 2:{testermetadata->close[testercurbar]=(testermetadata->close[testercurbar]+testermetadata->high[testercurbar])*0.5;break;}
				case 3:{testermetadata->close[testercurbar]=(testermetadata->close[testercurbar]+testermetadata->low[testercurbar])*0.5;break;}
			}
		}else
		if(testerbacktest==-1)if((testercntper-1)==testercurbar)
			switch(optcurbuysell){
				case 0:{testermetadata->close[testercurbar]+=(stopavg1b*testerpoint);testermetadata->high[testercurbar]+=(stopavg1b*testerpoint);break;}
				case 1:{testermetadata->close[testercurbar]-=(stopavg1s*testerpoint);testermetadata->low[testercurbar]-=(stopavg1s*testerpoint);break;}
				case 2:{testermetadata->close[testercurbar]+=(stopavg2b*testerpoint);testermetadata->high[testercurbar]+=(stopavg2b*testerpoint);break;}
				case 3:{testermetadata->close[testercurbar]-=(stopavg2s*testerpoint);testermetadata->low[testercurbar]-=(stopavg2s*testerpoint);break;}
			}

	}
	//if(mode!=optimizing){}
	//	l1+=8;l2+=8;
//	int limit;if(optcurbuysell<2)limit=(int)(stddev(10, PRICE_CLOSE, 0)*tester2point);else limit=(int)(atr(20, 0)*tester2point);

	switch(testercursma){
		case 0:sig=iadxmain(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 1:sig=iadxup(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 2:sig=iadxdn(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 3:sig=isma(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 4:sig=icci(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 5:sig=irsi(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 6:sig=iichimoku(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 7:sig=imomentum(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 8:sig=iema(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 9:sig=ibandsup(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 10:sig=ibandsdn(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 11:sig=ibears(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 12:sig=ibulls(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		//case 13:sig=iosma(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 13:sig=iwpr(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;//iosma(k1,d1,k2,d2,k3,d3,l1,l2,limit);
		case 14:sig=istddev(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		//case 15:sig=iforce(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		//case 16:sig=iac(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		//case 17:sig=itma(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		//case 18:sig=izerolagema(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 15:sig=ivolumesma(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;

		case 16:sig=iwpr(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 17:sig=idemarker(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 18:sig=iatr(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 19:sig=istochastic(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 20:sig=iadx(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 21:sig=igma(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 22:sig=istochastic2(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 23:sig=idemarker2(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 24:sig=ilssma(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 25:sig=ilwma(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		//case 30:sig=isinema(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 26:sig=irvi(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 27:sig=iadx2(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		case 28:sig=irvi2(k1,d1,k2,d2,k3,d3,l1,l2,limit);break;
		default:sig=0.0;break;
	}

    if(mode==optimizing)
	if((testercntper-avg1)>testercurbar){
		testermetadata->open[testercurbar]=topen;
		testermetadata->high[testercurbar]=thigh;
		testermetadata->low[testercurbar]=tlow;
		testermetadata->close[testercurbar]=tclose;
		testermetadata->volume[testercurbar]=tvolume;
	}

	//SleepEx(0,true);SleepEx(0,false);
	return sig;
}
double calculator::sma(const int period, const int price, const int shift){
	double sum=0.0,tmp;
	if(price==PRICE_CLOSE){
		for(int i=0;i<period;i++)
		{
			sum+=testermetadata->close[testercurbar-(i+shift)];
		}
		tmp=sum;tmp/=period;
		return(tmp);
	}else
	if(price==PRICE_MEDIAN){
		for(int i=0;i<period;i++)
		{
            tmp=testermetadata->high[testercurbar-(i+shift)];
            tmp+=testermetadata->low[testercurbar-(i+shift)];
            tmp*=0.5;
			sum+=tmp;
		}
		tmp=sum;tmp/=period;
		return(tmp);
	}else
	if(price==PRICE_TYPICAL){
		for(int i=0;i<period;i++)
		{
			tmp=testermetadata->high[testercurbar-(i+shift)];
			tmp+=testermetadata->low[testercurbar-(i+shift)];
			tmp+=testermetadata->close[testercurbar-(i+shift)];
			tmp/=3.0;
			sum+=tmp;
		}
		tmp=sum;tmp/=period;
		return(tmp);
	}
}
double calculator::isma(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=sma(k3,PRICE_CLOSE,l1);
	BS4=sma(d3,PRICE_CLOSE,l1);

	SS1=sma(k1,PRICE_CLOSE,l2+1);
	SS2=sma(d1,PRICE_CLOSE,l2+1);
	SS3=sma(k2,PRICE_CLOSE,l2);
	SS4=sma(d2,PRICE_CLOSE,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::cci(const int period, const int shift ){
	double price,sum,mul,CCIBuffer,RelBuffer,DevBuffer,MovBuffer;
	int k;
	MovBuffer = sma(period, PRICE_TYPICAL, shift);
	mul = 0.015/period;
	sum = 0.0;
	k = period-1;
	while(k>=0)
	{
		price=(testermetadata->high[testercurbar-(k+shift)]+testermetadata->low[testercurbar-(k+shift)]+testermetadata->close[testercurbar-(k+shift)])/3.0;
		sum+=fabs(price-MovBuffer);
		k--;
	}
	DevBuffer = sum*mul;
	price =(testermetadata->high[testercurbar-(shift)]+testermetadata->low[testercurbar-(shift)]+testermetadata->close[testercurbar-(shift)])/3.0;
	RelBuffer=price-MovBuffer;
	if(DevBuffer==0.0)CCIBuffer=0.0;else CCIBuffer = RelBuffer / DevBuffer;
	return(CCIBuffer);
}
double calculator::icci(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=cci(k3,l1);
	BS4=cci(d3,l1);

	SS1=cci(k1,l2+1);
	SS2=cci(d1,l2+1);
	SS3=cci(k2,l2);
	SS4=cci(d2,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::atr(const int period, const int shift){
	double prevclose,AtrBuffer=0.0,tmp;
	for(int i=0;i<period;i++){
		prevclose=testermetadata->close[testercurbar-(shift+1+i)];
		AtrBuffer+=fmax(testermetadata->high[testercurbar-(shift+i)],prevclose)-fmin(testermetadata->low[testercurbar-(shift+i)],prevclose);
	}
	tmp=AtrBuffer;tmp/=period;
	return(tmp);
}
double calculator::iatr(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=atr(k3,l1);
	BS4=atr(d3,l1);

	SS1=atr(k1,l2+1);
	SS2=atr(d1,l2+1);
	SS3=atr(k2,l2);
	SS4=atr(d2,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::rsi(const int period, const int price, const int shift ){
	double rel,negative,positive,sumn=0.0,sump=0.0,PosBuffer,NegBuffer,tmp;
	for(int i=0;i<=period*2;i++){
		rel=testermetadata->close[testercurbar-(shift+i)]-testermetadata->close[testercurbar-(shift+1+i)];
		if(rel>0) sump+=rel;
		else      sumn-=rel;
	}
	PosBuffer=sump/period;
	NegBuffer=sumn/period;
	for(int i=period*2-1;i>=0;i--){
		sumn=0.0;sump=0.0;
		rel=testermetadata->close[testercurbar-(shift+i)]-testermetadata->close[testercurbar-(shift+1+i)];
		if(rel>0) sump=rel;
		else      sumn=-rel;
		PosBuffer=(PosBuffer*(period-1)+sump)/period;
		NegBuffer=(NegBuffer*(period-1)+sumn)/period;
	}
	positive=PosBuffer;
	negative=NegBuffer;
    tmp=100.0-100.0/(1+positive/negative);
	return(tmp);
}
double calculator::irsi(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=rsi(k3,PRICE_CLOSE,l1);
	BS4=rsi(d3,PRICE_CLOSE,l1);

	SS1=rsi(k1,PRICE_CLOSE,l2+1);
	SS2=rsi(d1,PRICE_CLOSE,l2+1);
	SS3=rsi(k2,PRICE_CLOSE,l2);
	SS4=rsi(d2,PRICE_CLOSE,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::ichimoku(const int period, const int shift){
	double high,low,price,Tenkan_Buffer;int k;
	high=testermetadata->high[testercurbar-(shift+1)];
	low=testermetadata->low[testercurbar-(shift+1)];
	k=period-1;
	while(k>=0)
	{
		price=testermetadata->high[testercurbar-(shift+1+k)];
		if(high<price) high=price;
		price=testermetadata->low[testercurbar-(shift+1+k)];
		if(low>price)  low=price;
		k--;
	}
	Tenkan_Buffer=(high+low)*0.5;

	return(Tenkan_Buffer);
}
double calculator::iichimoku(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=ichimoku(k3,l1);
	BS4=ichimoku(d3,l1);

	SS1=ichimoku(k1,l2+1);
	SS2=ichimoku(d1,l2+1);
	SS3=ichimoku(k2,l2);
	SS4=ichimoku(d2,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::momentum(const int period, const int price, const int shift){
	double MomBuffer;MomBuffer=testermetadata->close[testercurbar-(shift+1)]*100/testermetadata->close[testercurbar-(shift+1+period)];

	return(MomBuffer);
}
double calculator::imomentum(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=momentum(k3,PRICE_CLOSE,l1);
	BS4=momentum(d3,PRICE_CLOSE,l1);

	SS1=momentum(k1,PRICE_CLOSE,l2+1);
	SS2=momentum(d1,PRICE_CLOSE,l2+1);
	SS3=momentum(k2,PRICE_CLOSE,l2);
	SS4=momentum(d2,PRICE_CLOSE,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::ema(const int period, const int price, const int shift){
	double pr,ExtMapBuffer;pr=2.0/(period+1);ExtMapBuffer=0.0;
	int    pos=period;
	ExtMapBuffer=testermetadata->close[testercurbar-(shift+pos+1)];
	while(pos>=0)
	{
		ExtMapBuffer=testermetadata->close[testercurbar-(shift+pos)]*pr+ExtMapBuffer*(1-pr);
		pos--;
	}
	return(ExtMapBuffer);

}
double calculator::iema(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=ema(k3,PRICE_CLOSE,l1);
	BS4=ema(d3,PRICE_CLOSE,l1);

	SS1=ema(k1,PRICE_CLOSE,l2+1);
	SS2=ema(d1,PRICE_CLOSE,l2+1);
	SS3=ema(k2,PRICE_CLOSE,l2);
	SS4=ema(d2,PRICE_CLOSE,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::bandsup(const int period, const int price, const int shift){
	double tmp;tmp=sma(period,price,shift)+2*stddev(period,price,shift);
	return(tmp);
}
double calculator::ibandsup(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=bears(k3,PRICE_CLOSE,l1);
	BS4=bears(d3,PRICE_CLOSE,l1);

	SS1=bears(k1,PRICE_CLOSE,l2+1);
	SS2=bears(d1,PRICE_CLOSE,l2+1);
	SS3=bears(k2,PRICE_CLOSE,l2);
	SS4=bears(d2,PRICE_CLOSE,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::bandsdn(const int period, const int price, const int shift){
	double tmp;tmp=sma(period,price,shift)-2*stddev(period,price,shift);
	return(tmp);
}
double calculator::ibandsdn(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=bears(k3,PRICE_CLOSE,l1);
	BS4=bears(d3,PRICE_CLOSE,l1);

	SS1=bears(k1,PRICE_CLOSE,l2+1);
	SS2=bears(d1,PRICE_CLOSE,l2+1);
	SS3=bears(k2,PRICE_CLOSE,l2);
	SS4=bears(d2,PRICE_CLOSE,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::bears(const int period, const int price, const int shift){
	double BearsBuffer;BearsBuffer=testermetadata->low[testercurbar-(shift+1)]-ema(period,price,shift);
	return(BearsBuffer);
}
double calculator::ibears(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=bears(k3,PRICE_CLOSE,l1);
	BS4=bears(d3,PRICE_CLOSE,l1);

	SS1=bears(k1,PRICE_CLOSE,l2+1);
	SS2=bears(d1,PRICE_CLOSE,l2+1);
	SS3=bears(k2,PRICE_CLOSE,l2);
	SS4=bears(d2,PRICE_CLOSE,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::bulls(const int period, const int price, const int shift){
	double BullsBuffer;BullsBuffer=testermetadata->high[testercurbar-(shift+1)]-ema(period,price,shift );
	return(BullsBuffer);
}
double calculator::ibulls(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=bulls(k3,PRICE_CLOSE,l1);
	BS4=bulls(d3,PRICE_CLOSE,l1);

	SS1=bulls(k1,PRICE_CLOSE,l2+1);
	SS2=bulls(d1,PRICE_CLOSE,l2+1);
	SS3=bulls(k2,PRICE_CLOSE,l2);
	SS4=bulls(d2,PRICE_CLOSE,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::osma(const int period, const int price, const int shift ){
	double MacdBuffer,SignalBuffer=0.0,tmp;

	MacdBuffer=sma(period,price,shift);
	MacdBuffer-=sma(20,price,shift);
	for(int i=0;i<period;i++){
		tmp=sma(period,price,shift+i);
		tmp-=sma(20,price,shift+i);
		SignalBuffer+=tmp;
	}
	tmp=SignalBuffer;tmp/=period;tmp=MacdBuffer-tmp;
	return(tmp);
}
double calculator::iosma(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=osma(k3,PRICE_CLOSE,l1);
	BS4=osma(d3,PRICE_CLOSE,l1);

	SS1=osma(k1,PRICE_CLOSE,l2+1);
	SS2=osma(d1,PRICE_CLOSE,l2+1);
	SS3=osma(k2,PRICE_CLOSE,l2);
	SS4=osma(d2,PRICE_CLOSE,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::demarker(const int period, const int price, const int shift){
	double demax=0.0,demin=0.0,tmp;int i1=0,i2=0;
	for(int i=0;i<period;i++){
		if(testermetadata->high[testercurbar-(shift+i+1)]>testermetadata->high[testercurbar-(shift+i)])
			{demax+=(testermetadata->high[testercurbar-(shift+i+1)]-testermetadata->high[testercurbar-(shift+i)]);i1++;}
		if(testermetadata->low[testercurbar-(shift+i+1)]<testermetadata->low[testercurbar-(shift+i)])
			{demin+=(testermetadata->low[testercurbar-(shift+i)]-testermetadata->low[testercurbar-(shift+i+1)]);i2++;}
	}
	demax/=i1;
	demin/=i2;
	tmp=demax/(demax+demin);
	return(tmp);
}
double calculator::idemarker(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=demarker(k3,PRICE_CLOSE,l1);
	BS4=demarker(d3,PRICE_CLOSE,l1);

	SS1=demarker(k1,PRICE_CLOSE,l2+1);
	SS2=demarker(d1,PRICE_CLOSE,l2+1);
	SS3=demarker(k2,PRICE_CLOSE,l2);
	SS4=demarker(d2,PRICE_CLOSE,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::demarker2(const int period, const int price, const int shift){
	double demax=0.0,demin=0.0,tmp;int i1=0,i2=0;
	for(int i=0;i<period;i++){
		if(testermetadata->high[testercurbar-(shift+i+1)]>testermetadata->high[testercurbar-(shift+i)])
			{demax+=GetAppliedPrice(2, testercurbar-(i+1))*(testermetadata->high[testercurbar-(shift+i+1)]-testermetadata->high[testercurbar-(shift+i)]);i1++;}
		if(testermetadata->low[testercurbar-(shift+i+1)]<testermetadata->low[testercurbar-(shift+i)])
			{demin+=GetAppliedPrice(3, testercurbar-(i+1))*(testermetadata->low[testercurbar-(shift+i)]-testermetadata->low[testercurbar-(shift+i+1)]);i2++;}
	}
	demax/=i1;
	demin/=i2;
	tmp=demax/(demax+demin);
	return(tmp);
}
double calculator::idemarker2(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=demarker(k3,PRICE_CLOSE,l1);
	BS4=demarker(d3,PRICE_CLOSE,l1);

	SS1=demarker(k1,PRICE_CLOSE,l2+1);
	SS2=demarker(d1,PRICE_CLOSE,l2+1);
	SS3=demarker(k2,PRICE_CLOSE,l2);
	SS4=demarker(d2,PRICE_CLOSE,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::stddev(const int period, const int price, const int shift)
{
	double std0=0.0,std1=0.0,tmp,tmp1;
	tmp1=sma(period,PRICE_CLOSE,shift);
	for(int i=0;i<period;i++){
        std0=testermetadata->close[testercurbar-(shift+i)]-tmp1;
		std1+=(std0*std0);
	}
	tmp=sqrt(std1/period);
	return(tmp);
}
double calculator::istddev(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il)
{
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=stddev(k3,PRICE_CLOSE,l1);
	BS4=stddev(d3,PRICE_CLOSE,l1);

	SS1=stddev(k1,PRICE_CLOSE,l2+1);
	SS2=stddev(d1,PRICE_CLOSE,l2+1);
	SS3=stddev(k2,PRICE_CLOSE,l2);
	SS4=stddev(d2,PRICE_CLOSE,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::stochastic(const int period, const int price, const int shift){
	double min=999999999.0,max=-999999999.0,k,smin=0.0,smax=0.0,tmp;
	for(int i1=0;i1<9;i1++){
		for(int i=0;i<period;i++){
			if(min>testermetadata->low[testercurbar-(shift+i+i1)])min=testermetadata->low[testercurbar-(shift+i+i1)];
			if(max<testermetadata->high[testercurbar-(shift+i+i1)])max=testermetadata->high[testercurbar-(shift+i+i1)];
		}
		smin+=(testermetadata->close[testercurbar-(shift+1+i1)]-min);
		smax+=(max-min);
	}

	tmp=smin;tmp/=smax;tmp*=100;
	return(tmp);
}
double calculator::istochastic(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=stochastic(k3,PRICE_CLOSE,l1);
	BS4=stochastic(d3,PRICE_CLOSE,l1);

	SS1=stochastic(k1,PRICE_CLOSE,l2+1);
	SS2=stochastic(d1,PRICE_CLOSE,l2+1);
	SS3=stochastic(k2,PRICE_CLOSE,l2);
	SS4=stochastic(d2,PRICE_CLOSE,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::stochastic2(const int period, const int price, const int shift){
	double min=999999999.0,max=-999999999.0,k,smin=0.0,smax=0.0,tmp;
	for(int i1=0;i1<6;i1++){
		for(int i=0;i<period;i++){
			if(min>testermetadata->low[testercurbar-(shift+i+i1)])min=testermetadata->low[testercurbar-(shift+i+i1)];
			if(max<testermetadata->high[testercurbar-(shift+i+i1)])max=testermetadata->high[testercurbar-(shift+i+i1)];
		}
		smin+=(testermetadata->close[testercurbar-(shift+1+i1)]-min);
		smax+=(max-min);
	}

	tmp=smin/smax*100;
	return(tmp);
}
double calculator::istochastic2(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=stochastic2(k3,PRICE_CLOSE,l1);
	BS4=stochastic2(d3,PRICE_CLOSE,l1);

	SS1=stochastic2(k1,PRICE_CLOSE,l2+1);
	SS2=stochastic2(d1,PRICE_CLOSE,l2+1);
	SS3=stochastic2(k2,PRICE_CLOSE,l2);
	SS4=stochastic2(d2,PRICE_CLOSE,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::force(const int period, const int price, const int shift){
	double tmp;
	tmp=ema(period,PRICE_TYPICAL,shift);
	tmp-=ema(period,PRICE_TYPICAL,shift+1);
	tmp*=testermetadata->volume[testercurbar-(shift)];
	return(tmp);
}
double calculator::iforce(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=force(k3,PRICE_CLOSE,l1);
	BS4=force(d3,PRICE_CLOSE,l1);

	SS1=force(k1,PRICE_CLOSE,l2+1);
	SS2=force(d1,PRICE_CLOSE,l2+1);
	SS3=force(k2,PRICE_CLOSE,l2);
	SS4=force(d2,PRICE_CLOSE,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::ac(const int period, const int price, const int shift ){
	double ao=0.0,ao0,ao1,tmp;
	for(int i=0;i<5;i++){
		ao1=(sma(period,price,shift+i)-sma(period+24,price,shift+i));
		if(i==1)ao0=ao1;
		ao+=ao1;
	}
	tmp=ao;tmp/=5.0;tmp=ao0-tmp;
	return(tmp);
}
double calculator::iac(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=ac(k3,PRICE_MEDIAN,l1);
	BS4=ac(d3,PRICE_MEDIAN,l1);

	SS1=ac(k1,PRICE_MEDIAN,l2+1);
	SS2=ac(d1,PRICE_MEDIAN,l2+1);
	SS3=ac(k2,PRICE_MEDIAN,l2);
	SS4=ac(d2,PRICE_MEDIAN,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::adx(const int period, const int price, const int aprice, const int shift ){
	double adx=0.0,tmp,tmp1,tmp2;
	int p1=period;
	double pdi=0.0,mdi=0.0,di;int i1=1,i2=1;
	if(aprice==MODE_PLUSDI||aprice==MODE_MINUSDI)p1=1;
	pdi=0.0;mdi=0.0;i1=1;i2=1;
	for(int i1=0;i1<p1;i1++){
		for(int i=1;i<=period;i++){
			di=testermetadata->close[testercurbar-(shift+i+i1)]-testermetadata->close[testercurbar-(shift+i+1+i1)];
			if(di<0.0){mdi+=di;i1++;}
			if(di>0.0){pdi+=di;i2++;}
		}
		mdi/=-i1;
		pdi/=i2;
		tmp1=pdi;tmp1-=mdi;
		tmp2=pdi;tmp2+=mdi;
		tmp1/=tmp2;
		adx+=tmp1;
	}
	tmp=adx;tmp/=period;
	if(aprice==MODE_PLUSDI)tmp=pdi;
	if(aprice==MODE_MINUSDI)tmp=mdi;
	return(tmp);
}
double calculator::iadx(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=adx(k3,PRICE_MEDIAN,MODE_PLUSDI,l1);
	BS4=adx(d3,PRICE_MEDIAN,MODE_MINUSDI,l1);

	SS1=adx(k1,PRICE_MEDIAN,MODE_PLUSDI,l2+1);
	SS2=adx(d1,PRICE_MEDIAN,MODE_MINUSDI,l2+1);
	SS3=adx(k2,PRICE_MEDIAN,MODE_PLUSDI,l2);
	SS4=adx(d2,PRICE_MEDIAN,MODE_MINUSDI,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::iadxmain(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=adx(k3,PRICE_MEDIAN,MODE_MAIN,l1);
	BS4=adx(d3,PRICE_MEDIAN,MODE_MAIN,l1);

	SS1=adx(k1,PRICE_MEDIAN,MODE_PLUSDI,l2+1);
	SS2=adx(d1,PRICE_MEDIAN,MODE_MINUSDI,l2+1);
	SS3=adx(k2,PRICE_MEDIAN,MODE_PLUSDI,l2);
	SS4=adx(d2,PRICE_MEDIAN,MODE_MINUSDI,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::iadxup(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=adx(k3,PRICE_MEDIAN,MODE_PLUSDI,l1);
	BS4=adx(d3,PRICE_MEDIAN,MODE_PLUSDI,l1);

	SS1=adx(k1,PRICE_MEDIAN,MODE_PLUSDI,l2+1);
	SS2=adx(d1,PRICE_MEDIAN,MODE_PLUSDI,l2+1);
	SS3=adx(k2,PRICE_MEDIAN,MODE_PLUSDI,l2);
	SS4=adx(d2,PRICE_MEDIAN,MODE_PLUSDI,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::iadxdn(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=adx(k3,PRICE_MEDIAN,MODE_MINUSDI,l1);
	BS4=adx(d3,PRICE_MEDIAN,MODE_MINUSDI,l1);

	SS1=adx(k1,PRICE_MEDIAN,MODE_MINUSDI,l2+1);
	SS2=adx(d1,PRICE_MEDIAN,MODE_MINUSDI,l2+1);
	SS3=adx(k2,PRICE_MEDIAN,MODE_MINUSDI,l2);
	SS4=adx(d2,PRICE_MEDIAN,MODE_MINUSDI,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::adx2(const int period, const int price, const int aprice, const int shift ){
	double adx=0.0,tmp,tmp1,tmp2;int p1=period;double pdi=0.0,mdi=0.0,di;int i1=1,i2=1;
	if(aprice==MODE_PLUSDI||aprice==MODE_MINUSDI)p1=1;
	for(int i1=0;i1<p1;i1++){
		pdi=0.0;mdi=0.0;i1=1;i2=1;
		for(int i=1;i<=period;i++){
			di=testermetadata->close[testercurbar-(shift+i+i1)]-testermetadata->close[testercurbar-(shift+i+1+i1)];
			if(di<0.0){mdi+=(di*GetAppliedPrice(3, testercurbar-(i+1)));i1++;}
			if(di>0.0){pdi+=(di*GetAppliedPrice(2, testercurbar-(i+1)));i2++;}
		}
		mdi/=-i1;
		pdi/=i2;
		tmp1=pdi;tmp1-=mdi;
		tmp2=pdi;tmp2+=mdi;
		tmp1/=tmp2;
		adx+=tmp1;
	}
	tmp=adx;tmp/=period;
	if(aprice==MODE_PLUSDI)tmp=pdi;
	if(aprice==MODE_MINUSDI)tmp=mdi;
	return(tmp);
}
double calculator::iadx2(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=adx(k3,PRICE_MEDIAN,MODE_PLUSDI,l1);
	BS4=adx(d3,PRICE_MEDIAN,MODE_MINUSDI,l1);

	SS1=adx(k1,PRICE_MEDIAN,MODE_PLUSDI,l2+1);
	SS2=adx(d1,PRICE_MEDIAN,MODE_MINUSDI,l2+1);
	SS3=adx(k2,PRICE_MEDIAN,MODE_PLUSDI,l2);
	SS4=adx(d2,PRICE_MEDIAN,MODE_MINUSDI,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::gma(const int period, const int price, const int shift){
	double gmean,tmp,tmp1;
	tmp=1.0;tmp/=period;
	gmean = pow(testermetadata->close[testercurbar-(shift+1)],tmp);
	for(int i = 0; i < period;i++) {tmp1=pow(testermetadata->close[testercurbar-(shift+i+1)],tmp);gmean *= tmp1;}

	return(gmean);
}
double calculator::igma(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=gma(k3,PRICE_MEDIAN,l1);
	BS4=gma(d3,PRICE_MEDIAN,l1);

	SS1=gma(k1,PRICE_MEDIAN,l2+1);
	SS2=gma(d1,PRICE_MEDIAN,l2+1);
	SS3=gma(k2,PRICE_MEDIAN,l2);
	SS4=gma(d2,PRICE_MEDIAN,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::tma(const int period, const int price, const int shift){
	int len;len = (int)ceil((period+1)>>1);

	double sum=0.0,tmp;
	for(int i = 0;i < len;i++)
	{
		sum += GetAppliedPrice(PRICE_TYPICAL,testercurbar-(shift+i));//sma(len,PRICE_TYPICAL,shift+i);
	}
	tmp=sum;tmp/=len;
	return(tmp);
}
double calculator::itma(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=tma(k3,PRICE_TYPICAL,l1);
	BS4=tma(d3,PRICE_TYPICAL,l1);

	SS1=tma(k1,PRICE_TYPICAL,l2+1);
	SS2=tma(d1,PRICE_TYPICAL,l2+1);
	SS3=tma(k2,PRICE_TYPICAL,l2);
	SS4=tma(d2,PRICE_TYPICAL,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::sinema(const int period, const int price, const int shift){
	double pi = 3.1415926535;
	double Sum = 0;
	double Weight = 0;
	double swma = 0;
	double sint = 0;
	double tmp;

	for(int i = 0;i < period;i++)
	{
		tmp=i+1;tmp*=pi;tmp/=(period+1);
		sint=sin(tmp);
        Weight+= sint;
        tmp=sint;tmp*=testermetadata->close[testercurbar-(shift+i)];
		Sum += tmp;
	}
	if(Weight>0) {swma = Sum;swma/=Weight;}
	return(swma);
}
double calculator::isinema(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=sinema(k3,PRICE_MEDIAN,l1);
	BS4=sinema(d3,PRICE_MEDIAN,l1);

	SS1=sinema(k1,PRICE_MEDIAN,l2+1);
	SS2=sinema(d1,PRICE_MEDIAN,l2+1);
	SS3=sinema(k2,PRICE_MEDIAN,l2);
	SS4=sinema(d2,PRICE_MEDIAN,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::randma(const int period, const int price, const int shift, const double pi){
	//double pi = 3.1415926535;
	double Sum = 0.0;
	double Weight = 0.0;
	double swma = 0.0;
	double sint = 0.0;

	for(int i = 0;i < period;i++)
	{
        sint=pi*i*GetAppliedPrice(4, testercurbar-(i+1));//rand();//sin(pi*(i+1)/(period+1));
        Weight+= sint;
		Sum += testermetadata->close[testercurbar-(shift+i)]*sint;
	}
	if(Weight>0.0) {swma = Sum;swma/=Weight;}
	return(swma);
}
double calculator::irandma(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;
	double m1,m2,m3,m4,m5,m6;
	m1=100000/k2*d2;
	m2=100000/d2*k1;
	m3=100000/k3*d2;
	m4=100000/d3*k2;
	m5=100000/k1*k3;
	m6=100000/d1*d3;

	BS3=randma(k3,PRICE_MEDIAN,l1   ,m1);
	BS4=randma(d3,PRICE_MEDIAN,l1   ,m2);

	SS1=randma(k1,PRICE_MEDIAN,l2+1 ,m3);
	SS2=randma(d1,PRICE_MEDIAN,l2+1 ,m4);
	SS3=randma(k2,PRICE_MEDIAN,l2   ,m5);
	SS4=randma(d2,PRICE_MEDIAN,l2   ,m6);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::zerolagema(const int period, const int price, const int shift){
	double alfa;alfa = 2.0;alfa/=(1+period);
	int lag;lag = period-1;lag=lag>>1;
	double zema,tmp;

	if(shift == lag) zema = testermetadata->close[testercurbar-(shift+1)];
	else
	if(shift > lag) {
		tmp=testermetadata->close[testercurbar-(shift+1)];tmp*=2.0;tmp-=testermetadata->close[testercurbar-(shift+1+lag)];tmp*=alfa;
		zema = ema(period,PRICE_TYPICAL,shift);
		zema*=(1-alfa);
		zema+=tmp;
	}

	return(zema);
}
double calculator::izerolagema(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=zerolagema(k3,PRICE_MEDIAN,l1);
	BS4=zerolagema(d3,PRICE_MEDIAN,l1);

	SS1=zerolagema(k1,PRICE_MEDIAN,l2+1);
	SS2=zerolagema(d1,PRICE_MEDIAN,l2+1);
	SS3=zerolagema(k2,PRICE_MEDIAN,l2);
	SS4=zerolagema(d2,PRICE_MEDIAN,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::lssma(const int period, const int price, const int shift){
	double Sum=0.0,tmp=0.0;
	for(int i=period; i>=0; i--){tmp=period+1;tmp/=3.0;tmp=i-tmp;tmp*=testermetadata->close[testercurbar-(shift+i)];Sum+=tmp;}
	Sum*=6.0;
	tmp=period+1;tmp*=period;
	double lsma;lsma = Sum;lsma/=tmp;
	return(lsma);
}
double calculator::ilssma(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=lssma(k3,PRICE_MEDIAN,l1);
	BS4=lssma(d3,PRICE_MEDIAN,l1);

	SS1=lssma(k1,PRICE_MEDIAN,l2+1);
	SS2=lssma(d1,PRICE_MEDIAN,l2+1);
	SS3=lssma(k2,PRICE_MEDIAN,l2);
	SS4=lssma(d2,PRICE_MEDIAN,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::lwma(const int period, const int price, const int shift){
	double Sum = 0;
	double Weight = 0;
	double lwma =0;

	for(int i = 0;i < period;i++)
	{
		Weight+= (period - i);
		Sum += testermetadata->close[testercurbar-(shift+i)]*(period - i);
	}
	if(Weight>0) {lwma = Sum;lwma/=Weight;}
	return(lwma);
}
double calculator::ilwma(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=lwma(k3,PRICE_MEDIAN,l1);
	BS4=lwma(d3,PRICE_MEDIAN,l1);

	SS1=lwma(k1,PRICE_MEDIAN,l2+1);
	SS2=lwma(d1,PRICE_MEDIAN,l2+1);
	SS3=lwma(k2,PRICE_MEDIAN,l2);
	SS4=lwma(d2,PRICE_MEDIAN,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::volumesma(const int period, const int price, const int shift){
	double Sum = 0;
	double Weight = 0;
	double vwma = 0;
	double tmp;
	for(int i = 0;i < period;i++)
	{
		tmp=testermetadata->volume[testercurbar-(shift+i)];
		Weight+= tmp;
		tmp=testermetadata->close[testercurbar-(shift+i)];
		tmp*=testermetadata->volume[testercurbar-(shift+i)];
		Sum += tmp;
	}
	if(Weight>0) {vwma = Sum;vwma/=Weight;}

	return(vwma);
}
double calculator::ivolumesma(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=volumesma(k3,PRICE_MEDIAN,l1);
	BS4=volumesma(d3,PRICE_MEDIAN,l1);

	SS1=volumesma(k1,PRICE_MEDIAN,l2+1);
	SS2=volumesma(d1,PRICE_MEDIAN,l2+1);
	SS3=volumesma(k2,PRICE_MEDIAN,l2);
	SS4=volumesma(d2,PRICE_MEDIAN,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::rvi(const int period, const int price, const int shift){
	double rvi = 0.0,tmp,tmp1,tmp2;
	for(int i = 0;i < period;i++)
	{
		tmp1=testermetadata->close[testercurbar-(shift+i)]-testermetadata->open[testercurbar-(shift+i)];
		tmp2=testermetadata->high[testercurbar-(shift+i)]-testermetadata->low[testercurbar-(shift+i)];
		tmp1/=tmp2;
		rvi+= tmp1;
	}
	tmp=rvi;tmp/=period;
	return(tmp);
}
double calculator::irvi(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=rvi(k3,PRICE_MEDIAN,l1);
	BS4=rvi(d3,PRICE_MEDIAN,l1);

	SS1=rvi(k1,PRICE_MEDIAN,l2+1);
	SS2=rvi(d1,PRICE_MEDIAN,l2+1);
	SS3=rvi(k2,PRICE_MEDIAN,l2);
	SS4=rvi(d2,PRICE_MEDIAN,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::rvi2(const int period, const int price, const int shift){
	double rvi = 0.0,tmp,tmp1,tmp2;
	for(int i = 0;i < period;i++)
	{
		tmp1=testermetadata->close[testercurbar-(shift+i)];tmp1-=testermetadata->open[testercurbar-(shift+i)];
		tmp2=testermetadata->high[testercurbar-(shift+i)];tmp2-=testermetadata->low[testercurbar-(shift+i)];
		tmp1/=tmp2;
		rvi+= tmp1;
		tmp1=rvi;tmp1+=GetAppliedPrice(6, testercurbar-(i+1));tmp1*=0.5;
		rvi=tmp1;
	}
	tmp=rvi;tmp/=period;
	return(tmp);
}
double calculator::irvi2(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=rvi(k3,PRICE_MEDIAN,l1);
	BS4=rvi(d3,PRICE_MEDIAN,l1);

	SS1=rvi(k1,PRICE_MEDIAN,l2+1);
	SS2=rvi(d1,PRICE_MEDIAN,l2+1);
	SS3=rvi(k2,PRICE_MEDIAN,l2);
	SS4=rvi(d2,PRICE_MEDIAN,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::wpr(const int period,  const int shift){
	double min=999999999.0,max=-999999999.0,k,r=0.0;
	for(int i=0;i<period;i++){
		if(min>testermetadata->low[testercurbar-(shift+i)])min=testermetadata->low[testercurbar-(shift+i)];
		if(max<testermetadata->high[testercurbar-(shift+i)])max=testermetadata->high[testercurbar-(shift+i)];
	}
	r=(max-testermetadata->close[testercurbar-(shift)]);
	r/=(max-min);r*=-100.0;
	return(r);
}
double calculator::iwpr(const int k1, const int d1, const int k2, const int d2, const int k3, const int d3, int l1, int l2, const int il){
	double BS3,BS4,SS1,SS2,SS3,SS4;

	BS3=wpr(k3,l1);
	BS4=wpr(d3,l1);

	SS1=wpr(k1,l2+1);
	SS2=wpr(d1,l2+1);
	SS3=wpr(k2,l2);
	SS4=wpr(d2,l2);

	double tmp=0.0;
	if(BS3>BS4&&SS1<SS2&&SS3>SS4){tmp=il;tmp*=testerpoint;tmp+=testermetadata->open[testercurbar];}else
	if(BS3<BS4&&SS1>SS2&&SS3<SS4){double tmp1;tmp1=il;tmp1*=testerpoint;tmp=testermetadata->open[testercurbar];tmp-=tmp1;tmp*=-1.0;}
	return(tmp);
}
double calculator::GetAppliedPrice(const int nAppliedPrice, const int nIndex){
	double dPrice;
	switch(nAppliedPrice)
	{
		case 0:  dPrice=testermetadata->close[nIndex];break;
		case 1:  dPrice=testermetadata->open[nIndex];break;
		case 2:  dPrice=testermetadata->high[nIndex];break;
		case 3:  dPrice=testermetadata->low[nIndex];break;
		case 4:  dPrice=(testermetadata->high[nIndex]+testermetadata->low[nIndex]);dPrice*=0.5;break;
		case 5:  dPrice=(testermetadata->high[nIndex]+testermetadata->low[nIndex]+testermetadata->close[nIndex]);dPrice/=3.0;break;
		case 6:  dPrice=(testermetadata->high[nIndex]+testermetadata->low[nIndex]+2*testermetadata->close[nIndex]);dPrice/=4.0;break;
		default: dPrice=0.0;
	}
	return(dPrice);
}
void calculator::test2()
{

	title(whwnd,"test");
	char *tmp=(char*)malloc(11255);
	char *tmp1=(char*)malloc(11255);
	testerinit();

	double temp1,temp2,temp3,temp4;

	wlog("Extremums:\r\n");

	datetimemin=time(NULL);
	datetimemax=0;
	double** sigs;sigs=(double**)malloc(testervalcnt*8);for(testercuritem=0;testercuritem<testervalcnt;testercuritem++)sigs[testercuritem]=(double*)malloc(11*8);
	int ccimin=0,ccimax=0,rsimin=0,rsimax=0,momentummin=0,momentummax=0,demarkermin=0,demarkermax=0,demarker2min=0,demarker2max=0;
	int stochasticmin=0,stochasticmax=0,stochastic2min=0,stochastic2max=0,adxmin=0,adxmax=0,rvimin=0,rvimax=0,rvi2min=0,rvi2max=0;
	int wprmin=0,wprmax=0;
	int period=412,price=PRICE_CLOSE,shift=0;

	for(testercuritem=0;testercuritem<testervalcnt;testercuritem++)
	{
		testerloaddata();
		testercurbar=testercntper-1;
		topen=testermetadata->open[testercurbar];
		double pf1b,pf1s,pf2b,pf2s;
		pf1b=testerpoint;pf1b*=stopavg1b;//pf1b*=21;
		pf1s=testerpoint;pf1s*=stopavg1s;//pf1s*=21;
		pf2b=testerpoint;pf2b*=stopavg2b;//pf2b*=21;
		pf2s=testerpoint;pf2s*=stopavg2s;//pf2s*=21;

		testermetadata->high[testercurbar]=topen;
		testermetadata->low[testercurbar]=topen;
		testermetadata->close[testercurbar]=topen;
		testermetadata->close[testercurbar]+=pf1b;testermetadata->high[testercurbar]+=pf2b;testermetadata->low[testercurbar]-=pf2s;
		testermetadata->volume[testercurbar]*=21;

		if(datetimemax<testermetadata->ctm[testercntper-1])datetimemax=testermetadata->ctm[testercntper-1];
		if(datetimemax!=0)
		if(datetimemax==testermetadata->ctm[testercntper-1])
		if(testerdataok){

			
			sigs[testercuritem][0]=cci(period,shift);if(sigs[testercuritem][0]>sigs[ccimax][0])ccimax=testercuritem;
			sigs[testercuritem][1]=rsi(period,price,shift);if(sigs[testercuritem][1]>sigs[rsimax][1])rsimax=testercuritem;
			sigs[testercuritem][2]=momentum(period,price,shift);if(sigs[testercuritem][2]>sigs[momentummax][2])momentummax=testercuritem;
			sigs[testercuritem][3]=demarker(period,price,shift);if(sigs[testercuritem][3]>sigs[demarkermax][3])demarkermax=testercuritem;
			sigs[testercuritem][4]=demarker2(period,price,shift);if(sigs[testercuritem][4]>sigs[demarker2max][4])demarker2max=testercuritem;
			sigs[testercuritem][5]=stochastic(period,price,shift);if(sigs[testercuritem][5]>sigs[stochasticmax][5])stochasticmax=testercuritem;
			sigs[testercuritem][6]=stochastic2(period,price,shift);if(sigs[testercuritem][6]>sigs[stochastic2max][6])stochastic2max=testercuritem;
			sigs[testercuritem][7]=adx(period,price,MODE_MAIN,shift);if(sigs[testercuritem][7]>sigs[adxmax][7])adxmax=testercuritem;
			sigs[testercuritem][8]=rvi(period,price,shift);if(sigs[testercuritem][8]>sigs[rvimax][8])rvimax=testercuritem;
			sigs[testercuritem][9]=rvi2(period,price,shift);if(sigs[testercuritem][9]>sigs[rvi2max][9])rvi2max=testercuritem;
			sigs[testercuritem][10]=wpr(period,shift);if(sigs[testercuritem][10]>sigs[wprmax][10])wprmax=testercuritem;

		}
	}
	for(testercuritem=0;testercuritem<testervalcnt;testercuritem++)
	{
		testerloaddata();
		testercurbar=testercntper-1;
		topen=testermetadata->open[testercurbar];
		double pf1b,pf1s,pf2b,pf2s;
		pf1b=testerpoint;pf1b*=stopavg1b;//pf1b*=21;
		pf1s=testerpoint;pf1s*=stopavg1s;//pf1s*=21;
		pf2b=testerpoint;pf2b*=stopavg2b;//pf2b*=21;
		pf2s=testerpoint;pf2s*=stopavg2s;//pf2s*=21;

		testermetadata->high[testercurbar]=topen;
		testermetadata->low[testercurbar]=topen;
		testermetadata->close[testercurbar]=topen;
		testermetadata->close[testercurbar]-=pf1s;testermetadata->low[testercurbar]-=pf2s;testermetadata->high[testercurbar]+=pf2b;
		testermetadata->volume[testercurbar]*=21;

		if(datetimemax<testermetadata->ctm[testercntper-1])datetimemax=testermetadata->ctm[testercntper-1];
		if(datetimemax!=0)
		if(datetimemax==testermetadata->ctm[testercntper-1])
		if(testerdataok){

			sigs[testercuritem][0]=cci(period,shift);if(sigs[testercuritem][0]<sigs[ccimin][0])ccimin=testercuritem;
			sigs[testercuritem][1]=rsi(period,price,shift);if(sigs[testercuritem][1]<sigs[rsimin][1])rsimin=testercuritem;
			sigs[testercuritem][2]=momentum(period,price,shift);if(sigs[testercuritem][2]<sigs[momentummin][2])momentummin=testercuritem;
			sigs[testercuritem][3]=demarker(period,price,shift);if(sigs[testercuritem][3]<sigs[demarkermin][3])demarkermin=testercuritem;
			sigs[testercuritem][4]=demarker2(period,price,shift);if(sigs[testercuritem][4]<sigs[demarker2min][4])demarker2min=testercuritem;
			sigs[testercuritem][5]=stochastic(period,price,shift);if(sigs[testercuritem][5]<sigs[stochasticmin][5])stochasticmin=testercuritem;
			sigs[testercuritem][6]=stochastic2(period,price,shift);if(sigs[testercuritem][6]<sigs[stochastic2min][6])stochastic2min=testercuritem;
			sigs[testercuritem][7]=adx(period,price,MODE_MAIN,shift);if(sigs[testercuritem][7]<sigs[adxmin][7])adxmin=testercuritem;
			sigs[testercuritem][8]=rvi(period,price,shift);if(sigs[testercuritem][8]<sigs[rvimin][8])rvimin=testercuritem;
			sigs[testercuritem][9]=rvi2(period,price,shift);if(sigs[testercuritem][9]<sigs[rvi2min][9])rvi2min=testercuritem;
			sigs[testercuritem][10]=wpr(period,shift);if(sigs[testercuritem][10]<sigs[wprmin][10])wprmin=testercuritem;

		}
	}
	//if(datetimemin!=0){wlog(timeToStr(datetimemin));wlog(" - ");wlog(timeToStr(datetimemax));wlog("\r\n");}
	memset(tmp1,0,sizeof(tmp1));

	for(testercuritem=0;testercuritem<testervalcnt;testercuritem++)
	if(datetimemax!=0)
	if(datetimemax==testermetadata->ctm[testercntper-1])
	{
		memset(tmp,0,sizeof(tmp));
		lstrcat(tmp,testervals[testercuritem]);
		if(testercuritem==ccimin)lstrcat(tmp," ccimin ");if(testercuritem==ccimax)lstrcat(tmp," ccimax ");
		if(testercuritem==rsimin)lstrcat(tmp," rsimin ");if(testercuritem==rsimax)lstrcat(tmp," rsimax ");
		if(testercuritem==momentummin)lstrcat(tmp," momentummin ");if(testercuritem==momentummax)lstrcat(tmp," momentummax ");
		if(testercuritem==demarkermin)lstrcat(tmp," demarkermin ");if(testercuritem==demarkermax)lstrcat(tmp," demarkermax ");
		if(testercuritem==demarker2min)lstrcat(tmp," demarker2min ");if(testercuritem==demarker2max)lstrcat(tmp," demarker2max ");
		if(testercuritem==stochasticmin)lstrcat(tmp," stochasticmin ");if(testercuritem==stochasticmax)lstrcat(tmp," stochasticmax ");
		if(testercuritem==stochastic2min)lstrcat(tmp," stochastic2min ");if(testercuritem==stochastic2max)lstrcat(tmp," stochastic2max ");
		if(testercuritem==adxmin)lstrcat(tmp," adxmin ");if(testercuritem==adxmax)lstrcat(tmp," adxmax ");
		if(testercuritem==rvimin)lstrcat(tmp," rvimin ");if(testercuritem==rvimax)lstrcat(tmp," rvimax ");
		if(testercuritem==rvi2min)lstrcat(tmp," rvi2min ");if(testercuritem==rvi2max)lstrcat(tmp," rvi2max ");
		if(testercuritem==wprmin)lstrcat(tmp," wprmin ");if(testercuritem==wprmax)lstrcat(tmp," wprmax ");
		lstrcat(tmp,"\r\n");
		if(strlen(tmp)<12)memset(tmp,0,sizeof(tmp));
		lstrcat(tmp1,tmp);
		
	}
	lstrcat(tmp1,":::::::::::::::::::::::::::::\r\n");
	wlog(tmp1);
	
	for(testercuritem=0;testercuritem<testervalcnt;testercuritem++)free(sigs[testercuritem]);free(sigs);
	free(tmp);
}



void calculator::test()
{

	title(whwnd,"test");

	char *tmp=(char*)malloc(11255);
	char *buf1=(char*)malloc(21130);
	tm tml;
	memset(buf1,0,21130);
	char* buf2;buf2=(char*)malloc(21140);memset(buf2,0,21140);
	tm tfrom,tto,topt;
	testerinit();
	unsorted = new consolidatesorted[testervalcnt+2];
	memset(unsorted,0,sizeof(struct consolidatesorted)*(testervalcnt+2));

	testerusefx();double temp1,temp2,temp3,temp4;
	if(testerfxok){
		time_t t0=0;
		datetimemin=time(NULL);
		datetimemax=0;
		for(testercuritem=0;testercuritem<testervalcnt;testercuritem++)
			for(testercursma=0;testercursma<testersmacnt;testercursma++)
				for(int ls=0;ls<4;ls++)if(testeroptval[testercuritem*testersmacnt+testercursma].params[ls].datetimeopt>t0)t0=testeroptval[testercuritem*testersmacnt+testercursma].params[ls].datetimeopt;
		tml = *gmtime((const time_t*)&t0);
		memset(buf1,0,sizeof(buf1));
		strftime(buf1,25,"%d.%m",&tml);
		lstrcat(tmp,buf1);
		if(actmode==light)lstrcat(tmp,"# MMCIS-Demo ");else
		if(actmode==medium)lstrcat(tmp,"# MMCIS-Real ");else
		if(actmode==hard)lstrcat(tmp,"# InstaForex-Demo.com ");
		lstrcat(tmp,"test ");
		lstrcat(tmp,intToStr(testerperiod));
		lstrcat(tmp,"\r\n");
		wlog(tmp);

		datetimemin=time(NULL);
		datetimemax=0;

		for(testercuritem=0;testercuritem<testervalcnt;testercuritem++)
		//for(testercuritem=0;testercuritem<1;testercuritem++)
		{
			testerloaddata();
			if(datetimemax<testermetadata->ctm[testercntper-1])datetimemax=testermetadata->ctm[testercntper-1];
			//if(datetimemax==testermetadata->ctm[testercntper-1])
			if(testerdataok){
				double midSELLSTOP=0.0;double midBUYSTOP=0.0;int cntSELLSTOP=0;int cntBUYSTOP=0;int cntSELLSTOP2=0;int cntBUYSTOP2=0;
				unsorted[testercuritem].cntSELLSTOP=cntSELLSTOP;unsorted[testercuritem].cntBUYSTOP=cntBUYSTOP;
				unsorted[testercuritem].midSELLSTOP=midSELLSTOP;unsorted[testercuritem].midBUYSTOP=midBUYSTOP;
				unsorted[testercuritem].powerBUYSTOP=0;unsorted[testercuritem].powerSELLSTOP=0;
				int tstopavg1b=0,tstopavg2b=0,tstopavg1s=0,tstopavg2s=0;
				int cntSELLSTOPtotal=0;int cntBUYSTOPtotal=0;int cntSELLSTOP2total=0;int cntBUYSTOP2total=0;
				for(testercursma=0;testercursma<testersmacnt;testercursma++)
				for(int ls=0;ls<4;ls++){
					int testercuritemptr=testercuritem*testersmacnt+testercursma;
					if(testeroptval[testercuritemptr].params[ls].datetimeopt!=0){
//						if(ls==0||ls==2){cntBUYSTOPtotal++;if(ls==2)cntBUYSTOP2total++;}else if(ls==1||ls==3){cntSELLSTOPtotal++;if(ls==3)cntSELLSTOP2total++;}
						if(ls==0)cntBUYSTOPtotal++;
						if(ls==2)cntBUYSTOP2total++;
						if(ls==1)cntSELLSTOPtotal++;
						if(ls==3)cntSELLSTOP2total++;
						switch(ls){
							case 0:{if(tstopavg1b==0)tstopavg1b=testeroptval[testercuritemptr].params[ls].param7;else tstopavg1b=(tstopavg1b+testeroptval[testercuritemptr].params[ls].param7)>>1;break;}
							case 1:{if(tstopavg1s==0)tstopavg1s=testeroptval[testercuritemptr].params[ls].param7;else tstopavg1s=(tstopavg1s+testeroptval[testercuritemptr].params[ls].param7)>>1;break;}
							case 2:{if(tstopavg2b==0)tstopavg2b=testeroptval[testercuritemptr].params[ls].param7;else tstopavg2b=(tstopavg2b+testeroptval[testercuritemptr].params[ls].param7)>>1;break;}
							case 3:{if(tstopavg2s==0)tstopavg2s=testeroptval[testercuritemptr].params[ls].param7;else tstopavg2s=(tstopavg2s+testeroptval[testercuritemptr].params[ls].param7)>>1;break;}
						}
					}
					stopavg1b=tstopavg1b;
					stopavg2b=tstopavg2b;
					stopavg1s=tstopavg1s;
					stopavg2s=tstopavg2s;
				}

				for(testercursma=0;testercursma<testersmacnt;testercursma++)
				//for(testercursma=23;testercursma<24;testercursma++)
				for(int ls=0;ls<4;ls++){
					int testercuritemptr=testercuritem*testersmacnt+testercursma;
					if(testeroptval[testercuritemptr].params[ls].datetimeopt!=0)
					{
						memset(tmp,0,11255);
						if(ls==0||ls==2)optcurbuysell=1;else optcurbuysell=-1;
						lstrcat(tmp,testervals[testercuritem]);lstrcat(tmp," ");
						lstrcat(tmp,testersmas[testercursma]);
					    switch(ls){
							case 0:{lstrcat(tmp," buy 1..");break;}//p[6]=stopavg1b;
							case 1:{lstrcat(tmp," sell 1..");break;}//p[6]=stopavg1s;
							case 2:{lstrcat(tmp," buy 2..");break;}//p[6]=stopavg2b;
							case 3:{lstrcat(tmp," sell 2..");break;}//p[6]=stopavg2s;
						}
						title(whpro,tmp);
						int p1,p2,p3,p4,p5,p6,p7,p8;
						p1=testeroptval[testercuritemptr].params[ls].param1;
						p2=testeroptval[testercuritemptr].params[ls].param2;
						p3=testeroptval[testercuritemptr].params[ls].param3;
						p4=testeroptval[testercuritemptr].params[ls].param4;
						p5=testeroptval[testercuritemptr].params[ls].param5;
						p6=testeroptval[testercuritemptr].params[ls].param6;
						p7=testeroptval[testercuritemptr].params[ls].param7;

						journalsinit();
						testertest(p1,p2,p3,p4,p5,p6,0,0,p7);

						int jmode=0,jcnt=0;
						if(iTradesTotal>0){jmode=MODE_TRADES;jcnt=iTradesTotal;}else if(iHistoryTotal>0){jmode=MODE_HISTORY;jcnt=iHistoryTotal;}
						for(int i=0;i<jcnt;i++){
							OrderSelect(i, jmode);
							if(testermetadata->ctm[testercntper-1]==curOrderOpenTime){
								if(ls==1||ls==3)if(curOrderType==OP_SELLSTOP){if(ls==3)cntSELLSTOP2++;if(ls==1)cntSELLSTOP++;if(midSELLSTOP==0.0)midSELLSTOP=curOrderOpenPrice;else {midSELLSTOP=(midSELLSTOP+curOrderOpenPrice)*0.5;if(ls==3)midSELLSTOP=(midSELLSTOP+curOrderOpenPrice)*0.5;} }
								if(ls==0||ls==2)if(curOrderType==OP_BUYSTOP){if(ls==2)cntBUYSTOP2++;if(ls==0)cntBUYSTOP++;if(midBUYSTOP==0.0)midBUYSTOP=curOrderOpenPrice;else {midBUYSTOP=(midBUYSTOP+curOrderOpenPrice)*0.5;if(ls==2)midBUYSTOP=(midBUYSTOP+curOrderOpenPrice)*0.5;} }
							}

						}

					}
			    }
				if(datetimemin>testermetadata->ctm[testercntper-1]&&testermetadata->ctm[testercntper-1]!=0)datetimemin=testermetadata->ctm[testercntper-1];
				if(datetimemax<testermetadata->ctm[testercntper-1]&&testermetadata->ctm[testercntper-1]!=0)datetimemax=testermetadata->ctm[testercntper-1];
				void* v=unsorted[testercuritem].val;
				memset(v,0,22);
				memcpy(v,&testervals[testercuritem],21);
				unsorted[testercuritem].digits=testerdigits;
				unsorted[testercuritem].priceopen=testercuro;
				unsorted[testercuritem].priceclose=testercurc;
				unsorted[testercuritem].pricehigh=testercurh;
				unsorted[testercuritem].pricelow=testercurl;
				unsorted[testercuritem].datetime=testermetadata->ctm[testercntper-1];
				unsorted[testercuritem].midSELLSTOP=midSELLSTOP;
				unsorted[testercuritem].cntSELLSTOP=cntSELLSTOP;
				unsorted[testercuritem].cntSELLSTOPtotal=cntSELLSTOPtotal;
				unsorted[testercuritem].cntSELLSTOP2total=cntSELLSTOP2total;
				unsorted[testercuritem].midBUYSTOP=midBUYSTOP;
				unsorted[testercuritem].cntBUYSTOP=cntBUYSTOP;
				unsorted[testercuritem].cntBUYSTOPtotal=cntBUYSTOPtotal;
				unsorted[testercuritem].cntBUYSTOP2total=cntBUYSTOP2total;
				unsorted[testercuritem].cntSELLSTOP2=cntSELLSTOP2;
				unsorted[testercuritem].cntBUYSTOP2=cntBUYSTOP2;
				double tmp1,tmp2;
				tmp1=tstopavg2b-tstopavg1b;
				double powerBUYSTOP=midBUYSTOP;powerBUYSTOP-=(tstopavg1b*testerpoint+testercuro);powerBUYSTOP*=tester2point;powerBUYSTOP/=tmp1;powerBUYSTOP*=100;
				tmp1=tstopavg2s-tstopavg1s;
				double powerSELLSTOP=midSELLSTOP;powerSELLSTOP=(testercuro-tstopavg1s*testerpoint)-powerSELLSTOP;powerSELLSTOP*=tester2point;powerSELLSTOP/=tmp1;powerSELLSTOP*=100;
				tmp2=fabs(powerBUYSTOP-powerSELLSTOP);
				//if((int)tmp2>0&&fmin(powerBUYSTOP,powerSELLSTOP)>0)
				{
					unsorted[testercuritem].powerBUYSTOP=powerBUYSTOP;unsorted[testercuritem].powerSELLSTOP=powerSELLSTOP;
					unsorted[testercuritem].profitBUYSTOP=testercurc-testercuro;unsorted[testercuritem].profitBUYSTOP*=tester2point;
					unsorted[testercuritem].profitSELLSTOP=testercuro-testercurc;unsorted[testercuritem].profitSELLSTOP*=tester2point;
					unsorted[testercuritem].profitBUYSTOP2=testercuro-testercurl;unsorted[testercuritem].profitBUYSTOP2*=tester2point;
					unsorted[testercuritem].profitSELLSTOP2=testercurh-testercuro;unsorted[testercuritem].profitSELLSTOP2*=tester2point;
				}

			}
   		}
	}
	if(datetimemin!=0){lstrcat(buf2,timeToStr(datetimemin));lstrcat(buf2," - ");lstrcat(buf2,timeToStr(datetimemax));lstrcat(buf2,"\r\n");}

	int sort1[255],sort2[255],sort1cnt=0,sort2cnt=0;for(int x1=0;x1<255;x1++){sort1[x1]=0;sort2[x1]=0;}
	for(testercuritem=0;testercuritem<testervalcnt;testercuritem++)
//		if((unsorted[testercuritem].midBUYSTOP!=0.0)&&(unsorted[testercuritem].midSELLSTOP!=0.0))
		if(unsorted[testercuritem].cntBUYSTOP2!=0.0||unsorted[testercuritem].cntSELLSTOP2!=0.0)
		{
				if(unsorted[testercuritem].cntBUYSTOP2>(1.8*unsorted[testercuritem].cntBUYSTOP))
				{
				if(unsorted[testercuritem].cntBUYSTOP2>0.0&&unsorted[testercuritem].cntBUYSTOP>0.0)
                //if(unsorted[testercuritem].powerBUYSTOP>(1.1*unsorted[testercuritem].powerSELLSTOP))
                //if(unsorted[testercuritem].powerBUYSTOP>0.0&&unsorted[testercuritem].powerSELLSTOP>0.0)
				{
					bool issort=false;
                    //if(sort1cnt==0){sort1[sort1cnt]=testercuritem;sort1cnt++;issort=true;}else
					for(int i=0;i<=sort1cnt;i++)
					if(unsorted[testercuritem].cntBUYSTOP2>unsorted[sort1[i]].cntBUYSTOP2){
						for(int i1=sort1cnt;i1>i;i1--)sort1[i1]=sort1[i1-1];
						sort1[i]=testercuritem;
						sort1cnt++;i=sort1cnt;
						issort=true;
					}
					if(!issort){sort1[sort1cnt]=testercuritem;sort1cnt++;}
				}}else
				if(unsorted[testercuritem].cntSELLSTOP2>(1.8*unsorted[testercuritem].cntSELLSTOP))
				{
					if(unsorted[testercuritem].cntSELLSTOP2>0.0&&unsorted[testercuritem].cntSELLSTOP>0.0)
					//if(unsorted[testercuritem].powerSELLSTOP>(1.1*unsorted[testercuritem].powerBUYSTOP))
					//if(unsorted[testercuritem].powerBUYSTOP>0.0&&unsorted[testercuritem].powerSELLSTOP>0.0)
					{
						bool issort=false;
						//if(sort2cnt==0){sort2[sort2cnt]=testercuritem;sort2cnt++;issort=true;}else
						for(int i=0;i<=sort2cnt;i++)
						if(unsorted[testercuritem].cntSELLSTOP2>unsorted[sort2[i]].cntSELLSTOP2){
							for(int i1=sort2cnt;i1>i;i1--)sort2[i1]=sort2[i1-1];
							sort2[i]=testercuritem;
							sort2cnt++;i=sort2cnt;
							issort=true;
						}
						if(!issort){sort2[sort2cnt]=testercuritem;sort2cnt++;}
					}
				}
		}
	//lstrcat(buf2,"\r\n===========================\r\n\r\n");
	int mx=(int)fmax(sort1cnt,sort2cnt);int correct=0,incorrect=0;
	for(int x2=0;x2<mx;x2++)
	{
			bool col1=false,col2=false;double tmp1,tmp2,tmp3,tmp4,tmp5,tmp6;
			testercuritem=sort1[x2];
			tmp1=(unsorted[testercuritem].cntBUYSTOP2);tmp1=(tmp1/unsorted[testercuritem].cntBUYSTOP2total);tmp1*=100;
			tmp2=(unsorted[testercuritem].cntSELLSTOP2);tmp2=(tmp2/unsorted[testercuritem].cntSELLSTOP2total);tmp2*=100;
			tmp3=tmp1-tmp2;tmp4=tmp2-tmp1;
			tmp1=(unsorted[testercuritem].cntBUYSTOP);tmp1=(tmp1/unsorted[testercuritem].cntBUYSTOPtotal);tmp1*=100;
			tmp2=(unsorted[testercuritem].cntSELLSTOP);tmp2=(tmp2/unsorted[testercuritem].cntSELLSTOPtotal);tmp2*=100;
			tmp5=tmp1-tmp2;tmp6=tmp2-tmp1;
			//if(unsorted[testercuritem].powerBUYSTOP>(2.0*unsorted[testercuritem].powerSELLSTOP))
			//if(tmp3>(2.0*tmp5))
			if(x2<sort1cnt){
				lstrcat(buf2,(const char*)unsorted[testercuritem].val);
				lstrcat(buf2," ");
//				lstrcat(buf2,doubleToStr(unsorted[testercuritem].powerBUYSTOP-unsorted[testercuritem].powerSELLSTOP,0));
//				lstrcat(buf2,"% ");
				lstrcat(buf2,doubleToStr(unsorted[testercuritem].profitBUYSTOP,0));
				lstrcat(buf2,"/");
				lstrcat(buf2,doubleToStr(unsorted[testercuritem].profitBUYSTOP2,0));
				lstrcat(buf2," ");
//				lstrcat(buf2,doubleToStr(tmp3,0));
//				lstrcat(buf2,"% ");
				lstrcat(buf2,intToStr(unsorted[testercuritem].cntBUYSTOP2));
				lstrcat(buf2,"         \t ");
				if(unsorted[testercuritem].profitBUYSTOP>=0.0)correct++;else incorrect++;
				col1=true;
			}
			testercuritem=sort2[x2];
			//if(unsorted[testercuritem].powerSELLSTOP>(2.0*unsorted[testercuritem].powerBUYSTOP))
			//if(tmp4>(2.0*tmp6))
			if(x2<sort2cnt){
				if(!col1)lstrcat(buf2,"                                           \t ");
				lstrcat(buf2,(const char*)unsorted[testercuritem].val);
				lstrcat(buf2," ");
//				lstrcat(buf2,doubleToStr(unsorted[testercuritem].powerSELLSTOP-unsorted[testercuritem].powerBUYSTOP,0));
//				lstrcat(buf2,"% ");
				lstrcat(buf2,doubleToStr(unsorted[testercuritem].profitSELLSTOP,0));
				lstrcat(buf2,"/");
				lstrcat(buf2,doubleToStr(unsorted[testercuritem].profitSELLSTOP2,0));
				lstrcat(buf2," ");
//				lstrcat(buf2,doubleToStr(tmp3,0));
//				lstrcat(buf2,"% ");
				lstrcat(buf2,intToStr(unsorted[testercuritem].cntSELLSTOP2));
				lstrcat(buf2,"\r\n");
				if(unsorted[testercuritem].profitSELLSTOP>=0.0)correct++;else incorrect++;
				col2=true;
			}
			if(!col2)lstrcat(buf2,"\r\n");
	}
	double valid=0;
	valid=correct;if(correct>0.0)valid/=(correct+incorrect);valid*=100;
	lstrcat(buf2,"\r\n  Correct: ");
	
	if(incorrect<correct){lstrcat(buf2,doubleToStr(valid,2));lstrcat(buf2,"%");}else if(incorrect<1&&correct<1) lstrcat(buf2,"none");else lstrcat(buf2,"incorrect");
	lstrcat(buf2,"\r\n");
	
	lstrcat(buf2,"\r\n");
	wlog(buf2);
	
	memset(buf2,0,240);
	
	if(actmode==light)lstrcat(buf2,"MMCIS-Demo:");else
	if(actmode==medium)lstrcat(buf2,"MMCIS-Real:");else
	if(actmode==hard)lstrcat(buf2,"InstaForex-Demo.com:");
	
	lstrcat(buf2,timeToStr());
	title(whwnd,buf2);title(whpro," ");
	free(buf1);free(tmp);
	
	delete[] testeroptval;
	delete[] testermetadata;
	free(history);
	free(trades);free(buf2);
	
	delete[] unsorted;
	//test2();

}
void calculator::initrandbytes(){
    //for(int z=0;z<256;z++)randbytes[z]=0;
	for(int z1=0;z1<51;z1++){for(int z=0;z<65536;z++){randbytes[z]^=((rand()<<1) % kperiod);if((z&31)==31)sleep(1);}sleep(1);}
	for(int z1=0;z1<5;z1++)for(int z=0;z<65536;z++)randbytes[z]^=(randbytes[(z+1)%65536]>>1);
	randptr=0;randcnt=65535;randcnt2=0;
}
int calculator::getrand(){
	if(randptr>randcnt){randptr=0;randcnt2++;if(randcnt2>randcnt)randcnt2=0;}
    int t;t=randbytes[randptr^randcnt2];
	//t=(t%128);
	t+=kperiod;
	randptr++;
	return t;
}

void calculator::optimize(){
sleep(10);
	testerinit();
	testerloaddata();
	const time_t dt0=time(0);tm tm6; tm tm5;srand(time(0));
	int crcerrorscnt=1;
	char* tmp;tmp=(char*)malloc(5255);
	char* buf1;buf1=(char*)malloc(525);
	memset(buf1,0,525);lstrcat(buf1,"# optimization period ");
	lstrcat(buf1,intToStr(testerperiod));
	lstrcat(buf1,", ");
	lstrcat(buf1,timeToStr());
	if(actmode==light)lstrcat(buf1,":MMCIS-Demo");else
	if(actmode==medium)lstrcat(buf1,":MMCIS-Real");else
	if(actmode==hard)lstrcat(buf1,":InstaForex-Demo.com");
	lstrcat(buf1,"\n ");
	wlog(buf1);
//	int wsleep=6,wsleep2=4;
	int wsleep=6,wsleep2=3;

    bool reopt=false,mreopt;
	testerusefx();
	if(testerfxok)reopt=true;mreopt=reopt;

	int p[]={222,222,222,222,222,222,0,0};
	double oprofitcnt,oordercnt,oprofitindex,odrawdowncnt,sbalance;
	double profitcnt2=0.0,ordercnt2=0.0,profitindex2=0.0,drawdowncnt2=0.0;
	int p2[8];int res1=0,sorl;time_t deltatime;
	for(testercuritem=0;testercuritem<testervalcnt;testercuritem++)
	if((testercuritem!=12)&&(testercuritem!=13))
	{
//	for(testercuritem=12;testercuritem<testervalcnt;testercuritem++){
		testerloaddata();//SleepEx(1000,true);
		//int y=2;if(testercuritem<5)y=1;
		if(testerdataok)
		for(int a=0;a<1;a++)
		{
			if(a!=0)reopt=true;
			//if(a==0){kperiod=128;kperiod1=10;}else {kperiod=88;kperiod1=3;}
			//int crctmp[testercntper*11],crc1,crc2;
			/*int *crctmp,crc1,crc2;crctmp=(int*)realloc(crctmp,testercntper*2);
			
			memcpy(&crctmp[0],&testermetadata->ctm[0],testercntper*44);
			crc1=0;for(int cc1=0;cc1<testercntper*2;cc1++)crc1^=crctmp[cc1];
			//wlog("crc : ");wlog(intToStr(crc1));wlog("\r\n");
			crc2=crc1;*/

			memset(tmp,0,5255);
			lstrcat(tmp,"\n ");
			lstrcat(tmp,testervals[testercuritem]);
			lstrcat(tmp," ");
			//tm5 = *gmtime(&testermetadata->ctm[0]);
			//tm6 = *gmtime(&testermetadata->ctm[testercntper-1]);
			//memset(buf1,0,sizeof(buf1));
			//strftime(buf1,25,"%d.%m.%Y %H:%M",&tm5);
			//lstrcat(tmp,buf1);
			lstrcat(tmp,timeToStr(testermetadata->ctm[0]));
			lstrcat(tmp,"-");
			//memset(buf1,0,sizeof(buf1));
			//strftime(buf1,25,"%d.%m.%Y %H:%M",&tm6);
			//lstrcat(tmp,buf1);
			lstrcat(tmp,timeToStr(testermetadata->ctm[testercntper-1]));
			lstrcat(tmp,"\n");
			lstrcat(tmp," avg:");
			lstrcat(tmp,intToStr(stopavg1b));
			lstrcat(tmp," ");
			lstrcat(tmp,intToStr(stopavg1s));
			lstrcat(tmp," ");
			lstrcat(tmp,intToStr(stopavg2b));
			lstrcat(tmp," ");
			lstrcat(tmp,intToStr(stopavg2s));
			lstrcat(tmp,"\n");
			wlog(tmp);
			int unoptimized=0,o;
			for(testercursma=0;testercursma<testersmacnt;testercursma++)
			for(int ls=0;ls<4;ls++){
			int testercuritemptr=testercuritem*testersmacnt+testercursma;
			if( (reopt&&testeroptval[testercuritemptr].params[ls].datetimeopt==0) ||!reopt)
			{
				memset(tmp,0,5255);
				optcurbuysell=ls;
				if(a==0){kperiod=128;kperiod1=10;}else {kperiod=64;kperiod1=3;}initrandbytes();
				lstrcat(tmp,"  ");
				lstrcat(tmp,testervals[testercuritem]);
				lstrcat(tmp," ");
				lstrcat(tmp,testersmas[testercursma]);
				switch(ls){
					case 0:{lstrcat(tmp," buy 1..");p[6]=stopavg1b;break;}
					case 1:{lstrcat(tmp," sell 1..");p[6]=stopavg1s;break;}
					case 2:{lstrcat(tmp," buy 2..");p[6]=stopavg2b;break;}
					case 3:{lstrcat(tmp," sell 2..");p[6]=stopavg2s;break;}
				}
				title(whpro,tmp);
				
				for(int i=0;i<6;i++)p[i]=getrand();
				profitcnt2=0.0;ordercnt2=0.0;profitindex2=0.0;drawdowncnt2=0.0;sbalance=1.5;res1=0;

				int i=0,ix=0,ix2=0;
				bool resfinded=false;
				int ttl,tt2,ttprev;
				ttl=time(0);
				tt2=ttl;ttprev=tt2;
				testeroptval[testercuritemptr].params[ls].datetimeopt=0;
				deltatime=ttl-86000*10000;
				double pindex=0.799;
				double slorderscnt,stmp1,stmp2,stmp3;
				while(i<50){
					journalsinit();
					o=testertest(p[0],p[1],p[2],p[3],p[4],p[5],0,0,p[6]);
					if(o!=-1)
					if(profitcnt>0.0)
					{
						oprofitcnt=maxprofit;
						oordercnt=drawdowncnt+profitcnt;
						oprofitindex=profitcnt;oprofitindex/=(drawdowncnt+profitcnt+0.0001);
						odrawdowncnt=minprofit;
						slorderscnt=sorderscnt;slorderscnt+=lorderscnt;slorderscnt*=0.5;
						if(oprofitindex>pindex)
						
						if((oordercnt>testermincnttrades)&&(((ordercnt2-oordercnt)<(ordercnt2*0.1)&&profitindex2<oprofitindex)||((oordercnt-ordercnt2)>(ordercnt2*0.5)&&(profitindex2-oprofitindex)<0.05)||(ordercnt2<oordercnt&&profitindex2<=oprofitindex)))
						if((ls==0||ls==2)?sorderscnt<lorderscnt:sorderscnt>lorderscnt)
						{
							stmp1=drawdowncnt;stmp1+=profitcnt;stmp1*=0.5;stmp2=(sorderscnt<lorderscnt?sorderscnt:lorderscnt);stmp3=slorderscnt;stmp3/=stmp2;
							if(slorderscnt<stmp1)
							if(sbalance<stmp3){
								res1++;
								deltatime=time(0);ttl=deltatime;
								profitcnt2=oprofitcnt;ordercnt2=oordercnt;profitindex2=oprofitindex;drawdowncnt2=odrawdowncnt;
								//sbalance=(slorderscnt/(sorderscnt<lorderscnt?sorderscnt:lorderscnt));
								for(int i9=0;i9<7;i9++)p2[i9]=p[i9];
								memset(tmp,0,5255);
								lstrcat(tmp,"  ");
								if(ls<0.0)
								lstrcat(tmp," -");else
								lstrcat(tmp,"  ");
								lstrcat(tmp,testersmas[testercursma]);
								lstrcat(tmp," ");
								lstrcat(tmp,intToStr(i+ix*50+ix2*550));
								lstrcat(tmp,": ");
								lstrcat(tmp,doubleToStr(sorderscnt,0));
								lstrcat(tmp," ");
								lstrcat(tmp,doubleToStr(oordercnt,0));
								lstrcat(tmp," ");
								lstrcat(tmp,doubleToStr(oprofitindex,3));
								lstrcat(tmp," ");
								lstrcat(tmp,doubleToStr(lorderscnt,0));
								lstrcat(tmp,"  ");
								lstrcat(tmp,intToStr(p2[0]));
								lstrcat(tmp," ");
								lstrcat(tmp,intToStr(p2[1]));
								lstrcat(tmp," ");
								lstrcat(tmp,intToStr(p2[2]));
								lstrcat(tmp," ");
								lstrcat(tmp,intToStr(p2[3]));
								lstrcat(tmp," ");
								lstrcat(tmp,intToStr(p2[4]));
								lstrcat(tmp," ");
								lstrcat(tmp,intToStr(p2[5]));
								lstrcat(tmp," ");
								lstrcat(tmp,intToStr(p2[6]));
								lstrcat(tmp,"\n");
								time_t t=deltatime;//time(0);
								tm tm5;memset(&tm5,0,sizeof(struct tm));tm5 = *localtime(&t);tm5.tm_isdst=-1;
								time_t t2=mktime(&tm5);
								testeroptval[testercuritemptr].params[ls].datetimeopt=t2;
	//							testeroptval[sorl].datetimestart=testermetadata->ctm[0];
	//							testeroptval[sorl].datetimeend=testermetadata->ctm[testercntper-1];
								testeroptval[testercuritemptr].params[ls].param1=p2[0];
								testeroptval[testercuritemptr].params[ls].param2=p2[1];
								testeroptval[testercuritemptr].params[ls].param3=p2[2];
								testeroptval[testercuritemptr].params[ls].param4=p2[3];
								testeroptval[testercuritemptr].params[ls].param5=p2[4];
								testeroptval[testercuritemptr].params[ls].param6=p2[5];
								testeroptval[testercuritemptr].params[ls].param7=p2[6];
	
								title(whpro,tmp);
								resfinded=true;
							}
						}
					}
					switch(ix){
						case 0:{for(int i1=0;i1<6;i1++){if((i&7)<=i1)p[i1]-=kperiod1;if(p[i1]<45)p[i1]=getrand();}}break;
						case 1:{for(int i1=0;i1<6;i1++){if((i&7)<=i1)p[i1]-=kperiod1;if(p[i1]<45)p[i1]=getrand();}}break;
						case 2:{for(int i1=0;i1<6;i1++){if((i&7)>=i1)p[i1]-=kperiod1;if(p[i1]<45)p[i1]=getrand();}}break;
						case 3:{for(int i1=0;i1<6;i1++){if((i&7)>=i1)p[i1]-=kperiod1;if(p[i1]<45)p[i1]=getrand();}}break;
						case 4:{for(int i1=0;i1<6;i1++){if((i&7)<=i1)p[i1]-=kperiod1;if(p[i1]<45)p[i1]=getrand();}}break;
						case 5:{for(int i1=0;i1<6;i1++){if((i&7)<=i1)p[i1]-=kperiod1;if(p[i1]<45)p[i1]=getrand();}}break;
						case 6:{for(int i1=0;i1<6;i1++){if((i&7)<=i1)p[i1]>>1;if(p[i1]<45)p[i1]=getrand();}}break;
						case 7:{for(int i1=0;i1<6;i1++){if((i&7)<=i1)p[i1]>>1;if(p[i1]<45)p[i1]=getrand();}}break;
						case 8:{for(int i1=0;i1<6;i1++){if((i&7)>=i1)p[i1]>>1;if(p[i1]<45)p[i1]=getrand();}}break;
						case 9:{for(int i1=0;i1<6;i1++){if((i&7)>=i1)p[i1]>>1;if(p[i1]<45)p[i1]=getrand();}}break;
						case 10:{for(int i1=0;i1<6;i1++){if((i&7)<=i1)p[i1]>>1;if(p[i1]<45)p[i1]=getrand();}}break;
						case 11:{for(int i1=0;i1<6;i1++){if((i&7)<=i1)p[i1]>>1;if(p[i1]<45)p[i1]=getrand();}}break;
						default:{for(int i1=0;i1<6;i1++){if((i&7)<=i1)p[i1]>>1;if(p[i1]<45)p[i1]=getrand();}}break;

					}
					i++;
					if((i&15)==15)
					{tt2=time(0);if(tt2!=ttprev){sleep(270);
					ttprev=tt2;}}
					if(i==50){i=0;ix++;}
					if(ix>11){i=0;ix=0;ix2++;}//if(ix2==5)
					//if((res1>26)||(tt2-deltatime)>15&&((tt2-deltatime)<10000)){i=99999;res1=0;}
					if(((tt2-ttl)>wsleep2)&&(res1>0))i=99999;
					if(((tt2-ttl)>wsleep)&&(res1==0)){i=99999;if(res1==0)unoptimized++;}

				}
			
				if(!resfinded){lstrcat(tmp,"\n");for(int i0=0;i0<8;i0++)p2[i0]=0;testeroptval[testercuritemptr].params[ls].datetimeopt=0;}
				wlog(tmp);
				memset(tmp,0,sizeof(tmp));
				double title1;
				title1=testercuritem*testersmacnt;
				title1+=testercursma;
				title1*=100;
				title1=title1/(testervalcnt*testersmacnt);
				lstrcat(tmp,doubleToStr(title1,2));lstrcat(tmp,"% optimized");
				if(actmode==light)lstrcat(tmp,": MMCIS-Demo ");else
				if(actmode==medium)lstrcat(tmp,": MMCIS-Real ");else
				if(actmode==hard)lstrcat(tmp,": InstaForex-Demo.com ");
				title1=time(NULL);title1-=dt0;title1/=60;
				lstrcat(tmp,doubleToStr(title1,1));
				lstrcat(tmp," minutes used");

				title(whwnd,tmp);

			}}
			testersavefx();

			/*memcpy(&crctmp[0],&testermetadata->ctm[0],testercntper*44);
			crc1=0;for(int cc1=0;cc1<testercntper*11;cc1++)crc1^=crctmp[cc1];
			if(crc1!=crc2){wlog("ERROR crc : ");wlog(intToStr(crcerrorscnt));wlog("\r\n");crcerrorscnt++;}*/
		}
		reopt=mreopt;
	}
	time_t t1=time(0);
	tm tm1;memset(&tm1,0,sizeof(struct tm));tm1=*localtime((const time_t *)&t1);
	memset(tmp,0,sizeof(tmp));
	strftime(tmp,80,"%d.%m.%Y %H:%M ",&tm1);
	lstrcat(tmp,"optimization complete.\nTotal time used: ");
	lstrcat(tmp,intToStr((time(NULL)-dt0)/60));
	lstrcat(tmp," minutes \n");
	wlog(tmp);
	testersavefx();
	memset(tmp,0,sizeof(tmp));
	title(whpro,tmp);
	memset(tmp,0,sizeof(tmp));
	lstrcat(tmp,"100% optimized, ");
	lstrcat(tmp,intToStr((time(NULL)-dt0)/60));
	lstrcat(tmp," minutes used.");
	title(whwnd,tmp);
	free(tmp);
	free(buf1);
}
/*void calculator::debug(){
 testerinit();
 testerusefx();
 if(testerfxok){
  for(testercuritem=0;testercuritem<testervalcnt;testercuritem++){
	testerloaddata();
	int sorl,p1,p2,p3,p4,p5,p6,p7,p8,drawdowncnt0,ordercnt0,profitcnt0,errors=0;double profitindex0;
	if(testerdataok){
		for(testercursma=0;testercursma<testersmacnt;testercursma++)
		for(int ls=-1;ls<=1;ls+=2){
			if(ls>0)sorl=testercuritem*testersmacnt*2+testercursma*2;
			else sorl=testercuritem*testersmacnt*2+testercursma*2+1;
			optcurbuysell=(testeroptval[sorl].stoporlimit)?-1:1;//ls;
			p1=testeroptval[sorl].param1;
			p2=testeroptval[sorl].param2;
			p3=testeroptval[sorl].param3;
			p4=testeroptval[sorl].param4;
			p5=testeroptval[sorl].param5;
			p6=testeroptval[sorl].param6;
			p7=testeroptval[sorl].param7;
			p8=testeroptval[sorl].param8;
			drawdowncnt0=testeroptval[sorl].totaldrawdown;
			ordercnt0=testeroptval[sorl].totalorders;
			profitcnt0=testeroptval[sorl].totalprofit;
			profitindex0=testeroptval[sorl].totalprofitindex;
			if(testeroptval[sorl].datetimeopt!=0){
				journalsinit();
				testertest(p1,p2,p3,p4,p5,p6,0,0,p7);
				int ot,ot1;
				char buf1[25];
				char* buf;buf=(char*)malloc(400000);memset(buf,0,400000);
				tm tm2,tm1;
				lstrcat(buf,"\r\n# ");
				lstrcat(buf,testervals[testercuritem]);
				lstrcat(buf," ");lstrcat(buf,testersmas[testercursma]);
				if(ls>0)lstrcat(buf," buy..");else lstrcat(buf," sell..");
				lstrcat(buf," ");
				lstrcat(buf,intToStr(testerperiod));
				lstrcat(buf,"   [");
				lstrcat(buf,gmtimeToStr(testermetadata->ctm[0]));
				lstrcat(buf,"-");
				lstrcat(buf,gmtimeToStr(testermetadata->ctm[testercntper-1]));
				lstrcat(buf,"] (");
				lstrcat(buf,intToStr(drawdowncnt+profitcnt));
				lstrcat(buf,") ");
				lstrcat(buf,"\r\n");
				//---------------------- HISTORY to log
				int cnthistory=iHistoryTotal-2;if(cnthistory<1)cnthistory=iHistoryTotal;
				//for(int i=cnthistory;i<iHistoryTotal;i++){//(iHistoryTotal+20)%iHistoryTotal//0
				for(int i=0;i<iHistoryTotal;i++){
					OrderSelect(i, MODE_HISTORY);//if(curOrderProfit>testerpoint*500){
					lstrcat(buf,intToStr(i));
					lstrcat(buf,":  ");
					tm1 = *gmtime((const time_t *)&curOrderOpenTime);
					tm2 = *gmtime((const time_t *)&curOrderCloseTime);
					memset(buf1,0,sizeof(buf1));
					strftime(buf1,80,"%d.%m.%Y %H:%M",&tm1);
					lstrcat(buf,buf1);
					lstrcat(buf,"-");
					memset(buf1,0,sizeof(buf1));
					if(tm1.tm_yday==tm2.tm_yday)
					strftime(buf1,80,"%H:%M",&tm2); else
					if(tm1.tm_yday!=tm2.tm_yday)
					strftime(buf1,80,"%d.%m.%Y %H:%M",&tm2);
					lstrcat(buf,buf1);
					lstrcat(buf," ");
					lstrcat(buf,intToStr(curOrderTicket));
					ot=(int)curOrderType;ot1=ot;
					if(ot==0)lstrcat(buf," buy"); else
					if(ot==1)lstrcat(buf," sell"); else
					if(ot==2)lstrcat(buf," buy limit"); else
					if(ot==3)lstrcat(buf," sell limit"); else
					if(ot==4)lstrcat(buf," buy stop"); else
					if(ot==5)lstrcat(buf," sell stop");

					lstrcat(buf," ");
					lstrcat(buf,doubleToStr(curOrderOpenPrice,testerdigits));
					if(ot<2){
						//lstrcat(buf," ");lstrcat(buf,doubleToStr(history[i].iOrderStopLoss,testerdigits));
						//lstrcat(buf," ");lstrcat(buf,doubleToStr(history[i].iOrderTakeProfit,testerdigits));

						lstrcat(buf,"-");
						lstrcat(buf,doubleToStr(curOrderClosePrice,testerdigits));
					} else {
						lstrcat(buf,"    ");
						lstrcat(buf,doubleToStr(curOpenPrice,testerdigits));
					}
					ot=(int)curOrderOpclose;
					if(ot1<2){
						if(ot==0)lstrcat(buf," sl"); else
						if(ot==1)lstrcat(buf," tp"); else
						if(ot==2)lstrcat(buf," close");
					}else
					if(ot==3)lstrcat(buf," delete");
					if(ot1<2){
						lstrcat(buf," ");
						lstrcat(buf,intToStr(curOrderProfit));
					}
					lstrcat(buf,"\r\n");
				//}
				}
				//------------------TRADES to log
				lstrcat(buf,"--TRADES\r\n");
				for(int i=0;i<iTradesTotal;i++){
					OrderSelect(i, MODE_TRADES);
					lstrcat(buf,intToStr(i));
					lstrcat(buf,":  ");
					tm1 = *gmtime((const time_t *)&curOrderOpenTime);
					tm2 = *gmtime((const time_t *)&curOrderCloseTime);
					memset(buf1,0,sizeof(buf1));
					strftime(buf1,80,"%d.%m.%Y %H:%M",&tm1);
					lstrcat(buf,buf1);
					lstrcat(buf," ");
					lstrcat(buf,intToStr(curOrderTicket));
					ot=(int)curOrderType;ot1=ot;
					if(ot==0)lstrcat(buf," buy"); else
					if(ot==1)lstrcat(buf," sell"); else
					if(ot==2)lstrcat(buf," buy limit"); else
					if(ot==3)lstrcat(buf," sell limit"); else
					if(ot==4)lstrcat(buf," buy stop"); else
					if(ot==5)lstrcat(buf," sell stop");
					lstrcat(buf," ");
					lstrcat(buf,doubleToStr(curOrderOpenPrice,testerdigits));
					if(ot<2){
						lstrcat(buf,"-");
						lstrcat(buf,doubleToStr(curOrderClosePrice,testerdigits));
					} else {
						lstrcat(buf,"    ");
						lstrcat(buf,doubleToStr(curOpenPrice,testerdigits));
					}
					if(ot1<2){
						lstrcat(buf," ");
						lstrcat(buf,intToStr(curOrderProfit));
					}
					lstrcat(buf,"\r\n");
				}
				lstrcat(buf,"Total orders: ");
				lstrcat(buf,intToStr(drawdowncnt+profitcnt));
				lstrcat(buf,"\r\n");
				lstrcat(buf,"Profitable: ");
				double tmp;tmp=profitcnt/(drawdowncnt+0.001);
				lstrcat(buf,doubleToStr(tmp,2));
				lstrcat(buf,"\r\n");
				lstrcat(buf,"drawdown total: ");
				lstrcat(buf,intToStr(minprofit));
				lstrcat(buf," (");
				lstrcat(buf,intToStr(drawdowncnt));
				lstrcat(buf,")\r\n");
				lstrcat(buf,"profit total: ");
				lstrcat(buf,intToStr(maxprofit));
				lstrcat(buf," (");
				lstrcat(buf,intToStr(profitcnt));
				lstrcat(buf,")\r\n");
				wlog(buf);
				free(buf);
			}
		}
	}
  }
 }
}
*/
void decode(int act,int tperiod,int actmode,bool tradecurbar,int tbackbar){
	calculator* calc;
	calc=new calculator;
	calc->action(act,tperiod,actmode,tradecurbar,tbackbar);
	delete calc;
}
int main(int argc, char *argv[]){

	int action=-1,period=-1,backbar=8;
	clen=0;
	cbuf =(char*) malloc(1000);memset(cbuf,0,1000);
   	for(int i=1;i<argc;i++){int cl=strlen(argv[i]);for(int i1=0;i1<cl;i1++)cbuf[clen+i1]=argv[i][i1];clen+=cl;}cbuf[clen]=cbuf[clen-1];//printf(cbuf);
	if(find(cbuf,"-aopt"))action=optimizing;else
	if(find(cbuf,"-atest"))action=testing;else
	if(find(cbuf,"-adebug"))action=debuging;
	
	if(find(cbuf,"-p1t"))period=1;else
	if(find(cbuf,"-p5t"))period=5;else
	if(find(cbuf,"-p15t"))period=15;else
	if(find(cbuf,"-p60t"))period=60;else
	if(find(cbuf,"-p240t"))period=240;else
	if(find(cbuf,"-p1440t"))period=1440;else
	if(find(cbuf,"-p10080t"))period=10080;else
	if(find(cbuf,"-p43200t"))period=43200;

	int mode=999;
	if(find(cbuf,"mode1"))mode=light;else
	if(find(cbuf,"mode2"))mode=medium;else
	if(find(cbuf,"mode3"))mode=hard;

	if(mode==999)printf("Usage: calculator [-a action] [-p period] mode\r\n\r\n\t-a\t\taction: opt | test | debug\r\n\t-p\t\tperiod: 1t | 5t | 15t | 60t | 240t | 1440t | 10080t | 43200t\r\n\tmode\t\tvals list: mode1 | mode2 | mode3\r\n\r\nReport bugs to <emailbogsuv [AT] gmail.com>.\r\n");
	else if(action!=optimizing)decode(action,period,mode,donottradecurrentbar,backbar);else decode(action,period,mode,tradecurrentbar,backbar);

	free(cbuf);

}
