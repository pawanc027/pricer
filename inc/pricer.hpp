#ifndef PRICER_H 
#define PRICER_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <atomic>
#include <cmath>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

namespace marketdata {

	const std::string ADD_ORDER    = "A";
	const std::string REDUCE_ORDER = "R";
	const std::string BUY          = "B";
	const std::string SELL         = "S";

	struct order
	{
		std::string   timestamp;
		std::string   order_id;
		std::string   side;
		long double   price;
		unsigned long size;

		order()
		{ 
			timestamp = "";
			order_id = "";
			side = "";
			price = 0;
			size = 0;
		}

		void clear()
		{ 
			timestamp = "";
			order_id = "";
			side = "";
			price = 0;
			size = 0;
		}
	};

	using order_t      = order ;
	using book_t       = std::unordered_map< std::string, order_t > ;
	using flatbook_t   = std::vector< std::pair< std::string, order_t > > ;

	class pricer 
	{
		public:
			explicit pricer(unsigned long target_size = 0);

			pricer(const pricer& src) = delete;
			pricer& operator=(const pricer& rhs) = delete;

			virtual void pushOrder(const order_t& newOrder);
			virtual ~pricer();

			friend ostream& operator<<(ostream& os, const pricer& mypricer);

		protected:
			virtual void processOrder();
			virtual void addOrder(const order_t& addOrder);
			virtual void reduceOrder(const order_t& reduceOrder);
			virtual void analyse();
			virtual void addBuyOrder(const order_t& addOrder);
			virtual void addSellOrder(const order_t& addOrder);
			virtual void reduceBuyOrder(const order_t& reduceOrder);
			virtual void reduceSellOrder(const order_t& reduceOrder);
			virtual void analyseBuy(); 
			virtual void analyseSell();

			virtual void setBidTimestamp(const std::string& timestamp)	{ bidTimestamp_ = timestamp; }
			virtual void setAskTimestamp(const std::string& timestamp)	{ askTimestamp_ = timestamp; }
			virtual void setBidSize(const unsigned long size)		{ bidSize_ = size ; }
			virtual void setAskSize(const unsigned long size)		{ askSize_ = size ; }
			virtual void setBuyInitialized(const bool flag)			{ buyInitialized_  = flag ; }
			virtual void setSellInitialized(const bool flag)		{ sellInitialized_ = flag ; }
			virtual void setBidBookModified(const bool flag)		{ bidBookModfied_  = flag ; }
			virtual void setAskBookModified(const bool flag)		{ askBookModfied_  = flag ; }

			virtual const std::string& getBidTimestamp()	const { return bidTimestamp_ ; }
			virtual const std::string& getAskTimestamp()	const { return askTimestamp_ ; }
			virtual bool  getBuyInitialized()		const { return buyInitialized_  ; }
			virtual bool  getSellInitialized()		const { return sellInitialized_ ; }
			virtual bool  getBidBookModified()		const { return bidBookModfied_  ; }
			virtual bool  getAskBookModified()		const { return askBookModfied_  ; }
			virtual unsigned long getTargetSize()		const { return target_size_ ; }
			virtual unsigned long getBidSize()		const { return bidSize_ ; }
			virtual unsigned long getAskSize()		const { return askSize_ ; }

		private:
			book_t askBook_;
			book_t bidBook_;
			long double prev_sell_income = 0;
			long double prev_buy_expense = 0;
			std::string bidTimestamp_;
			std::string askTimestamp_;

			std::atomic<unsigned long> target_size_{0};
			std::atomic<unsigned long> bidSize_ {0};
			std::atomic<unsigned long> askSize_ {0};
			std::atomic <bool> buyInitialized_ {false};
			std::atomic <bool> sellInitialized_{false};
			std::atomic <bool> bidBookModfied_{false};
			std::atomic <bool> askBookModfied_{false};

			std::queue<order_t> orderQueue_;
			std::mutex mutex_;
			std::condition_variable condVar_;
			std::mutex coutMutex_;
			std::thread thread_;
			bool exit_ = false;
	};
}

#endif 
