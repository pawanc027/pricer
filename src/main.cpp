#include <pricer.hpp>
using namespace marketdata;

int main(int argc, const char* argv[])
{
	if( argc == 1)
	{
		std::cerr<<"No arguments were passed."<<std::endl;
		return -1;
	}
	else if (argc != 2)
	{
		std::cerr<<"More than one argument was provided."<<std::endl;
		return -1;
	}

	unsigned long target_size;
	target_size = std::atoi(argv[1]);

	if( target_size == 0 )
	{
		std::cerr<<"Target size was passed as zero , thereby ending the price analysis"<<std::endl;
		return -1;
	}

	pricer mypricer(target_size);
	std::string line;
	std::stringstream ss;
	order_t newOrder;
	std::vector<std::string> vec(6);
	std::string word;

	// Add Order to Book : timestamp A order-id side price size
	// Reduce Order      : timestamp R order-id size
	// Pricer response   : timestamp action total

	while(getline(cin,line) && !line.empty())
	{
		try
		{
			ss.clear();
			vec.clear();
			newOrder.clear();

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

				mypricer.pushOrder(newOrder);

			}
			else if(!vec[1].compare(REDUCE_ORDER) && vec.size() == 4 )
			{
				newOrder.timestamp = vec[0];
				newOrder.order_id  = vec[2];
				newOrder.size      = std::stoul(vec[3]);

				mypricer.pushOrder(newOrder);
			}
			else
			{
				std::cerr<<"Invalid Message"<<std::endl;
				continue;
			}

			line.clear();
		}
		catch(const std::exception& e)
		{
			std::cout<<"Caught exception:"<<e.what()<<std::endl;
		}
		catch(...)
		{
			std::cout<<"Caught Unknown Exception"<<std::endl;
		}
	}

	return 0;
}
