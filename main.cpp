
#include <iostream>
#include "connection.h"
#include "CommonConnectionPool.h"
int main()
{
	/*
	测试connection的功能实现
	
	Connection conn;
	char sql[1024] = { 0 };
	sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')","zhangsan",20,"male");
	conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
	conn.update(sql);
	*/

#if 0	
	clock_t begin = clock();
	for (int i = 0;i < 10000;++i)
	{
		Connection conn;
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
		conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
		conn.update(sql);
	}
	clock_t end = clock();
	std::cout << (end - begin) << "ms" << std::endl;
	/*1000：5217ms*/
	/*5000：27932ms*/
	/*10000：55293ms*/
	
	
	clock_t begin = clock();
	ConnectionPool* cp = ConnectionPool::getConnectionPool();
	for (int i = 0;i < 5000;++i)
	{
		std::shared_ptr<Connection> sp=cp->getConnection();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
		sp->update(sql);
	}
	clock_t end = clock();
	std::cout << (end - begin) << "ms" << std::endl;
	
	/*1000：3412ms*/
	/*5000：16225ms*/
	/*10000：31749ms*/
#endif
#if 0
	clock_t begin = clock();
	ConnectionPool* cp = ConnectionPool::getConnectionPool();
	std::thread t1([&]() {
		for (int i = 0;i < 2500;++i)
		{
			std::shared_ptr<Connection> sp = cp->getConnection();
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
			sp->update(sql);
		}
		});
	std::thread t2([&]() {
		for (int i = 0;i < 2500;++i)
		{
			std::shared_ptr<Connection> sp = cp->getConnection();
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
			sp->update(sql);
		}
		});
	std::thread t3([&]() {
		for (int i = 0;i < 2500;++i)
		{
			std::shared_ptr<Connection> sp = cp->getConnection();
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
			sp->update(sql);
		}
		});
	std::thread t4([&]() {
		for (int i = 0;i < 2500;++i)
		{
			std::shared_ptr<Connection> sp = cp->getConnection();
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
			sp->update(sql);
		}
		});
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	clock_t end = clock();
	std::cout << (end - begin) << "ms" << std::endl;
	/*1000：2208ms*/
	/*5000：10748ms*/
	/*10000：20639ms*/
#endif
	Connection conn;
	clock_t begin = clock();
	conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
	std::thread t1([&]() {
		for (int i = 0;i < 2500;++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		}
		});
	std::thread t2([&]() {
		for (int i = 0;i < 2500;++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		}
		});
	std::thread t3([&]() {
		for (int i = 0;i < 2500;++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		}
		});
	std::thread t4([&]() {
		for (int i = 0;i < 2500;++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		}
		});
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	clock_t end = clock();
	std::cout << (end - begin) << "ms" << std::endl;
	return 0;
	/*1000：2297ms*/
	/*5000：11222ms*/
	/*10000：22342ms*/
}