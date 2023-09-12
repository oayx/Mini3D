#include "algorithm_test.h"
#include "algorithm_performance_test.h"
#include "vector_test.h"
#include "list_test.h"
#include "deque_test.h"
#include "queue_test.h"
#include "stack_test.h"
#include "map_test.h"
#include "set_test.h"
#include "hash_map_test.h"
#include "hash_set_test.h"
#include "string_test.h"

namespace tinystl 
{
	namespace test 
	{
		UnitTest* UnitTest::GetInstance() 
		{
			static UnitTest instance;
			return &instance;
		}

		TestCase* UnitTest::RegisterTestCase(TestCase* testcase) {
			testcases_.push_back(testcase);
			return testcase;
		}

		void UnitTest::Run() {
			for (auto it = testcases_.begin(); it != testcases_.end(); ++it) {
				TestCase* testcase = *it;
				CurrentTestCase = testcase;
				testcase->nTestResult = 1;
				testcase->nFailed = testcase->nPassed = 0;
				std::cout << green << "============================================" << std::endl;
				std::cout << green << " Run TestCase:" << testcase->testcase_name << " " << std::endl;
				testcase->Run();
				if (testcase->nFailed == 0)    std::cout << green;
				else std::cout << red;
				std::cout << " " << testcase->nPassed << " / " << testcase->nFailed + testcase->nPassed
					<< " Cases passed. ( " << testcase->nPassed / (testcase->nFailed + testcase->nPassed) * 100 << "% )" << std::endl;
				std::cout << green << " End TestCase:" << testcase->testcase_name << " " << std::endl;
				if (testcase->nTestResult)
					++nPassed;
				else
					++nFailed;
			}

			std::cout << green << "============================================" << std::endl;
			std::cout << green << " Total TestCase : " << nPassed + nFailed << std::endl;
			std::cout << green << " Total Passed : " << nPassed << std::endl;
			std::cout << red << " Total Failed : " << nFailed << std::endl;
			if (nFailed == 0)    std::cout << green;
			else
				std::cout << " " << nPassed << " / " << nFailed + nPassed
				<< " TestCases passed. ( " << nPassed / (nFailed + nPassed) * 100 << "% )" << std::endl;
			std::cout << green;
		}

	}
}

void tinystl_main() 
{
    
    // 运行所有简单测试、性能测试、容器测试
    RUN_ALL_TESTS();
    tinystl::test::algorithm_performance_test::algorithm_performance_test();
    //tinystl::test::vector_test::vector_test();
    //tinystl::test::list_test::list_test();
    //tinystl::test::deque_test::deque_test();
    //tinystl::test::queue_test::queue_test();
    //tinystl::test::queue_test::priority_test();
    //tinystl::test::stack_test::stack_test();
    tinystl::test::map_test::map_test();
    tinystl::test::map_test::multimap_test();
    tinystl::test::set_test::set_test();
    tinystl::test::set_test::multiset_test();
    tinystl::test::hash_map_test::hash_map_test();
    tinystl::test::hash_map_test::hash_multimap_test();
    tinystl::test::hash_set_test::hash_set_test();
    tinystl::test::hash_set_test::hash_multiset_test();
    tinystl::test::string_test::string_test();
}

