default: wordStatistics wordStatistics_opt 

inorder: inorder_wordStatistics inorder_wordStatistics_opt

wordStatistics: wordStatistics.cc 
	g++ -std=c++11 wordStatistics.cc -o wordStatistics -pthread

wordStatistics_opt: wordStatistics_optional.cc 
	g++ -std=c++11 wordStatistics_optional.cc -o wordStatistics_opt -pthread

inorder_wordStatistics: inorder_wordStatistics.cc 
	g++ -std=c++11 inorder_wordStatistics.cc -o inorder_wordStatistics -pthread

inorder_wordStatistics_opt: inorder_wordStatistics_Optional.cc 
	g++ -std=c++11 inorder_wordStatistics_Optional.cc -o inorder_wordStatistics_opt -pthread

clean:
	-rm -f wordStatistics wordStatistics_opt 
	-rm -f wordCount.txt letterCount.txt
	-rm -f inorder_wordStatistics_opt inorder_wordStatistics