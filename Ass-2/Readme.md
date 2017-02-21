#Multi threaded Map-Reduce program for word and letter statistics

Multi threaded Map-Reduce program to generate (word,value) & (letter, value) pairs.

Multi threaded Map-Reduce program has two executable files wordStatistics and wordStatistics_Optional. wordStatistics_Optional includes optional part. Both of them take arguments through command line. Below are the necessary command line arguments. 

wordStatistics
--------------
Input - Text File Name containing the words,  Number of Mapper threads, Number of Reducer threads.

Output : (word,Value) pairs for all the words in the given text file to wordCount.txt.


wordStatistics_Optional
-----------------------
Input - Text File Name containing the words,  Number of Mapper threads, Number of Reducer threads, Number of Summarizer threads.

Output : (word,Value) pairs for all the words to wordCount.txt and (letter,Value) pairs to letterCount.txt in the given text file.
