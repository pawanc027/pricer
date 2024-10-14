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
			initialize();
		}

		void initialize()
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

			virtual ~pricer();
			void readInputData();

			friend std::ostream& operator<<(std::ostream& os, const pricer& mypricer);

		protected:
			void pushOrder(const order_t& newOrder);
			void processOrder();
			void addOrder(const order_t& addOrder);
			void reduceOrder(const order_t& reduceOrder);
			void analyse();
			void addBuyOrder(const order_t& addOrder);
			void addSellOrder(const order_t& addOrder);
			void reduceBuyOrder(const order_t& reduceOrder);
			void reduceSellOrder(const order_t& reduceOrder);
			void analyseBuy(); 
			void analyseSell();

			void setBidTimestamp(const std::string& timestamp)	{ bidTimestamp_ = timestamp; }
			void setAskTimestamp(const std::string& timestamp)	{ askTimestamp_ = timestamp; }
			void setBidSize(const unsigned long size)		{ bidSize_ = size ; }
			void setAskSize(const unsigned long size)		{ askSize_ = size ; }
			void setBuyInitialized(const bool flag)			{ buyInitialized_  = flag ; }
			void setSellInitialized(const bool flag)		{ sellInitialized_ = flag ; }
			void setBidBookModified(const bool flag)		{ bidBookModfied_  = flag ; }
			void setAskBookModified(const bool flag)		{ askBookModfied_  = flag ; }

			const std::string& getBidTimestamp()	const { return bidTimestamp_ ; }
			const std::string& getAskTimestamp()	const { return askTimestamp_ ; }
			bool  getBuyInitialized()		const { return buyInitialized_  ; }
			bool  getSellInitialized()		const { return sellInitialized_ ; }
			bool  getBidBookModified()		const { return bidBookModfied_  ; }
			bool  getAskBookModified()		const { return askBookModfied_  ; }
			unsigned long getTargetSize()		const { return target_size_ ; }
			unsigned long getBidSize()		const { return bidSize_ ; }
			unsigned long getAskSize()		const { return askSize_ ; }

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
