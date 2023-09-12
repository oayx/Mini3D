#ifndef MYTINYSTL_SET_TEST_H_
#define MYTINYSTL_SET_TEST_H_

// set test : 测试 set, multiset 的接口与它们 insert 的性能

#include <set>

#include "../set.h"
#include "test.h"

namespace tinystl {
    namespace test {
        namespace set_test {

            // set,multiset insert 的性能测试
            #define SET_INSERT_TEST(mode, con, count) do { \
                srand((int)time(0)); \
                clock_t start, end; \
                mode::con<int> s; \
                char buf[10]; \
                start = clock(); \
                for (size_t i = 0; i < count; ++i) \
                    s.insert(rand()); \
                end = clock(); \
                int n = end - start; \
                sprintf(buf, "%d", n); \
                std::string t = buf; \
                t += "ms    |"; \
                std::cout << std::setw(WIDE) << t; \
            } while(0)

            void set_test(){
                std::cout << "[===============================================================]" << std::endl;
                std::cout << "[------------------ Run container test : set -------------------]" << std::endl;
                std::cout << "[-------------------------- API test ---------------------------]" << std::endl;
                int a[] = { 5,4,3,2,1 };
                tinystl::set<int> s1;
                tinystl::set<int> s2(a, a + 5);
                tinystl::set<int> s3(a, a + 5);
                tinystl::set<int> s4(s2);
                tinystl::set<int> s5(std::move(s2));
                tinystl::set<int> s6 = s3;
                tinystl::set<int> s7 = std::move(s3);

                FUN_AFTER(s1, s1.insert(a, a + 5));
                FUN_AFTER(s1, s1.insert(5));
                FUN_AFTER(s1, s1.insert(s1.end(), 5));
                FUN_AFTER(s1, s1.erase(s1.begin()));
                FUN_AFTER(s1, s1.erase(1));
                FUN_AFTER(s1, s1.erase(s1.begin(), s1.find(3)));
                FUN_AFTER(s1, s1.clear());
                FUN_AFTER(s1, s1.swap(s4));
                FUN_VALUE(*s1.begin());
                FUN_VALUE(*s1.rbegin());
                std::cout << std::boolalpha;
                FUN_VALUE(s1.empty());
                std::cout << std::noboolalpha;
                FUN_VALUE(s1.size());
                FUN_VALUE(s1.max_size());
                FUN_VALUE(s1.count(1));
                FUN_VALUE(*s1.find(3));
                FUN_VALUE(*s1.lower_bound(3));
                FUN_VALUE(*s1.upper_bound(3));
                auto first = *s1.equal_range(3).first;
                auto second = *s1.equal_range(3).second;
                std::cout << " s1.equal_range(3) : from " << first << " to " << second << std::endl;
                PASSED;
                std::cout << "[--------------------- Performance Testing ---------------------]" << std::endl;
                std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
                std::cout << "|       insert        |";
                TEST_LEN(LEN1, LEN2, LEN3, WIDE);
                std::cout << "|         std         |";
                SET_INSERT_TEST(std, set, LEN1);
                SET_INSERT_TEST(std, set, LEN2);
                SET_INSERT_TEST(std, set, LEN3);
                std::cout << std::endl << "|        tinystl        |";
                SET_INSERT_TEST(tinystl, set, LEN1);
                SET_INSERT_TEST(tinystl, set, LEN2);
                SET_INSERT_TEST(tinystl, set, LEN3);
                std::cout << std::endl;
                std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
                PASSED;
                std::cout << "[------------------ End container test : set -------------------]" << std::endl;
            }

            void multiset_test() {
                std::cout << "[===============================================================]" << std::endl;
                std::cout << "[---------------- Run container test : multiset ----------------]" << std::endl;
                std::cout << "[-------------------------- API test ---------------------------]" << std::endl;
                int a[] = { 5,4,3,2,1 };
                tinystl::multiset<int> s1;
                tinystl::multiset<int> s2(a, a + 5);
                tinystl::multiset<int> s3(a, a + 5);
                tinystl::multiset<int> s4(s2);
                tinystl::multiset<int> s5(std::move(s2));
                tinystl::multiset<int> s6 = s3;
                tinystl::multiset<int> s7 = std::move(s3);
            
                FUN_AFTER(s1, s1.insert(a, a + 5));
                FUN_AFTER(s1, s1.insert(5));
                FUN_AFTER(s1, s1.insert(s1.end(), 5));
                FUN_AFTER(s1, s1.erase(s1.begin()));
                FUN_AFTER(s1, s1.erase(1));
                FUN_AFTER(s1, s1.erase(s1.begin(), s1.find(3)));
                FUN_AFTER(s1, s1.clear());
                FUN_AFTER(s1, s1.swap(s4));
                FUN_VALUE(*s1.begin());
                FUN_VALUE(*s1.rbegin());
                std::cout << std::boolalpha;
                FUN_VALUE(s1.empty());
                std::cout << std::noboolalpha;
                FUN_VALUE(s1.size());
                FUN_VALUE(s1.max_size());
                FUN_VALUE(s1.count(1));
                FUN_VALUE(*s1.find(3));
                FUN_VALUE(*s1.lower_bound(3));
                FUN_VALUE(*s1.upper_bound(3));
                auto first = *s1.equal_range(3).first;
                auto second = *s1.equal_range(3).second;
                std::cout << " s1.equal_range(3) : from " << first << " to " << second << std::endl;
                PASSED;
                std::cout << "[--------------------- Performance Testing ---------------------]" << std::endl;
                std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
                std::cout << "|       insert        |";
                TEST_LEN(LEN1, LEN2, LEN3, WIDE);
                std::cout << "|         std         |";
                SET_INSERT_TEST(std, multiset, LEN1);
                SET_INSERT_TEST(std, multiset, LEN2);
                SET_INSERT_TEST(std, multiset, LEN3);
                std::cout << std::endl << "|        tinystl        |";
                SET_INSERT_TEST(tinystl, multiset, LEN1);
                SET_INSERT_TEST(tinystl, multiset, LEN2);
                SET_INSERT_TEST(tinystl, multiset, LEN3);
                std::cout << std::endl;
                std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
                PASSED;
                std::cout << "[---------------- End container test : multiset ----------------]" << std::endl;
            }
        }
    }
}
#endif // !MYTINYSTL_SET_TEST_H_

