ORDER BOOK PROGRAMMING PROBLEM

BACKGROUND
You inherit $3000 from your great aunt Gertrude and decide to invest in the Acme Internet Widget Company (AIWC). You place a limit order: "BUY 100 AIWC @ $30", indicating you won't pay more than $30 per share. This order, known as a "bid", remains in the market until filled.
Multiple buyers are interested. For example, one buyer bids $31 for 200 shares, and another bids $29 for 300 shares. When Warren Buffett sells 225 shares, he sells 200 to the highest bidder ($31) and 25 to you at $30, leaving you with 75 shares still to buy.
Next year, AIWC appreciates by 50%, and you want to sell your shares for at least $45 each. You place a limit order: "SELL 100 AIWC @ $45", known as an "ask". However, there are other sellers: one asking $44 and another $46. Alan Greenspan will buy at the lowest price first, so he'll purchase from the $44 ask before considering yours.
All standing bids and asks form the "limit order book". You can receive real-time updates about changes to this book through a data feed, which you can log and analyze later.

PROBLEM STATEMENT
You need to write a program, Pricer, to analyze a market data log. The program takes a single command-line argument: target-size. It reads a market data log from standard input and prints the total cost of buying target-size shares (from the lowest asks) and the total income from selling target-size shares (from the highest bids) each time there's a change.

INPUT FORMAT
The log contains one message per line. Each message is structured as follows:
1. ADD ORDER TO BOOK: timestamp A order-id side price size
    * timestamp: Time in milliseconds since midnight.
    * A: Indicates an "Add Order to Book".
    * order-id: Unique identifier for the order.
    * side: 'B' for buy (bid) or 'S' for sell (ask).
    * price: Limit price for the order.
    * size: Initial size of the order in shares.
2. REDUCE ORDER: timestamp R order-id size
    * timestamp: Time in milliseconds since midnight.
    * R: Indicates a "Reduce Order".
    * order-id: Unique identifier for the order to be reduced.
    * size: Amount to reduce the order size.
Log messages are sorted by timestamp. If there's an error in a message, output a warning to standard error and continue.

OUTPUT FORMAT
Output consists of one line per change:
timestamp action total
* timestamp: Timestamp from the input message that triggered the output.
* action: 'B' for buying expense, 'S' for selling income.
* total: Total expense (if 'B') or income (if 'S'), or 'NA' if the order cannot be filled.

EXAMPLE INPUT AND OUTPUT  
  
Here’s an example of input and output with a target-size of 200:  
  
INPUT  
  
28800538 A b S 44.26 100  
28800562 A c B 44.10 100  
28800744 R b 100  
28800758 A d B 44.18 157  
28800773 A e S 44.38 100  
28800796 R d 157  
28800812 A f B 44.18 157  
28800974 A g S 44.27 100  
28800975 R e 100  
28812071 R f 100  
28813129 A h B 43.68 50  
28813300 R f 57  
28813830 A i S 44.18 100  
28814087 A j S 44.18 1000  
28814834 R c 100  
28814864 A k B 44.09 100  
28815774 R k 100  
28815804 A l B 44.07 175  
28815937 R j 1000  
28816245 A m S 44.22 100  
  
OUTPUT  
  
28800758 S 8832.56  
28800796 S NA  
28800812 S 8832.56  
28800974 B 8865.00  
28800975 B NA  
28812071 S NA  
28813129 S 8806.50  
28813300 S NA  
28813830 B 8845.00  
28814087 B 8836.00  
28815804 S 8804.25  
28815937 B 8845.00  
28816245 B 8840.00  
  
