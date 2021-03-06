//#include <iostream>
//#include <boost/asio.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>
//
//class async_timer {
//public:
//	async_timer(
//		boost::asio::io_service &io_service,
//		boost::asio::deadline_timer::duration_type const &duration,
//		int iterations)
//		: m_timer(io_service, duration)
//		, m_duration(duration)
//		, m_iteration(0)
//		, m_maxIterations(iterations)
//	{
//		m_timer.async_wait(std::bind(&async_timer::print, this, std::placeholders::_1));
//	}
//
//	void print(boost::system::error_code const &e)
//	{
//		std::cout << "Timer Iteration " << m_iteration << std::endl;
//		m_iteration++;
//
//		if (m_iteration < m_maxIterations)
//		{
//			m_timer.expires_from_now(m_duration);
//			m_timer.async_wait(std::bind(&async_timer::print, this, std::placeholders::_1));
//		}
//	}
//
//private:
//	boost::asio::deadline_timer m_timer;
//	boost::asio::deadline_timer::duration_type m_duration;
//	int m_iteration;
//	int m_maxIterations;
//
//};
//
//int main()
//{
//	boost::asio::io_service io_service;
//
//	std::cout << "Before Timer" << std::endl;
//	async_timer(io_service, boost::posix_time::seconds(1), 5);
//
//	io_service.run();
//	std::cout << "Run returned." << std::endl;
//
//	return 0;
//}
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include "chat_message.hpp"

using boost::asio::ip::tcp;


namespace
{
	const char* SERVER_PORT = "80808";
}
//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;

//----------------------------------------------------------------------

class chat_participant
{
public:
	virtual ~chat_participant() {}
	virtual void deliver(const chat_message& msg) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------

class chat_room
{
public:
	void join(chat_participant_ptr participant)
	{
		participants_.insert(participant);
		for (auto msg : recent_msgs_)
			participant->deliver(msg);
	}

	void leave(chat_participant_ptr participant)
	{
		participants_.erase(participant);
	}

	void deliver(const chat_message& msg)
	{
		recent_msgs_.push_back(msg);
		while (recent_msgs_.size() > max_recent_msgs)
			recent_msgs_.pop_front();

		for (auto participant : participants_)
			participant->deliver(msg);
	}

private:
	std::set<chat_participant_ptr> participants_;
	enum { max_recent_msgs = 100 };
	chat_message_queue recent_msgs_;
};

//----------------------------------------------------------------------

class chat_session
	: public chat_participant,
	public std::enable_shared_from_this<chat_session>
{
public:
	chat_session(tcp::socket socket, chat_room& room)
		: socket_(std::move(socket)),
		room_(room)
	{
	}

	void start()
	{
		room_.join(shared_from_this());
		do_read_header();
	}

	void deliver(const chat_message& msg)
	{
		bool write_in_progress = !write_msgs_.empty();
		write_msgs_.push_back(msg);
		if (!write_in_progress)
		{
			do_write();
		}
	}

private:
	void do_read_header()
	{
		auto self(shared_from_this());
		boost::asio::async_read(socket_,
			boost::asio::buffer(read_msg_.data(), chat_message::header_length),
			[this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec && read_msg_.decode_header())
			{
				do_read_body();
			}
			else
			{
				room_.leave(shared_from_this());
			}
		});
	}

	void do_read_body()
	{
		auto self(shared_from_this());
		boost::asio::async_read(socket_,
			boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
			[this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				room_.deliver(read_msg_);
				do_read_header();
			}
			else
			{
				room_.leave(shared_from_this());
			}
		});
	}

	void do_write()
	{
		auto self(shared_from_this());
		boost::asio::async_write(socket_,
			boost::asio::buffer(write_msgs_.front().data(),
				write_msgs_.front().length()),
			[this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				write_msgs_.pop_front();
				if (!write_msgs_.empty())
				{
					do_write();
				}
			}
			else
			{
				room_.leave(shared_from_this());
			}
		});
	}

	tcp::socket socket_;
	chat_room& room_;
	chat_message read_msg_;
	chat_message_queue write_msgs_;
};

//----------------------------------------------------------------------

class chat_server
{
public:
	chat_server(boost::asio::io_service& io_service,
		const tcp::endpoint& endpoint)
		: acceptor_(io_service, endpoint),
		socket_(io_service)
	{
		do_accept();
	}

private:
	void do_accept()
	{
		acceptor_.async_accept(socket_,
			[this](boost::system::error_code ec)
		{
			if (!ec)
			{
				std::make_shared<chat_session>(std::move(socket_), room_)->start();
			}

			do_accept();
		});
	}

	tcp::acceptor acceptor_;
	tcp::socket socket_;
	chat_room room_;
};

//----------------------------------------------------------------------

int main()
{
	try
	{
		std::cout << "server running on port "<< SERVER_PORT<<std::endl;
		boost::asio::io_service io_service;

		std::list<chat_server> servers;
			tcp::endpoint endpoint(tcp::v4(), std::atoi(SERVER_PORT));
			servers.emplace_back(io_service, endpoint);

		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
	std::cin.get();
	return 0;
}


//sync

//#ifdef WIN32
//#define _WIN32_WINNT 0x0501
//#include <stdio.h>
//#endif
//
//#include <iostream>
//#include <boost/thread.hpp>
//#include <boost/bind.hpp>
//#include <boost/asio.hpp>
//#include <boost/shared_ptr.hpp>
//#include <boost/enable_shared_from_this.hpp>
//using namespace boost::asio;
//using namespace boost::posix_time;
//io_service service;
//
//struct talk_to_client;
//typedef boost::shared_ptr<talk_to_client> client_ptr;
//typedef std::vector<client_ptr> array;
//array clients;
//// thread-safe access to clients array
//boost::recursive_mutex cs;
//
//void update_clients_changed();
//
///** simple connection to server:
//- logs in just with username (no password)
//- all connections are initiated by the client: client asks, server answers
//- server disconnects any client that hasn't pinged for 5 seconds
//Possible requests:
//- gets a list of all connected clients
//- ping: the server answers either with "ping ok" or "ping client_list_changed"
//*/
//struct talk_to_client : boost::enable_shared_from_this<talk_to_client> {
//	talk_to_client()
//		: sock_(service), started_(false), already_read_(0) {
//		last_ping = microsec_clock::local_time();
//	}
//	std::string username() const { return username_; }
//
//	void answer_to_client() {
//		try {
//			read_request();
//			process_request();
//		}
//		catch (boost::system::system_error&) {
//			stop();
//		}
//		if (timed_out()) {
//			stop();
//			std::cout << "stopping " << username_ << " - no ping in time" << std::endl;
//		}
//	}
//	void set_clients_changed() { clients_changed_ = true; }
//	ip::tcp::socket & sock() { return sock_; }
//	bool timed_out() const {
//		ptime now = microsec_clock::local_time();
//		long long ms = (now - last_ping).total_milliseconds();
//		return ms > 5000;
//	}
//	void stop() {
//		// close client connection
//		boost::system::error_code err;
//		sock_.close(err);
//	}
//private:
//	void read_request() {
//		if (sock_.available())
//			already_read_ += sock_.read_some(
//				buffer(buff_ + already_read_, max_msg - already_read_));
//	}
//	void process_request() {
//		bool found_enter = std::find(buff_, buff_ + already_read_, '\n')
//			< buff_ + already_read_;
//		if (!found_enter)
//			return; // message is not full
//					// process the msg
//		last_ping = microsec_clock::local_time();
//		size_t pos = std::find(buff_, buff_ + already_read_, '\n') - buff_;
//		std::string msg(buff_, pos);
//		std::copy(buff_ + already_read_, buff_ + max_msg, buff_);
//		already_read_ -= pos + 1;
//
//		if (msg.find("login ") == 0) on_login(msg);
//		else if (msg.find("ping") == 0) on_ping();
//		else if (msg.find("ask_clients") == 0) on_clients();
//		else std::cerr << "invalid msg " << msg << std::endl;
//	}
//
//	void on_login(const std::string & msg) {
//		std::istringstream in(msg);
//		in >> username_ >> username_;
//		std::cout << username_ << " logged in" << std::endl;
//		write("login ok\n");
//		update_clients_changed();
//	}
//	void on_ping() {
//		write(clients_changed_ ? "ping client_list_changed\n" : "ping ok\n");
//		clients_changed_ = false;
//	}
//	void on_clients() {
//		std::string msg;
//		{ boost::recursive_mutex::scoped_lock lk(cs);
//		for (array::const_iterator b = clients.begin(), e = clients.end(); b != e; ++b)
//			msg += (*b)->username() + " ";
//		}
//		write("clients " + msg + "\n");
//	}
//
//
//	void write(const std::string & msg) {
//		sock_.write_some(buffer(msg));
//	}
//private:
//	ip::tcp::socket sock_;
//	enum { max_msg = 1024 };
//	int already_read_;
//	char buff_[max_msg];
//	bool started_;
//	std::string username_;
//	bool clients_changed_;
//	ptime last_ping;
//};
//
//void update_clients_changed() {
//	boost::recursive_mutex::scoped_lock lk(cs);
//	for (array::iterator b = clients.begin(), e = clients.end(); b != e; ++b)
//		(*b)->set_clients_changed();
//}
//
//
//
//void accept_thread() {
//	ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8001));
//	while (true) {
//		client_ptr new_(new talk_to_client);
//		acceptor.accept(new_->sock());
//
//		boost::recursive_mutex::scoped_lock lk(cs);
//		clients.push_back(new_);
//	}
//}
//
//void handle_clients_thread() {
//	while (true) {
//		boost::this_thread::sleep(millisec(1));
//		boost::recursive_mutex::scoped_lock lk(cs);
//		for (array::iterator b = clients.begin(), e = clients.end(); b != e; ++b)
//			(*b)->answer_to_client();
//		// erase clients that timed out
//		clients.erase(std::remove_if(clients.begin(), clients.end(),
//			boost::bind(&talk_to_client::timed_out, _1)), clients.end());
//	}
//}
//
//int main(int argc, char* argv[]) {
//	boost::thread_group threads;
//	threads.create_thread(accept_thread);
//	threads.create_thread(handle_clients_thread);
//	threads.join_all();
//}


//async
//#ifdef WIN32
//#define _WIN32_WINNT 0x0501
//#include <stdio.h>
//#endif
//
//
//#include <iostream>
//#include <boost/bind.hpp>
//#include <boost/asio.hpp>
//#include <boost/shared_ptr.hpp>
//#include <boost/enable_shared_from_this.hpp>
//using namespace boost::asio;
//using namespace boost::posix_time;
//io_service service;
//
//class talk_to_client;
//typedef boost::shared_ptr<talk_to_client> client_ptr;
//typedef std::vector<client_ptr> array;
//array clients;
//
//#define MEM_FN(x)       boost::bind(&self_type::x, shared_from_this())
//#define MEM_FN1(x,y)    boost::bind(&self_type::x, shared_from_this(),y)
//#define MEM_FN2(x,y,z)  boost::bind(&self_type::x, shared_from_this(),y,z)
//
//
//void update_clients_changed();
//
///** simple connection to server:
//- logs in just with username (no password)
//- all connections are initiated by the client: client asks, server answers
//- server disconnects any client that hasn't pinged for 5 seconds
//Possible client requests:
//- gets a list of all connected clients
//- ping: the server answers either with "ping ok" or "ping client_list_changed"
//*/
//class talk_to_client : public boost::enable_shared_from_this<talk_to_client>
//	, boost::noncopyable {
//	typedef talk_to_client self_type;
//	talk_to_client() : sock_(service), started_(false),
//		timer_(service), clients_changed_(false) {
//	}
//public:
//	typedef boost::system::error_code error_code;
//	typedef boost::shared_ptr<talk_to_client> ptr;
//
//	void start() {
//		started_ = true;
//		clients.push_back(shared_from_this());
//		last_ping = boost::posix_time::microsec_clock::local_time();
//		// first, we wait for client to login
//		do_read();
//	}
//	static ptr new_() {
//		ptr new_(new talk_to_client);
//		return new_;
//	}
//	void stop() {
//		if (!started_) return;
//		started_ = false;
//		sock_.close();
//
//		ptr self = shared_from_this();
//		array::iterator it = std::find(clients.begin(), clients.end(), self);
//		clients.erase(it);
//		update_clients_changed();
//	}
//	bool started() const { return started_; }
//	ip::tcp::socket & sock() { return sock_; }
//	std::string username() const { return username_; }
//	void set_clients_changed() { clients_changed_ = true; }
//private:
//	void on_read(const error_code & err, size_t bytes) {
//		if (err) stop();
//		if (!started()) return;
//		// process the msg
//		std::string msg(read_buffer_, bytes);
//		if (msg.find("login ") == 0) on_login(msg);
//		else if (msg.find("ping") == 0) on_ping();
//		else if (msg.find("ask_clients") == 0) on_clients();
//		else std::cerr << "invalid msg " << msg << std::endl;
//	}
//
//	void on_login(const std::string & msg) {
//		std::istringstream in(msg);
//		in >> username_ >> username_;
//		std::cout << username_ << " logged in" << std::endl;
//		do_write("login ok\n");
//		update_clients_changed();
//	}
//	void on_ping() {
//		do_write(clients_changed_ ? "ping client_list_changed\n" : "ping ok\n");
//		clients_changed_ = false;
//	}
//	void on_clients() {
//		std::string msg;
//		for (array::const_iterator b = clients.begin(), e = clients.end(); b != e; ++b)
//			msg += (*b)->username() + " ";
//		do_write("clients " + msg + "\n");
//	}
//
//	void do_ping() {
//		do_write("ping\n");
//	}
//	void do_ask_clients() {
//		do_write("ask_clients\n");
//	}
//
//	void on_check_ping() {
//		boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
//		if ((now - last_ping).total_milliseconds() > 5000) {
//			std::cout << "stopping " << username_ << " - no ping in time" << std::endl;
//			stop();
//		}
//		last_ping = boost::posix_time::microsec_clock::local_time();
//	}
//	void post_check_ping() {
//		timer_.expires_from_now(boost::posix_time::millisec(5000));
//		timer_.async_wait(MEM_FN(on_check_ping));
//	}
//
//
//	void on_write(const error_code & err, size_t bytes) {
//		do_read();
//	}
//	void do_read() {
//		async_read(sock_, buffer(read_buffer_),
//			MEM_FN2(read_complete, _1, _2), MEM_FN2(on_read, _1, _2));
//		post_check_ping();
//	}
//	void do_write(const std::string & msg) {
//		if (!started()) return;
//		std::copy(msg.begin(), msg.end(), write_buffer_);
//		sock_.async_write_some(buffer(write_buffer_, msg.size()),
//			MEM_FN2(on_write, _1, _2));
//	}
//	size_t read_complete(const boost::system::error_code & err, size_t bytes) {
//		if (err) return 0;
//		bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
//		// we read one-by-one until we get to enter, no buffering
//		return found ? 0 : 1;
//	}
//private:
//	ip::tcp::socket sock_;
//	enum { max_msg = 1024 };
//	char read_buffer_[max_msg];
//	char write_buffer_[max_msg];
//	bool started_;
//	std::string username_;
//	deadline_timer timer_;
//	boost::posix_time::ptime last_ping;
//	bool clients_changed_;
//};
//
//void update_clients_changed() {
//	for (array::iterator b = clients.begin(), e = clients.end(); b != e; ++b)
//		(*b)->set_clients_changed();
//}
//
//ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8001));
//
//void handle_accept(talk_to_client::ptr client, const boost::system::error_code & err) {
//	client->start();
//	talk_to_client::ptr new_client = talk_to_client::new_();
//	acceptor.async_accept(new_client->sock(), boost::bind(handle_accept, new_client, _1));
//}
//
//
//int main(int argc, char* argv[]) {
//	talk_to_client::ptr client = talk_to_client::new_();
//	acceptor.async_accept(client->sock(), boost::bind(handle_accept, client, _1));
//	service.run();
//}