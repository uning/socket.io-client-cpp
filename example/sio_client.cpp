
#include "handler.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <string>

using namespace websocketpp;


using namespace std;

int main(int argc, char* argv[]) {
	std::string uri;

	if (argc != 2) {
		std::cout << "Usage: `sio_client ws_uri`" << std::endl;
	} else {
		uri = argv[1];
	}

	try {
		/*
		std::cout<<"url: "<<uri<<endl;
		websocketpp::uri u(uri);
		//websocketpp::uri u("http://192.168.1.50:8880/socket.io/1/?t=1332823965725");
		std::cout<<"host: "<<u.get_host()<<"\nport: "<<u.get_port()<<"\npath: "<<u.get_resource()<<endl;

		//return 0;
		handshake(u.get_host(),u.get_port_str(),u.get_resource());
		return 0;
		*/
		sio_client_handler_ptr handler(new my_handler());
		sioclient::connection_ptr con;
		sioclient endpoint(handler);
	

		
		endpoint.alog().set_level(websocketpp::log::alevel::ALL);
		endpoint.elog().set_level(websocketpp::log::elevel::ALL);

		/*
		endpoint.elog().set_level(websocketpp::log::elevel::RERROR);
		endpoint.elog().set_level(websocketpp::log::elevel::FATAL);
		*/

		con = endpoint.get_connection(uri);
		
		




		endpoint.connect(con);
		Json::Value v;
		v["int"] = 1;
		v["string"] = "string";
		std::cout<<v.toStyledString()<<std::endl;

		
		

		boost::thread th(boost::bind(&sioclient::run, &endpoint, false));

		char line[512];
		while (std::cin.getline(line, 512)) {
			std::string arstr = "{\"t\":1,\"c\":\"";
			arstr= arstr+line+"\"}";
			if(line == "/close"){
				handler->close();
			}
			handler->emit("message",arstr);
		}

		th.join();
	} catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	return 0;
}
