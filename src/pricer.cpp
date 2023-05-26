#include <pricer.hpp>
using namespace marketdata;

bool compare_double(long double x, long double y, long double epsilon = 0.0000001f)
{
	if(fabs(x - y) < epsilon)
		return true; 
	return false; 
}

pricer::pricer(unsigned long target_size): target_size_(target_size)
{
	thread_ = std::thread{ &pricer::processOrder, this };
}

pricer::~pricer()
{
	{
		std::unique_lock<std::mutex> lock(mutex_);
		exit_ = true;
		condVar_.notify_all();
	}
	thread_.join();
}

void pricer::readInputData()
{
        std::string line;
        std::stringstream ss; 
        order_t newOrder;
        std::vector<std::string> vec(6);
        std::string word;

        // Add Order to Book : timestamp A order-id side price size
        // Reduce Order      : timestamp R order-id size
        // Pricer response   : timestamp action total

        while(getline(std::cin,line) && !line.empty())
        {   
                try 
                {   
                        ss.clear();
                        vec.clear();
                        newOrder.initialize();

                        ss << line;
                        while(ss >> word)
                                vec.push_back(word);

                        if(vec[1].compare(ADD_ORDER) == 0 && vec.size() == 6 ) 
                        {   
                                newOrder.timestamp = vec[0];
                                newOrder.order_id  = vec[2];
                                newOrder.side      = vec[3];
                                newOrder.price     = std::stod(vec[4]);
                                newOrder.size      = std::stoul(vec[5]);

                                pushOrder(newOrder);

                        }   
                        else if(!vec[1].compare(REDUCE_ORDER) && vec.size() == 4 ) 
                        {   
                                newOrder.timestamp = vec[0];
                                newOrder.order_id  = vec[2];
                                newOrder.size      = std::stoul(vec[3]);

                                pushOrder(newOrder);
                        }   
                        else
                        {   
                                std::cerr << "Invalid Message\n";
                                continue;
                        }   

                        //line.clear();
                }   
                catch(const std::exception& e)
                {   
                        std::cout << "Caught exception:" << e.what()<<std::endl;
                }   
                catch(...)
                {   
                        std::cout << "Caught Unknown Exceptionn" << std::endl;
                }   
        }   


}

void pricer::pushOrder(const order_t& newOrder)
{
	std::unique_lock<std::mutex>  lock(mutex_);
	orderQueue_.push(newOrder);
	condVar_.notify_all();
}

void pricer::processOrder()
{
	std::unique_lock<std::mutex>  lock(mutex_);
	while(true)
	{
		if(!exit_)
			condVar_.wait(lock);
		lock.unlock();
		while(true)
		{
			lock.lock();
			if(orderQueue_.empty())
				break;
			else
			{
				auto newOrder = orderQueue_.front();
				orderQueue_.pop();
				lock.unlock();

				if(bidBook_.count(newOrder.order_id) > 0 || askBook_.count(newOrder.order_id) > 0)
				{
					reduceOrder(newOrder);
				}
				else 
				{
					addOrder(newOrder);
				}
				analyse();
			}
		}
		if(exit_)
			break;
	}
}

void pricer::addOrder(const order_t& addOrder)
{
	setAskBookModified(false);
	setBidBookModified(false);

	if(!addOrder.side.compare(BUY))
	{
		addBuyOrder(addOrder);
	}
	else if(!addOrder.side.compare(SELL))
	{
		addSellOrder(addOrder);
	}
}

void pricer::addBuyOrder(const order_t& addOrder)
{
	bidBook_[addOrder.order_id] = addOrder;

	setBidSize(getBidSize() + addOrder.size);
	setBidTimestamp(addOrder.timestamp);
	setBidBookModified(true);
}

void pricer::addSellOrder(const order_t& addOrder)
{
	askBook_[addOrder.order_id] = addOrder;

	setAskSize(getAskSize() + addOrder.size);
	setAskTimestamp(addOrder.timestamp);
	setAskBookModified(true);
}

void pricer::reduceOrder(const order_t& reduceOrder)
{
	setAskBookModified(false);
	setBidBookModified(false);

	if(bidBook_.count(reduceOrder.order_id) > 0)
	{
		reduceBuyOrder(reduceOrder);
	}
	else if(askBook_.count(reduceOrder.order_id) > 0)
	{
		reduceSellOrder(reduceOrder);
	}
}

void pricer::reduceBuyOrder(const order_t& reduceOrder)
{
	auto itr = bidBook_.find(reduceOrder.order_id);
	if( itr != bidBook_.end())
	{
		if(reduceOrder.size < itr->second.size)
		{
			setBidSize(getBidSize() - reduceOrder.size);
			itr->second.size -= reduceOrder.size;
		}
		else
		{
			setBidSize(getBidSize() - itr->second.size);
			bidBook_.erase(itr);
		}
		setBidTimestamp(reduceOrder.timestamp);
		setBidBookModified(true);
	}
}

void pricer::reduceSellOrder(const order_t& reduceOrder)
{
	auto itr = askBook_.find(reduceOrder.order_id);
	if( itr != askBook_.end())
	{
		if(reduceOrder.size < itr->second.size)
		{
			setAskSize(getAskSize() - reduceOrder.size);
			itr->second.size -= reduceOrder.size;
		}
		else
		{
			setAskSize(getAskSize() - itr->second.size);
			askBook_.erase(itr);
		}
		setAskTimestamp(reduceOrder.timestamp);
		setAskBookModified(true);
	}
}

void pricer::analyse()
{
	if(getBidBookModified())
	{
		analyseSell();
	}
	else if(getAskBookModified())
	{
		analyseBuy();
	}
}

void pricer::analyseSell() 
{
	if(getTargetSize() <= getBidSize())
	{
		//Analyse BID Book
		flatbook_t vec;
		std::copy(bidBook_.begin(),
				bidBook_.end(),
				std::back_inserter<flatbook_t>(vec));

		std::sort(vec.begin(), vec.end(),
				[](const auto& l, const auto& r) { return l.second.price > r.second.price; });

		long double result = 0;
		unsigned long int sellSize = getTargetSize();

		std::cout  << std::setprecision(2) << std::fixed;
		for( size_t x = 0 ; x < vec.size() ; ++x)
		{
			if(sellSize > 0 && sellSize <= vec[x].second.size)
			{
				result  +=  vec[x].second.price * sellSize;
				sellSize = 0;
				if(!compare_double(result,prev_sell_income))
				{
					setSellInitialized(true);
					prev_sell_income = result;

					{
						std::unique_lock<std::mutex>  lock(coutMutex_);
						std::cout<< getBidTimestamp() <<" "<<SELL<<" "<< result <<std::endl;
					}

					break;
				}
			}
			else if( sellSize > 0 && sellSize > vec[x].second.size)
			{
				result   +=  vec[x].second.price * vec[x].second.size;
				sellSize -=  vec[x].second.size;
			}
		}
	}
	else if(getSellInitialized())
	{
		setSellInitialized(false);
		prev_sell_income = -1;

		{
			std::unique_lock<std::mutex>  lock(coutMutex_);
			std::cout<< getBidTimestamp() <<" "<<SELL<<" "<<"NA"<<std::endl;
		}
	}
}

void pricer::analyseBuy() 
{
	if(getTargetSize() <= getAskSize())
	{
		//Analyse ASK book
		flatbook_t vec;
		std::copy(askBook_.begin(),
				askBook_.end(),
				std::back_inserter<flatbook_t>(vec));

		std::sort(vec.begin(), vec.end(),
				[](const auto& l, const auto& r) { return l.second.price < r.second.price; });

		long double result = 0;
		unsigned long int buySize = getTargetSize();
		std::cout  << std::setprecision(2) << std::fixed;
		for( size_t x = 0 ; x < vec.size() ; ++x)
		{
			if(buySize > 0 && buySize <= vec[x].second.size)
			{
				result  +=  vec[x].second.price * buySize;
				buySize = 0;
				if(!compare_double(result,prev_buy_expense ))
				{
					setBuyInitialized(true);
					prev_buy_expense = result;

					{
						std::unique_lock<std::mutex>  lock(coutMutex_);
						std::cout<< getAskTimestamp() <<" "<<BUY<<" "<< result <<std::endl;
					}

					break;
				}
			}
			else if( buySize > 0 && buySize > vec[x].second.size)
			{
				result  +=  vec[x].second.price * vec[x].second.size;
				buySize -=  vec[x].second.size;
			}
		}
	}
	else if(getBuyInitialized())
	{
		setBuyInitialized(false);
		prev_buy_expense = -1;

		{
			std::unique_lock<std::mutex>  lock(coutMutex_);
			std::cout<< getAskTimestamp() <<" "<<BUY<<" "<<"NA"<<std::endl;
		}
	}
}

namespace marketdata 
{
	std::ostream& operator<<(std::ostream& os, const pricer& mypricer)
	{
		using namespace marketdata;
		os  << "print limit book:"<<std::endl;
		os  << std::setprecision(2) << std::fixed;

		for(const auto& order : mypricer.bidBook_ )
			os <<order.first           <<" "
				<<order.second.timestamp<<" "  
				<<order.second.order_id <<" " 
				<<order.second.side     <<" " 
				<<order.second.price    <<" " 
				<<order.second.size<<std::endl;

		for(const auto& order : mypricer.askBook_ )
			os <<order.first           <<" "
				<<order.second.timestamp<<" "  
				<<order.second.order_id <<" " 
				<<order.second.side     <<" " 
				<<order.second.price    <<" " 
				<<order.second.size<<std::endl;

		os <<"size:"<< mypricer.bidBook_.size() + mypricer.askBook_.size() <<std::endl;
		return os;
	}
}
